#ifndef RUNHEAD_H
#define RUNHEAD_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#include "TObject.h"

#define DATA_PATH "/meg/data1/offline/run/"

class RunHeader : public TObject
{
public:
   RunHeader(const int &, const int &);
   ~RunHeader(){};

   virtual RunHeader &AddRange(const int &, const int &);

   const std::vector<std::string> &GetFileNames() { return fileNames_; };
   const std::string &GetHistName() { return histName_; };

   ClassDef(RunHeader, 1);

protected:
   RunHeader(){};

   static int nIstances_;

   std::string histName_;
   std::vector<std::string> fileNames_;
};

#endif