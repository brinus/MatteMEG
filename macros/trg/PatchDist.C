// --- PatchDist.C -------------------------------------------------------------o
//                                                                              |
// Version:     0.1                                                             |
// Author:      Matteo Brini                                                    |
// Date:        13/04/2024                                                      |
// E-mail:      brinimatteo@gmail.com                                           |
//                                                                              |
// Description:                                                                 |
//      Macro to study distance, given a patch, betweeen offline and online     |
//      position reconstructed of the photon.                                   |
//                                                                              |
//------------------------------------------------------------------------------o

#include "../../include/MMUtils.h"
R__ADD_LIBRARY_PATH(/meg/home/brini_m/Git/MatteMEG/lib)
R__LOAD_LIBRARY(libMMUtils.so)

Int_t GetPatch(MEGXECPMRunHeader * pm)
{
    Int_t chip = pm->GetDRSChipID();
    return (chip-64)/2;
}
void PatchDist()
{
    // Initial setup
    Bool_t kPDF = false;
    Bool_t kVerbose = false;
    MMUtils utils; 
    MEGPhysicsSelection physSelect;

    utils.SetChainPath("/meg/data1/offline/processes/20240320");
    physSelect.SetThresholds(EBeamPeriodID::kBeamPeriod2021, kTRUE);

    // TChain setup
    TChain * rec = utils.MakeTChainFromFile("/meg/home/brini_m/Documents/runlistCW.txt");

    auto nEntries = rec->GetEntries();
    cout << "Got " << nEntries << " entries" << endl;

    // XECPMRunHeader
    TFile * file = rec->GetFile();
    MEGMEGParameters * runParameters = (MEGMEGParameters*)file->Get("MEGParameters");
    MEGXECRunHeader  * xecRunHeader = static_cast<MEGXECRunHeader*>(file->Get("XECRunHeader"));
    TClonesArray     * XECPMRunHeaderRA = (TClonesArray*)file->Get("XECPMRunHeader");

    Double_t LXeRadius = 67.6; // TO BE CHECKED!
    Double_t uMin = -999;
    Double_t uMax = -999;
    Double_t vMin = -999;
    Double_t vMax = -999;

    if(xecRunHeader)
    {
        cout << "Run Headers:" << endl;
        cout << " - nPM: " << xecRunHeader->GetNPM() << endl;
        cout << " - LXe inner face radius: " << xecRunHeader->GetActiveVolumeRadiusInner() << endl;
        LXeRadius = xecRunHeader->GetActiveVolumeRadiusInner();

        for(Int_t i = 0; i < xecRunHeader->GetNPM(); i++)
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
    
    // Plots 
    TH2F * hUVDist = new TH2F("", "", 100, uMin, uMax, 100, vMin, vMax);
    array<TH2F *, 256> arrDistTRGReco;
    array<TH2F *, 256> arrDistRecoTRG;
    for (Int_t i = 0; i < 256; ++i)
    {
        arrDistTRGReco[i] = new TH2F(Form("trgPM%i", i), Form("trgPM%i", i), 100, -50, 50, 100, -50, 50);
        arrDistRecoTRG[i] = new TH2F(Form("recoPM%i", i), Form("recoPM%i", i), 100, -50, 50, 100, -50, 50);
    }
    TGraph * gXECTRG = new TGraph();
    TGraph * gXECReco = new TGraph();
    TH1F * hTRG = new TH1F("hTRG", "hTRG", 256, -0.5, 255.5);
    TH1F * hTRGCut = new TH1F("hTRGCut", "hTRGCut", 256, -0.5, 255.5);
    TH1F * hReco = new TH1F("hReco", "hReco", 256, -0.5, 255.5);
    TH1F * hRecoCut = new TH1F("hRecoCut", "hRecoCut", 256, -0.5, 255.5);

    auto UPhys = physSelect.fUGamma[1] - physSelect.fUGamma[0];
    auto VPhys = physSelect.fVGamma[1] - physSelect.fVGamma[0];
    TH2F * hUVPatchProj = new TH2F("hUVPatchProj", "hUVPatchProj", round(UPhys / 2), physSelect.fUGamma[0], physSelect.fUGamma[1],
                                                                   round(VPhys / 2), physSelect.fVGamma[0], physSelect.fVGamma[1]);
    TH3F * hUVPatch = new TH3F("hUVPatch", "hUVPatch", round(UPhys / 2), physSelect.fUGamma[0], physSelect.fUGamma[1],
                                                       round(VPhys / 2), physSelect.fVGamma[0], physSelect.fVGamma[1],
                                                       256, -0.5, 255.5);

    // TTree setup
    ROMETreeInfo    * InfoRV = 0;
    MEGEventHeader  * EventHeaderRV = 0;
    MEGTRGXECOnline * TRGXECOnlineRV = 0;
    MEGRecData      * RecDataRV = 0;

    rec->SetBranchStatus("*", 0);
    rec->SetBranchStatus("Info.*", 1);
    rec->SetBranchStatus("eventheader.*", 1);
    rec->SetBranchStatus("reco.*", 1);
    rec->SetBranchStatus("trgxeconline.*", 1);

    rec->SetBranchAddress("Info.", &InfoRV);
    rec->SetBranchAddress("eventheader.", &EventHeaderRV);
    rec->SetBranchAddress("reco.", &RecDataRV);
    rec->SetBranchAddress("trgxeconline.", &TRGXECOnlineRV);

    // GAMMA loop
    for (Int_t iEntry = 0; iEntry < nEntries; ++iEntry)
    {
        rec->GetEntry(iEntry);
        if (iEntry % 1000 == 0)
            cout << iEntry << " events finished" << endl;

        auto mask = EventHeaderRV->Getmask();
        if (mask != 10) continue;

        auto nGamma = RecDataRV->GetNGamma();
        if (nGamma != 1) continue;      

        // TRGXECOnline
        auto tdcPeakTime = TRGXECOnlineRV->GetTdcPeakTime();
        if (tdcPeakTime < 95 || tdcPeakTime > 105) continue;

        auto trgPatchId = TRGXECOnlineRV->GetPatchId();

        pair<Double_t, Double_t> posTRG = {0, 0};
        Int_t nTRGPM = 0;
        
        for (Int_t iPM = 0; iPM < 4760 && nTRGPM < 16; ++iPM)
        {
            auto pmRH = (MEGXECPMRunHeader *)XECPMRunHeaderRA->At(iPM);
            auto patch = (pmRH->GetDRSChipID() - 64) / 2; 
            if (patch == trgPatchId)
            {
                posTRG.first += pmRH->GetUVWAt(0);
                posTRG.second += pmRH->GetUVWAt(1);
                nTRGPM++;
            }
        }

        posTRG.first /= nTRGPM;
        posTRG.second /= nTRGPM;

        // RecData
        for (Int_t iGamma = 0; iGamma < nGamma; ++iGamma)
        {
            auto eGamma = RecDataRV->GetEGammaAt(iGamma);
            if (eGamma < 0.0165 || eGamma > 0.0185) continue;

            auto pileUp = RecDataRV->GetEvstatGammaAt(iGamma);
            if (pileUp != 0) continue;
        
            auto tGamma = RecDataRV->GetTGammaAt(iGamma);
            if (tGamma < -0.65e-6 || tGamma > -0.5e-6) continue;
            
            // Evaluate distance UV between TRG and Reco position
            pair<Double_t, Double_t> posGamma = {0, 0};
            posGamma.first = RecDataRV->GetUGammaAt(iGamma); // U-Reco position
            posGamma.second = RecDataRV->GetVGammaAt(iGamma); // V-Reco position
            auto posGammaW = RecDataRV->GetWGammaAt(iGamma); // W-Reco position
            
            auto uDist = posTRG.first - posGamma.first;
            auto vDist = posTRG.second - posGamma.second;
           
            // Get PatchId from RecData
            Bool_t badU = posGamma.first < uMin || posGamma.first > uMax;
            Bool_t badV = posGamma.second < vMin || posGamma.second > vMax;
            if (badU || badV) 
                continue;
            
            Int_t recoUID = round((posGamma.first-uMin)*43/(uMax-uMin)); 
            Int_t recoVID = 92-round((posGamma.second-vMin)*92/(vMax-vMin)); 
            auto * pmReco =(MEGXECPMRunHeader *)XECPMRunHeaderRA->At(recoUID + recoVID * 44);
            auto recoPatchId = GetPatch(pmReco); // Patch ID

            // Info prints
            if (kVerbose)
            {
                if (recoPatchId % 4 == 3)
                    cout << "UV reco: "<< posGamma.first << " " << posGamma.second << endl;
            }

            hTRG->Fill(trgPatchId);
            hReco->Fill(recoPatchId);

            TVector3 posXEC(posGamma.first, posGamma.second, posGammaW);
            if (physSelect.XECAcceptance(posXEC))
            {
                // Fill
                arrDistTRGReco[trgPatchId]->Fill(uDist, vDist);
                arrDistRecoTRG[recoPatchId]->Fill(-uDist, -vDist);
                hUVDist->Fill(posGamma.first, posGamma.second);
                hTRGCut->Fill(trgPatchId);
                hRecoCut->Fill(recoPatchId);
                hUVPatchProj->Fill(posGamma.first, posGamma.second);
                hUVPatch->Fill(posGamma.first, posGamma.second, trgPatchId);
            }
        }
    } 

    if (kPDF)
    {
        TCanvas * cTRGReco = new TCanvas("cTRGReco", "cTRGReco", 1);
        cTRGReco->Print("DistTRGReco.pdf[");
        for (Int_t i = 0; i < 256; ++i)
        {
            arrDistTRGReco[i]->SetTitle(Form("trg patch %i;u [cm]; v [cm]", i));
            arrDistTRGReco[i]->Draw("COLZ");
            cTRGReco->Print("DistTRGReco.pdf");
        }
        cTRGReco->Print("DistTRGReco.pdf]");

        TCanvas * cRecoTRG = new TCanvas("cRecoTRG", "cRecoTRG", 1);
        cRecoTRG->Print("DistRecoTRG.pdf[");
        for (Int_t i = 0; i < 256; ++i)
        {
            arrDistRecoTRG[i]->SetTitle(Form("reco patch %i;u [cm]; v [cm]", i));
            arrDistRecoTRG[i]->Draw("COLZ");
            cRecoTRG->Print("DistRecoTRG.pdf");
        }
        cRecoTRG->Print("DistRecoTRG.pdf]");
    }

    TCanvas * cUV = new TCanvas();
    hUVDist->SetTitle(";u [cm];v [cm]");
    hUVDist->Draw("COLZ");

    TCanvas * cPatch = new TCanvas("cP", "cP", 1000, 900);
    cPatch->Divide(1, 2);
    cPatch->cd(1);
    hTRG->SetLineColor(kRed);
    hTRGCut->SetMarkerColor(kRed);
    hTRG->Draw();
    hTRGCut->Draw("SAME E");

    cPatch->cd(2);
    hReco->SetLineColor(kBlue);
    hRecoCut->SetMarkerColor(kBlue);
    hReco->Draw();
    hRecoCut->Draw("SAME E");

    TFile * fOut = new TFile("/meg/home/brini_m/Git/MatteMEG/outfiles/UVPatch.root", "RECREATE");
    hUVPatchProj->Write();
    hUVPatch->Write();
}
