// --- DMEfficiency.C ---
//
// Version:     0.2
// Author:      Matteo Brini
// Date:        14/03/2024
// E-mail:      brinimatteo@gmail.com
//              
// Description:
//      Computation of trigger efficiency for direction match.
//      This version implement the computation of efficiency based on two
//      discriminants: the former is the entry of the DM table given by the 
//      offline reconstruction of the pixel ID, the latter is the logic OR
//      between all the TRG pixel ID in the wanted clock time (as function
//      of runnumber). 

Bool_t DMTable[256][512];

Int_t GetPatch(MEGXECPMRunHeader *);
Bool_t isCoarseValid(Int_t, Int_t);
Bool_t isHitValid(Int_t, Float_t);
Bool_t isOrTRG(Int_t, vector<Int_t> &);
void loadDMTable(const char *);

void DMEfficiency_v0_2()
{
    // Initial setup
    const Bool_t kVerbose = false;
    loadDMTable("/meg/home/francesconi_m/git/online/scripts/trigger/dmnarrow.mem");

    // TChain
    TChain * rec = new TChain("rec");

    Int_t runNum = 393500;
    Int_t nInFiles = 1200;
    Int_t nFiles = 0;
    Int_t nEntries = 0;

    for (Int_t iFile = runNum; iFile < runNum + nInFiles; ++iFile)
        nFiles += rec->Add(Form("/meg/data1/offline/run/%03dxxx/rec%06d_unbiassed.root", iFile / 1000, iFile));

    nEntries = rec->GetEntries();
    cout << "Got " << nFiles << " files" << endl;
    cout << "Got " << nEntries << " entries" << endl;
    
    // XECPMRunHeader
    TFile * file = rec->GetFile();
    MEGMEGParameters * runParameters = (MEGMEGParameters*)file->Get("MEGParameters");
    MEGXECRunHeader  * xecRunHeader = static_cast<MEGXECRunHeader*>(file->Get("XECRunHeader"));
    TClonesArray     * XECPMRunHeaderRA = (TClonesArray*)file->Get("XECPMRunHeader");

    Double_t LXeRadius = 67.6; // TO BE CHECKED!
    Double_t uMin = -999;
    Double_t uMax = -999;
    Double_t vMin = -999;
    Double_t vMax = -999;

    if(xecRunHeader)
    {
        cout << "Run Headers:" << endl;
        cout << " - nPM: " << xecRunHeader->GetNPM() << endl;
        cout << " - LXe inner face radius: " << xecRunHeader->GetActiveVolumeRadiusInner() << endl;
        LXeRadius = xecRunHeader->GetActiveVolumeRadiusInner();

        for(Int_t i = 0; i < xecRunHeader->GetNPM(); i++)
        {
            MEGXECPMRunHeader * pmrunh = (MEGXECPMRunHeader * )XECPMRunHeaderRA->At(i);
            if(pmrunh->GetIsSiPM())
            {
                if(uMin==-999 || uMin > pmrunh->GetUVWAt(0))
                    uMin = pmrunh->GetUVWAt(0);
                if(uMax==-999 || uMax < pmrunh->GetUVWAt(0))
                    uMax = pmrunh->GetUVWAt(0);
                if(vMin==-999 || vMin > pmrunh->GetUVWAt(1))
                    vMin = pmrunh->GetUVWAt(1);
                if(vMax==-999 || vMax < pmrunh->GetUVWAt(1))
                    vMax = pmrunh->GetUVWAt(1);
            }
        }

        printf("Inner face:\n");
        printf(" - U [%f, %f]\n", uMin, uMax);
        printf(" - V [%f, %f]\n", vMin, vMax);
    }
    
    
    // TChain SETUP
    ROMETreeInfo        * InfoRV = 0;
    MEGEventHeader  * EventHeaderRV = 0;
    MEGRecData      * RecDataRV = 0;
    MEGTRGSPXOnline * TRGSPXOnlineRV = 0;
    TClonesArray    * SPXHitsRA = 0;
    TClonesArray    * GLBPositronRA = 0;

    rec->SetBranchStatus("*", 0);
    rec->SetBranchStatus("Info.*", 1);
    rec->SetBranchStatus("eventheader.*");
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

    // ROOT Histogams
    TH1F * hEPosReco = new TH1F("hEPosReco", "", 120, 40, 56);
    TH1F * hEPosTRG  = new TH1F("hEPosTRG", "", 120, 40, 56);
    TH1F * hEPos     = new TH1F("hEPos", "", 120, 40, 56);

    // TChain LOOP
    for (Int_t iEntry = 0; iEntry < nEntries; ++iEntry)
    {
        rec->GetEntry(iEntry);
        if (iEntry%1000 == 0)
            cout << iEntry << " events finished\r" << flush;

        auto run = InfoRV->GetRunNumber();
        auto event = InfoRV->GetEventNumber();
        
        auto mask = EventHeaderRV->Getmask();
        if (mask!=21) continue;

        // TRGSPXOnline LOOP
        vector<Int_t> vecpixelIdTRG = {};
        auto nTRGSPXTracks = TRGSPXOnlineRV->GetnSPXTracks();
        for (Int_t iTrack = 0; iTrack < nTRGSPXTracks; ++iTrack)
        {
            auto pixelIdTRG = TRGSPXOnlineRV->GetTrackTileIdAt(iTrack);
            auto coarse = TRGSPXOnlineRV->GetTrackCoarseTimeAt(iTrack);
            if (isCoarseValid(run, coarse))
                vecpixelIdTRG.push_back(pixelIdTRG);
        }

        // SPXHits LOOP
        vector<Int_t> vecpixelIdSPX = {};
        auto nSPXHits = SPXHitsRA->GetEntries();
        for (Int_t iHit = 0; iHit < nSPXHits; ++iHit)
        {
            auto pixelIdSPX = ((MEGSPXHit *)SPXHitsRA->At(iHit))->Getpixelid();
            auto time = ((MEGSPXHit *)SPXHitsRA->At(iHit))->Gettime();
            if (isHitValid(run, time))
                vecpixelIdSPX.push_back(pixelIdSPX);
        }

        // RecData LOOP
        auto nPositrons = RecDataRV->GetNPositron();
        for (Int_t iPositron = 0; iPositron < nPositrons; iPositron++) 
        {
            MEGGLBPositron * positron =  (MEGGLBPositron *)GLBPositronRA->At(RecDataRV->GetPositronIndexAt(iPositron));
            
            // RecData positron 
            Int_t pixelIdReco = positron->GetMatchedPixelID();
            
            // Gamma reconstruction from positron
            Double_t xpos, ypos, zpos, phipos, thetapos, epos;
            xpos = RecDataRV->GetXPositronAt(iPositron);
            ypos = RecDataRV->GetYPositronAt(iPositron);
            zpos = RecDataRV->GetZPositronAt(iPositron);
            phipos = RecDataRV->GetPhiPositronAt(iPositron);
            thetapos = RecDataRV->GetThetaPositronAt(iPositron);
            epos = RecDataRV->GetEPositronAt(iPositron);

            Double_t gammatheta = 180. - thetapos;
            Double_t gammaphi = phipos + 180.;

            Double_t alpha = TMath::Sin(gammatheta*TMath::DegToRad()) * TMath::Cos(gammaphi*TMath::DegToRad());  
            Double_t beta = TMath::Sin(gammatheta*TMath::DegToRad()) * TMath::Sin(gammaphi*TMath::DegToRad());
            Double_t gamma = TMath::Cos(gammatheta*TMath::DegToRad());

            Double_t tplus =  (-alpha*xpos - beta*ypos + TMath::Sqrt(LXeRadius*LXeRadius*(alpha*alpha + beta*beta) -1*(beta*xpos - alpha*ypos)*(beta*xpos - alpha*ypos)))/(alpha*alpha+beta*beta);
            Double_t tminus = (-alpha*xpos - beta*ypos - TMath::Sqrt(LXeRadius*LXeRadius*(alpha*alpha + beta*beta) -1*(beta*xpos - alpha*ypos)*(beta*xpos - alpha*ypos)))/(alpha*alpha+beta*beta);

            Double_t LXextemp[2], LXeytemp[2], LXeztemp[2];
            Double_t LXex, LXey, LXez;

            LXextemp[0] = xpos + alpha * tplus;
            LXextemp[1] = xpos + alpha * tminus;

            LXeytemp[0] = ypos + beta * tplus;
            LXeytemp[1] = ypos + beta * tminus;

            LXeztemp[0] = zpos + gamma * tplus;
            LXeztemp[1] = zpos + gamma * tminus;


            if(LXextemp[0]<LXextemp[1]) {
                LXex = LXextemp[0];
                LXey =  ypos + beta * tplus;
                LXez =  zpos + gamma * tplus;
            }
            else {
                LXex = LXextemp[1];
                LXey =  ypos + beta * tminus;
                LXez =  zpos + gamma * tminus;
            }

            Double_t ugamma = LXez;
            Double_t vgamma = (TMath::ATan2(LXey, -LXex))*LXeRadius;

            Int_t vID = 92-round((vgamma-vMin)*92/(vMax-vMin)); 
            Int_t uID = round((ugamma-uMin)*43/(uMax-uMin)); 

            if(vID <0 || vID > 92) 
                continue;
            if(uID <0 || uID > 44) 
                continue;
            
            MEGXECPMRunHeader * pmrunh =(MEGXECPMRunHeader *)XECPMRunHeaderRA->At(uID + vID * 44);
            Int_t patchId = GetPatch(pmrunh);

            // Quality cuts
            bool ePosCut = epos < 0.040 || epos > 0.053;
            bool posXECAcceptance = positron->GetXECAcceptance();
            bool posNGoodhits = positron->Getngoodhits() > 45;

            if (ePosCut || !posXECAcceptance)
                continue;
           
            // Check if pixelIdReco is in postmark
            bool isRecoInPostStamp = false;
            for (auto & pixelIdSPX : vecpixelIdSPX)
            {
                if (pixelIdReco == pixelIdSPX)
                    isRecoInPostStamp = true;
            }

            // Quality checks
            bool goodPatchId = patchId >= 0 && patchId < 256;
            bool goodPixelId = pixelIdReco >= 0 && pixelIdReco < 256;
            
            // Filling histograms
            if(goodPatchId && goodPixelId && isRecoInPostStamp)
            {
                hEPos->Fill(epos * 1000);

                if (kVerbose)
                {
                    cout << "--------------------------------" << endl;
                    cout << "Positron " << RecDataRV->GetPositronIndexAt(iPositron) << "/" << GLBPositronRA->GetEntries() << endl;
                    cout << "Patch : " << setw(3) << patchId << endl;
                    cout << "Pixel : " << setw(3) << pixelIdReco << endl;
                    cout << "DM = " << DMTable[patchId][pixelIdReco] << endl;
                    cout << "--------------------------------" << endl;
                }
            
                if (DMTable[patchId][pixelIdReco])
                    hEPosReco->Fill(epos * 1000);
           
                if (isOrTRG(patchId, vecpixelIdTRG))
                {
                    hEPosTRG->Fill(epos * 1000);   
                }
            }
        }
    }
    cout << endl;

    // Plots
    const Bool_t isROOTFileOn = false;
    const Bool_t isPlotOn = true;
    if (isROOTFileOn)
    {
        TFile outFile("outDMEfficiency.root", "RECREATE");  
        hEPos->Write();
        hEPosReco->Write();
        hEPosTRG->Write(); 
    }
   
    if (!isPlotOn)
        return;

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

Int_t GetPatch(MEGXECPMRunHeader * pm)
{
    Int_t chip = pm->GetDRSChipID();
    return (chip - 64) / 2;
}

Bool_t isCoarseValid(Int_t runnum, Int_t coarse)
{
    if (runnum < 393800)
        return coarse == 114 || coarse == 115;
    else if (runnum < 393900)
        return coarse == 97 || coarse == 98;
    else
        return coarse == 98 || coarse == 99;
}

Bool_t isHitValid(Int_t runnum, Float_t hit)
{
    if (runnum < 393800)
        return hit >= -0.44e-6 && hit <= -0.41e-6;
    else
        return hit >= -0.65e-6 && hit <= -0.61e-6;
}

Bool_t isOrTRG(Int_t patchId, vector<Int_t> & vecpixelIdTRG)
{
    for (auto pixelIdTRG : vecpixelIdTRG)
    {
        if (DMTable[patchId][pixelIdTRG])
            return 1;
    }
    return 0;
}

void loadDMTable(const char* path)
{
    FILE *tabin = fopen(path,"r");
    Int_t data;

    for(Int_t iTile = 0; iTile<512; iTile++)
    {
        for(Int_t iPatch = 0; iPatch<256; iPatch++)
        {
            fscanf(tabin,"%d",&data);
            if(data)
                DMTable[iPatch][iTile] = true;
            else
                DMTable[iPatch][iTile] = false;
        }
    }
}
