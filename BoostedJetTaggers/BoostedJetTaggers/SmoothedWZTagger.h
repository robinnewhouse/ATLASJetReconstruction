// for editors : this file is -*- C++ -*-
#ifndef BOOSTEDJETSTAGGERS_SMOOTHEDWZTAGGER_H_
#define BOOSTEDJETSTAGGERS_SMOOTHEDWZTAGGER_H_

#include "BoostedJetTaggers/JSSTaggerBase.h"
#include "AsgTools/AsgTool.h"

class TF1;

////////////////////////////////////////////////
/// \class SmoothedWZTagger
///
/// Tag xAOD::Jet as decaying from a W or Z according to the mass and the D2 substructure variable of the jet.
/// Tagging results are obtain either with the IJSSTagger interface:
///     bool ok = tool->tag( jet );
/// Or full detailed results can be obtain using the 'result()' method which returns a Result enum:
///     SmoothedWZTagger::Result r = tool->result( jet );
///     if( r == SmoothedWZTagger::MassPassD2Fail ) { ... do things ... }
///     if( r & D2Pass ) { ... true if the D2 cut pass, whatever the Mass cut outcome was }
///
/// If the `decorate=true` argument is given to `result()` then the jet will be decorated with 3 floats corresponding
/// to the mass and D2 cut values which were tested for this jet.
/// The decorations are "WZLowWMassCut", "WZHighWMassCut" and "WZD2Cut".
///
/// The tool should be configured by
///   * setting the "ConfigFile" property to point to a relevant file
///   * OR by setting the "MassCutLowFunc" , "MassCutHighFunc" and "D2CutFunc" properties
///
/// See more info on :
///
//////////////////////////////////

class SmoothedWZTagger : public  JSSTaggerBase {
  ASG_TOOL_CLASS0(SmoothedWZTagger)
  public:

  enum Result {

    OutOfRangeHighPt = -3,
    OutOfRangeLowPt = -2,
    OutOfRangeEta = -1,
    InvalidJet = 0,

    AllPassed = 1,
    MassPassD2Pass = 1,

    MassPassD2Fail     = 2,
    LowMassFailD2Pass  = 4,
    LowMassFailD2Fail  = 8,
    HighMassFailD2Pass = 16,
    HighMassFailD2Fail = 32,


    MassPass = MassPassD2Fail | MassPassD2Pass,
    D2Pass   = MassPassD2Pass | HighMassFailD2Pass | LowMassFailD2Pass
  };


  //Default - so root can load based on a name
  SmoothedWZTagger(const std::string& name);

  // Default - so we can clean up
  ~SmoothedWZTagger();


  // Implement IJSSTagger interface
  virtual bool tag(const xAOD::Jet& jet, bool decorate) const {return (result(jet,decorate)==AllPassed); };


  // Run once at the start of the job to setup everything
  StatusCode initialize();

  // Get the tagging result -- run for every jet!
  Result result(const xAOD::Jet& jet, bool decorate=true) const ;

  StatusCode finalize();

  private:

    // need to set in initialization
    std::string m_wkpt;
    std::string m_tagType;
    std::string m_configFile;

    // the kinematic bounds for the jet - these are in MeV (not GeV!)
    float m_jetPtMin;
    float m_jetPtMax;
    float m_jetEtaMax;

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
