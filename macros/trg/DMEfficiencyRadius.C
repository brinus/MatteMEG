void loadDMTable(const char* path, std::array<std::array<Bool_t, 512>, 256> &DMTable){
   FILE *tabin = fopen(path,"r");
   Int_t data;

   for(Int_t iTile = 0; iTile<512; iTile++) {
      for(Int_t iPatch = 0; iPatch<256; iPatch++) {
         fscanf(tabin,"%d",&data);
         if(data)
            DMTable[iPatch][iTile] = true;
         else
            DMTable[iPatch][iTile] = false;
      }
   }
}

Double_t getAverage(std::array<std::array<Bool_t, 512>, 256> &DMTable){
   std::vector<Int_t> nTiles(256);
   for(int iPatch = 0; iPatch<256; iPatch++) {
      nTiles[iPatch] = 0;
      for(Int_t iTile = 0; iTile<512; iTile++) {
         if(DMTable[iPatch][iTile])
            nTiles[iPatch]++;
      }
   }
   double sum = accumulate(nTiles.begin(), nTiles.end(), 0.0); 
   return sum/(512*256);
}

void DMEfficiencyRadius(){
   std::vector<std::tuple<float, std::string, std::string>> files ={
      {1.0, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_full1R0_prio.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full1R0.mem"},
      {1.25, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_full1R25_prio.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full1R25.mem"},
      {1.5, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_full1R5_prio.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full1R5.mem"},
      {1.75, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_full1R75_prio.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full1R75.mem"},
      {2.0, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_full2R0_prio.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full2R0.mem"},
      {2.25, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_full2R25_prio.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full2R25.mem"},
      {2.5, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_full2R5_prio.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full2R5.mem"},
   };
   std::vector<std::tuple<float, std::string, std::string>> files22 ={
      {1.0, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm22_full1R0.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full1R0.mem"},
      {1.25, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm22_full1R25.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full1R25.mem"},
      {1.5, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm22_full1R5.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full1R5.mem"},
      {1.75, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm22_full1R75.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full1R75.mem"},
      {2.0, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm22_full2R0.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full2R0.mem"},
      {2.25, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm22_full2R25.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full2R25.mem"},
      {2.5, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm22_full2R5.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full2R5.mem"},
   };
   std::vector<std::tuple<float, std::string, std::string>> filesSmeared ={
      {1.0, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_full1R0sm_prio.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full1R0sm.mem"},
      {1.25, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_full1R25sm_prio.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full1R25sm.mem"},
      {1.5, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_full1R5sm_prio.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full1R5sm.mem"},
      {1.75, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_full1R75sm_prio.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full1R75sm.mem"},
      {2.0, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_full2R0sm_prio.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full2R0sm.mem"},
      {2.25, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_full2R25sm_prio.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full2R25sm.mem"},
      {2.5, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_full2R5sm_prio.root", "/meg/home/brini_m/Git/MatteMEG/outfiles/full2R5sm.mem"},
   };

   std::tuple<float, std::string, std::string> narrow = 
   {1.1, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_narrow_prio.root", "/meg/home/galli/offline/analyzer/macros/luca/dirmatch/dmnarrow.mem"};
   std::tuple<float, std::string, std::string> narrowSmeared = 
   {1.4, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_test_prio.root", "/meg/home/francesconi_m/git/offline/analyzer/test.mem"};
   std::tuple<float, std::string, std::string> wide = 
   {1.65, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_wide_prio.root", "/meg/home/francesconi_m/git/online/scripts/trigger/dmwide.mem"};
   std::tuple<float, std::string, std::string> dummy = 
   {2.7, "/meg/home/brini_m/Git/MatteMEG/outfiles/dm21_dummy.root", "/meg/home/francesconi_m/git/offline/analyzer/dmdummy.mem"};

   auto g = new TGraph;
   g->SetMarkerStyle(20);
   g->GetXaxis()->SetTitle("Radius Cut [cm]");
   g->GetYaxis()->SetTitle("DM Efficiency on 2021 data");
   auto gRate = new TGraph;
   gRate->SetMarkerStyle(20);
   gRate->GetXaxis()->SetTitle("Radius Cut [cm]");
   gRate->GetYaxis()->SetTitle("Average fraction of enabled Tiles");
   gRate->GetYaxis()->SetRangeUser(0.1, 0.4);
   auto g22 = new TGraph;
   g22->SetMarkerStyle(20);
   g22->SetMarkerColor(kViolet);
   g22->GetXaxis()->SetTitle("Radius Cut [cm]");
   g22->GetYaxis()->SetTitle("DM Efficiency on 2021 data");
   auto gRate22 = new TGraph;
   gRate22->SetMarkerStyle(20);
   gRate22->SetMarkerColor(kViolet);
   gRate22->GetXaxis()->SetTitle("Radius Cut [cm]");
   gRate22->GetYaxis()->SetTitle("Average fraction of enabled Tiles");
   gRate22->GetYaxis()->SetRangeUser(0.1, 0.4);
   auto gSmeared = new TGraph;
   gSmeared->SetMarkerStyle(20);
   gSmeared->SetMarkerColor(kGray);
   gSmeared->GetXaxis()->SetTitle("Radius Cut [cm]");
   gSmeared->GetYaxis()->SetTitle("DM Efficiency on 2021 data");
   auto gRateSmeared = new TGraph;
   gRateSmeared->SetMarkerStyle(20);
   gRateSmeared->SetMarkerColor(kGray);
   gRateSmeared->GetXaxis()->SetTitle("Radius Cut [cm]");
   gRateSmeared->GetYaxis()->SetTitle("Average fraction of enabled Tiles");
   gRateSmeared->GetYaxis()->SetRangeUser(0.1, 0.4);
   auto gNarrow = new TGraph;
   gNarrow->SetMarkerStyle(20);
   gNarrow->SetMarkerColor(kRed);
   gNarrow->GetXaxis()->SetTitle("Radius Cut [cm]");
   gNarrow->GetYaxis()->SetTitle("DM Efficiency on 2021 data");
   auto gRateNarrow = new TGraph;
   gRateNarrow->SetMarkerStyle(20);
   gRateNarrow->SetMarkerColor(kRed);
   gRateNarrow->GetXaxis()->SetTitle("Radius Cut [cm]");
   gRateNarrow->GetYaxis()->SetTitle("Average fraction of enabled Tiles");
   auto gNarrowSmeared = new TGraph;
   gNarrowSmeared->SetMarkerStyle(20);
   gNarrowSmeared->SetMarkerColor(kOrange);
   gNarrowSmeared->GetXaxis()->SetTitle("Radius Cut [cm]");
   gNarrowSmeared->GetYaxis()->SetTitle("DM Efficiency on 2021 data");
   auto gRateNarrowSmeared = new TGraph;
   gRateNarrowSmeared->SetMarkerStyle(20);
   gRateNarrowSmeared->SetMarkerColor(kOrange);
   gRateNarrowSmeared->GetXaxis()->SetTitle("Radius Cut [cm]");
   gRateNarrowSmeared->GetYaxis()->SetTitle("Average fraction of enabled Tiles");
   auto gWide = new TGraph;
   gWide->SetMarkerStyle(20);
   gWide->SetMarkerColor(kBlue);
   gWide->GetXaxis()->SetTitle("Radius Cut [cm]");
   gWide->GetYaxis()->SetTitle("DM Efficiency on 2021 data");
   auto gRateWide = new TGraph;
   gRateWide->SetMarkerStyle(20);
   gRateWide->SetMarkerColor(kBlue);
   gRateWide->GetXaxis()->SetTitle("Radius Cut [cm]");
   gRateWide->GetYaxis()->SetTitle("Average fraction of enabled Tiles");
   auto gDummy = new TGraph;
   gDummy->SetMarkerStyle(20);
   gDummy->SetMarkerColor(kGreen);
   gDummy->GetXaxis()->SetTitle("Radius Cut [cm]");
   gDummy->GetYaxis()->SetTitle("DM Efficiency on 2021 data");
   auto gRateDummy = new TGraph;
   gRateDummy->SetMarkerStyle(20);
   gRateDummy->SetMarkerColor(kGreen);
   gRateDummy->GetXaxis()->SetTitle("Radius Cut [cm]");
   gRateDummy->GetYaxis()->SetTitle("Average fraction of enabled Tiles");
   for (auto& val : files){
      std::cout << "Processing " << get<1>(val).c_str() << std::endl;
      TFile* f = new TFile(get<1>(val).c_str());

      TH1F* hDen = (TH1F*) f->Get("hEPos");
      TH1F* hNum = (TH1F*) f->Get("hEPosTRGw");
      std::array<std::array<Bool_t, 512>, 256> DMTable;
      loadDMTable(get<2>(val).c_str(), DMTable);
      std::cout << "Table " <<get<2>(val) << " loaded" <<std::endl;

      if(hDen != nullptr && hNum != nullptr){
         TEfficiency e(*hNum, *hDen);
         Double_t value = e.GetEfficiency(e.FindFixBin(52.7));
         std::cout << value << std::endl;
         std::cout << getAverage(DMTable) << std::endl;
         g->AddPoint(get<0>(val), value);
         gRate->AddPoint(get<0>(val), getAverage(DMTable));
      } else {
         std::cout << "cannot read " << get<1>(val) << std::endl;
      }
   }
   for (auto& val : files22){
      std::cout << "Processing " << get<1>(val).c_str() << std::endl;
      TFile* f = new TFile(get<1>(val).c_str());

      TH1F* hDen = (TH1F*) f->Get("hEPos");
      TH1F* hNum = (TH1F*) f->Get("hEPosTRGw");
      std::array<std::array<Bool_t, 512>, 256> DMTable;
      loadDMTable(get<2>(val).c_str(), DMTable);
      std::cout << "Table " <<get<2>(val) << " loaded" <<std::endl;


      if(hDen != nullptr && hNum != nullptr){
         TEfficiency e(*hNum, *hDen);
         Double_t value = e.GetEfficiency(e.FindFixBin(52.7));
         std::cout << value << std::endl;
         std::cout << getAverage(DMTable) << std::endl;
         g22->AddPoint(get<0>(val), value);
         gRate22->AddPoint(get<0>(val), getAverage(DMTable));
      } else {
         std::cout << "cannot read " << get<1>(val) << std::endl;
      }
   }
   for (auto& val : filesSmeared){
      std::cout << "Processing " << get<1>(val).c_str() << std::endl;
      TFile* f = new TFile(get<1>(val).c_str());

      TH1F* hDen = (TH1F*) f->Get("hEPos");
      TH1F* hNum = (TH1F*) f->Get("hEPosTRGw");
      std::array<std::array<Bool_t, 512>, 256> DMTable;
      loadDMTable(get<2>(val).c_str(), DMTable);
      std::cout << "Table " <<get<2>(val) << " loaded" <<std::endl;


      if(hDen != nullptr && hNum != nullptr){
         TEfficiency e(*hNum, *hDen);
         Double_t value = e.GetEfficiency(e.FindFixBin(52.7));
         std::cout << value << std::endl;
         std::cout << getAverage(DMTable) << std::endl;
         gSmeared->AddPoint(get<0>(val), value);
         gRateSmeared->AddPoint(get<0>(val), getAverage(DMTable));
      } else {
         std::cout << "cannot read " << get<1>(val) << std::endl;
      }
   }
   {
      TFile* f = new TFile(get<1>(narrow).c_str());
      TH1F* hDen = (TH1F*) f->Get("hEPos");
      TH1F* hNum = (TH1F*) f->Get("hEPosTRGw");
      std::array<std::array<Bool_t, 512>, 256> DMTable;
      loadDMTable(get<2>(narrow).c_str(), DMTable);
      if(hDen != nullptr && hNum != nullptr){
         TEfficiency e(*hNum, *hDen);
         Double_t value = e.GetEfficiency(e.FindFixBin(52.7));
         std::cout << "Narrow Table" << std::endl;
         std::cout << value << std::endl;
         std::cout << getAverage(DMTable) << std::endl;
         gNarrow->AddPoint(get<0>(narrow), value);
         gRateNarrow->AddPoint(get<0>(narrow), getAverage(DMTable));
      }
   }
   {
      TFile* f = new TFile(get<1>(narrowSmeared).c_str());
      TH1F* hDen = (TH1F*) f->Get("hEPos");
      TH1F* hNum = (TH1F*) f->Get("hEPosTRGw");
      std::array<std::array<Bool_t, 512>, 256> DMTable;
      loadDMTable(get<2>(narrowSmeared).c_str(), DMTable);
      if(hDen != nullptr && hNum != nullptr){
         TEfficiency e(*hNum, *hDen);
         Double_t value = e.GetEfficiency(e.FindFixBin(52.7));
         std::cout << "Narrow Smeared Table" << std::endl;
         std::cout << value << std::endl;
         std::cout << getAverage(DMTable) << std::endl;
         gNarrowSmeared->AddPoint(get<0>(narrowSmeared), value);
         gRateNarrowSmeared->AddPoint(get<0>(narrowSmeared), getAverage(DMTable));
      }
   }
   {
      TFile* f = new TFile(get<1>(wide).c_str());
      TH1F* hDen = (TH1F*) f->Get("hEPos");
      TH1F* hNum = (TH1F*) f->Get("hEPosTRGw");
      std::array<std::array<Bool_t, 512>, 256> DMTable;
      loadDMTable(get<2>(wide).c_str(), DMTable);
      if(hDen != nullptr && hNum != nullptr){
         TEfficiency e(*hNum, *hDen);
         Double_t value = e.GetEfficiency(e.FindFixBin(52.7));
         std::cout << "Wide Table" << std::endl;
         std::cout << value << std::endl;
         std::cout << getAverage(DMTable) << std::endl;
         gWide->AddPoint(get<0>(wide), value);
         gRateWide->AddPoint(get<0>(wide), getAverage(DMTable));
      }
   }
   {
      TFile* f = new TFile(get<1>(dummy).c_str());
      TH1F* hDen = (TH1F*) f->Get("hEPos");
      TH1F* hNum = (TH1F*) f->Get("hEPosTRGw");
      std::array<std::array<Bool_t, 512>, 256> DMTable;
      loadDMTable(get<2>(dummy).c_str(), DMTable);
      if(hDen != nullptr && hNum != nullptr){
         TEfficiency e(*hNum, *hDen);
         Double_t value = e.GetEfficiency(e.FindFixBin(52.7));
         std::cout << "Dummy Table" << std::endl;
         std::cout << value << std::endl;
         std::cout << getAverage(DMTable) << std::endl;
         gDummy->AddPoint(get<0>(dummy), value);
         gRateDummy->AddPoint(get<0>(dummy), getAverage(DMTable));
      }
   }

   TLegend *l = new TLegend();
   l->AddEntry(g, "Radius Scan");
   //l->AddEntry(g22, "Radius Scan, run22");
   l->AddEntry(gSmeared, "Radius Scan, smeared");
   l->AddEntry(gNarrow, "DM Narrow");
   l->AddEntry(gNarrowSmeared, "DM Narrow, smeared");
   l->AddEntry(gWide, "DM Wide");
   l->AddEntry(gDummy, "RMD Trigger");

   TMultiGraph *mg = new TMultiGraph();
   mg->SetTitle(g->GetTitle());
   mg->GetXaxis()->SetTitle("Radius Cut [cm]");
   mg->GetYaxis()->SetTitle("DM Efficiency");
   mg->Add(g);
   //mg->Add(g22);
   mg->Add(gSmeared);
   mg->Add(gNarrow);
   mg->Add(gNarrowSmeared);
   mg->Add(gWide);
   mg->Add(gDummy);

   mg->Draw("APL");
   l->Draw();
   new TCanvas();
   TMultiGraph *mgRate = new TMultiGraph();
   mgRate->SetTitle(gRate->GetTitle());
   mgRate->GetXaxis()->SetTitle("Radius Cut [cm]");
   mgRate->GetYaxis()->SetTitle("Average fraction of enabled Tiles");
   mgRate->Add(gRate);
   //mgRate->Add(gRate22);
   mgRate->Add(gRateSmeared);
   mgRate->Add(gRateNarrow);
   mgRate->Add(gRateNarrowSmeared);
   mgRate->Add(gRateWide);
   mgRate->Add(gRateDummy);
   mgRate->Draw("APL");
   l->Draw();
}
