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

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <TChain.h>

class MMUtils
{
    public:
        MMUtils();
 
        // Setter
        void SetTreeName(const std::string &);
        void SetRecType(const std::string &);
        void SetChainPath(const std::string &);

        // Getter
        const std::string & GetTreeName();
        const std::string & GetRecType();
        const std::string & GetChainPath();

        // Methods
        TChain * MakeTChain(const std::string &);
        TChain * MakeTChain(const int &, const int &);
        TChain * MakeTChainFromFile(const std::string &);
        
        const std::vector<int> ParseLine(const std::string &);

    private:
        std::string treeName_;
        std::string recType_;
        std::string chainPath_;
};

#endif
