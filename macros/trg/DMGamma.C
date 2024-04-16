// --- DMGamma.C ---------------------------------------------------------------o
//                                                                              |
// Version:     0.1                                                             |
// Author:      Matteo Brini                                                    |
// Date:        24/03/2024                                                      |
// E-mail:      brinimatteo@gmail.com                                           |
//                                                                              |
// Description:                                                                 |
//      Macro to study direction match efficiency with photon corrections.      |
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

void DMGamma()
{
    // Initial setup;
    const Bool_t kVerbose = true;
    Int_t selectedRun = -1;
    Int_t selectedEvent = -1;
    MMUtils utils;
    //utils.SetRecType("unbiassed");
    
    // TChain
    //TChain * rec = utils.MakeTChain("/meg/home/brini_m/Git/offline/analyzer/distilled/dm21/rec411999.root");
    //TChain * rec = utils.MakeTChain(383500, 100);
    TChain * rec = utils.MakeTChainFromFile("/meg/home/brini_m/Documents/runlistCW.txt");

    Long_t nEntries = rec->GetEntries();
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
 
    // Histograms && Plots
    TH1F * hUVDist = new TH1F("hUVDist", "hUVDist", 100, 0, 0);
    TH2F * hTRGReco = new TH2F("hTRGReco", "hTRGReco", 256, -0.5, 255.5, 256, -0.5, 255.5);
    TH2F * hTRGXECFit = new TH2F("hTRGXECFit", "hTRGXECFit", 256, -0.5, 255.5, 256, -0.5, 255.5);
    TH2F * hUVDistW = new TH2F("hUVDistW", "hUVDistW", 100, 0, 0, 100, 0, 0);
    TH2F * hTimeStamp = new TH2F("hTimeStamp", "hTimeStamp", 128, -0.5, 127.5, 100, -0.65e-6, -0.5e-6); // Poststamp in time
    
    // TChain SETUP
    ROMETreeInfo    * InfoRV = 0;  
    MEGEventHeader  * EventHeaderRV = 0;
    MEGRecData      * RecDataRV = 0;
    MEGTRGXECOnline * TRGXECOnlineRV = 0;

    rec->SetBranchStatus("*", 0);
    rec->SetBranchStatus("Info.*", 1);
    rec->SetBranchStatus("eventheader.*");
    rec->SetBranchStatus("reco.*", 1);
    rec->SetBranchStatus("trgxeconline.*", 1);

    rec->SetBranchAddress("Info.", &InfoRV);
    rec->SetBranchAddress("eventheader.", &EventHeaderRV);
    rec->SetBranchAddress("reco.", &RecDataRV);
    rec->SetBranchAddress("trgxeconline.", &TRGXECOnlineRV);

    cout << "Tree ready!" << endl;

    // GAMMA LOOP -----------------------------------------
    for (Int_t iEntry = 0; iEntry < nEntries; ++iEntry)
    {
        rec->GetEntry(iEntry);
        if (iEntry%1000 == 0)
            cout << iEntry << " events finished" << endl;

        auto run = InfoRV->GetRunNumber();
        auto event = InfoRV->GetEventNumber();

        auto mask = EventHeaderRV->Getmask();
        if (mask != 10) continue;

        auto nGamma = RecDataRV->GetNGamma();
        if (nGamma != 1) continue;

        // Find UV from TRG data -------------------------- 
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

        // RecData analysis -------------------------------
        for (Int_t iGamma = 0; iGamma < nGamma; ++iGamma)
        {
            auto tdcPeakTime = TRGXECOnlineRV->GetTdcPeakTime();
            if (tdcPeakTime < 95 || tdcPeakTime > 105) continue;

            // Energy cut
            auto egamma = RecDataRV->GetEGammaAt(iGamma);
            auto eCWCut = egamma < 0.0165 || egamma > 0.018;
            auto eSignalCut = egamma < 0.050 || egamma > 0.054;
            if (eCWCut) // GeV
                continue;

            auto evstat = RecDataRV->GetEvstatGammaAt(iGamma);
            if (evstat != 0) continue; // no-pileup condition

            // Evaluate distance UV between TRG and Reco position
            auto recoUGamma = RecDataRV->GetUGammaAt(iGamma); // U-Reco position
            auto recoVGamma = RecDataRV->GetVGammaAt(iGamma); // V-Reco position
            auto recoWGamma = RecDataRV->GetWGammaAt(iGamma); // W-Reco position

            if (recoUGamma < uMin || recoUGamma > uMax) continue;
            if (recoVGamma < vMin || recoVGamma > vMax) continue;

            auto uDist = TMath::Abs(uvTRG.first - recoUGamma);
            auto vDist = TMath::Abs(uvTRG.second - recoVGamma);
            auto UVDist = TMath::Sqrt(uDist * uDist + vDist * vDist); // Distance UV

            // Get PatchId from RecData
            Int_t recoVID = 92-round((recoVGamma-vMin)*92/(vMax-vMin)); 
            Int_t recoUID = round((recoUGamma-uMin)*43/(uMax-uMin)); 
            auto * pmReco =(MEGXECPMRunHeader *)XECPMRunHeaderRA->At(recoUID + recoVID * 44);
            auto recoPatchId = GetPatch(pmReco); // Patch ID
           
            // Get Time from RecData 
            auto timeReco = RecDataRV->GetTGammaAt(iGamma);

            // Verbose and Histogram Fill
            if (kVerbose)
            {
                if (trgPatchId != recoPatchId && UVDist > 120)
                {
                    cout << "Run: " << run << endl;
                    cout << "Event: " << event << endl;
                    cout << "Gamma: " << iGamma << "/" << nGamma << endl;
                    cout << "reco[U/V]Gamma: " << recoUGamma << "/" << recoVGamma << endl;
                    cout << "reco[U/V]ID: " << recoUID << "/" << recoVID << endl;
                    cout << "reco[U/V]ID: " << recoUID << "/" << recoVID << endl;
                    cout << "[u/v]TRG: " << uvTRG.first << "/" << uvTRG.second << endl;
                    cout << "PATCH RECO-TRG: " << recoPatchId << "-" << trgPatchId << endl;
                    cout << "UVDist: " << UVDist << endl;
                    cout << "EGamma [MeV] = " << egamma * 1000 << endl << endl;
                }
            }

            hTRGReco->Fill(trgPatchId, recoPatchId);
            hUVDist->Fill(UVDist);
            hUVDistW->Fill(UVDist, recoWGamma);
            hTimeStamp->Fill(tdcPeakTime, timeReco);
        }
    }

    TCanvas * c1 = new TCanvas("c1", "c1", 1);
    hTRGReco->Draw("COLZ");

    TCanvas * c3 = new TCanvas("c3", "c3", 1);
    c3->SetLogy();
    hUVDist->SetTitle(";UV distance [cm];Occurencies");
    hUVDist->Draw();

    TCanvas * c4 = new TCanvas("c4", "c4", 1);
    hTimeStamp->SetTitle(";TdcPeakTime;TGamma");
    hTimeStamp->Draw("COLZ");

    TCanvas * c5 = new TCanvas("c5", "c5", 1);
    hUVDistW->Draw("COLZ");

}
