class PatchHeader
{
   public:
      ~PatchHeader(){};
      PatchHeader(const Int_t & idStart, const Int_t idStop, const Int_t & idPatch)
      {
         if (idPatch > 24 || idPatch < 0)
         {
            std::cout << "Invalid patch number:" << idPatch << std::endl;
            exit(0);
            return;
         }
         
         idPatch_ = idPatch;
         (*this).AddRange(idStart, idStop);

         std::ostringstream ss;
         ss << std::setw(2) << std::setfill('0') << idPatch;
         histName_ = "hPatch" + ss.str(); 
      }

      void AddRange(const Int_t & idStart, const Int_t & idStop)
      {
         Int_t last3Start = idStart % 1000;
         Int_t last3Stop = idStop % 1000;
         Int_t folderStart = (idStart - last3Start) / 1000;
         Int_t folderStop = (idStop - last3Stop) / 1000;


         if (folderStart == folderStop)
         {
            std::string folderData = DATA_PATH + std::to_string(folderStart) + "xxx/";
            for (Int_t idRun = idStart; idRun <= idStop; idRun++)
            {
               std::string fileName = "rec" + std::to_string(idRun) + ".root";
               fileNames_.push_back(folderData + fileName);   
            }
         }        
         else
         {
            for (Int_t idFolder = folderStart; idFolder <= folderStop; idFolder++)
            {
               std::string folderData = DATA_PATH + std::to_string(idFolder) + "xxx/";
               if (idFolder == folderStart)
               {
                  for (Int_t idRun = idStart; idRun <= idFolder * 1000 + 999; idRun++)
                  {
                     std::string fileName = "rec" + std::to_string(idRun) + ".root";
                     fileNames_.push_back(folderData + fileName);
                  }
               }
               else if (idFolder == folderStop)
               {
                  for (Int_t idRun = idFolder * 1000; idRun <= idStop; idRun++)
                  {
                     std::string fileName = "rec" + std::to_string(idRun) + ".root";
                     fileNames_.push_back(folderData + fileName);
                  }
                  
               }
               else
               {
                  for (Int_t idRun = idFolder * 1000; idRun <= idFolder * 1000 + 999; idRun++)
                  {
                     std::string fileName = "rec" + std::to_string(idRun) + ".root";
                     fileNames_.push_back(folderData + fileName);
                  }

               }
            }  
         }
      }

      const std::vector<std::string> & GetFileNames(){ return fileNames_;};
      const std::string & GetHistName(){ return histName_;};
      const Int_t & GetIdPatch(){ return idPatch_;};
   private:
      PatchHeader(){};

      Int_t idPatch_;
      std::string histName_;
      std::vector<std::string> fileNames_;
};
 
