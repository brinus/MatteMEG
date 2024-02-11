#include "../include/XecHeader.hpp"

#include <vector>
#include <string>

#include <ROOT/RDataFrame.hxx>
#include "TSystem.h"
#include "TFile.h"

void ReadRun()
{
   gSystem->AddLinkedLibs("-L../build -llibLibXec");

   std::vector<Pi0PatchHeader> *vecPatchList;
   TFile *file = new TFile("path/to/file.root", "UPDATE");

   file->Get<std::vector<Pi0PatchHeader>>("patchlist", vecPatchList);

   for (auto &patch : *vecPatchList)
   {
      std::cout << "Processing PATCH " << patch.GetIdPatch() << std::endl;

      ROOT::RDataFrame rec("rec", patch.GetFileNames());

      auto df = rec.Filter("eventheader.mask==51")
                    .Filter("trgxeconline.QsumAmplitude < 5e6")
                    .Filter("bgocexresult.openingAngle[0] < 180")
                    .Filter("bgocexresult.openingAngle[0] > 175");

      auto h1 = df.Histo1D({patch.GetHistName().c_str(), patch.GetHistName().c_str(), 300u, 1.0e6, 5.0e6}, "trgxeconline.QsumAmplitude");
      vecPatchList->push_back(patch);

      h1->Write();
   }
   file->WriteObject(&vecPatchList, "patchlist");
   file->Close();
}