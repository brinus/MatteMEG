// --- makeRunList.C -----------------------------------------------------------o
//                                                                              |
// Version:     0.1                                                             |
// Author:      Matteo Brini                                                    |
// Date:        28/03/2024                                                      |
// E-mail:      brinimatteo@gmail.com                                           |
//                                                                              |
// Description:                                                                 |
//      Macro to read a .txt file and make a TChain containing all the existing |
//      files. The .txt file must be formatted as follow:                       |
//                     383500-383510                                            |
//                     383624                                                   |
//                     384924-384976                                            |
//                                                                              |
//------------------------------------------------------------------------------o

TChain * makeRunList(const Char_t * inFileName, const Char_t * treeName = "rec", const Char_t inFolder = "/meg/data1/offline/run/")
{
    TChain * chain = new TChain(treeName);
    
}
