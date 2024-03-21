// --- readDMEff.C -------------------------------------------------------------o
//                                                                              |
// Version:     0.3                                                             |
// Author:      Matteo Brini                                                    |
// Date:        19/03/2024                                                      |
// E-mail:      brinimatteo@gmail.com                                           |
//                                                                              |
// Description:                                                                 |    
//      Macro to read output file of macros/trg/DMEfficiency.C. This macro      |
//      generates the efficiency plots for direction match.                     |
//                                                                              |
// -----------------------------------------------------------------------------o

void readDMEff()
{
    //TFile * DMFile = new TFile("/meg/home/brini_m/Git/MatteMEG/outfiles/distill_dm21_wide001.root", "READ");
    TFile * DMFile = new TFile("/meg/home/brini_m/Git/MatteMEG/outfiles/distill_dm21_nar001.root", "READ");
    
    TH1F * hEPos = (TH1F *)DMFile->Get("hEPos");
    TH1F * hEPosReco = (TH1F *)DMFile->Get("hEPosReco");
    TH1F * hEPosTRG = (TH1F *)DMFile->Get("hEPosTRG");

    TH2F * hTarget = (TH2F *)DMFile->Get("hTarget");
    TH2F * hTargetReco = (TH2F *)DMFile->Get("hTargetReco");
    TH2F * hTargetTRG = (TH2F *)DMFile->Get("hTargetTRG");

    TH2F * hZY = (TH2F *)DMFile->Get("hZY");
    TH2F * hZYReco = (TH2F *)DMFile->Get("hZYReco");
    TH2F * hZYTRG = (TH2F *)DMFile->Get("hZYTRG");

    TH2F * hZTheta = (TH2F *)DMFile->Get("hZTheta");
    TH2F * hZThetaReco = (TH2F *)DMFile->Get("hZThetaReco");
    TH2F * hZThetaTRG = (TH2F *)DMFile->Get("hZThetaTRG");

    TH2F * hPixelReco = (TH2F *)DMFile->Get("hPixelReco");
    TH2F * hPixelRecoGood = (TH2F *)DMFile->Get("hPixelRecoGood");
    TH2F * hPixelTRG = (TH2F *)DMFile->Get("hPixelTRG");
    TH2F * hPixelTRGGood = (TH2F *)DMFile->Get("hPixelTRGGood");

    TH1F * hNHits = (TH1F *)DMFile->Get("hNHits");
    TH1F * hNHitsReco = (TH1F *)DMFile->Get("hNHitsReco");
    TH1F * hNHitsTRG = (TH1F *)DMFile->Get("hNHitsTRG");

    TH1F * hEPosHits = (TH1F *)DMFile->Get("hEPosHits");
    TH1F * hEPosHitsReco = (TH1F *)DMFile->Get("hEPosHitsReco");
    TH1F * hEPosHitsTRG = (TH1F *)DMFile->Get("hEPosHitsTRG");

    TH1F * hTheta = (TH1F *)DMFile->Get("hTheta");
    TH1F * hThetaReco = (TH1F *)DMFile->Get("hThetaReco");
    TH1F * hThetaTRG = (TH1F *)DMFile->Get("hThetaTRG");

    TH1F * hPhi = (TH1F *)DMFile->Get("hPhi");
    TH1F * hPhiReco = (TH1F *)DMFile->Get("hPhiReco");
    TH1F * hPhiTRG = (TH1F *)DMFile->Get("hPhiTRG");

    // Efficiency Plot
    TCanvas * cEff = new TCanvas("cEff", "cEff", 1);
    TLegend * legEff = new TLegend(0.15, 0.7, 0.25, 0.9, "", "NDC");
    cEff->Divide(1, 2);

    cEff->cd(1);
    hEPos->GetXaxis()->SetTitle("Positron Energy [MeV]");
    hEPos->GetYaxis()->SetTitle("Events");
    hEPos->SetLineColor(8);
    hEPos->SetLineWidth(2);
    hEPos->Draw();
    hEPosReco->SetLineColor(kBlack);
    hEPosReco->Draw("SAME");
    hEPosTRG->SetLineColor(kRed);
    hEPosTRG->Draw("SAME");

    legEff->AddEntry(hEPos, "e^{+}", "L");
    legEff->AddEntry(hEPosReco, "e^{+}_{reco}", "L");
    legEff->AddEntry(hEPosTRG, "e^{+}_{TRG}", "L");
    legEff->Draw();

    cEff->cd(2); 
    TEfficiency *effReco = new TEfficiency(*hEPosReco, *hEPos);
    TEfficiency *effTRG  = new TEfficiency(*hEPosTRG, *hEPos);

    effReco->SetTitle(";;Efficiency");

    effReco->SetLineColor(kBlack);
    effReco->Draw();
    effTRG->SetLineColor(kRed);
    effTRG->Draw("SAME");

    // Target Plot
    TCanvas * cTarget = new TCanvas("cTarget", "cTarget", 1000, 500);
    cTarget->Divide(2, 1);

    TH2F * hTargetRecoRatio = new TH2F(*hTargetReco);
    TH2F * hTargetTRGRatio = new TH2F(*hTargetTRG);
    hTargetRecoRatio->Divide(hTargetReco, hTarget);
    hTargetTRGRatio->Divide(hTargetTRG, hTarget);

    cTarget->cd(1);
    gPad->SetRightMargin(0.15);
    hTargetRecoRatio->Draw("COLZ");

    cTarget->cd(2);
    gPad->SetRightMargin(0.15);
    hTargetTRGRatio->Draw("COLZ");
    
    // Z vs Y
    TCanvas * cZY = new TCanvas("cZY", "cZY", 1000, 500);
    cZY->Divide(2, 1);

    TH2F * hZYRecoRatio = new TH2F(*hZYReco);
    TH2F * hZYTRGRatio = new TH2F(*hZYTRG);
    hZYRecoRatio->Divide(hZYReco, hZY);
    hZYTRGRatio->Divide(hZYTRG, hZY);

    cZY->cd(1);
    gPad->SetRightMargin(0.15);
    hZYRecoRatio->Draw("COLZ");

    cZY->cd(2);
    gPad->SetRightMargin(0.15);
    hZYTRGRatio->Draw("COLZ");

    // Z vs Theta
    TCanvas * cZTheta = new TCanvas("cZTheta", "cZTheta");
    cZTheta->Divide(2, 1);

    TH2F * hZThetaRecoRatio = new TH2F(*hZThetaReco);
    TH2F * hZThetaTRGRatio = new TH2F(*hZThetaTRG);
    hZThetaRecoRatio->Divide(hZThetaReco, hZTheta);
    hZThetaTRGRatio->Divide(hZThetaTRG, hZTheta);

    cZTheta->cd(1);
    gPad->SetRightMargin(0.15);
    hZThetaRecoRatio->Draw("COLZ");

    cZTheta->cd(2);
    gPad->SetRightMargin(0.15);
    hZThetaTRGRatio->Draw("COLZ");

    // Pixel Plot
    TCanvas * cPixel = new TCanvas("cPixel", "cPixel", 1000, 500);
    cPixel->Divide(2, 1);

    TH2F * hPixelRecoRatio = new TH2F(*hPixelRecoGood);
    TH2F * hPixelTRGRatio = new TH2F(*hPixelTRGGood);
    hPixelRecoRatio->Divide(hPixelRecoGood, hPixelReco);
    hPixelTRGRatio->Divide(hPixelTRGGood, hPixelTRG);

    cPixel->cd(1);
    gPad->SetRightMargin(0.15);
    hPixelRecoRatio->Draw("COLZ");

    cPixel->cd(2);
    gPad->SetRightMargin(0.15);
    hPixelTRGRatio->Draw("COLZ");
    
    // NHits
    TCanvas * cNHits = new TCanvas("cNHits", "cNHits", 1);
    TLegend * legNHits = new TLegend(0.15, 0.7, 0.25, 0.9, "", "NDC");
    cNHits->Divide(1, 2);

    cNHits->cd(1);
    hNHits->SetLineColor(8);
    hNHits->SetLineWidth(2);
    hNHits->Draw();
    hNHitsReco->SetLineColor(kBlack);
    hNHitsTRG->SetLineColor(kRed);
    hNHitsReco->Draw("SAME");
    hNHitsTRG->Draw("SAME");

    legNHits->AddEntry(hNHits, "e^{+}", "L");
    legNHits->AddEntry(hNHitsReco, "e^{+}_{reco}", "L");
    legNHits->AddEntry(hNHitsTRG, "e^{+}_{TRG}", "L");
    legNHits->Draw();
    
    cNHits->cd(2);
    TEfficiency * effNHitsReco = new TEfficiency(*hNHitsReco, *hNHits);
    TEfficiency * effNHitsTRG = new TEfficiency(*hNHitsTRG, *hNHits);
    effNHitsReco->SetLineColor(kBlack);
    effNHitsTRG->SetLineColor(kRed);
    effNHitsReco->Draw();
    effNHitsTRG->Draw("SAME");

    // EPos for nHits > 1
    TCanvas * cEPosHits = new TCanvas("cEPosHits", "cEPosHits", 1);
    TLegend * legHitsEff = new TLegend(0.15, 0.7, 0.25, 0.9, "", "NDC");
    cEPosHits->Divide(1, 2);

    cEPosHits->cd(1);
    hEPosHits->SetLineColor(8);
    hEPosHits->SetLineWidth(2);
    hEPosHits->Draw();
    hEPosHitsReco->SetLineColor(kBlack);
    hEPosHitsTRG->SetLineColor(kRed);
    hEPosHitsReco->Draw("SAME");
    hEPosHitsTRG->Draw("SAME");

    legHitsEff->AddEntry(hEPosHits, "e^{+}", "L");
    legHitsEff->AddEntry(hEPosHitsReco, "e^{+}_{reco}", "L");
    legHitsEff->AddEntry(hEPosHitsTRG, "e^{+}_{TRG}", "L");
    legHitsEff->Draw();
    
    cEPosHits->cd(2);
    TEfficiency * effEPosHitsReco = new TEfficiency(*hEPosHitsReco, *hEPosHits);
    TEfficiency * effEPosHitsTRG = new TEfficiency(*hEPosHitsTRG, *hEPosHits);
    effEPosHitsReco->SetLineColor(kBlack);
    effEPosHitsTRG->SetLineColor(kRed);
    effEPosHitsReco->Draw();
    effEPosHitsTRG->Draw("SAME");
    
    // Theta
    TCanvas * cTheta = new TCanvas("cTheta", "cTheta", 1);
    cTheta->Divide(1, 2);

    cTheta->cd(1);
    hTheta->SetLineColor(8);
    hTheta->SetLineWidth(3);
    hTheta->Draw();
    hThetaReco->SetLineColor(kBlack);
    hThetaReco->SetLineWidth(2);
    hThetaTRG->SetLineColor(kRed);
    hThetaTRG->SetLineWidth(2);
    hThetaReco->Draw("SAME");
    hThetaTRG->Draw("SAME");

    cTheta->cd(2);
    TEfficiency * effThetaReco = new TEfficiency(*hThetaReco, *hTheta);
    TEfficiency * effThetaTRG = new TEfficiency(*hThetaTRG, *hTheta);
    effThetaReco->SetLineColor(kBlack);
    effThetaReco->SetLineWidth(2);
    effThetaTRG->SetLineColor(kRed);
    effThetaTRG->SetLineWidth(2);
    effThetaReco->Draw();
    effThetaTRG->Draw("SAME");

    // Phi
    TCanvas * cPhi = new TCanvas("cPhi", "cPhi", 1);
    cPhi->Divide(1, 2);

    cPhi->cd(1);
    hPhi->SetLineColor(8);
    hPhi->SetLineWidth(3);
    hPhi->Draw();
    hPhiReco->SetLineColor(kBlack);
    hPhiReco->SetLineWidth(2);
    hPhiTRG->SetLineColor(kRed);
    hPhiTRG->SetLineWidth(2);
    hPhiReco->Draw("SAME");
    hPhiTRG->Draw("SAME");

    cPhi->cd(2);
    TEfficiency * effPhiReco = new TEfficiency(*hPhiReco, *hPhi);
    TEfficiency * effPhiTRG = new TEfficiency(*hPhiTRG, *hPhi);
    effPhiReco->SetLineColor(kBlack);
    effPhiReco->SetLineWidth(2);
    effPhiTRG->SetLineColor(kRed);
    effPhiTRG->SetLineWidth(2);
    effPhiReco->Draw();
    effPhiTRG->Draw("SAME");
}
