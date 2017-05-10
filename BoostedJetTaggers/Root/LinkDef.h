// this file is -*- C++ -*-
// #include "BoostedJetTaggers/JSSWTopTaggerBDT.h"
// #include "BoostedJetTaggers/JSSWTopTaggerDNN.h"
#include "BoostedJetTaggers/IJSSTagger.h"
#include "BoostedJetTaggers/SubstructureTopTagger.h"
#include "BoostedJetTaggers/SubstructureTopTaggerHelpers.h"
#include "BoostedJetTaggers/BoostedXbbTagger.h"
#include "BoostedJetTaggers/SmoothedTopTagger.h"
#include "BoostedJetTaggers/SmoothedWZTagger.h"

#ifdef __ROOTCLING__


#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;


#pragma link C++ class IJSSTagger;
// #pragma link C++ class JSSWTopTaggerBDT;
// #pragma link C++ class JSSWTopTaggerDNN;

#pragma link C++ class SmoothedTopTagger;
#pragma link C++ class SmoothedWZTagger;

#pragma link C++ class BoostedXbbTagger;


#pragma link C++ class SubstructureTopTagger;
#pragma link C++ namespace STTHelpers;

#pragma link C++ class STTHelpers::TopTagSmoothMassCut;
#pragma link C++ class STTHelpers::TopTagSmoothTau32Cut;

#pragma link C++ function STTHelpers::configSubstTagger;


#endif
