// --- ShowEvent.C -------------------------------------------------------------o
//                                                                              |
// Version:     0.2                                                             |
// Author:      Matteo Brini                                                    |
// Date:        23/03/24                                                        |
// E-mail:      brinimatteo@gmail.com                                           |
//                                                                              |
// Description:                                                                 |
//      Macro to draw a visualization of timing counter pixels. The user must   |
//      give in input a runnumber and an event number. If a third input is      |
//      given, it is stored as the patch number selected. In this case, the     |
//      green set of SPX tiles is substituted by the yellow set of entries from |
//      the DMTable. The yellow tiles then are those selected from the DMTable  |
//      for that specific patch Id.                                             |
//                                                                              |
//------------------------------------------------------------------------------o

using namespace std;

Bool_t DMTable[256][512];

int priority[] ={  0,   1,  
                   16,  17,   2,   3,  
                   32,  33,  18,  19,   4,   5,  
                   48,  49,  34,  35,  20,  21,   6,   7,  
                   64,  65,  50,  51,  36,  37,  22,  23,   8,   9,  
                   80,  81,  66,  67,  52,  53,  38,  39,  24,  25,  10,  11, 
                   96,  97,  82,  83,  68,  69,  54,  55,  40,  41,  26,  27,  12,  13, 
                  112, 113,  98,  99,  84,  85,  70,  71,  56,  57,  42,  43,  28,  29,  14, 15, 
                  114, 115, 100, 101,  86,  87,  72,  73,  58,  59,  44,  45,  30,  31, 
                  116, 117, 102, 103,  88,  89,  74,  75,  60,  61,  46,  47, 
                  118, 119, 104, 105,  90,  91,  76,  77,  62,  63, 
                  120, 121, 106, 107,  92,  93,  78,  79, 
                  122, 123, 108, 109,  94,  95, 
                  124, 125, 110, 111,
                  126, 127}; 
                        
int priority_old[] ={    0,  
                    16,   1,   2,  
                    32,  17,  18,   3,   4,  
                    48,  33,  34,  19,  20,   5,   6,  
                    64,  49,  50,  35,  36,  21,  22,   7,   8,  
                    80,  65,  66,  51,  52,  37,  38,  23,  24,   9,  10, 
                    96,  81,  82,  67,  68,  53,  57,  39,  40,  25,  26,  11,  12, 
                    112,  97,  98,  83,  84,  69,  70,  55,  56,  41,  42,  27,  28,  13,  14, 
                    113, 114,  99, 100,  85,  86,  71,  72,  57,  58,  43,  44,  29,  30,  15, 
                    115, 116, 101, 102,  87,  88,  73,  74,  59,  60,  45,  46,  31, 
                    117, 118, 103, 104,  89,  90,  75,  76,  61,  62,  47, 
                    119, 120, 105, 106,  91,  92,  77,  78,  63, 
                    121, 122, 107, 108,  93,  94,  79, 
                    123, 124, 109, 110,  95, 
                    125, 126, 111,
                    127};
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

void ShowEvent(Int_t runnum = -1, Int_t eventNum = -1, Int_t patchId = -1)
{
    if (runnum == -1 || eventNum == -1)
    {
        cout << "Please select a runnumber and an eventNum!" << endl;
        return;
    }

    if (patchId < -1 || patchId >= 256)
    {
        cout << "Invalid patchId passed: " << patchId << endl;
        return;
    }
    else
    {
        loadDMTable("/meg/home/francesconi_m/git/online/scripts/trigger/dmnarrow.mem");
        //loadDMTable("/meg/home/brini_m/Git/MatteMEG/outfiles/full2R0.mem");
        //loadDMTable("/meg/home/brini_m/Git/offline/analyzer/dmout.mem");
    }

    std::map<Int_t, Int_t> mpriority_old;
    std::map<Int_t, Int_t> mpriority;

    for (Int_t idx = 0; idx < 128; ++idx)
    {
        mpriority_old[priority_old[idx]] = idx;
        mpriority[priority[idx]] = idx;
    } 

    TFile * f = new TFile(Form("/meg/data1/offline/run/%03dxxx/rec%06d.root", runnum/1000, runnum));
    //TFile * f = new TFile(Form("/meg/data1/offline/run/%03dxxx/rec%06d_open.root", runnum/1000, runnum));
    //TFile * f = new TFile(Form("/meg/data1/offline/run/%03dxxx/rec%06d_check.root", runnum/1000, runnum));

    TTree * rec = (TTree*)f->Get("rec");
    MEGTRGRunHeader * trgRunHeader = static_cast<MEGTRGRunHeader*>(f->Get("TRGRunHeader"));
    if(rec == nullptr || trgRunHeader == nullptr){
        cout << "Missing tree or TRG run header!" << endl;
        return;
    }
    
    ROMETreeInfo   * InfoRV = 0;
    MEGEventHeader * EventHeaderRV = 0;
    MEGTRGData     * TRGDataRV = 0;
    MEGRecData     * RecDataRV = 0;
    TClonesArray   * SPXHitsRA = 0;
    TClonesArray   * GLBPositronRA = 0;

    rec->SetBranchAddress("Info.", &InfoRV);
    rec->SetBranchAddress("eventheader.", &EventHeaderRV);
    rec->SetBranchAddress("trgdata.", &TRGDataRV);
    rec->SetBranchAddress("reco.", &RecDataRV);
    rec->SetBranchAddress("spxhits", &SPXHitsRA);
    rec->SetBranchAddress("positron", &GLBPositronRA);

    rec->SetBranchStatus("*", 0);
    rec->SetBranchStatus("Info.*", 1);
    rec->SetBranchStatus("eventheader.*", 1);
    rec->SetBranchStatus("trgdata.*", 1);
    rec->SetBranchStatus("reco.*", 1);
    rec->SetBranchStatus("spxhits*", 1);
    rec->SetBranchStatus("positron*", 1);

    Long_t entryId = -1;
    Long_t nEntries = rec->GetEntries();
    for(Long_t iEntry=0; iEntry < nEntries && entryId < 0; iEntry++){
        rec->GetEntry(iEntry);

        if(eventNum==-1)
            entryId = iEntry;
        if(eventNum<-1)
            eventNum++;

        if(InfoRV->GetEventNumber() == eventNum)
            entryId = iEntry;
    }

    if(entryId==-1){
        cout << "Event not found!" << endl;
        return;
    }

    Int_t triggerId = EventHeaderRV->Getid();
    Int_t triggerMask = EventHeaderRV->Getmask();
    Int_t tctoOdb = trgRunHeader->GetTcTimeOffset();
    cout << "---------------------------------------------" << endl;
    cout << "Event infos: " << endl;
    cout << "   Run: " << runnum << endl;
    cout << "   Event: " << eventNum << endl;
    cout << "   Trigger ID: " << triggerId << endl;
    cout << "   Trigger mask: " << triggerMask << endl;
    if (patchId != -1)
        cout << "   Patch ID: " << patchId << endl;

    TCanvas* cSPX = new TCanvas("cSPX", "cSPX", 1000, 500);
    TVirtualPad * pad = cSPX->cd();
    
    MEGSPXRunHeader * spxRunHeader = (MEGSPXRunHeader*)f->Get("SPXRunHeader");
    TClonesArray * spxPixelRunHeader = (TClonesArray*)f->Get("SPXPixelRunHeader"); 
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
    Int_t nPixel = fGeometry->GetNPixels();
    for (Int_t iPixel = 0; iPixel < nPixel; iPixel++) {
        pixelSet[iPixel].push_back(kGray);
    }

    cout << "---------------------------------------------" << endl;
    cout << "MEGSPXHit infos:" << endl;
    Int_t min_id = 511;
    Int_t min_id_old = 511;
    nPixel = SPXHitsRA->GetEntries();
    for (Int_t iPixel = 0; iPixel < nPixel; iPixel++) {
        auto pixelid = ((MEGSPXHit *)(SPXHitsRA->At(iPixel)))->Getpixelid();
        auto time = ((MEGSPXHit *)SPXHitsRA->At(iPixel))->Gettime();
        cout << "\tPixelID: " << setw(3) << pixelid << " time: " << setw(12) << time;
        if (isHitValid(runnum, time))
        {
            cout << " isValid";
            if (patchId == -1)
                pixelSet[pixelid][0] = kGreen;
            bool tcb_prio = pixelid%256 / 128 <= min_id%256 / 128;
            bool tcb_prio_old = pixelid%256 / 128 <= min_id_old%256 / 128;
            if (mpriority[pixelid%128] < mpriority[min_id%128] && tcb_prio)
                min_id = pixelid;
            if (mpriority_old[pixelid%128] < mpriority_old[min_id_old%128] && tcb_prio_old)
                min_id_old = pixelid;
        }
        cout << endl;
    }
    
    cout << "min_id " << min_id << endl;
    cout << "min_id_old" << min_id_old << endl;
    // DMTable
    if (patchId != -1)
    {

        for (Int_t iPixel = 0; iPixel < 512; ++iPixel)
        {
            if (DMTable[patchId][iPixel])
            {
                pixelSet[iPixel][0] = kYellow;
            }
        }
    }

    cout << "---------------------------------------------" << endl;
    cout << "Reco Positron" << endl;
    Int_t nPositron = RecDataRV->GetNPositron();
    for (Int_t iPos = 0; iPos < nPositron; ++iPos)
    {
        auto positron = (MEGGLBPositron *)GLBPositronRA->At(RecDataRV->GetPositronIndexAt(iPos));
        auto pixelid = positron->GetMatchedPixelID();
        auto epos = RecDataRV->GetEPositronAt(iPos);
        auto theta = RecDataRV->GetThetaPositronAt(iPos); 
        auto phi = RecDataRV->GetPhiPositronAt(iPos);
        auto xpos = RecDataRV->GetXPositronAt(iPos);
        auto ypos = RecDataRV->GetYPositronAt(iPos);
        auto zpos = RecDataRV->GetZPositronAt(iPos);
        Int_t iPixel = 0;
        while (pixelid != ((MEGSPXHit *)(SPXHitsRA->At(iPixel)))->Getpixelid())
            ++iPixel;
        auto time = ((MEGSPXHit *)SPXHitsRA->At(iPixel))->Gettime();
        cout << "Positron " << iPos + 1 << "/" << nPositron << endl;
        cout << "\tPixelID: " << pixelid << endl;      
        cout << "\tEnergy: " << epos * 1000 << " MeV" << endl;
        cout << "\tTheta: " << theta << endl;
        cout << "\tPhi: " << phi << endl;
        cout << "\tXPos: " << xpos << endl;
        cout << "\tYPos: " << ypos << endl;
        cout << "\tZPos: " << zpos << endl;
        if (isHitValid(runnum, time))
        {
            pixelSet[pixelid][0] = kBlue;
        }
    }
    if (pixelSet[min_id][0] == kBlue)
        pixelSet[min_id].push_back(kWhite);
    else
        pixelSet[min_id][0] = kWhite;
    if (min_id != min_id_old)
    {
        if (pixelSet[min_id_old][0] == kBlue)
            pixelSet[min_id_old].push_back(kOrange);
        else
            pixelSet[min_id_old][0] = kOrange;
    }   

    MEGDiscrWaveform * trackHitWaveform[4];
    MEGFADCWaveform  * trackTileIdWaveform[4];
    MEGFADCWaveform  * trackTileTimeWaveform[4];
    for(Int_t i = 0; i < 4; i++){
        trackHitWaveform[i] = TRGDataRV->GetTCTrackHitAt(i);
        trackTileIdWaveform[i] = TRGDataRV->GetTCTrackTileIdAt(i);
        trackTileTimeWaveform[i] = TRGDataRV->GetTCTrackTileTimeAt(i);

        Double_t maxTileIdWf = trackTileIdWaveform[i]->GetHistogramMaximum();
        Double_t minTileIdWf = trackTileIdWaveform[i]->GetHistogramMinimum();
    }
    cout << "---------------------------------------------" << endl;
    cout << "MEGTRGData infos:" << endl;
    for(Int_t i = 0; i < trackHitWaveform[0]->GetNPoints(); i++){
        for(Int_t iTrack = 0; iTrack < 4; iTrack++){
            if(trackHitWaveform[iTrack]->GetAmplitudeAt(i)){
                auto tileId = trackTileIdWaveform[iTrack]->GetAmplitudeAt(i);
                auto tileTime = trackTileTimeWaveform[iTrack]->GetAmplitudeAt(i);
                auto tileTime_ns = (tileTime + tctoOdb) * 0.78125;
                cout << "\tClock " << setw(3) << i 
                     << ", Track " << iTrack 
                     << ", TileId " << setw(3) << tileId 
                     << ", TileTime " << setw(2) << tileTime 
                     << ": " << setw(7) << tileTime_ns << " ns" << endl;
                if (isCoarseValid(runnum , i))
                    pixelSet[tileId].push_back(kRed);
            }
        }
    }

    // TRG FW Study
    

    fGeometry->DrawZPhi2DView(pixelSet, "", 1, kTRUE);
}
