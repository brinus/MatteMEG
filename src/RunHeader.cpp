#include "../include/RunHeader.hpp"

int RunHeader::nIstances_ = 0;

RunHeader::RunHeader(const int &idStart, const int &idStop)
{
   std::ostringstream ss;
   ss << std::setw(2) << std::setfill('0') << nIstances_;
   histName_ = "h" + ss.str();

   nIstances_++;
   (*this).AddRange(idStart, idStop);
   return;
}

RunHeader &RunHeader::AddRange(const int &idStart, const int &idStop)
{
   int last3Start = idStart % 1000;
   int last3Stop = idStop % 1000;
   int folderStart = (idStart - last3Start) / 1000;
   int folderStop = (idStop - last3Stop) / 1000;

   int loopFrom, loopTo;

   if (folderStart <= folderStop)
   {
      for (int idFolder = folderStart; idFolder <= folderStop; idFolder++)
      {
         std::string folderData = DATA_PATH + std::to_string(idFolder) + "xxx/";
         if (folderStart == folderStop)
         {
            loopFrom = idStart;
            loopTo = idStop;
         }
         else if (idFolder == folderStart)
         {
            loopFrom = idStart;
            loopTo = folderStart * 1000 + 999;
         }
         else if (idFolder == folderStop)
         {
            loopFrom = folderStop * 1000;
            loopTo = idStop;
         }
         else // folderStart < idFolder < folderStop
         {
            loopFrom = idFolder * 1000;
            loopTo = idFolder * 1000 + 999;
         }

         for (int idRun = loopFrom; idRun <= loopTo; idRun++)
         {
            std::string fileName = "rec" + std::to_string(idRun) + ".root";
            fileNames_.push_back(folderData + fileName);
         }
      }
   }
   else
   {
      std::cout << "ERROR" << std::endl;
      std::cout << "Wrong IDs: " << idStart << " " << idStop << std::endl;
      exit(0);
   }
   return *this;
}
