#include "../include/PatchHeader.hpp"

RunHeader::RunHeader(const int & idStart, const int & idStop)
{
   (*this).AddRange(idStart, idStop);
   return;
}

const RunHeader & RunHeader::AddRange(const int & idStart, const int & idStop)
{
   int last3Start = idStart % 1000;
   int lats3Stop = idStop % 1000;
   int folderStart = (idStart - last3Start) / 1000;
   int folderStop = (idStop - last3Stop) / 1000;

   int loopFrom, loopTo;

   if (folderStart <= folderStop)
   {
      for (int idFolder = folderStart; idFolder <= folderStop; idFolder++)
      {
         std::string folderData = DATA_PATH + std::to_string(folderStart) + "xxx/";
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
            
         }
      }
   }
   else 
}
