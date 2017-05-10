// this file is -*- C++ -*-
#ifndef __SUBSTRUCTURETOPTAGGERHELPERS__
#define __SUBSTRUCTURETOPTAGGERHELPERS__
////////////////////////////////////////////////////
///
/// \file SubstructureTopTaggerHelpers.h
/// \brief helper functions to configure and implement top tagging tools.
///
///
/// This file has 2 purposes : 
/// - Define  classes which implement some very toptagging specific jet selection tools : smooth cuts on 
/// mass and Tau32 variables (TopTagSmoothMassCut and TopTagSmoothTau32 ). 
/// - a configuration function to be used in RootCore : configSubstTagger(). It allows to easily instantiates a configured
/// SubstructureTopTagger tool ready for use.
///
///  
///
/////////////////////////////////////////////////////
#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetSelector.h"
// The xAOD jet type
#include "xAODJet/Jet.h"
#include "BoostedJetTaggers/SubstructureTopTagger.h"

namespace STTHelpers {

  /////////////////////////////////////////////
  /// \class Interpolator
  ///  A minimal linear interpolation code used to compute
  /// pt-dependent cut values.
  struct Interpolator {

    float eval(float x) const ;

    void setValues(const std::vector<float>& x, const std::vector<float>& y);
    void setXValues(const std::vector<float>& x);
    void setYValues(const std::vector<float>& y);
    void initValues();

    protected:
    std::vector<float> m_xvalues,m_yvalues;
    float m_xmin,m_xmax,m_ymin,m_ymax;
  };



  class TopTagSmoothMassCut : public Interpolator
                            , public asg::AsgTool , virtual public IJetSelector {
    
    ASG_TOOL_CLASS(TopTagSmoothMassCut,IJetSelector)
    
    public: 
  
    /** Standard constructor */
    TopTagSmoothMassCut(const std::string& name="TopTagSmoothMassCut") ;

    /// IJetSelector interface
    virtual int keep(const xAOD::Jet& jet) const ;    

    // Initialize method 
    virtual StatusCode initialize() {initValues(); return StatusCode::SUCCESS;}

    /// low level Implementation
    int keep(double cal_pt, double cal_m) const ;
  };



  class TopTagSmoothTau32Cut : public Interpolator
                          , public asg::AsgTool , virtual public IJetSelector {
    
    ASG_TOOL_CLASS(TopTagSmoothTau32Cut,IJetSelector)
  
    public: 
  
    /** Standard constructor */
    TopTagSmoothTau32Cut(const std::string& name="TopTagSmoothTau32Cut") ;

    /// IJetSelector interface
    virtual int keep(const xAOD::Jet& jet) const ;

    // Initialize method 
    virtual StatusCode initialize() {initValues(); return StatusCode::SUCCESS;}

    /// low level Implementation    
    int keep( double pt, double tau32) const ;    
    int keep( double pt, double tau3, double tau2) const ;    

  };


  /// \fn configSubstTagger
  /// 
  /// Intended to be used in RootCore to help with configuration.
  /// returns a SubstructureTopTagger pointer (owned by caller). 
  /// 
  /// 3 forms of the function exists. The simplest and recommended form takes 2 arguments :
  /// \param tagname (string) : name of the tool (this is also what the tool will set as an attribute to tagged jets)
  /// \param taggingShortCut (string) : a key referring to a predefined top tagging strategy. 
  /// 
  ///  Predefined top tagging strategies are :
  ///    - SmoothCut_50  : pT dependent cut maintaining ~50% signal efficiency
  ///    - SmoothCut_80  : pT dependent cut maintaining ~80% signal efficiency
  ///    - FixedCut_LowPt_50 : 50% efficiency in pT<500GeV region
  ///    - FixedCut_LowPt_80 : 80% efficiency in pT<500GeV region
  ///    - FixedCut_HighPt_50 : 50% efficiency in pT>500GeV region
  ///    - FixedCut_HighPt_80 : 80% efficiency in pT>500GeV region
  ///
  /// Example : 
  ///    SubstructureTopTagger * t = configSubstTagger("LooseTopTag", "SmoothCut_80");
  /// 
  ///
  /// The 2 other forms allow to customize the tagging cuts. The 2nd parameter is then 
  /// \param cutdescs : either a vector<string> or a vector<vector<string> >. Each string : 
  ///    - describe a cut in the form "12.34<Att<45.67" (or "12.34<Att" or "Att<45.6"). Where "Att" must be a jet attribute or one of a known keys (known keys represent simple values calculated from raw attribute, for ex : "abs(eta)", see below for the list).  
  ///     - or a special shortcut refering to a known tagger (see below).
  /// 
  /// Cuts within a vector<string> are combined with a AND.
  /// Cuts represented by a vector<string> within a vector<vector<string> > are combined with a OR.
  ///
  /// Available attribute keys :
  ///   - "abs(eta)"
  ///   - "Tau32_wta" , "Tau21_wta" (n-subjettiness ratios).
  ///
  /// Available shortcuts : 
  ///   - "SmoothMassCut_50" and "SmoothTau32Cut_50" for pT-dependent cuts aiming at 50% signal efficiency
  ///   - "SmoothMassCut_80" and "SmoothTau32Cut_80" for pT-dependent cuts aiming at 80% signal efficiency
  ///   - one of the above with the "_higheta" or "_loweta" suffix : cuts optimized for jets in |eta|<1 or 1<|eta|<2.
  ///
  /// example usage  :
  ///  SubstructureTopTagger * t = configSubstTagger("MassAndTau32Tag", {"120000<m", "Tau32_wta<0.61"} );
  ///  SubstructureTopTagger * t = configSubstTagger("SmoothMTag", {"SmoothMassCut_50", "SmoothTau32Cut_50"} );
  ///
  /// // 1st and 2nd line below are combined with a OR 
  ///  SubstructureTopTagger * t = configSubstTagger("MassAndTau32Tag", 
  ///                                                {   {"abs(eta)<1","120000<m", "Tau32_wta<0.61"} ,    
  ///                                                    {"1<abs(eta)","120000<m", "Tau32_wta<0.61"} } );


  SubstructureTopTagger * configSubstTagger(const std::string &tagname, const std::string & taggingShortCut);
  SubstructureTopTagger * configSubstTagger(const std::string &tagname, const std::vector<std::string> & cutdescs);
  SubstructureTopTagger * configSubstTagger(const std::string &tagname, const std::vector<std::vector<std::string> > & cutdescsVec);



}
#endif
