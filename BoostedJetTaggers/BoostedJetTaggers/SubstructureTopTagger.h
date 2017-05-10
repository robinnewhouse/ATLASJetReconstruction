// this file is -*- C++ -*-
#ifndef __SUBSTRUCTURETOPTAGGER__
#define __SUBSTRUCTURETOPTAGGER__
/////////////////////////////////////////////////////
/// \class SubstructureTopTagger
/// \brief Tag a jet as a top hadronic decay using jet substructure variable.
///
///  This tool is actually a high level tool : it performs top tagging by combining 
/// several conditions (ex: '100GeV<mass', 'Tau32<0.5') in a logical AND.
/// The conditions are implemented through IJetSelector instances in a configurable array 
/// of tools (property : "SelectionTools"). 
/// 
/// The tool also implements the ISingleJetModifier interface : it is able to set a jet attribute : an int equals 
/// to 1 if the jet is tagged. (see 'doTag()')
/// 
/// For configuration in RootCore, see the helper functions in SubstructureTopTaggerHelpers.h.
///
///  
////////////////////////////////////////////////////////

// Base classes
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "JetInterface/IJetSelector.h"
//#include "JetInterface/ISingleJetModifier.h"
#include "JetInterface/IJetModifier.h"


class SubstructureTopTagger : public asg::AsgTool , virtual public IJetSelector, virtual public IJetModifier {
  
  ASG_TOOL_CLASS2(SubstructureTopTagger,IJetSelector, IJetModifier)
  
public: 
  
  /** Standard constructor */
  SubstructureTopTagger(const std::string& name="SubstructureTopTagger");
  
  
  /** Standard destructor */
  ~SubstructureTopTagger() {}
  
  /** Initialize method */
  virtual StatusCode initialize();
  
  /// IJetSelector interface (equivalent to 'isTagged(jet)')
  virtual int keep(const xAOD::Jet& jet) const {return isTagged(jet);};

  /// IJetModifier interface
  virtual int modify(xAOD::JetContainer& jetCont) const;
  

  /// Actual toptagging implementation
  bool isTagged(const xAOD::Jet& jet) const ;

  /// Returns 'isTagged(jet)'  and set the value as a jet attribute.
  bool doTag( xAOD::Jet& jet) const ;
  

protected:
  std::string m_taggerName;    
  ToolHandleArray<IJetSelector> m_taggingSelectors; /// This tool just performs a AND of all these tools
  bool m_ANDcombine;

}; // End: class definition

#endif
