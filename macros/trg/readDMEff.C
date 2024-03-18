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
    TFile * DMFile = new TFile("/meg/home/brini_m/Git/MatteMEG/outfiles/outDMEfficiency.root", "READ");
    
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

    // Efficiency Plot
    TCanvas * cEff = new TCanvas("cEff", "cEff", 1);
    TLegend * legEff = new TLegend(0, 0.7, 0.2, 0.9, "", "NDC");
    cEff->Divide(1, 2);

    cEff->cd(1);
    hEPos->GetXaxis()->SetTitle("Positron Energy [MeV]");
    hEPos->GetYaxis()->SetTitle("Events");
    hEPos->Draw();
    hEPosReco->SetLineColor(kRed);
    hEPosReco->Draw("SAME");
    hEPosTRG->SetLineColor(kBlue);
    hEPosTRG->Draw("SAME");

    legEff->AddEntry(hEPos, "e^{+}", "L");
    legEff->AddEntry(hEPosReco, "e^{+}_{reco}", "L");
    legEff->AddEntry(hEPosTRG, "e^{+}_{TRG}", "L");
    legEff->Draw();

    cEff->cd(2); 
    TEfficiency *effReco = new TEfficiency(*hEPosReco, *hEPos);
    TEfficiency *effTRG  = new TEfficiency(*hEPosTRG, *hEPos);

    effReco->SetTitle(";;Efficiency");

    effReco->SetLineColor(kRed);
    effReco->Draw();
    effTRG->SetLineColor(kBlue);
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
}
