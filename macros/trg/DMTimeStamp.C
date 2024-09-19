#include "../../include/MMUtils.h"
R__ADD_LIBRARY_PATH(/meg/home/brini_m/Git/MatteMEG/lib)
R__LOAD_LIBRARY(libMMUtils.so)

void DMTimeStamp()
{
    MMUtils utils;

    //std::array<TChain *, 2> arr_rec;
    //arr_rec[0] = utils.MakeTChain("/meg/home/brini_m/Git/offline/analyzer/distilled/dm21/rec*.root");
    //arr_rec[1] = utils.MakeTChain("/meg/home/brini_m/Git/offline/analyzer/distilled/dm22_unbiassed/rec*.root");

    TChain * rec = utils.MakeTChain(566347, 20);
    TH2F * hTC = new TH2F("hTC", "hTC", 100, -0.7e-6, -0.4e-6, 128, -0.5, 127.5);
    rec->Draw("TrackCoarseTime:TPositron >> hTC", "mask==20", "COLZ");

    //TH2F * hRC21 = new TH2F("hRC21", "hRC21", 100, 380000, 480000, 128, -0.5, 127.5);
    //TH2F * hRT21 = new TH2F("hRT21", "hRT21", 100, 380000, 480000, 100, -0.7e-6, -0.2e-6);
    //TH2F * hTC21 = new TH2F("hTC21", "hTC21", 100, -0.7e-6, -0.2e-6, 128, -0.5, 127.5);
    //TH2F * hRC22 = new TH2F("hRC22", "hRC22", 100, 380000, 480000, 128, -0.5, 127.5);
    //TH2F * hRT22 = new TH2F("hRT22", "hRT22", 100, 380000, 480000, 100, -0.7e-6, -0.2e-6);
    //TH2F * hTC22 = new TH2F("hTC22", "hTC22", 100, -0.7e-6, -0.2e-6, 128, -0.5, 127.5);

    //arr_rec[0]->Draw("TrackCoarseTime:run >> hRC21", "mask==20 || mask==21", "COLZ");
    //arr_rec[0]->Draw("TPositron:run >> hRT21", "mask==20 || mask==21", "COLZ");
    //arr_rec[0]->Draw("TrackCoarseTime:TPositron >> hTC21", "mask==20 || mask==21", "COLZ");
    //arr_rec[1]->Draw("TrackCoarseTime:run >> hRC22", "mask==20 || mask==21", "COLZ");
    //arr_rec[1]->Draw("TPositron:run >> hRT22", "mask==20 || mask==21", "COLZ");
    //arr_rec[1]->Draw("TrackCoarseTime:TPositron >> hTC22", "mask==20 || mask==21", "COLZ");
    //
    //TFile * fOut = new TFile("/meg/home/brini_m/Git/MatteMEG/outfiles/timeanalysis.root", "RECREATE"); 

    //hRC21->Write();
    //hRT21->Write();
    //hTC21->Write();
    //hRC22->Write();
    //hRT22->Write();
    //hTC22->Write();

    //fOut->Close();
}
