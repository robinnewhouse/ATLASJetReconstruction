#ifndef JSSTutorial_JSSTutorial_H
#define JSSTutorial_JSSTutorial_H

// EL include(s):
#include <EventLoop/StatusCode.h>
#include <EventLoop/Algorithm.h>

// EDM include(s):
#include <xAODEventInfo/EventInfo.h>
#include <xAODJet/JetContainer.h>
#include <xAODJet/JetAuxContainer.h>
#include <xAODBase/IParticle.h>
#include <xAODBase/IParticleContainer.h>

// For the cluster and tracks information
#include "xAODJet/JetConstituentVector.h"
#include "xAODBTagging/BTagging.h"

// for retrieving of topocluster container
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

// Infrastructure include(s):
#include <xAODRootAccess/Init.h>
#include <xAODRootAccess/TEvent.h>
#include <xAODRootAccess/TStore.h>

// algorithm wrapper
#include <xAODAnaHelpers/Algorithm.h>
#include <xAODAnaHelpers/TreeAlgo.h>

// for event count
#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetAttributes.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"

// Subjet Calibration
#include "JetCalibTools/JetCalibrationTool.h"

// for Soft Drop
#include "fastjet/contrib/SoftDrop.hh"
#include "fastjet/ClusterSequence.hh"

// for Trimming
#include <fastjet/tools/Filter.hh>

// for Pruning
#include <fastjet/tools/Pruner.hh>

// for ECF
#include <fastjet/contrib/EnergyCorrelator.hh>

// for NSubjettiness
#include <fastjet/contrib/Nsubjettiness.hh>

// for Shower Deconstruction
#include "ShowerDeconstruction/ShowerDeconstruction.h"


// root includes
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TTree.h"
#include <TFile.h>
#include <TLorentzVector.h>

class JSSTutorialAlgo : public TreeAlgo
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
  TH1D *h_jet_Ungroomed_pt; //!
  TH1D *h_jet_Ungroomed_m; //!
  TH1D *h_jet_Ungroomed_d2; //!
  TH1D *h_jet_Ungroomed_tau32; //!

  TH1D *h_jet_Trimmed_pt; //!
  TH1D *h_jet_Trimmed_m; //!
  TH1D *h_jet_Trimmed_d2; //!
  TH1D *h_jet_Trimmed_tau32; //!

  TH1D *h_jet_Pruned_pt; //!
  TH1D *h_jet_Pruned_m; //!
  TH1D *h_jet_Pruned_d2; //!
  TH1D *h_jet_Pruned_tau32; //!

  TH1D *h_jet_SoftDrop_pt; //!
  TH1D *h_jet_SoftDrop_m; //!
  TH1D *h_jet_SoftDrop_d2; //!
  TH1D *h_jet_SoftDrop_tau32; //!

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

  // Subjet Calibration
  JetCalibrationTool* m_jetCalibration_subjet; //!

//FUNCTION DECLARATIONS
public:

  // this is a standard constructor
  JSSTutorialAlgo ();

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
  ClassDef(JSSTutorialAlgo, 1);
};

#endif
