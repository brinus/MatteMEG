//this macro plots the history of pedestal RMS

#define ADDERSEL 0

typedef std::pair<Int_t, Int_t> RANGE;

void testPedestal(){
    // ROOT SETTINGS
    gROOT->SetStyle("MEG_Pub");

    // VARIABLES
    std::vector<Double_t> y;
    std::vector<Double_t> x;
    std::vector<Double_t> yerr;
    std::vector<Double_t> xerr;

    std::map<Int_t, std::vector<RANGE>> RUN24;
    RUN24[0] = {};
    RUN24[1] = {};
    RUN24[2] = {};

    // Ranges for different DELAYS and ADDERSEL
    RUN24[0].push_back(RANGE{566839, 566870});
    RUN24[0].push_back(RANGE{566887, 566918});
    RUN24[1].push_back(RANGE{566919, 566950});
    RUN24[1].push_back(RANGE{566871, 566886});
    RUN24[1].push_back(RANGE{566951, 566966});

    Int_t delay = 0;

    for (auto & range : RUN24[ADDERSEL])
    {
        for (Int_t run = range.first; run <= range.second; ++run)
        {
            cout << " run: "<< run;
            TFile * f = new TFile(Form("/meg/home/brini_m/Documents/pedscan/rec%06d.root", run));
            TTree * rec = (TTree *)f->Get("rec");

            if (rec)
            {
                rec->Draw("QsumAmplitude >> h", "QsumAmplitude < 300e3", "goff");
                TH1F * h = (TH1F *)f->Get("h");

                if (h)
                {
                    y.push_back(h->GetMean());
                    yerr.push_back(h->GetRMS());
                    x.push_back(delay);
                    xerr.push_back(0.01);               
                }
            }
            cout << ", delay: " << delay << endl;
            ++delay;

            f->Close();
            delete f;
        }
    }

    TGraphErrors* g = new TGraphErrors(y.size(), x.data(), y.data(), xerr.data(), yerr.data());
    g->SetTitle("Pedestal scan addersel: 0;Delay;Mean");
    g->SetMarkerColor(kBlack);
    g->SetMarkerStyle(kCircle);
    g->Draw("AP");
}
