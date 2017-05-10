// for editors : this file is -*- C++ -*-
#ifndef BOOSTEDJETSTAGGERS_IJSSTAGGER_H_
#define BOOSTEDJETSTAGGERS_IJSSTAGGER_H_

#include "AsgTools/IAsgTool.h"

#include "xAODJet/Jet.h"


class IJSSTagger :  virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IJSSTagger)

  public:

  virtual bool tag(const xAOD::Jet& jet, bool decorate=true) const = 0;


};

#endif
