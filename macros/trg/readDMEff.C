// --- readDMEff.C -------------------------------------------------------------o
//                                                                              |
// Version:     0.4                                                             |
// Author:      Matteo Brini                                                    |
// Date:        21/03/2024                                                      |
// E-mail:      brinimatteo@gmail.com                                           |
//                                                                              |
// Description:                                                                 |    
//      Macro to read output file of macros/trg/DMEfficiency.C. This macro      |
//      generates the efficiency plots for direction match.                     |
//                                                                              |
// -----------------------------------------------------------------------------o

#include "TStyle.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TEfficiency.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"

void readDMEff()

{
    gStyle->SetLabelFont(22, "x"); // Set the font for the x-axis label
    gStyle->SetLabelFont(22, "y"); // Set the font for the y-axis label
    gStyle->SetLabelFont(22, "z"); // Set the font for the z-axis label
    gStyle->SetLabelSize(0.06, "x"); // Set the size of the x-axis label
    gStyle->SetLabelSize(0.06, "y"); // Set the size of the y-axis label
    gStyle->SetLabelSize(0.06, "z"); // Set the size of the z-axis label

    gStyle->SetTitleFont(22, "x"); // Set the font for the x-axis title
    gStyle->SetTitleFont(22, "y"); // Set the font for the y-axis title
    gStyle->SetTitleFont(22, "z"); // Set the font for the z-axis title
    gStyle->SetTitleSize(0.06, "x"); // Set the size of the x-axis title
    gStyle->SetTitleSize(0.06, "y"); // Set the size of the y-axis title
    gStyle->SetTitleSize(0.06, "z"); // Set the size of the z-axis title
    gStyle->SetTitleOffset(0.8, "x"); // Set the offset of the x-axis title
    gStyle->SetTitleOffset(0.8, "y"); // Set the offset of the y-axis title
    gStyle->SetTitleOffset(0.8, "z"); // Set the offset of the z-axis title

    gStyle->SetPadGridX(1);
    gStyle->SetPadGridY(1);

    gStyle->SetPadLeftMargin(0.15);
    gStyle->SetPadRightMargin(0.15);
    gStyle->SetPadTopMargin(0.1);
    gStyle->SetPadBottomMargin(0.15);

    gStyle->SetCanvasDefW(700); // Set the default width of the canvas
    gStyle->SetCanvasDefH(800); // Set the default height of the canvas

    gStyle->SetStatFont(22); // Set the font for the statbox
    gStyle->SetStatFontSize(0.06); // Set the font size for the statbox
    gStyle->SetStatX(0.9); // Set the X position of the statbox
    gStyle->SetStatY(0.9); // Set the Y position of the statbox
    gStyle->SetStatW(0.2); // Set the width of the statbox
    gStyle->SetStatH(0.15); // Set the height of the statbox

    TFile * DMFile = new TFile("/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_narrow_unb.root", "READ");
    
    TH1F * hEPos = (TH1F *)DMFile->Get("hEPos");
    TH1F * hEPosReco = (TH1F *)DMFile->Get("hEPosReco");
    TH1F * hEPosTRG = (TH1F *)DMFile->Get("hEPosTRG");
    TH1F * hEPosTRGw = (TH1F *)DMFile->Get("hEPosTRGw");
    TH1F * hEPosPrio = (TH1F *)DMFile->Get("hEPosPrio");

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
    hEPosTRGw->SetLineColor(kRed);
    hEPosTRGw->SetLineStyle(2);
    hEPosTRGw->Draw("SAME");
    if (hEPosPrio)
    {
        hEPosPrio->SetLineColor(kBlue);
        hEPosPrio->Draw("SAME");
    }

    legEff->AddEntry(hEPos, "e^{+}", "L");
    legEff->AddEntry(hEPosReco, "e^{+}_{reco}", "L");
    legEff->AddEntry(hEPosTRG, "e^{+}_{TRG}", "L");
    legEff->AddEntry(hEPosTRGw, "e^{+}_{TRGw}", "L");
    if (hEPosPrio)
        legEff->AddEntry(hEPosPrio, "e^{+}_{Prio}", "L");
    legEff->Draw();

    cEff->cd(2); 
    TEfficiency *effReco = new TEfficiency(*hEPosReco, *hEPos);
    TEfficiency *effTRG  = new TEfficiency(*hEPosTRG, *hEPos);
    TEfficiency *effTRGw  = new TEfficiency(*hEPosTRGw, *hEPos);
    TEfficiency * effPrio = 0;
    if (hEPosPrio)
        effPrio  = new TEfficiency(*hEPosPrio, *hEPos);

    effReco->SetTitle(";;Efficiency");

    effReco->SetLineColor(kBlack);
    effReco->Draw();
    effTRG->SetLineColor(kRed);
    effTRG->Draw("SAME");
    effTRGw->SetLineColor(kRed);
    effTRGw->SetLineStyle(2);
    effTRGw->Draw("SAME");
    if (hEPosPrio)
    {
        effPrio->SetLineColor(kBlue);
        effPrio->Draw("SAME");
    }

    // Target Plot
    TCanvas * cTarget = new TCanvas("cTarget", "cTarget", 1000, 500);
    cTarget->Divide(2, 1);

    TH2F * hTargetRecoRatio = new TH2F(*hTargetReco);
    TH2F * hTargetTRGRatio = new TH2F(*hTargetTRG);
    hTargetRecoRatio->SetTitle(";x [cm];y [cm]");
    hTargetTRGRatio->SetTitle(";x [cm];y [cm]");
    hTargetRecoRatio->Divide(hTargetReco, hTarget);
    hTargetTRGRatio->Divide(hTargetTRG, hTarget);

    cTarget->cd(1);
    hTargetRecoRatio->Draw("COLZ");

    cTarget->cd(2);
    hTargetTRGRatio->Draw("COLZ");
    
    // Z vs Y
    TCanvas * cZY = new TCanvas("cZY", "cZY", 1000, 500);
    cZY->Divide(2, 1);

    TH2F * hZYRecoRatio = new TH2F(*hZYReco);
    TH2F * hZYTRGRatio = new TH2F(*hZYTRG);
    hZYRecoRatio->SetTitle(";z [cm];y [cm]");
    hZYTRGRatio->SetTitle(";z [cm];y [cm]");
    hZYRecoRatio->Divide(hZYReco, hZY);
    hZYTRGRatio->Divide(hZYTRG, hZY);

    cZY->cd(1);
    hZYRecoRatio->Draw("COLZ");

    cZY->cd(2);
    hZYTRGRatio->Draw("COLZ");

    // Z vs Theta
    TCanvas * cZTheta = new TCanvas("cZTheta", "cZTheta");
    cZTheta->Divide(2, 1);

    TH2F * hZThetaRecoRatio = new TH2F(*hZThetaReco);
    TH2F * hZThetaTRGRatio = new TH2F(*hZThetaTRG);
    hZThetaRecoRatio->SetMinimum(-0.1);
    hZThetaRecoRatio->SetMaximum(1.1);
    hZThetaTRGRatio->SetMinimum(-0.1);
    hZThetaTRGRatio->SetMaximum(1.1);
    hZThetaRecoRatio->SetTitle(";z [cm];#theta [#circ]");
    hZThetaTRGRatio->SetTitle(";z [cm];#theta [#circ]");
    hZThetaRecoRatio->Divide(hZThetaReco, hZTheta);
    hZThetaTRGRatio->Divide(hZThetaTRG, hZTheta);

    cZTheta->cd(1);
    hZThetaRecoRatio->Draw("COLZ");

    cZTheta->cd(2);
    hZThetaTRGRatio->Draw("COLZ");

    // Pixel Plot
    TCanvas * cPixel = new TCanvas("cPixel", "cPixel", 1000, 500);
    cPixel->Divide(2, 1);

    TH2F * hPixelRecoRatio = new TH2F(*hPixelRecoGood);
    TH2F * hPixelTRGRatio = new TH2F(*hPixelTRGGood);
    hPixelRecoRatio->Divide(hPixelRecoGood, hPixelReco);
    hPixelTRGRatio->Divide(hPixelTRGGood, hPixelTRG);

    cPixel->cd(1);
    hPixelRecoRatio->Draw("COLZ");

    cPixel->cd(2);
    gPad->SetLeftMargin(0.15);
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
    TLegend * legTheta = new TLegend(0.15, 0.7, 0.25, 0.9, "", "NDC");
    cTheta->Divide(1, 2);

    cTheta->cd(1);
    hTheta->SetTitle(";#theta [#circ];Events");
    hTheta->SetLineColor(8);
    hTheta->SetLineWidth(3);
    hTheta->Draw();
    hThetaReco->SetLineColor(kBlack);
    hThetaReco->SetLineWidth(2);
    hThetaTRG->SetLineColor(kRed);
    hThetaTRG->SetLineWidth(2);
    hThetaReco->Draw("SAME");
    hThetaTRG->Draw("SAME");

    legTheta->AddEntry(hTheta, "e^{+}", "L");
    legTheta->AddEntry(hThetaReco, "e^{+}_{reco}", "L");
    legTheta->AddEntry(hThetaTRG, "e^{+}_{TRG}", "L");
    legTheta->Draw();

    cTheta->cd(2);
    TEfficiency * effThetaReco = new TEfficiency(*hThetaReco, *hTheta);
    TEfficiency * effThetaTRG = new TEfficiency(*hThetaTRG, *hTheta);
    effThetaReco->SetTitle(";#theta [#circ];Efficiency");
    effThetaReco->SetLineColor(kBlack);
    effThetaReco->SetLineWidth(2);
    effThetaTRG->SetLineColor(kRed);
    effThetaTRG->SetLineWidth(2);
    effThetaReco->Draw();
    effThetaTRG->Draw("SAME");

    // Phi
    TCanvas * cPhi = new TCanvas("cPhi", "cPhi", 1);
    TLegend * legPhi = new TLegend(0.15, 0.7, 0.25, 0.9, "", "NDC");
    cPhi->Divide(1, 2);

    cPhi->cd(1);
    hPhi->SetTitle(";#phi [#circ];Events");
    hPhi->SetLineColor(8);
    hPhi->SetLineWidth(3);
    hPhi->Draw();
    hPhiReco->SetLineColor(kBlack);
    hPhiReco->SetLineWidth(2);
    hPhiTRG->SetLineColor(kRed);
    hPhiTRG->SetLineWidth(2);
    hPhiReco->Draw("SAME");
    hPhiTRG->Draw("SAME");

    legPhi->AddEntry(hPhi, "e^{+}", "L");
    legPhi->AddEntry(hPhiReco, "e^{+}_{reco}", "L");
    legPhi->AddEntry(hPhiTRG, "e^{+}_{TRG}", "L");
    legPhi->Draw();

    cPhi->cd(2);
    TEfficiency * effPhiReco = new TEfficiency(*hPhiReco, *hPhi);
    TEfficiency * effPhiTRG = new TEfficiency(*hPhiTRG, *hPhi);
    effPhiReco->SetTitle(";#phi [#circ];Efficiency");
    effPhiReco->SetLineColor(kBlack);
    effPhiReco->SetLineWidth(2);
    effPhiTRG->SetLineColor(kRed);
    effPhiTRG->SetLineWidth(2);
    effPhiReco->Draw();
    effPhiTRG->Draw("SAME");

    // Finding the region in ZTheta for the cut
    TF1 * func1 = new TF1("f1", "pol1", -20, 20);
    func1->SetParameters(95, 1);
    func1->Draw("SAME");
    TF1 * func2 = new TF1("f2", "pol1", -20, 20);
    func2->SetParameters(89, 1);
    func2->Draw("SAME");
}
