// for editors : this file is -*- C++ -*- 
#ifndef BOOSTEDJETSTAGGERS_JSSTAGGERBase_H_
#define BOOSTEDJETSTAGGERS_JSSTAGGERBase_H_


#include "AsgTools/AsgTool.h"
#include "BoostedJetTaggers/IJSSTagger.h"
#include "JetInterface/IJetSelector.h"

class JSSTaggerBase :   public asg::AsgTool ,
                        virtual public IJSSTagger,
                        virtual public IJetSelector {
  ASG_TOOL_CLASS2(JSSTaggerBase, IJSSTagger, IJetSelector )
  
  public:
  
  JSSTaggerBase(const std::string &name);
  
  // provide a default implementation for IJetSelector::keep :
  virtual int keep(const xAOD::Jet& jet) const {return tag(jet,false);}
  
  
};

#endif
