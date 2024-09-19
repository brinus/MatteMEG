#include <vector>
#include <fstream>

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

void saveDMTable(const char* path, std::array<std::array<Bool_t, 512>, 256> &DMTable){
   FILE *tabout = fopen(path,"w");

   for(Int_t iTile = 0; iTile<512; iTile++) {
      for(Int_t iPatch = 0; iPatch<256; iPatch++) {
         if(DMTable[iPatch][iTile])
            fprintf(tabout, "1 ");
         else
            fprintf(tabout, "0 ");
      }

      fprintf(tabout, "\n");
   }
}

int getMult(std::array<Bool_t, 512> DMTable, int index){
   Int_t phi = (index%256)%16;
   Int_t z = (index%256)/16;
   Int_t module = index/256;

   Int_t counter=0;
   if(phi==0) counter++;
   else if(DMTable[(phi-1)+16*z+256*module]) counter++;

   if(phi==15) counter++;
   else if(DMTable[(phi+1)+16*z+256*module]) counter++;

   if(z==0) counter++;
   else if(DMTable[phi+16*(z-1)+256*module]) counter++;

   if(z==15) counter++;
   else if(DMTable[phi+16*(z+1)+256*module]) counter++;

   if(phi==0 || z==0) counter+=0;
   else if(DMTable[(phi-1)+16*(z-1)+256*module]) counter++;

   if(phi==0 || z==15) counter+=0;
   else if(DMTable[(phi-1)+16*(z+1)+256*module]) counter++;

   if(phi==15 || z==0) counter+=0;
   else if(DMTable[(phi+1)+16*(z-1)+256*module]) counter++;

   if(phi==15 || z==15) counter+=0;
   else if(DMTable[(phi+1)+16*(z+1)+256*module]) counter++;

   return counter;
}

void computeAdjacentrTiles(std::array<std::array<Bool_t, 512>, 256> &DMTable, std::array<std::array<Int_t, 512>, 256> &DMMult)
{
   for(Int_t iPatch = 0; iPatch<256; iPatch++) {
      for(Int_t iTile = 0; iTile<512; iTile++) {
         DMMult[iPatch][iTile] = getMult(DMTable[iPatch], iTile);
      }
   }
}

void discriminateMultiplicity(std::array<std::array<Bool_t, 512>, 256> &DMTable, std::array<std::array<Int_t, 512>, 256> &DMMult)
{
   for(Int_t iPatch = 0; iPatch<256; iPatch++) {
      for(Int_t iTile = 0; iTile<512; iTile++) {
         if(DMTable[iPatch][iTile] && DMMult[iPatch][iTile]<1) DMTable[iPatch][iTile] = kFALSE;
         if(!DMTable[iPatch][iTile] && DMMult[iPatch][iTile]>3) DMTable[iPatch][iTile] = kTRUE;
      }
   }
}

void printDMTable(std::array<Bool_t, 512> &DMTable){
   for(int imodule=0; imodule<2; imodule++){
      printf("Module %d\n", imodule);
      for(int iphi=0; iphi<16; iphi++){
         for(int iz=0; iz<16; iz++){
            printf("%d ", DMTable[iphi+16*iz+256*imodule]);
         }
         printf("\n");
      }
      printf("\n");
   }
}
void printMultTable(std::array<Int_t, 512> &PMTable){
   for(int imodule=0; imodule<2; imodule++){
      printf("Module %d\n", imodule);
      for(int iphi=0; iphi<16; iphi++){
         for(int iz=0; iz<16; iz++){
            printf("%d ", PMTable[iphi+16*iz+256*imodule]);
         }
         printf("\n");
      }
      printf("\n");
   }
}

void DMSmearing(){
   std::array<std::array<Bool_t, 512>, 256> table;
   loadDMTable("/meg/home/brini_m/Git/MatteMEG/outfiles/full2R25.mem", table);
   printDMTable(table[128]);

   std::array<std::array<Int_t, 512>, 256> mult;
   computeAdjacentrTiles(table, mult);
   //printMultTable(mult[128]);

   discriminateMultiplicity(table, mult);
   printDMTable(table[128]);

   saveDMTable("/meg/home/brini_m/Git/MatteMEG/outfiles/full2R25sm.mem", table);
}
