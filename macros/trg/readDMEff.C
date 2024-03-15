// --- readDMEff.C -------------------------------------------------------------o
//                                                                              |
// Version:     0.1                                                             |
// Author:      Matteo Brini                                                    |
// Date:        15/03/2024                                                      |
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

    TCanvas* c = new TCanvas();
    c->Divide(0,2);
    
    c->cd(1);
    hEPos->GetXaxis()->SetTitle("Positron Energy [MeV]");
    hEPos->GetYaxis()->SetTitle("Events");
    hEPos->GetXaxis()->SetRangeUser(40, 55);
    hEPos->Draw();
    hEPosReco->SetLineColor(kRed);
    hEPosReco->Draw("SAME");
    hEPosTRG->SetLineColor(kBlue);
    hEPosTRG->Draw("SAME");
    c->BuildLegend();

    c->cd(2);
    TEfficiency *effReco = new TEfficiency(*hEPosReco, *hEPos);
    TEfficiency *effTRG  = new TEfficiency(*hEPosTRG, *hEPos);
   
    effReco->SetTitle(";Positron Energy [MeV];Efficiency");
    effReco->SetLineColor(kRed);
    effReco->Draw();
    effTRG->SetLineColor(kBlue);
    effTRG->Draw("SAME");
}
