#include "BoostedJetTaggers/SubstructureTopTagger.h"

SubstructureTopTagger::SubstructureTopTagger(const std::string& name) : asg::AsgTool(name) 
                                                                      , m_taggerName("TopTag")
                                                                      , m_taggingSelectors()
                                                                      , m_ANDcombine(true){
  
  declareProperty("TaggerName",m_taggerName);
  declareProperty("SelectionTools",m_taggingSelectors);
  declareProperty("CombineWithAND", m_ANDcombine);
}


StatusCode SubstructureTopTagger::initialize(){
  ATH_MSG_INFO(" initialize ");
  return m_taggingSelectors.retrieve();
}



/// Actual toptagging implementation
bool SubstructureTopTagger::isTagged(const xAOD::Jet& jet) const {
  //std::cout << " SubstructureTopTagger::isTagged "<< jet.index() << "  numC="<< jet.numConstituents() << std::endl;
  if( jet.numConstituents() <=2 ) return false;
  if(m_ANDcombine) {
    for(const ToolHandle<IJetSelector>& sel : m_taggingSelectors){
      if(! bool(sel->keep(jet) ) ) return false;
    }
    return true;

  } else { // combine with OR
    for(const ToolHandle<IJetSelector>& sel : m_taggingSelectors){
      if( bool(sel->keep(jet) ) ) return true;
    }
    return false;    
  }

}


/// Returns 'isTagged(jet)'  and set the value as a jet attribute.
bool SubstructureTopTagger::doTag( xAOD::Jet& jet) const {

  bool tagged = isTagged(jet);
  jet.setAttribute<int>(m_taggerName, (int)tagged);
  return tagged;
}

int SubstructureTopTagger::modify(xAOD::JetContainer& jetCont) const {
  for( xAOD::Jet * jet : jetCont) { doTag(*jet) ; }
  return 0;
}
