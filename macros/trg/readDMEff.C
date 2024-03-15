// --- readDMEff.C ---
//

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
    hEPos->Draw();
    hEPosReco->SetLineColor(kRed);
    hEPosReco->Draw("SAME");
    hEPosTRG->SetLineColor(kBlue);
    hEPosTRG->Draw("SAME");

    c->cd(2); 
    TEfficiency *effReco = new TEfficiency(*hEPosReco, *hEPos);
    TEfficiency *effTRG  = new TEfficiency(*hEPosTRG, *hEPos);
   
    effReco->SetTitle(";;Efficiency");

    effReco->SetLineColor(kRed);
    effReco->Draw();
    effTRG->SetLineColor(kBlue);
    effTRG->Draw("SAME");
}
