// --- readDMEff.C -------------------------------------------------------------o
//                                                                              |
// Version:     0.2                                                             |
// Author:      Matteo Brini                                                    |
// Date:        18/03/2024                                                      |
// E-mail:      brinimatteo@gmail.com                                           |
//                                                                              |    
// Macro to read output file of macros/trg/DMEfficiency.C. This macro generates |
// the efficiency plots for direction match.                                    |
//                                                                              |
// -----------------------------------------------------------------------------o

void readDMEff()
{
    TFile * DMFile = new TFile("/meg/home/brini_m/Git/MatteMEG/outfiles/outDMEfficiency_002.root", "READ");
    
    TH1F * hEPos = (TH1F *)DMFile->Get("hEPos");
    TH1F * hEPosReco = (TH1F *)DMFile->Get("hEPosReco");
    TH1F * hEPosTRG = (TH1F *)DMFile->Get("hEPosTRG");

    TH2F * hTarget = (TH2F *)DMFile->Get("hTarget");
    TH2F * hTargetReco = (TH2F *)DMFile->Get("hTargetReco");
    TH2F * hTargetTRG = (TH2F *)DMFile->Get("hTargetTRG");

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
}
