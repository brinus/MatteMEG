#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#define DATA_PATH "/meg/data1/offline/run/" 

class RunHeader
{
   public:
      ~RunHeader(){};
      RunHeader(const Int_t & idStart, const Int_t idStop, const Int_t & idPatch)
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

      RunHeadder &AddRange(const int &, const int &);

      const std::vector<std::string> & GetFileNames(){ return fileNames_;};
      const std::string & GetHistName(){ return histName_;};

   private:

      static int nIstances_;

      std::string histName_;
      std::vector<std::string> fileNames_;
};

class Pi0Header : public RunHeader
{
   public:
      
}
