// --- TileDist.C --------------------------------------------------------------o
//                                                                              |
// Version:     0.1                                                             |
// Author:      Matteo Brini                                                    |
// Date:        20/05/24                                                        |
// E-mail:      brinimatteo@gmail.com                                           |
//                                                                              |
// Description:                                                                 |
//      Macro to compare two direction match tables given the patch number.     | 
//                                                                              |
//------------------------------------------------------------------------------o

#include "../../include/MMUtils.h"
R__ADD_LIBRARY_PATH(/meg/home/brini_m/Git/MatteMEG/lib)
R__LOAD_LIBRARY(libMMUtils.so)

void loadDMTable(const char *, Bool_t (& DMTable)[512][256]);


void TileDist()
{
    Bool_t DMTable0[512][256];
    Bool_t DMTable1[512][256];
    //loadDMTable("/meg/home/francesconi_m/git/online/scripts/trigger/dmnarrow.mem", DMTable0);
    //loadDMTable("/meg/home/francesconi_m/git/offline/analyzer/test.mem", DMTable1);
    loadDMTable("/meg/home/brini_m/Git/MatteMEG/outfiles/full1R0.mem", DMTable0);
    loadDMTable("/meg/home/brini_m/Git/MatteMEG/outfiles/full1R0sm.mem", DMTable1);
    cout << "DMTables loaded" << endl;

    Int_t runnum = 391766; // Random 2021 physics run number
    TFile * file = new TFile(Form("/meg/data1/offline/run/%03dxxx/rec%06d.root", runnum/1000, runnum), "READ");
    TCanvas * cSPX = new TCanvas("cSPX", "cSPX", 1000, 500); 
    TVirtualPad * pad = cSPX->cd();
    
    MEGSPXRunHeader * spxRunHeader = (MEGSPXRunHeader*)file->Get("SPXRunHeader");
    TClonesArray * spxPixelRunHeader = (TClonesArray*)file->Get("SPXPixelRunHeader"); 
    if(spxRunHeader==nullptr || spxPixelRunHeader==nullptr){
        cout << "Missing SPX run headers!" << endl; 
        printf("missing SPX run headers\n");
        return;
    }
    MEGSPXGeometry * fGeometry = MEGSPXGeometry::GetInstance(spxRunHeader, spxPixelRunHeader);

    pad->SetFillColor(kBlack);
    pad->SetFrameFillColor(kBlack);
    pad->Draw();
    TH2F * fFrameHist = new TH2F("TRGSPX Frame hist", "", 20, -120, 120, 20, -86., 4.);
    fFrameHist->Draw();

    std::map<Int_t, std::vector<Color_t>> pixelSet;
    for (Int_t iPixel = 0; iPixel < 512; iPixel++) {
        pixelSet[iPixel].push_back(kGray);
    }
    auto map_backup = pixelSet;


    // int test = 71;
    // if (test%2 == 0)
    // {
    //     pixelSet[test].push_back(kGreen);
    //     pixelSet[test+1].push_back(kOrange);
    //     pixelSet[test+31].push_back(kOrange);
    //     pixelSet[test+16].push_back(kOrange);
    //     pixelSet[test+17].push_back(kOrange);
    // }
    // if (test%2 == 1)
    // {
    //     pixelSet[test].push_back(kGreen);
    //     pixelSet[test+1].push_back(kOrange);
    //     pixelSet[test+3].push_back(kOrange);
    //     pixelSet[test+16].push_back(kOrange);
    //     pixelSet[test+15].push_back(kOrange);
    // }
    // fGeometry->DrawZPhi2DView(pixelSet, "", 1, kTRUE);

    // DMTable
    cout << "ciao" << endl;
    cSPX->Print("1R0vs1R0sm.pdf[");
    for (Int_t patch = 0; patch < 256; ++patch)
    {
        pixelSet = map_backup;
        for (Int_t iPixel = 0; iPixel < 512; ++iPixel)
        {
            if (DMTable0[iPixel][patch])
                pixelSet[iPixel][0] = kRed;

            if (DMTable1[iPixel][patch])
                pixelSet[iPixel].push_back(kGreen);
        }

        fGeometry->DrawZPhi2DView(pixelSet, "", 1, kTRUE);
        
        cSPX->Print("1R0vs1R0sm.pdf");

        pad->Clear();

    }
    cSPX->Print("1R0vs1R0sm.pdf]");
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
