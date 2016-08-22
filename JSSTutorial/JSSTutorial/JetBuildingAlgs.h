// this file is -*- C++ -*-
#ifndef JETBUILDINGALGS_JETBUILDINGALGS_H
#define JETBUILDINGALGS_JETBUILDINGALGS_H

// algorithm wrapper
#include "xAODAnaHelpers/Algorithm.h"

#include "JetInterface/IJetExecuteTool.h"

/// \class JetExecuteToolAlgo
///
/// This class simply holds a IJetExecuteTool and call its execute() method.
/// the IJetExecuteTool concrete class is specified by m_className
/// The IJetExecuteTool is from a python script and/or call specified by m_configScript and m_configCall.
///  The script and call are executed during initialize(). Within the script and call the tool to be configured is 
/// available under the python variable 'tool'.
class JetExecuteToolAlgo : public xAH::Algorithm
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.

  // allpublic variables are configurable and do not have //!
  // all private and protected variables are not configurable and need //!

public:

  IJetExecuteTool * m_jetRecTool; //!
  std::string m_className;      /// the type of the tool to be excecuted
  std::string m_configScript;   /// a python script to be executed to configure the JetRecTool
  std::string m_configCall;     /// a python expression (expected to be a function call like "myconfig(wrapper.jetRecTool, ...)") 
                                /// will be executed after m_configScript

private:

//FUNCTION DECLARATIONS
public:

  // this is a standard constructor
  JetExecuteToolAlgo ();

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();


  // this is needed to distribute the algorithm to the workers
  ClassDef(JetExecuteToolAlgo, 1);
};


/// \class JetRecToolAlgo
///
/// Exactly identical to JetExecuteToolAlgo, except the class name of the tool
/// is JetRecTool and does not need to be configured.
class JetRecToolAlgo : public JetExecuteToolAlgo {
public:
  JetRecToolAlgo() {m_className = "JetRecTool";}
  // this is needed to distribute the algorithm to the workers
  ClassDef(JetRecToolAlgo, 1);

};
#endif
