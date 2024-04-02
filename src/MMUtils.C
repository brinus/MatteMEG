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

MMUtils::MMUtils()
{
    MMUtils::treeName_  = "rec";
    MMUtils::recType_   = "";
    MMUtils::chainPath_ = "/meg/data1/offline/run";
}

// Setter
void MMUtils::SetTreeName(const std::string & treeName)
{
    MMUtils::treeName_ = treeName;
}

void MMUtils::SetRecType(const std::string & recType)
{
    MMUtils::recType_ = recType;
}

void MMUtils::SetChainPath(const std::string & chainPath)
{
    MMUtils::chainPath_ = chainPath;
}

// Getter
const std::string & MMUtils::GetTreeName()
{
    return treeName_;
}

const std::string & MMUtils::GetRecType()
{
    return MMUtils::recType_;
}

const std::string & MMUtils::GetChainPath()
{
    return MMUtils::chainPath_;
}

TChain * MMUtils::MakeTChain(const std::string & pathFiles)
{
    TChain * chain = new TChain(MMUtils::treeName_.c_str());
    chain->Add(pathFiles.c_str());
    return chain;
}

TChain * MMUtils::MakeTChain(const int & startRun, const int & nFiles)
{
    TChain * chain = new TChain(MMUtils::treeName_.c_str());
    char * fileName;
    for (int i = startRun; i < startRun + nFiles; ++i)
    {
        fileName = Form("%s/%3dxxx/rec%6d.root", MMUtils::chainPath_.c_str(), i/1000, i);
        chain->Add(fileName);
    }
    return chain;
}

TChain * MMUtils::MakeTChainFromFile(const std::string & inFileName)
{
    std::ifstream inputFile(inFileName);
    if (!inputFile.is_open()) {
        std::cout << "Error in file opening!" << std::endl;
        return 0;
    }

    std::vector<int> numbers;
    std::string line;
    while (getline(inputFile, line)) 
    {
        std::vector<int> parsedNumbers = MMUtils::ParseLine(line);
        if (parsedNumbers.size() == 1)
            numbers.push_back(parsedNumbers[0]);
        else if (parsedNumbers.size() == 2)
            for (int i = parsedNumbers[0]; i <= parsedNumbers[1]; ++i)
                numbers.push_back(i);
    }

    TChain * chain = new TChain(MMUtils::treeName_.c_str());
    char * fileName;
    for (auto run : numbers)
    {
        fileName = Form("%s/%3dxxx/rec%6d.root", MMUtils::chainPath_.c_str(), run/1000, run);
        chain->Add(fileName);
    }

    inputFile.close();
    return chain;
}

const std::vector<int> MMUtils::ParseLine(const std::string & line) 
{
    std::vector<int> numbers;
    std::stringstream ss(line);
    std::string token;
    while (getline(ss, token, '-')) {
        std::stringstream tokenizer(token);
        int num;
        while (tokenizer >> num) {
            numbers.push_back(num);
        }
    }
    return numbers;
}
