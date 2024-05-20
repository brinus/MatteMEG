// --- MC2DMTable.C ------------------------------------------------------------o
//                                                                              |
// Version:     0.1                                                             |
// Author:      Matteo Brini                                                    |
// Date:        15/05/2024                                                      |
// E-mail:      brinimatteo@gmail.com                                           |
//                                                                              |
// Description:                                                                 |
//      Macro to study MC data, preliminary to create a DM table.               |
//                                                                              |
//------------------------------------------------------------------------------o

#include "TMath.h"

#include "../../include/MMUtils.h"
    R__ADD_LIBRARY_PATH(/meg/home/brini_m/Git/MatteMEG/lib)
R__LOAD_LIBRARY(libMMUtils.so)

int getPatch(int pmID)
{
    if (pmID < 44*92)
    {
        int row = pmID / 44;
        int column = pmID % 44;
        return ((column/4)+(row/4)*11);
    } 
    else 
    {
        if(pmID<44*92+16)
            return 253;
        else if (pmID<44*93-16)
            return 254;
        else
            return 255;
    }
}

void MC2DMTable()
{
    // Setup environment
    MMUtils utils;
    utils.SetTreeName("sim");

    // Setup TChain
    TChain * sim = utils.MakeTChain("/meg/data1/shared/subprojects/trg/MC/signal/simsigPosPh*.root");

    auto nEntries = sim->GetEntries();
    cout << "Got " << nEntries << " entries" << endl;

    // Setup Branches
    MEGMCSPXEvent  * simMCSPXEvent = 0;
    MEGMCKineEvent * simMCKineEvent = 0;
    TClonesArray   * simMCXECPMEvent = 0;
    TClonesArray   * simMCSPXPPDEvent = 0;

    sim->SetBranchStatus("*", 0);
    sim->SetBranchStatus("mcspx.*", 1);
    sim->SetBranchStatus("mckine.*", 1);
    sim->SetBranchStatus("mcxecpm*", 1);
    sim->SetBranchStatus("mcspxppd*", 1);

    sim->SetBranchAddress("mcspx.", &simMCSPXEvent);
    sim->SetBranchAddress("mckine.", &simMCKineEvent);
    sim->SetBranchAddress("mcxecpm", &simMCXECPMEvent);
    sim->SetBranchAddress("mcspxppd", &simMCSPXPPDEvent);

    // Constants
    static const Int_t kMaxNXePM = 10000;
    static const Int_t kNPMInner = 4092;
    static const Bool_t kVerbose = 1;

    // Variables
    Int_t pmMaxIndex = -1000, pixelIDFirst = -1000, pixelID = -1000, nPixel = -1000;
    Double_t chargeMax = -1000., chargePM = -1000.;
    Double_t tSpxMin = 1.e+9, tSpx = 1.e+9;
    Double_t cutRadius = 1.75; // cm, radius to cut beamspot on target in x-y plane

    int hit[256][512];  // 256 WDB * 512 pixelID
    int nhits[256];     // 256 WDB

    // Loop on events
    for(int i = 0; i < 256; i++)
    {
        nhits[i] = 0;
        for(int j = 0; j < 512; j++)
            hit[i][j] = 0;
    }

    for(Long_t iEntry = 0; iEntry < nEntries; iEntry++)
    {
        sim->GetEntry(iEntry);
        if (iEntry%1000 == 0 && iEntry != 0)
            cout << iEntry << endl;

        // Cut on beamspot
        auto nvtx = simMCKineEvent->Getnvtx();
        if (nvtx != 2) continue;

        Bool_t goodRadius = 0;
        for (Int_t i = 0; i < nvtx; ++i)
        {
            auto xvtx = simMCKineEvent->GetxvtxAt(i);
            auto yvtx = simMCKineEvent->GetyvtxAt(i);

            if ((xvtx * xvtx) + (yvtx * yvtx) < (cutRadius * cutRadius))
                goodRadius = 1;
        }
        if (!goodRadius) continue; 

        // XEC Loop
        chargeMax = -1000.;
        pmMaxIndex = -1000;
        for (Int_t iPM = 0; iPM < kNPMInner; iPM++)
        {
            chargePM = ((MEGMCXECPMEvent*)(simMCXECPMEvent->At(iPM)))->Getnphe();
            if (chargePM > chargeMax)
            {
                chargeMax = chargePM;
                pmMaxIndex = iPM;
            }
        }   

        // SPX Loop
        nPixel = simMCSPXEvent->Getnpixel();
        pixelIDFirst = -1000;  
        tSpxMin = 1.e+9;       
        for (Int_t iPixel = 0; iPixel < nPixel; iPixel++) {
            tSpx = ((MEGMCSPXPPDEvent*)(simMCSPXPPDEvent->At(iPixel)))->Gettpeppd();
            pixelID = ((MEGMCSPXPPDEvent*)(simMCSPXPPDEvent->At(iPixel)))->GetpixelID();
            if (tSpx < tSpxMin) {
                tSpxMin = tSpx;
                pixelIDFirst = pixelID;
            }  
        }

        // DMTable
        if(pmMaxIndex > 0 && pixelIDFirst > 0)
        {
            Int_t patch = getPatch(pmMaxIndex);
            if(patch < 256 && pixelIDFirst < 512)
            {
                hit[patch][pixelIDFirst]++;
                nhits[patch]++;
            }
        }   
    }
    
    std::ofstream file;
    file.open("/meg/home/brini_m/Git/MatteMEG/outfiles/full1R75.txt");
    for(Int_t i = 0; i < 256; i++)
    {
        cout << "Patch ID: " << setw(3) << i << ", nhits: " << setw(6) << nhits[i];
        Int_t curHits=0;
        Int_t sqrtSum=0;
        while(curHits < nhits[i] * 0.97)
        {
            // Find maxpos
            Int_t maxpos = 0;
            for(Int_t j = 0; j < 512; j++)
                if(hit[i][j] > hit[i][maxpos])
                    maxpos = j;

            // Add maxpos
            auto mean = hit[i][maxpos] * 1. / nhits[i];
            auto err = TMath::Sqrt(hit[i][maxpos]) * 1. / nhits[i];
            file << i << " " << maxpos << " " <<  setprecision(3) << mean << " +/- " << setprecision(3) << err << "\n";
            curHits += hit[i][maxpos];
            sqrtSum += TMath::Sqrt(hit[i][maxpos]);
            hit[i][maxpos] = 0;
        }
        cout << " occ (%) = "<<setprecision(3) << curHits * 100. / nhits[i] << " +/- " << setprecision(3) << sqrtSum * 100. / nhits[i] << endl;
    }

    file.close();
}
