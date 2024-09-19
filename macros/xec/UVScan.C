#include "../../include/MMUtils.h"
R__ADD_LIBRARY_PATH(/meg/home/brini_m/Git/MatteMEG/lib)
R__LOAD_LIBRARY(libMMUtils.so)

#define NPATCH 24
#define NRUN 2

void UVScan()
{
    MMUtils utils;

    TCanvas * c1 = new TCanvas("c1", "c1");
    c1->Divide(3, 8);

    Int_t runStart = 566501;

    
    for (Int_t runnum = runStart; runnum < runStart + NPATCH * NRUN; runnum += NRUN)
    {
        Int_t iPatch = (runnum - runStart) / 2 + 1;
        cout << iPatch << endl;
        c1->cd(((iPatch-1)%8) * 3 + ((iPatch-1)/8+1));
        TChain * rec = utils.MakeTChain(runnum, NRUN);
        Char_t * hName = Form("hXEC%d", iPatch);
        TH2F *hXEC = new TH2F(hName, hName, 100, -30, 30, 100, -60, 60);
        rec->Draw(Form("VGamma:UGamma >> hXEC%d", iPatch), "mask==10 ", "COLZ");
        hXEC->Draw();
    }

    std::vector<Int_t> vecPatchId = {7, 10, 12, 15, 20, 21, 22}; // runStart 566588
    runStart = 566588;
    for (Int_t runnum = runStart; runnum < runStart + vecPatchId.size() * NRUN; runnum += NRUN)
    {
        Int_t iPatch = vecPatchId[(runnum - runStart) / 2];
        cout << iPatch << endl;
        c1->cd(((iPatch-1)%8) * 3 + ((iPatch-1)/8+1));
        TChain * rec = utils.MakeTChain(runnum, NRUN);
        Char_t * hName = Form("hXEC%d_2", iPatch);
        TH2F *hXEC2 = new TH2F(hName, hName, 100, -30, 30, 100, -60, 60);
        rec->Draw(Form("vave:uave >> hXEC%d_2", iPatch), "mask==10 ", "COLZ");
        hXEC2->Draw();
    }
    
    vecPatchId = {12, 21, 22}; // runStart 566632
    runStart = 566632;
    for (Int_t runnum = runStart; runnum < runStart + vecPatchId.size() * NRUN; runnum += NRUN)
    {
        Int_t iPatch = vecPatchId[(runnum - runStart) / 2];
        cout << iPatch << endl;
        c1->cd(((iPatch-1)%8) * 3 + ((iPatch-1)/8+1));
        TChain * rec = utils.MakeTChain(runnum, NRUN);
        Char_t * hName = Form("hXEC%d_2", iPatch);
        TH2F *hXEC2 = new TH2F(hName, hName, 100, -30, 30, 100, -60, 60);
        rec->Draw(Form("vave:uave >> hXEC%d_2", iPatch), "mask==10 ", "COLZ");
        hXEC2->Draw();
    }
}
