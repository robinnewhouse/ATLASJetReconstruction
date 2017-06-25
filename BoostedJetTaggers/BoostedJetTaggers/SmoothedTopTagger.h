// for editors : this file is -*- C++ -*-
#ifndef BOOSTEDJETSTAGGERS_SMOOTHEDTOPTAGGER_H_
#define BOOSTEDJETSTAGGERS_SMOOTHEDTOPTAGGER_H_

////////////////////////////////////////////////
/// \class SmoothedTopTagger
///
/// Tag xAOD::Jet as decaying from a top according to some substructure variables of the jet.
/// Which variables are used depends on the configuration of the tool.
/// Currently 3 pairs of variable are supported : (mass, tau32), (Qw, tau32) and (tau32,split23).
///
/// Tagging results are obtain either with the IJSSTagger interface:
///     bool ok = tool->tag( jet );
/// Or full detailed results can be obtain using the 'result()' method and the helper class:
///     SmoothedTopTagger::Result r = tool->result( jet );
///     if( ! r.massPassed() ) { ... do things ... }
///     if( r ) { ... r evaluates to true if the jet passed all cuts }
///
///
/// If the `decorate=true` argument is given to `result()` then the jet will be decorated with 2 floats corresponding
/// to the cut values which were tested for this jet.
/// The decorations depends on pair of variable used and have the form "TopTagXXXCut" where XXX is
/// "Mass", "Tau32" ot "Split23"
///
/// The tool should be configured by
///   * setting the "ConfigFile" property to point to a relevant file or
///   * OR setting the "TaggerMode" , "Var1CutFunc" and "Var2CutFunc" properties.
/// See more info on :
///
//////////////////////////////////

#include "BoostedJetTaggers/JSSTaggerBase.h"
#include "AsgTools/AsgTool.h"

class TF1;

class SmoothedTopTagger : public JSSTaggerBase {
  ASG_TOOL_CLASS0(SmoothedTopTagger)
  public:

  // Top tagging can run in different mode.
  // the mode is determined by the configuration.
  enum Mode {
    MassTau32,
    MassSplit23,
    Tau32Split23,
    QwTau32,
  };

  //Default - so root can load based on a name
  SmoothedTopTagger(const std::string& name);

  ~SmoothedTopTagger();

  // implement IJSSTagger interface. Returns true if all cuts pass
  virtual Root::TAccept tag(const xAOD::Jet& jet) const;

  // Run once at the start of the job to setup everything
  StatusCode initialize();

  StatusCode finalize();

  // returns the name of substructure variables used by this tool
  std::string var1() const {return m_var1CutName;}
  std::string var2() const {return m_var2CutName;}

  private:

  // used to build tau32 out of tau3 and tau2
  float buildTau32(const xAOD::Jet& j) const ;

  // need to set in initialization
  std::string m_wkpt;
  std::string m_configFile;

  // parameters for smooth fits
  std::string m_var1CutExpr;
  std::string m_var2CutExpr;

  // name of the variable used by this tool (ex: Mass, Tau32)
  std::string m_var1CutName;
  std::string m_var2CutName;

  // functions implementing the cut values vs pt
  // they are build from m_varXCutExpr
  TF1* m_var1CutFunc=0;
  TF1* m_var2CutFunc=0;

  // mode in which the tool is operating (ex: MassTau32 or Tau32Split23)
  std::string m_modeName;
  Mode m_mode;


};

#endif
