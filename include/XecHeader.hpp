#ifndef XECHEAD_H
#define XECHEAD_H

#include "../include/RunHeader.hpp"

class XecHeader : public RunHeader
{
public:
    XecHeader(const int &idStart, const int &idStop) : RunHeader(idStart, idStop){};
    ~XecHeader(){};

protected:
    XecHeader(){};
};

class Pi0PatchHeader : public XecHeader
{
public:
    ~Pi0PatchHeader(){};
    Pi0PatchHeader(const int &, const int &, const int &);

    const int &GetIdPatch() { return idPatch_; };

protected:
    int idPatch_;
};

#endif