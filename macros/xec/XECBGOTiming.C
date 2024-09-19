// --- XECBGOTiming.C ----------------------------------------------------------o
//                                                                              |
// Version:     0.1                                                             |
// Author:      Matteo Brini                                                    |
// Date:        25/06/2024                                                      |
// E-mail:      brinimatteo@gmail.com                                           |
//                                                                              |
// Description:                                                                 |
//      Macro to plot time difference when observing trigger 50 (coincidence    |
//      between LXe and BGO), so cosmic rays.                                   |
//                                                                              |
//------------------------------------------------------------------------------o

#include "../../include/MMUtils.h"
R__ADD_LIBRARY_PATH(/meg/home/brini_m/Git/MatteMEG/lib)
R__LOAD_LIBRARY(libMMUtils.so)

void XECBGOTiming()
{
    gROOT->ForceStyle("MEG_Pub");
    gStyle->SetPadRightMargin(0.18);
    
    auto c = new TCanvas("c1", "c1");
    auto h = new TH2F("h", "h", 100, 0, 30, 100, -60, 60);

    MMUtils utils;
    TChain * rec = utils.MakeTChain(566751, 3);
    rec->Add("/meg/data1/offline/run/566xxx/rec566711.root");
    rec->Draw("vave : (bgoresult.time-TGamma)*1e9 >> h", "mask == 50 && bgoresult.time < 1. && bgoresult.time > -0.7e-6 && TGamma < 1.", "COLZ");
    
    h->SetTitle("Time difference BGO-LXe;t_{BGO} - t_{LXe} [ns];v_{LXe} [cm]");
}
