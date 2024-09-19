

void RADvsEFF()
{
    std::vector<std::string> radLabels = {"1R0", "1R25", "1R5", "1R75", "2R5"};
    std::string filePath = "/meg/home/brini_m/Git/MatteMEG/outfiles/";

    TGraph * gEff = new TGraph();
    TGraph * gEffw = new TGraph();


    Int_t i = 0;
    for (auto & rad : radLabels)
    {
        std::string modRad = rad;
        std::replace(modRad.begin(), modRad.end(), 'R', '.');
        Float_t radFloat = std::stof(modRad);

        TFile * file = new TFile(Form("%sdm21_full%s.root", filePath.data(), rad.data()));
        auto * hEPos = (TH1F *)file->Get("hEPos");
        auto * hEPosTRG = (TH1F *)file->Get("hEPosTRG");
        auto * hEPosTRGw = (TH1F *)file->Get("hEPosTRGw");
        cout << hEPosTRG->GetBinContent(10) << " " << hEPos->GetBinContent(10) << endl;

        auto Nbins = hEPos->GetNbinsX();
        Double_t mean = 0;
        Double_t mean_w = 0;
        for (Int_t bin = Nbins - 3; bin < Nbins; ++bin)
        {
            mean += hEPosTRG->GetBinContent(bin) / hEPos->GetBinContent(bin);
            mean_w += hEPosTRGw->GetBinContent(bin) / hEPos->GetBinContent(bin);
        }

        gEff->AddPoint(radFloat, mean/3);
        gEffw->AddPoint(radFloat, mean_w/3);
        ++i; 
    }    
    
    gEff->SetMarkerSize(2);
    gEff->SetMarkerStyle(kCircle);
    gEff->SetMarkerColor(kRed); 
    gEffw->SetMarkerSize(2);
    gEffw->SetMarkerStyle(21);
    gEffw->SetMarkerColor(kBlue); 

    TMultiGraph * mg = new TMultiGraph();
    mg->Add(gEff, "AP");
    mg->Add(gEffw, "AP");

    mg->Draw("AP");
}
