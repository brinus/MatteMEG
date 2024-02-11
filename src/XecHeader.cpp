#include "../include/XecHeader.hpp"

Pi0PatchHeader::Pi0PatchHeader(const int &idStart, const int &idStop, const int &idPatch)
{
    if (idPatch <= 0 || idPatch > 24)
    {
        std::cout << "ERROR" << std::endl;
        std::cout << "Invalid patch number: " << idPatch << std::endl;
        exit(0);
    }

    idPatch_ = idPatch;
    (*this).AddRange(idStart, idStop);

    std::ostringstream ss;
    ss << std::setw(2) << std::setfill('0') << idPatch;
    histName_ = "hPatch" + ss.str();
}