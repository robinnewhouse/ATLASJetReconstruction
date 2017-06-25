// for editors : this file is -*- C++ -*-
#ifndef BOOSTEDJETSTAGGERS_SMOOTHEDWZTAGGER_H_
#define BOOSTEDJETSTAGGERS_SMOOTHEDWZTAGGER_H_

//////////////////////////////////////////////////////
// \class SmoothedWZTagger
//
// Tag xAOD::Jet as decaying from a W or Z according to the mass and the D2 substructure variable of the jet.
//
// Tagging results are obtain either with the IJSSTagger interface:
//     bool ok = tool->tag( jet );
// Or by running the Tagger::tag() function and then querying the tool :
//     bool tagState = tool->getCutResult("TaggerState");
// where "TaggerState" is one of the specific states that the tagger may end in
//
//////////////////////////////////////////////////////

#include "BoostedJetTaggers/JSSTaggerBase.h"
#include "AsgTools/AsgTool.h"

#include "PATCore/TAccept.h"

class TF1;

class SmoothedWZTagger : public  JSSTaggerBase {
  ASG_TOOL_CLASS0(SmoothedWZTagger)
  public:

  //Default - so root can load based on a name
  SmoothedWZTagger(const std::string& name);

  // Default - so we can clean up
  ~SmoothedWZTagger();

  // Run once at the start of the job to setup everything
  StatusCode initialize();

  // Run once at the end of the job to clean up
  StatusCode finalize();

  // Implement IJSSTagger interface
  virtual Root::TAccept tag(const xAOD::Jet& jet) const;

  private:

    // need to set in initialization
    std::string m_wkpt;
    std::string m_tagType;
    std::string m_configFile;

    // variables to be used for making selection
    float m_mass;
    float m_D2;

    // parameters to store specific cut values
    std::string m_strMassCutLow;
    std::string m_strMassCutHigh;
    std::string m_strD2Cut;

    // functions that are configurable for specific cut values
    TF1* m_funcMassCutLow;
    TF1* m_funcMassCutHigh;
    TF1* m_funcD2Cut;

};

#endif
