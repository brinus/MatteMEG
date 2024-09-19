#include "../../include/MMUtils.h"
R__ADD_LIBRARY_PATH(/meg/home/brini_m/Git/MatteMEG/lib)
R__LOAD_LIBRARY(libMMUtils.so)

Bool_t isCoarseValid(Int_t runnum, Int_t coarse)
{
    if (runnum < 393800)
        return coarse == 114 || coarse == 115;
    else if (runnum < 393900)
        return coarse == 97 || coarse == 98;
    else if (runnum < 427120)
        return coarse == 98 || coarse == 99;
    else if (runnum < 566000)
        return coarse == 100 || coarse == 0; // TO BE CHECKED
    else
        return coarse == 97 || coarse == 98 || coarse == 0;
}

Bool_t isHitValid(Int_t runnum, Float_t hit)
{
    if (runnum < 393800)
        return hit >= -0.44e-6 && hit <= -0.41e-6;
    else if (runnum < 427120)
        return hit >= -0.65e-6 && hit <= -0.61e-6;
    else if (runnum < 566000)
        return hit >= -0.623e-6 && hit <= -0.608e-6; // TO BE CHECKED
    else
        return hit >= -0.68e-6 && hit <= -0.64e-6;
}

void TileOffOn()
{
    MMUtils utils;
    TChain * rec = utils.MakeTChain(566347, 19);

    auto nEntries = rec->GetEntries();
    cout << "Got " << nEntries << " entries" << endl;

    // TFile SETUP
    TFile * file = rec->GetFile();
    TClonesArray * SPXPixelRunHeaderRA = (TClonesArray *)file->Get("SPXPixelRunHeader");

    // TChain SETUP
    ROMETreeInfo    * InfoRV = 0;
    MEGEventHeader  * EventHeaderRV = 0;
    MEGRecData      * RecDataRV = 0;
    MEGTRGSPXOnline * TRGSPXOnlineRV = 0;
    TClonesArray    * SPXHitsRA = 0;
    TClonesArray    * GLBPositronRA = 0;

    rec->SetBranchStatus("*", 0);
    rec->SetBranchStatus("Info.*", 1);
    rec->SetBranchStatus("eventheader.*", 1);
    rec->SetBranchStatus("reco.*", 1);
    rec->SetBranchStatus("trgspxonline.*", 1);
    rec->SetBranchStatus("spxhits*", 1);
    rec->SetBranchStatus("positron*", 1);

    rec->SetBranchAddress("Info.", &InfoRV);
    rec->SetBranchAddress("eventheader.", &EventHeaderRV);
    rec->SetBranchAddress("reco.", &RecDataRV);
    rec->SetBranchAddress("trgspxonline.", &TRGSPXOnlineRV);
    rec->SetBranchAddress("spxhits", &SPXHitsRA);
    rec->SetBranchAddress("positron", &GLBPositronRA);

    // --- Histograms ----------------------
    TH1F * hX = new TH1F("hX", "hX", 100, -50, 50);
    TH1F * hY = new TH1F("hY", "hY", 100, -50, 50);
    TH1F * hZ = new TH1F("hZ", "hZ", 100, -50, 50);
    TH1F * hDist = new TH1F("hDist", "hDist", 100, 0, 0);

    // --- Variables for FOR-LOOP ----------
    std::vector<Int_t> vecpixelIdTRG = {};
    std::vector<Int_t> vecpixelIdSPX = {};
    Int_t nTRGSPXTracks;
    Int_t nSPXHits;
    Int_t skipTRGSize = 0;
    Int_t skipSPXSize = 0;
    // -------------------------------------

    // --- Tree FOR-LOOP ------------------- 
    for (Int_t iEntry = 0; iEntry < nEntries; ++iEntry)
    {
        rec->GetEntry(iEntry);
        if (iEntry != 0 && iEntry%1000 == 0)
            cout << iEntry << " events finished" << endl;

        auto mask = EventHeaderRV->Getmask();
        if (mask != 20 && mask != 21) continue;

        auto run = InfoRV->GetRunNumber();
        auto event = InfoRV->GetEventNumber();
        
        // --- TRGSPXOnline LOOP ----------
        vecpixelIdTRG = {};
        nTRGSPXTracks = TRGSPXOnlineRV->GetnSPXTracks();
        for (Int_t iTrack = 0; iTrack < nTRGSPXTracks; ++iTrack)
        {
            auto pixelIdTRG = TRGSPXOnlineRV->GetTrackTileIdAt(iTrack);
            auto coarse = TRGSPXOnlineRV->GetTrackCoarseTimeAt(iTrack);
            if (isCoarseValid(run, coarse))
                vecpixelIdTRG.push_back(pixelIdTRG);
        }
        if (vecpixelIdTRG.size() == 0)
        {
            ++skipTRGSize;
            continue;
        }
        // --------------------------------
        
        // --- SPXHits LOOP ---------------
        vecpixelIdSPX = {};
        nSPXHits = SPXHitsRA->GetEntries();
        for (Int_t iHit = 0; iHit < nSPXHits; ++iHit)
        {
            auto pixelIdSPX = ((MEGSPXHit *)SPXHitsRA->At(iHit))->Getpixelid();
            auto time = ((MEGSPXHit *)SPXHitsRA->At(iHit))->Gettime();
            if (isHitValid(run, time))
                vecpixelIdSPX.push_back(pixelIdSPX);
        }
        if (vecpixelIdSPX.size() == 0)
        {
            ++skipSPXSize;
            continue;
        }
        // --------------------------------

        auto nPos = RecDataRV->GetNPositron();
        if (nPos != 1)
            continue;
        for (Int_t iPos = 0; iPos < nPos; ++iPos)
        {
            MEGGLBPositron * positron =  (MEGGLBPositron *)GLBPositronRA->At(RecDataRV->GetPositronIndexAt(iPos));
            Int_t pixelIdReco = positron->GetMatchedPixelID();

            // Check for Reco in timestamp
            Bool_t isRecoInTimeStamp = false;
            for (auto & pixelIdSPX : vecpixelIdSPX)
                isRecoInTimeStamp += pixelIdSPX == pixelIdReco;
            if (!isRecoInTimeStamp)
                continue;

            // Evaluating distance
            Double_t xyz_reco[3];
            Double_t xyz_trg[3];

            Bool_t isRecoInTRG = false;
            for (auto & pixelIdTRG : vecpixelIdTRG)
                isRecoInTRG += pixelIdTRG == pixelIdReco;
            if (isRecoInTRG)
            {
                hDist->Fill(0);
                continue;
            }

            ((MEGSPXPixelRunHeader *)(SPXPixelRunHeaderRA->At(pixelIdReco)))->GetXYZCopy(3, xyz_reco);
            for (Int_t iTRG = 0; iTRG < vecpixelIdTRG.size(); ++iTRG)
            {
                ((MEGSPXPixelRunHeader *)(SPXPixelRunHeaderRA->At(vecpixelIdTRG[iTRG])))->GetXYZCopy(3, xyz_trg);

                Double_t x_diff = xyz_trg[0] - xyz_reco[0];
                Double_t y_diff = xyz_trg[1] - xyz_reco[1];
                Double_t z_diff = xyz_trg[2] - xyz_reco[2];

                hX->Fill(x_diff);   
                hY->Fill(y_diff);   
                hZ->Fill(z_diff);

                Double_t dist = TMath::Sqrt(TMath::Power(x_diff, 2) + TMath::Power(y_diff, 2) + TMath::Power(z_diff, 2));   
                hDist->Fill(dist);
                if (dist > 50)
                {
                    cout << " r" << run << " e" << event << endl;
                    cout << " Pixel ID Reco: " << pixelIdReco << endl;
                    cout << " Pixel ID TRG:  " << vecpixelIdTRG[iTRG] << endl;
                    cout << " Dist: " << dist << endl;
                    cout << "---------------------" << endl;
                }
            }
        }
    }

    cout << "skip due to TRG off-time: " << skipTRGSize << endl;
    cout << "skip due to SPX off-time: " << skipSPXSize << endl;

    TCanvas * c1 = new TCanvas("c1", "c1", 1000, 500);
    c1->Divide(2, 1);
    c1->cd(1);
    hX->SetLineColor(kRed);
    hX->Draw();
    hY->SetLineColor(kGreen);
    hY->Draw("SAME");
    hZ->SetLineColor(kBlue);
    hZ->Draw("SAME");

    c1->cd(2);
    hDist->Draw();
}
