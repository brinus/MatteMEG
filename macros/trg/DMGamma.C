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
    const Bool_t kVerbose = false;
    Int_t selectedRun = -1;
    Int_t selectedEvent = -1;
    MMUtils utils;
    
    // TChain
    //TChain * rec = utils.MakeTChain("/meg/home/brini_m/Git/offline/analyzer/distilled/dm21");
    //TChain * rec = utils.MakeTChain(383500, 100);
    TChain * rec = utils.MakeTChainFromFile("/meg/home/brini_m/Documents/CWrunlist.txt");

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
    TGraph * gXECTRG = new TGraph();
    TGraph * gXECReco = new TGraph();
    TMultiGraph * mgXEC = new TMultiGraph();

    // TChain SETUP
    ROMETreeInfo    * InfoRV = 0;  
    MEGEventHeader  * EventHeaderRV = 0;
    MEGRecData      * RecDataRV = 0;
    MEGTRGXECOnline * TRGXECOnlineRV = 0;
    //TClonesArray    * XECPLFResRA = 0;

    rec->SetBranchStatus("*", 0);
    rec->SetBranchStatus("Info.*", 1);
    rec->SetBranchStatus("eventheader.*");
    rec->SetBranchStatus("reco.*", 1);
    rec->SetBranchStatus("trgxeconline.*", 1);
    //rec->SetBranchStatus("xecposlfit*", 1);

    rec->SetBranchAddress("Info.", &InfoRV);
    rec->SetBranchAddress("eventheader.", &EventHeaderRV);
    rec->SetBranchAddress("reco.", &RecDataRV);
    rec->SetBranchAddress("trgxeconline.", &TRGXECOnlineRV);
    //rec->SetBranchAddress("xecposlfit", &XECPLFResRA);

    for (Int_t iEntry = 0; iEntry < nEntries; ++iEntry)
    {
        rec->GetEntry(iEntry);
        
        if (iEntry%1000 == 0)
            cout << iEntry << "finished\r" << flush;

        auto run = InfoRV->GetRunNumber();
        auto event = InfoRV->GetEventNumber();

        auto mask = EventHeaderRV->Getmask();
        if (mask != 10) continue;

        auto nGamma = RecDataRV->GetNGamma();
        if (nGamma != 1) continue;
        
        auto trgPatchId = TRGXECOnlineRV->GetPatchId();
        auto uTRG = ((MEGXECPMRunHeader *)XECPMRunHeaderRA->At(trgPatchId))->GetUVWAt(0);
        auto vTRG = ((MEGXECPMRunHeader *)XECPMRunHeaderRA->At(trgPatchId))->GetUVWAt(1);
        if (run == selectedRun && event == selectedEvent)
            gXECTRG->AddPoint(uTRG, vTRG);

        for (Int_t iGamma = 0; iGamma < nGamma; ++iGamma)
        {

            auto tdcPeakTime = TRGXECOnlineRV->GetTdcPeakTime();
            if (tdcPeakTime < 95 || tdcPeakTime > 105) continue;

            auto egamma = RecDataRV->GetEGammaAt(iGamma);
            if (egamma < 0.04) // GeV
                continue;

            auto evstat = RecDataRV->GetEvstatGammaAt(iGamma);
            if (evstat != 0) continue;

            auto recoUGamma = RecDataRV->GetUGammaAt(iGamma);
            auto recoVGamma = RecDataRV->GetVGammaAt(iGamma);

            if (TMath::Abs(recoUGamma) > 100) continue;
            if (TMath::Abs(recoVGamma) > 100) continue;

            Double_t uDist = TMath::Abs(uTRG - recoUGamma);
            Double_t vDist = TMath::Abs(vTRG - recoVGamma);

            //auto xecfitUGamma = ((MEGXECPosLocalFitResult *)XECPLFResRA->At(iGamma))->GetuvwAt(0);
            //auto xecfitVGamma = ((MEGXECPosLocalFitResult *)XECPLFResRA->At(iGamma))->GetuvwAt(1);
            
            Int_t recoVID = 92-round((recoVGamma-vMin)*92/(vMax-vMin)); 
            Int_t recoUID = round((recoUGamma-uMin)*43/(uMax-uMin)); 
            //Int_t xecfitVID = 92-round((xecfitVGamma-vMin)*92/(vMax-vMin)); 
            //Int_t xecfitUID = round((xecfitUGamma-uMin)*43/(uMax-uMin)); 

            if(recoVID <0 || recoVID > 92) continue;
            if(recoUID <0 || recoUID > 44) continue;

            MEGXECPMRunHeader * pmReco =(MEGXECPMRunHeader *)XECPMRunHeaderRA->At(recoUID + recoVID * 44);
            //MEGXECPMRunHeader * pmXECFit =(MEGXECPMRunHeader *)XECPMRunHeaderRA->At(xecfitUID + xecfitVID * 44);
            Int_t recoPatchId = GetPatch(pmReco);
            //Int_t xecfitPatchId = GetPatch(pmXECFit);
            
            if (kVerbose)
            {
                if (trgPatchId != recoPatchId)
                {
                    cout << "Run: " << run << endl;
                    cout << "Event: " << event << endl;
                    cout << "Gamma: " << iGamma << "/" << nGamma << endl;
                    cout << "UVReco: " << recoUGamma << " " << recoVGamma << endl;
                    cout << "PATCH RECO-TRG: " << recoPatchId << "-" << trgPatchId << endl;
                    cout << "EGamma [MeV] = " << egamma * 1000 << endl << endl;
                }
            }

            hTRGReco->Fill(trgPatchId, recoPatchId);
            //hTRGXECFit->Fill(trgPatchId, xecfitPatchId);
            hUVDist->Fill(TMath::Sqrt(uDist * uDist + vDist * vDist));
            if (run == selectedRun && event == selectedEvent)
                gXECReco->AddPoint(recoUGamma, recoVGamma);
        }
    }

    TCanvas * c1 = new TCanvas("c1", "c1", 1);
    hTRGReco->Draw("COLZ");

    //TCanvas * c2 = new TCanvas("c2", "c2", 1);
    //hTRGXECFit->Draw("COLZ");

    TCanvas * c3 = new TCanvas("c3", "c3", 1);
    hUVDist->Draw();

    if (selectedRun != -1 && selectedEvent != -1)
    {
        TCanvas * c4 = new TCanvas("c4", "c4", 1);
        gXECTRG->SetMarkerStyle(8);
        gXECTRG->SetMarkerColor(kRed);
        gXECTRG->SetMarkerSize(2.5);
        gXECReco->SetMarkerStyle(8);
        gXECReco->SetMarkerColor(kBlue);
        gXECReco->SetMarkerSize(2.5);
        mgXEC->Add(gXECTRG, "AP");
        mgXEC->Add(gXECReco, "AP");
        mgXEC->GetHistogram()->GetXaxis()->SetRangeUser(uMin, uMax);
        mgXEC->GetHistogram()->GetXaxis()->SetLimits(uMin, uMax);
        mgXEC->GetHistogram()->GetYaxis()->SetRangeUser(vMin, vMax);
        mgXEC->GetHistogram()->GetYaxis()->SetLimits(vMin, vMax);
        mgXEC->GetHistogram()->SetTitle("XEC inner face;u [cm];v [cm]");
        mgXEC->Draw("AP");
    }
}
