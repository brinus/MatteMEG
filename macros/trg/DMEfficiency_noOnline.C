// --- DMEfficiency.C ----------------------------------------------------------o
//                                                                              |
// Version:     0.4                                                             |
// Author:      Matteo Brini                                                    |
// Date:        16/04/2024                                                      |
// E-mail:      brinimatteo@gmail.com                                           |
//                                                                              |
// Description:                                                                 |
//      Computation of trigger efficiency for direction match. This version     |
//      implements the computation of efficiency based on two discriminants:    |
//      the former is the entry of the DM table given by the offline            | 
//      reconstruction of the pixel ID, the latter is the logic OR between all  |
//      the TRG pixel ID in the wanted clock time (as function of runnumber).   |
//      Effects from TRG position detection on LXe are also considered in this  |
//      version.                                                                |
//      The user can set two flags: isROOTFileOn and isPlotOn. The defalut      | 
//      values are respectively 0 and 1.                                        |
//                                                                              |
//------------------------------------------------------------------------------o

#include "../../include/MMUtils.h"
R__ADD_LIBRARY_PATH(/meg/home/brini_m/Git/MatteMEG/lib)
R__LOAD_LIBRARY(libMMUtils.so)

Bool_t DMTable[256][512];

Int_t GetPatch(MEGXECPMRunHeader *);
Bool_t isCoarseValid(Int_t, Int_t);
Bool_t isHitValid(Int_t, Float_t);
Bool_t isOrTRG(Int_t, vector<Int_t> &);
Bool_t isDoubleTurn(Double_t z, Double_t theta);
void loadDMTable(const char *);

void DMEfficiency_noOnline(Bool_t isROOTFileOn = false, Bool_t isPlotOn = true)
{
    // Initial setup
    MMUtils utils;
    const Bool_t kVerbose = false;
    MEGPhysicsSelection physSelect;
    loadDMTable("/meg/home/francesconi_m/git/online/scripts/trigger/dmnarrow.mem");
    //loadDMTable("/meg/home/francesconi_m/git/online/scripts/trigger/dmwide.mem");
    //loadDMTable("/meg/home/francesconi_m/git/offline/analyzer/dmdummy.mem");
    //loadDMTable("/meg/home/francesconi_m/git/offline/analyzer/test.mem");
    //loadDMTable("/meg/home/brini_m/Git/MatteMEG/outfiles/full2R5sm.mem");

    physSelect.SetThresholds(EBeamPeriodID::kBeamPeriod2022, kTRUE);

    // TChain
    //TChain * rec = utils.MakeTChain("/meg/home/brini_m/Git/offline/analyzer/distilled/dm22_unbiassed/rec*.root");
    TChain * rec = utils.MakeTChain("/meg/data1/shared/subprojects/trg/eventi/positroni_35/ana/rec*.root");

    Long_t nEntries = rec->GetEntries();
    cout << "Got " << nEntries << " entries" << endl;

    // XECPMRunHeader
    TFile * file = new TFile("/meg/home/brini_m/Git/offline/analyzer/distilled/dm22_unbiassed/rec442000.root", "READ");
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

    // TChain SETUP
    ROMETreeInfo    * InfoRV = 0;
    MEGEventHeader  * EventHeaderRV = 0;
    MEGRecData      * RecDataRV = 0;
    TClonesArray    * SPXHitsRA = 0;
    TClonesArray    * GLBPositronRA = 0;

    rec->SetBranchStatus("*", 0);
    rec->SetBranchStatus("Info.*", 1);
    rec->SetBranchStatus("eventheader.*", 1);
    rec->SetBranchStatus("reco.*", 1);
    rec->SetBranchStatus("spxhits*", 1);
    rec->SetBranchStatus("positron*", 1);

    rec->SetBranchAddress("Info.", &InfoRV);
    rec->SetBranchAddress("eventheader.", &EventHeaderRV);
    rec->SetBranchAddress("reco.", &RecDataRV);
    rec->SetBranchAddress("spxhits", &SPXHitsRA);
    rec->SetBranchAddress("positron", &GLBPositronRA);

    // ROOT Histogams
    TH1F * hEPosReco = new TH1F("hEPosReco", "", 120, 40, 56);
    TH1F * hEPosTRGw = new TH1F("hEPosTRGw", "", 120, 40, 56);
    TH1F * hEPosTRG  = new TH1F("hEPosTRG", "", 120, 40, 56);
    TH1F * hEPos     = new TH1F("hEPos", "", 120, 40, 56);

    TH2F * hPixelReco = new TH2F("hPixelReco", "hPixelReco", 16, -0.5, 15.5, 32, -0.5, 31.5);
    TH2F * hPixelTRG  = new TH2F("hPixelTRG", "hPixelTRG", 16, -0.5, 15.5, 32, -0.5, 31.5);
    TH2F * hPixelRecoGood = new TH2F("hPixelRecoGood", "hPixelRecoGood", 16, -0.5, 15.5, 32, -0.5, 31.5);
    TH2F * hPixelTRGGood  = new TH2F("hPixelTRGGood", "hPixelTRGGood", 16, -0.5, 15.5, 32, -0.5, 31.5);

    TH2F * hTarget     = new TH2F("hTarget", "hTarget", 20, -5, 5, 20, -5, 5);
    TH2F * hTargetReco = new TH2F("hTargetReco", "hTargetReco", 20, -5, 5, 20, -5, 5);
    TH2F * hTargetTRG  = new TH2F("hTargetTRG", "hTargetTRG", 20, -5, 5, 20, -5, 5);

    TH2F * hZY     = new TH2F("hZY", "hZY", 80, -20, 20, 20, -5, 5);
    TH2F * hZYReco = new TH2F("hZYReco", "hZYReco", 80, -20, 20, 20, -5, 5);
    TH2F * hZYTRG  = new TH2F("hZYTRG", "hZYTRG", 80, -20, 20, 20, -5, 5);

    TH2F * hZTheta = new TH2F("hZTheta", "hZTheta", 80, -20, 20, 100, 60, 120);
    TH2F * hZThetaReco = new TH2F("hZThetaReco", "hZThetaReco", 80, -20, 20, 100, 60, 120);
    TH2F * hZThetaTRG = new TH2F("hZThetaTRG", "hZThetaTRG", 80, -20, 20, 100, 60, 120);

    TH1F * hNHits     = new TH1F("hNHits", "hNHits", 20, -0.5, 19.5);
    TH1F * hNHitsReco = new TH1F("hNHitsReco", "hNHitsReco", 20, -0.5, 19.5);
    TH1F * hNHitsTRG  = new TH1F("hNHitsTRG", "hNHitsTRG", 20, -0.5, 19.5);
    
    TH1F * hEPosHits     = new TH1F("hEPosHits", "", 120, 40, 56);
    TH1F * hEPosHitsReco = new TH1F("hEPosHitsReco", "", 120, 40, 56);
    TH1F * hEPosHitsTRG  = new TH1F("hEPosHitsTRG", "", 120, 40, 56);

    TH1F * hTheta     = new TH1F("hTheta", "hTheta", 100, 60, 120);
    TH1F * hThetaReco = new TH1F("hThetaReco", "hThetaReco", 100, 60, 120);
    TH1F * hThetaTRG  = new TH1F("hThetaTRG", "hThetaTRG", 100, 60, 120);

    TH1F * hPhi = new TH1F("hPhi", "hPhi", 100, -70, 70);
    TH1F * hPhiReco = new TH1F("hPhiReco", "hPhiReco", 100, -70, 70);
    TH1F * hPhiTRG = new TH1F("hPhiTRG", "hPhiTRG", 100, -70, 70);

    // TChain LOOP
    for (Int_t iEntry = 0; iEntry < nEntries; ++iEntry)
    {
        rec->GetEntry(iEntry);
        if (iEntry%1000 == 0)
            cout << iEntry << " events finished\r" << flush;
        
        // RecData LOOP
        auto nPositrons = RecDataRV->GetNPositron();
        for (Int_t iPositron = 0; iPositron < nPositrons; iPositron++) 
        {
            MEGGLBPositron * positron =  (MEGGLBPositron *)GLBPositronRA->At(RecDataRV->GetPositronIndexAt(iPositron));

            // RecData positron pixelId
            Int_t pixelIdReco = positron->GetMatchedPixelID();

            // Positron initial values infos
            Double_t xpos, ypos, zpos, phipos, thetapos, epos;
            xpos = RecDataRV->GetXPositronAt(iPositron);
            ypos = RecDataRV->GetYPositronAt(iPositron);
            zpos = RecDataRV->GetZPositronAt(iPositron);
            phipos = RecDataRV->GetPhiPositronAt(iPositron);
            thetapos = RecDataRV->GetThetaPositronAt(iPositron);
            epos = RecDataRV->GetEPositronAt(iPositron);

            // Quality cuts -------------------------------------
            bool ePosCut = epos < 0.040 || epos > 0.053;
            bool ePosCutFine = epos < 0.05265 || epos > 0.05295;
            bool posXECAcceptance = positron->GetXECAcceptance();
            bool posNGoodhits = positron->Getngoodhits() > 45;
            bool isInRadius = xpos * xpos + ypos * ypos < 1;

            if (ePosCut || !posXECAcceptance)
                continue;
            // -------------------------------------------------- 

            // Gamma patch position reconstruction
            Double_t gammatheta = 180. - thetapos;
            Double_t gammaphi = phipos + 180.;

            Double_t alpha = TMath::Sin(gammatheta*TMath::DegToRad()) * TMath::Cos(gammaphi*TMath::DegToRad());  
            Double_t beta = TMath::Sin(gammatheta*TMath::DegToRad()) * TMath::Sin(gammaphi*TMath::DegToRad());
            Double_t gamma = TMath::Cos(gammatheta*TMath::DegToRad());

            Double_t tplus =  (-alpha*xpos - beta*ypos + TMath::Sqrt(LXeRadius*LXeRadius*(alpha*alpha + beta*beta) -1*(beta*xpos - alpha*ypos)*(beta*xpos - alpha*ypos)))/(alpha*alpha+beta*beta);
            Double_t tminus = (-alpha*xpos - beta*ypos - TMath::Sqrt(LXeRadius*LXeRadius*(alpha*alpha + beta*beta) -1*(beta*xpos - alpha*ypos)*(beta*xpos - alpha*ypos)))/(alpha*alpha+beta*beta);

            Double_t LXextemp[2], LXeytemp[2], LXeztemp[2];
            Double_t LXex, LXey, LXez;

            LXextemp[0] = xpos + alpha * tplus;
            LXextemp[1] = xpos + alpha * tminus;

            LXeytemp[0] = ypos + beta * tplus;
            LXeytemp[1] = ypos + beta * tminus;

            LXeztemp[0] = zpos + gamma * tplus;
            LXeztemp[1] = zpos + gamma * tminus;


            if(LXextemp[0]<LXextemp[1]) {
                LXex = LXextemp[0];
                LXey =  ypos + beta * tplus;
                LXez =  zpos + gamma * tplus;
            }
            else {
                LXex = LXextemp[1];
                LXey =  ypos + beta * tminus;
                LXez =  zpos + gamma * tminus;
            }

            Double_t ugamma = LXez;
            Double_t vgamma = (TMath::ATan2(LXey, -LXex))*LXeRadius;

            Int_t vID = 92-round((vgamma-vMin)*92/(vMax-vMin)); 
            Int_t uID = round((ugamma-uMin)*43/(uMax-uMin)); 

            if(vID <0 || vID > 92) 
                continue;
            if(uID <0 || uID > 44) 
                continue;

            MEGXECPMRunHeader * pmrunh =(MEGXECPMRunHeader *)XECPMRunHeaderRA->At(uID + vID * 44);
            Int_t patchId = GetPatch(pmrunh);

            // Quality checks
            bool goodPatchId = patchId >= 0 && patchId < 256;
            bool goodPixelId = pixelIdReco >= 0 && pixelIdReco < 512;
            bool uAcc = ugamma > physSelect.fUGamma[0] && ugamma < physSelect.fUGamma[1];
            bool vAcc = vgamma > physSelect.fVGamma[0] && vgamma < physSelect.fVGamma[1];
            bool goodDTurn = isDoubleTurn(zpos, thetapos);

            // Filling histograms
            if(goodPatchId && goodPixelId && uAcc & vAcc)
            {
                hEPos->Fill(epos * 1000);
                if (!ePosCutFine)
                {
                    hTarget->Fill(xpos, ypos);
                    hZY->Fill(zpos, ypos);
                    hZTheta->Fill(zpos, thetapos);
                    hPixelReco->Fill(pixelIdReco % 16, pixelIdReco / 16);
                    hTheta->Fill(thetapos);
                    hPhi->Fill(phipos);
                }

                if (kVerbose)
                {
                    cout << "--------------------------------" << endl;
                    cout << "Positron " << 1 + RecDataRV->GetPositronIndexAt(iPositron) << "/" << GLBPositronRA->GetEntries() << endl;
                    cout << "E_Pos [MeV]: " << epos * 1000 << endl;
                    cout << "Patch : " << setw(3) << patchId << endl;
                    cout << "Pixel : " << setw(3) << pixelIdReco << endl;
                    cout << "--------------------------------" << endl;
                }

                if (DMTable[patchId][pixelIdReco])
                {
                    hEPosReco->Fill(epos * 1000);
                    if (!ePosCutFine)
                    {
                        hTargetReco->Fill(xpos, ypos);
                        hZYReco->Fill(zpos, ypos);
                        hZThetaReco->Fill(zpos, thetapos);
                        hPixelRecoGood->Fill(pixelIdReco % 16, pixelIdReco / 16);
                        hThetaReco->Fill(thetapos);
                        hPhiReco->Fill(phipos);
                    }
                }
            }
        }
    }
    cout << endl;

    // Plots
    if (isROOTFileOn)
    {
        const Char_t * outPath = "/meg/home/brini_m/Git/MatteMEG/outfiles/bar_narrow.root";
        cout << "Writing to " << outPath << endl;
        TFile outFile(outPath, "RECREATE");  
        hEPos->Write();
        hEPosReco->Write();
        
        // Target
        hTarget->Write();
        hTargetReco->Write();

        // Z vs Y
        hZY->Write();
        hZYReco->Write();

        // Z vs Theta
        hZTheta->Write();
        hZThetaReco->Write();

        // Pixel
        hPixelReco->Write();
        hPixelRecoGood->Write();

        // nHits
        hNHits->Write();
        hNHitsReco->Write();

        // EPosHits
        hEPosHits->Write();
        hEPosHitsReco->Write();
    
        // Theta
        hTheta->Write();
        hThetaReco->Write();

        // Phi
        hPhi->Write();
        hPhiReco->Write();
    }

    if (!isPlotOn)
        return;
    
    // Efficiency Plot
    TCanvas * cEff = new TCanvas("cEff", "cEff", 1);
    cEff->Divide(1, 2);

    cEff->cd(1);
    hEPos->GetXaxis()->SetTitle("Positron Energy [MeV]");
    hEPos->GetYaxis()->SetTitle("Events");
    hEPos->Draw();
    hEPosReco->SetLineColor(kRed);
    hEPosReco->Draw("SAME");

    cEff->cd(2); 
    TEfficiency *effReco = new TEfficiency(*hEPosReco, *hEPos);

    effReco->SetTitle(";;Efficiency");

    effReco->SetLineColor(kBlue);
    effReco->Draw();

    // Target Plot
    TCanvas * cTarget = new TCanvas("cTarget", "cTarget", 1000, 500);

    TH2F * hTargetRecoRatio = new TH2F(*hTargetReco);
    hTargetRecoRatio->Divide(hTargetReco, hTarget);

    gPad->SetRightMargin(0.15);
    hTargetRecoRatio->Draw("COLZ");

    // Z vs Y
    TCanvas * cZY = new TCanvas("cZY", "cZY", 1000, 500);

    TH2F * hZYRecoRatio = new TH2F(*hZYReco);
    hZYRecoRatio->Divide(hZYReco, hZY);

    gPad->SetRightMargin(0.15);
    hZYRecoRatio->Draw("COLZ");

    // Z vs Theta
    TCanvas * cZTheta = new TCanvas("cZTheta", "cZTheta");

    TH2F * hZThetaRecoRatio = new TH2F(*hZThetaReco);
    hZThetaRecoRatio->Divide(hZThetaReco, hZTheta);

    gPad->SetRightMargin(0.15);
    hZThetaRecoRatio->Draw("COLZ");


    // Pixel Plot
    TCanvas * cPixel = new TCanvas("cPixel", "cPixel", 1000, 500);

    TH2F * hPixelRecoRatio = new TH2F(*hPixelRecoGood);
    hPixelRecoRatio->Divide(hPixelRecoGood, hPixelReco);

    gPad->SetRightMargin(0.15);
    hPixelRecoRatio->Draw("COLZ");

    // NHits
    TCanvas * cNHits = new TCanvas("cNHits", "cNHits", 1);
    cNHits->Divide(1, 2);

    cNHits->cd(1);
    hNHits->Draw();
    hNHitsReco->SetLineColor(kRed);
    hNHitsReco->Draw("SAME");

    cNHits->cd(2);
    TEfficiency * effNHitsReco = new TEfficiency(*hNHitsReco, *hNHits);
    effNHitsReco->SetLineColor(kRed);
    effNHitsReco->Draw();

    // EPos for nHits > 1
    TCanvas * cEPosHits = new TCanvas("cEPosHits", "cEPosHits", 1);
    cEPosHits->Divide(1, 2);

    cEPosHits->cd(1);
    hEPosHits->Draw();
    hEPosHitsReco->SetLineColor(kRed);
    hEPosHitsReco->Draw("SAME");

    cEPosHits->cd(2);
    TEfficiency * effEPosHitsReco = new TEfficiency(*hEPosHitsReco, *hEPosHits);
    effEPosHitsReco->SetLineColor(kRed);
    effEPosHitsReco->Draw();

    // Theta
    TCanvas * cTheta = new TCanvas("cTheta", "cTheta", 1);
    cTheta->Divide(1, 2);

    cTheta->cd(1);
    hTheta->SetLineColor(8);
    hTheta->SetLineWidth(3);
    hTheta->Draw();
    hThetaReco->SetLineColor(kBlack);
    hThetaReco->SetLineWidth(2);
    hThetaReco->Draw("SAME");

    cTheta->cd(2);
    TEfficiency * effThetaReco = new TEfficiency(*hThetaReco, *hTheta);
    effThetaReco->SetLineColor(kBlack);
    effThetaReco->Draw();

    // Phi
    TCanvas * cPhi = new TCanvas("cPhi", "cPhi", 1);
    cPhi->Divide(1, 2);

    cPhi->cd(1);
    hPhi->SetLineColor(8);
    hPhi->SetLineWidth(3);
    hPhi->Draw();
    hPhiReco->SetLineColor(kBlack);
    hPhiReco->SetLineWidth(2);
    hPhiReco->Draw("SAME");

    cPhi->cd(2);
    TEfficiency * effPhiReco = new TEfficiency(*hPhiReco, *hPhi);
    effPhiReco->SetLineColor(kBlack);
    effPhiReco->Draw();
}

Int_t GetPatch(MEGXECPMRunHeader * pm)
{
    Int_t chip = pm->GetDRSChipID();
    return (chip - 64) / 2;
}

Bool_t isCoarseValid(Int_t runnum, Int_t coarse)
{
    if (runnum < 393800)
        return coarse == 114 || coarse == 115;
    else if (runnum < 393900)
        return coarse == 97 || coarse == 98;
    else if (runnum < 427120)
        return coarse == 98 || coarse == 99;
    else return coarse == 100 || coarse == 0; // TO BE CHECKED
}

Bool_t isHitValid(Int_t runnum, Float_t hit)
{
    if (runnum < 393800)
        return hit >= -0.44e-6 && hit <= -0.41e-6;
    else if (runnum < 427120)
        return hit >= -0.65e-6 && hit <= -0.61e-6;
    else return hit >= -0.623e-6 && hit <= -0.608e-6; // TO BE CHECKED
}

Bool_t isOrTRG(Int_t patchId, vector<Int_t> & vecpixelIdTRG)
{
    for (auto pixelIdTRG : vecpixelIdTRG)
    {
        if (DMTable[patchId][pixelIdTRG])
            return 1;
    }
    return 0;
}

Bool_t isDoubleTurn(Double_t z, Double_t theta)
{
    if (z + 95 < theta) return 1;
    if (z + 89 > theta) return 1;
    return 0;   
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
