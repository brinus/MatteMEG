// --- MMUtils.h ---------------------------------------------------------------o
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

#ifndef MMUTILS_H
#define MMUTILS_H

#include <string>
#include <TChain.h>

namespace MMUtils
{
    // Functions
    TChain * MakeTChain(const std::string pathFiles);
    TChain * MakeTChain(const int & startRun, const int & nFiles);

    // Variables
    std::string treeName = "rec";
    std::string rectype = "_unbiassed";
    std::string path = "/meg/data1/offline/run";
}

#endif
