#ifndef JSSTutorial_JSSTutorialJETPYTHONCONFIGALGO_H
#define JSSTutorial_JSSTutorialJETPYTHONCONFIGALGO_H


// EDM include(s):
#include <xAODEventInfo/EventInfo.h>
#include <xAODJet/JetContainer.h>
#include <xAODJet/JetAuxContainer.h>


// algorithm wrapper
#include <xAODAnaHelpers/TreeAlgo.h>


// root includes
#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"
#include <TLorentzVector.h>

class JSSTutorialPythonConfigAlgo : public TreeAlgo
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.

  // allpublic variables are configurable and do not have //!
  // all private and protected variables are not configurable and need //!

public:

  // configuration variables
  std::string m_MyNewVariable;
  std::string m_TreeName;
  

private:

  // event counter
  int m_eventCounter; //!
  TH1D *h_EventCounter; //!

  // histograms to save output
  TH1D *h_jet_pyconfig_Ungroomed_pt; //!
  TH1D *h_jet_pyconfig_Ungroomed_m; //!
  TH1D *h_jet_pyconfig_Ungroomed_d2; //!
  TH1D *h_jet_pyconfig_Ungroomed_tau32; //!

  TH1D *h_jet_pyconfig_Trimmed_pt; //!
  TH1D *h_jet_pyconfig_Trimmed_m; //!
  TH1D *h_jet_pyconfig_Trimmed_d2; //!
  TH1D *h_jet_pyconfig_Trimmed_tau32; //!

  TH1D *h_jet_pyconfig_Pruned_pt; //!
  TH1D *h_jet_pyconfig_Pruned_m; //!
  TH1D *h_jet_pyconfig_Pruned_d2; //!
  TH1D *h_jet_pyconfig_Pruned_tau32; //!

  TH1D *h_jet_pyconfig_SoftDrop_pt; //!
  TH1D *h_jet_pyconfig_SoftDrop_m; //!
  TH1D *h_jet_pyconfig_SoftDrop_d2; //!
  TH1D *h_jet_pyconfig_SoftDrop_tau32; //!

  // TTree to save output results
  TTree *outTree;   //!

  double tvar_jet_Ungroomed_pt; //!
  double tvar_jet_Ungroomed_m; //!
  double tvar_jet_Ungroomed_d2; //!
  double tvar_jet_Ungroomed_tau32; //!
  double tvar_jet_Trimmed_pt; //!
  double tvar_jet_Trimmed_m; //!
  double tvar_jet_Trimmed_d2; //!
  double tvar_jet_Trimmed_tau32; //!
  double tvar_jet_Pruned_pt; //!
  double tvar_jet_Pruned_m; //!
  double tvar_jet_Pruned_d2; //!
  double tvar_jet_Pruned_tau32; //!
  double tvar_jet_SoftDrop_pt; //!
  double tvar_jet_SoftDrop_m; //!
  double tvar_jet_SoftDrop_d2; //!
  double tvar_jet_SoftDrop_tau32; //!

  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker
  // node (done by the //!)


//FUNCTION DECLARATIONS
public:

  // this is a standard constructor
  JSSTutorialPythonConfigAlgo ();

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
  ClassDef(JSSTutorialPythonConfigAlgo, 1);
};

#endif
