// for editors : this file is -*- C++ -*-
#ifndef BOOSTEDJETSTAGGERS_SMOOTHEDTOPTAGGER_H_
#define BOOSTEDJETSTAGGERS_SMOOTHEDTOPTAGGER_H_

#include "BoostedJetTaggers/JSSTaggerBase.h"
#include "AsgTools/AsgTool.h"


class TF1;

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

class SmoothedTopTagger : public JSSTaggerBase {
  ASG_TOOL_CLASS0(SmoothedTopTagger)
  public:

  // Top tagging can run in different mode.
  //  the mode is determined by the configuration.
  enum Mode {
    MassTau32,
    MassSplit23,
    Tau32Split23,
    QwTau32,
  };


  /// detailed information on the result
  class Result {
  public:
    enum Validity {
      Ok = 1, // tagging procedure proceeded normally.
      OutOfRangeHighPt = -3,
      OutOfRangeLowPt = -2,
      OutOfRangeEta = -1,
      InvalidJet = 0, //  Jet is missing variable or is corrupted
    };

    Result(Validity v, Mode m) : m_v1(false), m_v2(false), m_mode(m), m_validity(v) {}
    Result( bool v1, bool v2, Mode m) : m_v1(v1), m_v2(v2), m_mode(m), m_validity(Ok) {}



    operator bool() const  {return allPassed();}
    bool allPassed() const {return (m_v1&&m_v2);}
    bool outOfRange()const {return (m_validity==OutOfRangeEta || m_validity==OutOfRangeLowPt || m_validity==OutOfRangeHighPt);}
    bool outOfRangeEta()const {return m_validity==OutOfRangeEta;}
    bool outOfRangePtLow()const {return m_validity==OutOfRangeLowPt;}
    bool outOfRangePtHigh()const {return m_validity==OutOfRangeHighPt;}
    bool invalidJet()const {return m_validity==InvalidJet;}
    bool ok() const {return m_validity==Ok;}

    Validity getValidity() const {return m_validity;}

    bool massPassed() const;
    bool split23Passed() const;
    bool tau32Passed() const ;
    bool qwPassed() const;

  protected:

    bool m_v1, m_v2;
    Mode m_mode;
    Validity m_validity;
  };

  //Default - so root can load based on a name
  SmoothedTopTagger(const std::string& name);

  ~SmoothedTopTagger();

  // implement IJSSTagger interface. Returns true if all cuts pass
  virtual bool tag(const xAOD::Jet& jet, bool decorate) const {return bool( result(jet,decorate) ); };

  // Run once at the start of the job to setup everything
  StatusCode initialize();

  // Get the tagging result -- run for every jet!
  Result result(const xAOD::Jet& jet, bool decorate=true) const;

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

  // the kinematic bounds for the jet - these are in MeV (not GeV!)
  float m_jetPtMin;
  float m_jetPtMax;
  float m_jetEtaMax;


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
