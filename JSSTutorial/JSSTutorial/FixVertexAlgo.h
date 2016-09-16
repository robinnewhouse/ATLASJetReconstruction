// this file is -*- C++ -*-
#ifndef JSSTUTORIAL_FIXVERTEX_H
#define JSSTUTORIAL_FIXVERTEX_H

// algorithm wrapper
#include "xAODAnaHelpers/Algorithm.h"

/// \class JetGlobalOptionsAlgo
///
class FixVertexAlgo : public xAH::Algorithm
{

public:

  std::string m_optionString;      /// a python 'pickled' string representing a dictionnary to overwrite jetConfig.globalOptions
  

  ~FixVertexAlgo(){ }
private:

//FUNCTION DECLARATIONS
public:

  // this is a standard constructor
  FixVertexAlgo ();

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
  ClassDef(FixVertexAlgo, 1);
};

#endif
