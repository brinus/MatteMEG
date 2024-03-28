// --- MMUtils.C ---------------------------------------------------------------o
//                                                                              |
// Version:     0.1                                                             |
// Author:      Matteo Brini                                                    |
// Date:        28/03/24                                                        |
// E-mail:      brinimatteo@gmail.com                                           |
//                                                                              |
// Description:                                                                 |
//          Namespace for some utlity functions.                                |
//                                                                              |
//------------------------------------------------------------------------------o

#include "../include/MMUtils.h"

TChain * MMUtils::MakeTChain(const std::string pathFiles)
{
    TChain * chain = new TChain(MMUtils::treeName.c_str());
    chain->Add(pathFiles.c_str());
    return chain;
}

TChain * MMUtils::MakeTChain(const int & startRun, const int & nFiles)
{
    TChain * chain = new TChain(MMUtils::treeName.c_str());
    char * fileName;
    for (int i = startRun; i < startRun + nFiles; ++i)
    {
        fileName = Form("%s/%3dxxx/rec%6d%s.root", MMUtils::path, i/1000, i, MMUtils::rectype);
        chain->Add(fileName);
    }
}
