#include "../../include/RunHeader.hpp"

#include "TFile.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TF1.h"
#include "TLine.h"
#include "TText.h"

void CexMapPatch()
{
    gStyle->SetPadRightMargin(0.15);

    Double_t resolution1;
    Double_t resolution2;
    Double_t unif1;
    Double_t unif2;
    Double_t lRatio;
    Double_t prev_resolution1 = 1;
    Double_t prev_resolution2 = 1;
    Double_t prev_unif1 = 5e6;
    Double_t prev_unif2 = 5e6;
    Double_t prev_lRatio = 1e3;
    std::vector<std::string> *vPatchList;

    TFile *filePatch = new TFile("pi01115_1116.root", "UPDATE");
    filePatch->GetObject("patchlist", vPatchList);

    TH2D *hPatchMapRes1 = new TH2D("hPatchMapResolution1", "#sigma(E)/E@55MeV", 3, 0, 3, 8, 0, 8);
    TH2D *hPatchMapRes2 = new TH2D("hPatchMapResolution2", "#sigma(E)/E@83MeV", 3, 0, 3, 8, 0, 8);
    TH2D *hPatchMapUni1 = new TH2D("hPatchMapUniform1", "E@55MeV", 3, 0, 3, 8, 0, 8);
    TH2D *hPatchMapUni2 = new TH2D("hPatchMapUniform2", "E@83MeV", 3, 0, 3, 8, 0, 8);
    TH2D *hPatchLRatio = new TH2D("hPatchLineRatio", "(E@55MeV/E@83MeV) / (55MeV/83MeV)", 3, 0, 3, 8, 0, 8);

    hPatchMapRes1->GetXaxis()->SetTickSize(0);
    hPatchMapRes1->GetYaxis()->SetTickSize(0);
    hPatchMapRes1->GetXaxis()->SetLabelSize(0);
    hPatchMapRes1->GetYaxis()->SetLabelSize(0);
    hPatchMapRes2->GetXaxis()->SetTickSize(0);
    hPatchMapRes2->GetYaxis()->SetTickSize(0);
    hPatchMapRes2->GetXaxis()->SetLabelSize(0);
    hPatchMapRes2->GetYaxis()->SetLabelSize(0);
    hPatchMapUni1->GetXaxis()->SetTickSize(0);
    hPatchMapUni1->GetYaxis()->SetTickSize(0);
    hPatchMapUni1->GetXaxis()->SetLabelSize(0);
    hPatchMapUni1->GetYaxis()->SetLabelSize(0);
    hPatchMapUni2->GetXaxis()->SetTickSize(0);
    hPatchMapUni2->GetYaxis()->SetTickSize(0);
    hPatchMapUni2->GetXaxis()->SetLabelSize(0);
    hPatchMapUni2->GetYaxis()->SetLabelSize(0);
    hPatchLRatio->GetXaxis()->SetTickSize(0);
    hPatchLRatio->GetYaxis()->SetTickSize(0);
    hPatchLRatio->GetXaxis()->SetLabelSize(0);
    hPatchLRatio->GetYaxis()->SetLabelSize(0);

    TF1 *fPi0 = new TF1("fPi0", "((x>[1])?[0]* TMath::Exp(-0.5*(x-[1])* (x-[1])/([2]* [2])):[0]* TMath::Exp(-0.5*(x-[1])* (x-[1])/(([2]-[3]* (x-[1]))* ([2]-[3]* (x-[1])))))+((x>[5])?[4]* TMath::Exp(-0.5*(x-[5])* (x-[5])/([6]* [6])):[4]* TMath::Exp(-0.5*(x-[5])* (x-[5])/(([6]-[7]* (x-[5]))* ([6]-[7]*(x-[5])))))+[8]* (1.-TMath::Freq((x-[1])/[2]))+[9]*(1.-TMath::Freq((x-[5])/[6]))", 1.2e6, 3.2e6);

    fPi0->SetLineColor(kRed);

    for (const auto &histName : *vPatchList)
    {
        auto hist = (TH1 *)filePatch->Get(histName.c_str());

        // Fit configuration
        Double_t peakPos = hist->GetXaxis()->GetBinCenter(hist->GetMaximumBin());

        fPi0->SetParLimits(0, 0, 800);
        fPi0->SetParameter(1, peakPos);
        fPi0->SetParameter(2, 0.05e6);
        fPi0->SetParLimits(3, 0, 15);
        fPi0->SetParLimits(4, 0, 800);
        fPi0->SetParameter(5, peakPos * 1.5);
        fPi0->SetParameter(6, 0.05e6);
        fPi0->SetParLimits(7, 0, 15);
        fPi0->SetParLimits(7, 0, 15);
        fPi0->SetParLimits(8, 0, 500);
        fPi0->SetParLimits(9, 0, 500);

        std::cout << histName << std::endl;

        hist->Fit("fPi0", "LRSQ", "", 1.2e6, 3.2e6);
        TFitResultPtr r = hist->Fit("fPi0", "LRS", "", 1.2e6, 3.2e6);

        hist->SetName(("fit" + histName).c_str());
        hist->Write(hist->GetName(), 2);

        std::string idStrPatch = histName.substr(histName.size() - 2);
        Int_t idPatch = std::stoi(idStrPatch);

        Int_t xPos = (idPatch - 1) / 8 + 1;
        Int_t yPos = 8 - ((idPatch - 1) % 8);

        // Fit results
        std::cout << "Status: " << r->IsValid() << std::endl;
        std::cout << "Chi2/ndf = " << r->Chi2() << "/" << r->Ndf() << std::endl;

        // Resolution at 55 MeV
        resolution1 = abs(fPi0->GetParameter(2)) / fPi0->GetParameter(1);
        unif1 = fPi0->GetParameter(1);

        std::cout << std::setprecision(3) << "Sigma(E) / E [55 MeV] = " << resolution1 << std::endl;

        if (resolution1 < prev_resolution1)
        {
            prev_resolution1 = resolution1;
            hPatchMapRes1->SetMinimum(resolution1);
        }
        if (unif1 < prev_unif1)
        {
            prev_unif1 = unif1;
            hPatchMapUni1->SetMinimum(unif1);
        }

        hPatchMapRes1->SetBinContent(xPos, yPos, resolution1);
        hPatchMapUni1->SetBinContent(xPos, yPos, unif1);

        // Resolution at 83 MeV
        resolution2 = abs(fPi0->GetParameter(6)) / fPi0->GetParameter(5);
        unif2 = fPi0->GetParameter(5);

        std::cout << std::setprecision(3) << "Sigma(E) / E [83 MeV] = " << resolution2 << std::endl;
        std::cout << std::endl;

        if (resolution2 < prev_resolution2)
        {
            prev_resolution2 = resolution2;
            hPatchMapRes2->SetMinimum(resolution2);
        }
        if (unif2 < prev_unif2)
        {
            prev_unif2 = unif2;
            hPatchMapUni2->SetMinimum(unif2);
        }

        hPatchMapRes2->SetBinContent(xPos, yPos, resolution2);
        hPatchMapUni2->SetBinContent(xPos, yPos, unif2);

        // Lines Ratio
        lRatio = (unif1 / unif2) / (54.9 / 82.9);
        if (lRatio < prev_lRatio)
        {
            prev_lRatio = lRatio;
            hPatchLRatio->SetMinimum(lRatio);
        }

        hPatchLRatio->SetBinContent(xPos, yPos, lRatio);
    }

    hPatchMapRes1->Write(hPatchMapRes1->GetName(), 2);
    hPatchMapRes2->Write(hPatchMapRes2->GetName(), 2);
    hPatchMapUni1->Write(hPatchMapUni1->GetName(), 2);
    hPatchMapUni2->Write(hPatchMapUni2->GetName(), 2);

    TCanvas *c1 = new TCanvas("cPatchMap", "Patch Map", 800, 800);
    c1->SetRightMargin(0.01);
    c1->SetLeftMargin(0.01);
    c1->Divide(2, 2);

    c1->cd(1);
    hPatchMapRes1->Draw("COLZ");

    c1->cd(2);
    hPatchMapRes2->Draw("COLZ");

    c1->cd(3);
    hPatchMapUni1->Draw("COLZ");

    c1->cd(4);
    hPatchMapUni2->Draw("COLZ");

    for (int i = 0; i < 4; i++)
    {
        c1->cd(i + 1);
        gStyle->SetOptStat(0);
        gPad->SetGridx(0);
        gPad->SetGridy(0);
        for (int i = 1; i <= 7; i++)
        {
            TLine *line = new TLine(0, i, 3, i);
            line->Draw("SAME");
        }

        for (int i = 1; i <= 2; i++)
        {
            TLine *line = new TLine(i, 0, i, 8);
            line->Draw("SAME");
        }

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                Int_t id = (i * 8) + j + 1;
                TText *text = new TText(i + 0.5, 8 - j - 0.5, Form("%d", id));
                text->SetTextAlign(22);
                text->Draw("SAME");
            }
        }
    }

    TCanvas *c2 = new TCanvas("cRelativeLines", "Relative Lines", 1);
    c2->cd();
    hPatchLRatio->Draw("COLZ");

    gStyle->SetOptStat(0);
    gPad->SetGridx(0);
    gPad->SetGridy(0);
    for (int i = 1; i <= 7; i++)
    {
        TLine *line = new TLine(0, i, 3, i);
        line->Draw("SAME");
    }

    for (int i = 1; i <= 2; i++)
    {
        TLine *line = new TLine(i, 0, i, 8);
        line->Draw("SAME");
    }

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Int_t id = (i * 8) + j + 1;
            TText *text = new TText(i + 0.5, 8 - j - 0.5, Form("%d", id));
            text->SetTextAlign(22);
            text->Draw("SAME");
        }
    }
}