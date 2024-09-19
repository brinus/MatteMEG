void loadDMTable(const char *);
void loadDMTable(const char *, Bool_t (& DMTable)[512][256]);

Bool_t DMTable[256][512];

void freqLUT()
{
    auto file = TFile::Open("/meg/home/brini_m/Git/MatteMEG/outfiles/pixelOcc.root", "READ");
    auto hPixel = (TH1F *)file->Get("hpixel");
    auto c1 = new TCanvas("c1", "c1", 1);
    auto hHits0 = new TH1F("hHits0", "hHits0", 100, 0, 0);
    auto hHits1 = new TH1F("hHits1", "hHits1", 100, 0, 0);
    
    Bool_t DMTable0[512][256];
    Bool_t DMTable1[512][256];
    
    loadDMTable("/meg/home/francesconi_m/git/online/scripts/trigger/dmnarrow.mem", DMTable0);
    loadDMTable("/meg/home/francesconi_m/git/offline/analyzer/dmdummy.mem", DMTable1);

    Float_t nHits0 = 0;
    Float_t nHitsInPatch0 = 0;
    Float_t nHits1 = 0;
    Float_t nHitsInPatch1 = 0;

    for (Int_t iPatch = 0; iPatch < 256; ++iPatch)
    {
        nHitsInPatch0 = 0;
        nHitsInPatch1 = 0;
        for (Int_t iTile = 0; iTile < 512; ++iTile)
        {
            if (DMTable0[iTile][iPatch])
            {
                auto currHits = hPixel->GetBinContent(iTile + 1);
                nHits0 += currHits;  
                nHitsInPatch0 += currHits;
            }
            if (DMTable1[iTile][iPatch])
            {
                auto currHits = hPixel->GetBinContent(iTile + 1);
                nHits1 += currHits;  
                nHitsInPatch1 += currHits;
            }
        }
        hHits0->Fill(nHitsInPatch0);
        hHits1->Fill(nHitsInPatch1);
    }
    cout << "nHits[0] = "<< nHits0 << endl;
    cout << "nHits[1] = "<< nHits1 << endl;
    cout << "Ratio = " << setprecision(3) << nHits1 / nHits0 << "\t\tIncrement = " << (nHits1 - nHits0)/nHits0 * 100 << "%" << endl;
    
    hHits1->SetLineColor(kRed);
    hHits0->Draw();
    hHits1->Draw("SAMES");

}

void loadDMTable(const char* path, Bool_t (& DMTable)[512][256])
{
    FILE *tabin = fopen(path,"r");
    Int_t data;

    for(Int_t iTile = 0; iTile<512; iTile++)
    {
        for(Int_t iPatch = 0; iPatch<256; iPatch++)
        {
            fscanf(tabin,"%d",&data);
            if(data)
                DMTable[iTile][iPatch] = true;
            else
                DMTable[iTile][iPatch] = false;
        }
    }
}
