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

vector<PMMarker *> fFindPM(const vector<Int_t> &, const MEGXECRunHeader *, const TClonesArray *);

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
    MEGTRGData      * TRGDataRV = 0;
    TClonesArray * recXECCl = 0;

    rec->SetBranchAddress("Info.", &InfoRV);
    rec->SetBranchAddress("eventheader.", &EventHeaderRV);
    rec->SetBranchAddress("reco.", &RecDataRV);
    rec->SetBranchAddress("trgxeconline.", &TRGXECOnlineRV);
    rec->SetBranchAddress("trgdata.", &TRGDataRV);
    rec->SetBranchAddress("xeccl", &recXECCl);

    rec->SetBranchStatus("*", 0);
    rec->SetBranchStatus("Info.*", 1);
    rec->SetBranchStatus("eventheader.*", 1);
    rec->SetBranchStatus("reco.*", 1);
    rec->SetBranchStatus("trgxeconline.*", 1);
    rec->SetBranchStatus("trgdata.*", 1);
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
    
    TCanvas * c1 = new TCanvas("cXEC", "cXEC", 1000, 800);
    PMMap * pmMap = new PMMap(c1->GetPad(0));
    MEGXECRunHeader * XECRunHeaderRV = (MEGXECRunHeader *)f->Get("XECRunHeader");
    TClonesArray * XECPMRunHeaderRA = (TClonesArray *)f->Get("XECPMRunHeader");
    Double_t LXeRadius = 67.6; // TO BE CHECKED!
    Double_t uMin = -999;
    Double_t uMax = -999;
    Double_t vMin = -999;
    Double_t vMax = -999;

    if(XECRunHeaderRV)
    {
        cout << "Run Headers:" << endl;
        cout << " - nPM: " << XECRunHeaderRV->GetNPM() << endl;
        cout << " - LXe inner face radius: " << XECRunHeaderRV->GetActiveVolumeRadiusInner() << endl;
        LXeRadius = XECRunHeaderRV->GetActiveVolumeRadiusInner();

        for(Int_t i = 0; i < XECRunHeaderRV->GetNPM(); i++)
        {
            MEGXECPMRunHeader * pmrunh = (MEGXECPMRunHeader * )XECPMRunHeaderRA->At(i);
            if(pmrunh->GetIsSiPM())
            {
                if(uMin==-999 || uMin > pmrunh->GetUVWAt(0))
                    uMin = pmrunh->GetUVWAt(0);
                if(uMax==-999 || uMax < pmrunh->GetUVWAt(0))
                    uMax = pmrunh->GetUVWAt(0);
                if(vMin==-999 || vMin > pmrunh->GetUVWAt(1))
                    vMin = pmrunh->GetUVWAt(1);
                if(vMax==-999 || vMax < pmrunh->GetUVWAt(1))
                    vMax = pmrunh->GetUVWAt(1);
            }
        }

        printf("Inner face:\n");
        printf(" - U [%f, %f]\n", uMin, uMax);
        printf(" - V [%f, %f]\n", vMin, vMax);
    }
    
    //XECTOOLS::InitXECGeometryParameters(XECTOOLS::UVWDefinition::Local, 64.84, 106.27, 67.03, 96., 125.52, 0, 0, 0);
    XECTOOLS::InitXECGeometryParameters(XECTOOLS::UVWDefinition::Local,
                                        XECRunHeaderRV->GetActiveVolumeRadiusInner(),
                                        XECRunHeaderRV->GetActiveVolumeRadiusOuter(),
                                        XECRunHeaderRV->GetActiveVolumeZLengthInner(),
                                        XECRunHeaderRV->GetActiveVolumeZLengthOuter(),
                                        125.52);
    pmMap->Init(XECPMRunHeaderRA);

    pmMap->SetPadMode(0);

    // XEC Cluster
    std::vector<float> DrawVal(4760, 0.);
    std::vector<bool> Invalid(4760, false);
    for(Int_t i=0; i<4760; i++)
    {
        auto npho = ((MEGXECPMCluster*)(recXECCl->At(i)))->Getnpho();
        auto invalid = ((MEGXECPMCluster*)(recXECCl->At(i)))->Getinvalid();
        for(auto inv : invalid)
            if(inv) Invalid[i]=true;

        DrawVal[i] = 0;
        for(auto &val: npho)
            DrawVal[i] += val;
    }
    pmMap->PaintPM(DrawVal, Invalid, 1, 1000, true);

    // Reco
    cout << "---------------------------------------------" << endl;
    cout << "Reco Gamma" << endl;
    auto nGamma = RecDataRV->GetNGamma();
    for (Int_t iGamma = 0; iGamma < nGamma; ++iGamma)
    {
        auto uGamma = RecDataRV->GetUGammaAt(iGamma); // NEED TO BE INVERTED FOR CONSISTENCY IN DRAWING
        auto vGamma = RecDataRV->GetVGammaAt(iGamma);
        auto markGamma = new TMarker(-uGamma, vGamma, 29); // INVERTED U
        markGamma->SetMarkerSize(1.5);
        markGamma->SetMarkerColor(kWhite);
        markGamma->Draw("SAME");
        markGamma = new TMarker(-uGamma, vGamma, 29); // INVERTED U
        markGamma->SetMarkerSize(1.3);
        markGamma->SetMarkerColor(kBlue);
        markGamma->Draw("SAME");
        cout << "Gamma " << iGamma + 1 << "/" << nGamma << endl;
        cout << "\tu: " << setw(8) << uGamma << endl;
        cout << "\tv: " << setw(8) << vGamma << endl;
    }

    // TRG
    cout << "---------------------------------------------" << endl;
    cout << "MEGTRGXECOnline" << endl;
    map<Int_t, PMMarker *> mapPmTRG;
    auto trgPatchId = TRGXECOnlineRV->GetPatchId();
    pair<Double_t, Double_t> uvTRG = {0, 0};
    Int_t nPMTRGPatch = 0;
    for (Int_t iPM = 0; iPM < 4760 && nPMTRGPatch < 16; ++iPM)
    {
        auto pmRH = (MEGXECPMRunHeader *)XECPMRunHeaderRA->At(iPM);
        auto patch = (pmRH->GetDRSChipID() - 64) / 2; 
        if (patch == trgPatchId)
        {
            uvTRG.first += pmRH->GetUVWAt(0);
            uvTRG.second += pmRH->GetUVWAt(1);
            nPMTRGPatch++;
        }
    }
    uvTRG.first /= nPMTRGPatch;     // U-TRG position
    uvTRG.second /= nPMTRGPatch;    // V-TRG position
    PMMarker * trgMarker = new PMMarker(-uvTRG.first, uvTRG.second, 20, 0);
    trgMarker->SetMarkerSize(1.5);
    trgMarker->SetMarkerColor(kWhite);
    trgMarker->Draw("SAME");

    cout << "PatchID: " << trgPatchId << " (" << trgPatchId/16 << "/" << trgPatchId%16 << ")" << endl;

    // Waveforms
    MEGFADCWaveform * maxWF = TRGDataRV->GetXECMAXWaveform();
    MEGFADCWaveform * tdcNumWF = TRGDataRV->GetXECTdcNumWaveform();
    MEGDiscrWaveform * maxValidWF = TRGDataRV->GetXECMaxValidWaveform();
    
    Bool_t isMaxValidWF = maxValidWF->GetNPoints();

    TCanvas * cWF = new TCanvas("cWF", "cWF", 700, 250 * (2 + isMaxValidWF));
    cWF->Divide(1,2 + isMaxValidWF);

    cWF->cd(1);
    maxWF->Draw();

    cWF->cd(2);
    tdcNumWF->Draw();

    if (isMaxValidWF)
    {
        cWF->cd(3);
        maxValidWF->Draw();
    }

    // Get all patches from maxWF
    c1->cd(0);
    gStyle->SetPalette(kSolar);
    vector<Int_t> vecMaxWF;
    for (Int_t i = 0; i < maxWF->GetNPoints(); ++i)
    {
        Int_t patchWF = maxWF->GetAmplitudeAt(i);
        if (patchWF)
            vecMaxWF.push_back(patchWF);
    }
    
    vector<PMMarker *> vecPMMarkerMaxWF = fFindPM(vecMaxWF, XECRunHeaderRV, XECPMRunHeaderRA);

    for (Int_t i = 0; i < vecMaxWF.size(); ++i)
    {
        vecPMMarkerMaxWF[i]->SetMarkerColor(kMagenta + (4 - i)%5);
        vecPMMarkerMaxWF[i]->SetMarkerSize(1.5);
        vecPMMarkerMaxWF[i]->Draw("SAME");
    }
}

vector<PMMarker *> fFindPM(const vector<Int_t> & vecMaxWF, const MEGXECRunHeader * XECRunHeaderRV, const TClonesArray * XECPMRunHeaderRA)
{
    vector<PMMarker *> vecPMMarker(vecMaxWF.size());
    vector<pair<Int_t, Int_t>> uvMaxWF(vecMaxWF.size());
    vector<Int_t> vecNPM(vecMaxWF.size());
    
    for (Int_t iPM = 0; iPM < XECRunHeaderRV->GetNPM(); ++iPM)
    {
        auto pmRH = (MEGXECPMRunHeader *)XECPMRunHeaderRA->At(iPM);
        auto patch = (pmRH->GetDRSChipID() - 64) / 2; 
        for (auto & patchMaxWF : vecMaxWF)
        {
            if (patch == patchMaxWF)
            {
                auto  iter = std::find(vecMaxWF.begin(), vecMaxWF.end(), patchMaxWF);
                auto index = std::distance(vecMaxWF.begin(), iter);
                uvMaxWF[index].first += pmRH->GetUVWAt(0);
                uvMaxWF[index].second += pmRH->GetUVWAt(1);
                vecNPM[index]++;
            }
        }

    }
    
    for (Int_t i = 0; i < vecMaxWF.size(); ++i)
    {
        uvMaxWF[i].first /= vecNPM[i];
        uvMaxWF[i].second /= vecNPM[i];
    }       
    
    Int_t i = 0;
    for (auto & uvPair : uvMaxWF)
    {
        vecPMMarker[i] = new PMMarker(-uvPair.first, uvPair.second, 20, i); // INVERTED U
        i++;
    }

    return vecPMMarker;

}
