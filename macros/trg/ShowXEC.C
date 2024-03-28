// --- ShowXEC.C ---------------------------------------------------------------o
//                                                                              |
// Version:     0.1                                                             |
// Author:      Matteo Brini                                                    |
// Date:        26/03/24                                                        |
// E-mail:      brinimatteo@gmail.com                                           |
//                                                                              |
// Description:                                                                 |
//      Macro to draw a visualization of XEC detector. The user must give in    |
//      input the runnumber and the event id.                                   |
//                                                                              |
//------------------------------------------------------------------------------o

std::map<Int_t, Int_t> gIdMapPmt;
std::map<Int_t, Int_t> gIdMapMppc;

void populateXECPMMaps(MEGXECRunHeader *xecRunHeader, TClonesArray* xecPmRunHeaders){
   Int_t nPM = xecRunHeader->GetNPM();
   Int_t minMPPC = -1;
   Int_t minPMT = -1;
   for(Int_t iPM=0; iPM<nPM; iPM++){
      MEGXECPMRunHeader* pmRh = (MEGXECPMRunHeader*)(xecPmRunHeaders->At(iPM));
      Int_t chipId = pmRh->GetDRSChipID(); 
      Int_t channelOnChip = pmRh->GetDRSChannelOnChip(); 
      if(pmRh->GetIsSiPM()){
         if (minMPPC == -1 || minMPPC > chipId*8 + channelOnChip){
            minMPPC = chipId*8 + channelOnChip;
         }
      } else {
         if (minPMT == -1 || minPMT > chipId*8 + channelOnChip){
            minPMT = chipId*8 + channelOnChip;
         }
      }
   }
   for(Int_t iPM=0; iPM<nPM; iPM++){
      MEGXECPMRunHeader* pmRh = (MEGXECPMRunHeader*)(xecPmRunHeaders->At(iPM));
      Int_t chipId = pmRh->GetDRSChipID(); 
      Int_t channelOnChip = pmRh->GetDRSChannelOnChip(); 
      if(pmRh->GetIsSiPM()){
         gIdMapMppc[chipId*8 + channelOnChip - minMPPC] = iPM;
      } else {
         gIdMapPmt[chipId*8 + channelOnChip - minPMT] = iPM;
      }
   }
}

void ShowXEC(Int_t run = -1, Int_t event = -1)
{
    if (run == -1 || event == -1)
    {
        cout << "Please select a runnumber and an eventNum!" << endl;
        return;
    }


    TFile * f = new TFile(Form("/meg/data1/offline/run/%03dxxx/rec%06d.root", run/1000, run));

    TTree * rec = (TTree*)f->Get("rec");
    
    ROMETreeInfo    * InfoRV = 0;
    MEGEventHeader  * EventHeaderRV = 0;
    MEGRecData      * RecDataRV = 0;
    MEGTRGXECOnline * TRGXECOnlineRV = 0;
    TClonesArray * recXECCl = 0;

    rec->SetBranchAddress("Info.", &InfoRV);
    rec->SetBranchAddress("eventheader.", &EventHeaderRV);
    rec->SetBranchAddress("reco.", &RecDataRV);
    rec->SetBranchAddress("trgxeconline.", &TRGXECOnlineRV);
    rec->SetBranchAddress("xeccl", &recXECCl);

    rec->SetBranchStatus("*", 0);
    rec->SetBranchStatus("Info.*", 1);
    rec->SetBranchStatus("eventheader.*", 1);
    rec->SetBranchStatus("reco.*", 1);
    rec->SetBranchStatus("trgxeconline.*", 1);
    rec->SetBranchStatus("xeccl*", 1);

    Long_t entryId = -1;
    Long_t nEntries = rec->GetEntries();
    for(Long_t iEntry=0; iEntry < nEntries && entryId < 0; iEntry++){
        rec->GetEntry(iEntry);

        if(event==-1)
            entryId = iEntry;
        if(event<-1)
            event++;

        if(InfoRV->GetEventNumber() == event)
            entryId = iEntry;
    }

    if(entryId==-1){
        cout << "Event not found!" << endl;
        return;
    }
    
    TCanvas * c1 = new TCanvas("cXEC", "cXEC", 1000, 700);
    PMMap * pmMap = new PMMap(c1->GetPad(0));
    MEGXECRunHeader * XECRunHeaderRV = (MEGXECRunHeader *)f->Get("XECRunHeader");
    TClonesArray * XECPMRunHeaderRA = (TClonesArray *)f->Get("XECPMRunHeader");
    
    {
        XECTOOLS::InitXECGeometryParameters(XECTOOLS::UVWDefinition::Local, 64.84, 106.27, 67.03, 96., 125.52, 0, 0, 0);
        pmMap->Init(XECPMRunHeaderRA);
    }

    populateXECPMMaps(XECRunHeaderRV, XECPMRunHeaderRA);
    pmMap->SetPadMode(0);
    std::vector<float> DrawVal(4760, 0.);
    std::vector<bool> Invalid(4760, false);
    float pmMax=0;
    float pmMin=0;
    for(Int_t i=0; i<4760; i++)
    {
        auto npho = ((MEGXECPMCluster*)(recXECCl->At(i)))->Getnpho();
        auto invalid = ((MEGXECPMCluster*)(recXECCl->At(i)))->Getinvalid();
        for(auto inv : invalid)
            if(inv) Invalid[i]=true;

        DrawVal[i] = 0;
        for(auto &val: npho)
            DrawVal[i] += val;

        if(DrawVal[i]>pmMax)
            pmMax = DrawVal[i];
        if(DrawVal[i]<pmMin)
            pmMin = DrawVal[i];
    }
    pmMap->PaintPM(DrawVal, Invalid, pmMin, pmMax, false);

}
