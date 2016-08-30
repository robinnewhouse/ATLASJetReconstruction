#ifndef JSSTutorial_JSSMINIXAOD_H
#define JSSTutorial_JSSMINIXAOD_H

#include <vector>
#include <string>
// algorithm wrapper
#include <xAODAnaHelpers/MinixAOD.h>

class JSSMinixAOD : public MinixAOD
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.

  // allpublic variables are configurable and do not have //!
  // all private and protected variables are not configurable and need //!

public:

  // configuration variables
  std::vector<std::string> m_selectedAuxVars;

public:

  // this is a standard constructor
  JSSMinixAOD ();

  virtual EL::StatusCode initialize ();

  // this is needed to distribute the algorithm to the workers
  ClassDef(JSSMinixAOD, 1);
};

#endif
