// --- ShowEvent.C -------------------------------------------------------------o
//                                                                              |
// Version:     0.1                                                             |
// Author:      Matteo Brini                                                    |
// Date:        16/03/24                                                        |
// E-mail:      brinimatteo@gmail.com                                           |
//                                                                              |
// Description:                                                                 |
//      Macro to draw a visualization of timing counter pixels. The user must   |
//      give in input a runnumber and an event number.                          |
//                                                                              |
//------------------------------------------------------------------------------o

using namespace std;

Bool_t DMTable[256][512];

bool isCoarseValid(Int_t runnum, Int_t coarse)
{
    if (runnum < 393800)
        return coarse == 114 || coarse == 115;
    else if (runnum < 393900)
        return coarse == 97 || coarse == 98;
    else
        return coarse == 98 || coarse == 99;
}

bool isHitValid(Int_t runnum, Float_t hit)
{
    if (runnum < 393800)
        return hit >= -0.44e-6 && hit <= -0.41e-6;
    else
        return hit >= -0.65e-6 && hit <= -0.61e-6;
}

void loadDMTable(const char* path)
{
    FILE *tabin = fopen(path,"r");
    Int_t data;

    for(Int_t iTile = 0; iTile<512; iTile++)
    {
        for(Int_t iPatch = 0; iPatch<256; iPatch++)
        {
            fscanf(tabin,"%d",&data);
            if(data)
                DMTable[iPatch][iTile] = true;
            else
                DMTable[iPatch][iTile] = false;
        }
    }
}

void ShowEvent(Int_t runnum = -1, Int_t eventNum = -1, Int_t patchId = -1)
{
    if (runnum == -1 || eventNum == -1)
    {
        cout << "Please select a runnumber and an eventNum!" << endl;
        return;
    }

    if (patchId < -1 || patchId >= 256)
    {
        cout << "Invalid patchId passed: " << patchId << endl;
        return;
    }
    else
    {
        loadDMTable("/meg/home/francesconi_m/git/online/scripts/trigger/dmnarrow.mem");
        //loadDMTable("/meg/home/brini_m/Git/offline/analyzer/dmout.mem");
    }

    TFile * f = new TFile(Form("/meg/data1/offline/run/%03dxxx/rec%06d_unbiassed.root", runnum/1000, runnum));
    //TFile * f = new TFile(Form("/meg/data1/offline/run/%03dxxx/rec%06d_open.root", runnum/1000, runnum));
    //TFile * f = new TFile(Form("/meg/data1/offline/run/%03dxxx/rec%06d_check.root", runnum/1000, runnum));

    TTree * rec = (TTree*)f->Get("rec");
    MEGTRGRunHeader * trgRunHeader = static_cast<MEGTRGRunHeader*>(f->Get("TRGRunHeader"));
    if(rec == nullptr || trgRunHeader == nullptr){
        cout << "Missing tree or TRG run header!" << endl;
        return;
    }
    
    ROMETreeInfo   * InfoRV = 0;
    MEGEventHeader * EventHeaderRV = 0;
    MEGTRGData     * TRGDataRV = 0;
    MEGRecData     * RecDataRV = 0;
    TClonesArray   * SPXHitsRA = 0;
    TClonesArray   * GLBPositronRA = 0;

    rec->SetBranchAddress("Info.", &InfoRV);
    rec->SetBranchAddress("eventheader.", &EventHeaderRV);
    rec->SetBranchAddress("trgdata.", &TRGDataRV);
    rec->SetBranchAddress("reco.", &RecDataRV);
    rec->SetBranchAddress("spxhits", &SPXHitsRA);
    rec->SetBranchAddress("positron", &GLBPositronRA);

    rec->SetBranchStatus("*", 0);
    rec->SetBranchStatus("Info.*", 1);
    rec->SetBranchStatus("eventheader.*", 1);
    rec->SetBranchStatus("trgdata.*", 1);
    rec->SetBranchStatus("reco.*", 1);
    rec->SetBranchStatus("spxhits*", 1);
    rec->SetBranchStatus("positron*", 1);

    Long_t entryId = -1;
    Long_t nEntries = rec->GetEntries();
    for(Long_t iEntry=0; iEntry < nEntries && entryId < 0; iEntry++){
        rec->GetEntry(iEntry);

        if(eventNum==-1)
            entryId = iEntry;
        if(eventNum<-1)
            eventNum++;

        if(InfoRV->GetEventNumber() == eventNum)
            entryId = iEntry;
    }

    if(entryId==-1){
        cout << "Event not found!" << endl;
        return;
    }

    Int_t triggerId = EventHeaderRV->Getid();
    Int_t triggerMask = EventHeaderRV->Getmask();
    Int_t tctoOdb = trgRunHeader->GetTcTimeOffset();
    cout << "---------------------------------------------" << endl;
    cout << "Event infos: " << endl;
    cout << "   Run: " << runnum << endl;
    cout << "   Event: " << eventNum << endl;
    cout << "   Trigger ID: " << triggerId << endl;
    cout << "   Trigger mask: " << triggerMask << endl;
    if (patchId != -1)
        cout << "   Patch ID: " << patchId << endl;

    TCanvas* cSPX = new TCanvas("cSPX", "cSPX", 1000, 500);
    TVirtualPad * pad = cSPX->cd();
    
    MEGSPXRunHeader * spxRunHeader = (MEGSPXRunHeader*)f->Get("SPXRunHeader");
    TClonesArray * spxPixelRunHeader = (TClonesArray*)f->Get("SPXPixelRunHeader"); 
    if(spxRunHeader==nullptr || spxPixelRunHeader==nullptr){
        cout << "Missing SPX run headers!" << endl; 
        printf("missing SPX run headers\n");
        return;
    }
    MEGSPXGeometry * fGeometry = MEGSPXGeometry::GetInstance(spxRunHeader, spxPixelRunHeader);
    
    pad->SetFillColor(kBlack);
    pad->SetFrameFillColor(kBlack);
    pad->Draw();
    TH2F * fFrameHist = new TH2F("TRGSPX Frame hist", "", 20, -120, 120, 20, -86., 4.);
    fFrameHist->Draw();
    
    std::map<Int_t, std::vector<Color_t>> pixelSet;
    Int_t nPixel = fGeometry->GetNPixels();
    for (Int_t iPixel = 0; iPixel < nPixel; iPixel++) {
        pixelSet[iPixel].push_back(kGray);
    }

    cout << "---------------------------------------------" << endl;
    cout << "MEGSPXHit infos:" << endl;
    nPixel = SPXHitsRA->GetEntries();
    for (Int_t iPixel = 0; iPixel < nPixel; iPixel++) {
        auto pixelid = ((MEGSPXHit *)(SPXHitsRA->At(iPixel)))->Getpixelid();
        auto time = ((MEGSPXHit *)SPXHitsRA->At(iPixel))->Gettime();
        cout << "\tPixelID: " << setw(3) << pixelid << " time: " << setw(12) << time;
        if (isHitValid(runnum, time))
        {
            cout << " isValid";
            if (patchId == -1)
                pixelSet[pixelid][0] = kGreen;
        }
        cout << endl;
    }
    
    // DMTable
    if (patchId != -1)
    {

        for (Int_t iPixel = 0; iPixel < 512; ++iPixel)
        {
            if (DMTable[patchId][iPixel])
                pixelSet[iPixel][0] = kYellow;
        }
    }

    cout << "---------------------------------------------" << endl;
    cout << "Reco Positron" << endl;
    Int_t nPositron = RecDataRV->GetNPositron();
    for (Int_t iPos = 0; iPos < nPositron; ++iPos)
    {
        auto positron = (MEGGLBPositron *)GLBPositronRA->At(RecDataRV->GetPositronIndexAt(iPos));
        auto pixelid = positron->GetMatchedPixelID();
        Int_t iPixel = 0;
        while (pixelid != ((MEGSPXHit *)(SPXHitsRA->At(iPixel)))->Getpixelid())
            ++iPixel;
        auto time = ((MEGSPXHit *)SPXHitsRA->At(iPixel))->Gettime();
        cout << "Positron " << iPos + 1 << "/" << nPositron << endl;
        cout << "\tPixelID: " << pixelid << endl;      
        if (isHitValid(runnum, time))
            pixelSet[pixelid][0] = kBlue;
    }

    MEGDiscrWaveform * trackHitWaveform[4];
    MEGFADCWaveform  * trackTileIdWaveform[4];
    MEGFADCWaveform  * trackTileTimeWaveform[4];
    for(Int_t i = 0; i < 4; i++){
        trackHitWaveform[i] = TRGDataRV->GetTCTrackHitAt(i);
        trackTileIdWaveform[i] = TRGDataRV->GetTCTrackTileIdAt(i);
        trackTileTimeWaveform[i] = TRGDataRV->GetTCTrackTileTimeAt(i);

        Double_t maxTileIdWf = trackTileIdWaveform[i]->GetHistogramMaximum();
        Double_t minTileIdWf = trackTileIdWaveform[i]->GetHistogramMinimum();
    }
    cout << "---------------------------------------------" << endl;
    cout << "MEGTRGData infos:" << endl;
    for(Int_t i = 0; i < trackHitWaveform[0]->GetNPoints(); i++){
        for(Int_t iTrack = 0; iTrack < 4; iTrack++){
            if(trackHitWaveform[iTrack]->GetAmplitudeAt(i)){
                auto tileId = trackTileIdWaveform[iTrack]->GetAmplitudeAt(i);
                auto tileTime = trackTileTimeWaveform[iTrack]->GetAmplitudeAt(i);
                auto tileTime_ns = (tileTime + tctoOdb) * 0.78125;
                cout << "\tClock " << setw(3) << i 
                     << ", Track " << iTrack 
                     << ", TileId " << setw(3) << tileId 
                     << ", TileTime " << setw(2) << tileTime 
                     << ": " << setw(7) << tileTime_ns << " ns" << endl;
                if (isCoarseValid(runnum , i))
                    pixelSet[trackTileIdWaveform[iTrack]->GetAmplitudeAt(i)].push_back(kRed);
            }
        }
    }
    fGeometry->DrawZPhi2DView(pixelSet, "", 1, kTRUE);
}
