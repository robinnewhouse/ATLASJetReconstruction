// EL include(s):
#include <EventLoop/Job.h>
#include <EventLoop/Worker.h>
#include <EventLoop/OutputStream.h>

// EDM include(s):
#include <xAODEventInfo/EventInfo.h>
#include <xAODJet/JetContainer.h>


// for event count
#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"

#include <xAODAnaHelpers/HelperFunctions.h>
#include <xAODAnaHelpers/tools/ReturnCheck.h>



#include <JSSTutorial/JSSTutorialPythonConfigAlgo.h>

// c++ include(s)
//#include <stdexcept>

// this is needed to distribute the algorithm to the workers
ClassImp(JSSTutorialPythonConfigAlgo)

/// This macro simplify the StatusCode checking
#define SC_CHECK( exp ) RETURN_CHECK("JSSTutorialPythonConfigAlgo", exp , "" )
//#define SC_CHECK( exp ) { EL::StatusCode sc= (EL::StatusCode) exp; if(sc==EL::StatusCode::FAILURE) {s//td::cout<< "Config  ERROR   StatusCode failure. "<< std::endl; return sc;}  }



JSSTutorialPythonConfigAlgo :: JSSTutorialPythonConfigAlgo ()
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().

  msg().setName( "JSSTutorialPythonConfigAlgo" );

  ATH_MSG_INFO("Calling constructor");

  m_MyNewVariable = "";
  m_TreeName = "";
}

EL::StatusCode JSSTutorialPythonConfigAlgo :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.

  job.useXAOD();
  xAOD::Init( "JSSTutorialPythonConfigAlgo" ).ignore(); // call before opening first file

  EL::OutputStream outForTree("tree_pyconfig");
  job.outputAdd (outForTree);

  return EL::StatusCode::SUCCESS;

}

EL::StatusCode JSSTutorialPythonConfigAlgo :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.
  ATH_MSG_INFO( "Calling histInitialize");

  std::cout<<"Printing the observable you loaded in (m_MyNewVariable) : "<<m_MyNewVariable<<std::endl;

  // Number of events
  h_EventCounter = new TH1D("h_EventCounter","h_EventCounter",10,0,10);
  wk()->addOutput (h_EventCounter);

  //////////////////////////
  // Histograms
  //////////////////////////
  h_jet_pyconfig_Ungroomed_pt    = new TH1D("h_jet_pyconfig_Ungroomed_pt",   "h_jet_pyconfig_Ungroomed_pt",100,0,1000);
  h_jet_pyconfig_Ungroomed_m     = new TH1D("h_jet_pyconfig_Ungroomed_m",    "h_jet_pyconfig_Ungroomed_m",100,0,1000);
  h_jet_pyconfig_Ungroomed_d2    = new TH1D("h_jet_pyconfig_Ungroomed_d2",   "h_jet_pyconfig_Ungroomed_d2",100,0,10);
  h_jet_pyconfig_Ungroomed_tau32 = new TH1D("h_jet_pyconfig_Ungroomed_tau32","h_jet_pyconfig_Ungroomed_tau32",100,0,10);

  h_jet_pyconfig_Trimmed_pt      = new TH1D("h_jet_pyconfig_Trimmed_pt",    "h_jet_pyconfig_Trimmed_pt",100,0,1000);
  h_jet_pyconfig_Trimmed_m       = new TH1D("h_jet_pyconfig_Trimmed_m",     "h_jet_pyconfig_Trimmed_m",100,0,1000);
  h_jet_pyconfig_Trimmed_d2      = new TH1D("h_jet_pyconfig_Trimmed_d2",    "h_jet_pyconfig_Trimmed_d2",100,0,10);
  h_jet_pyconfig_Trimmed_tau32   = new TH1D("h_jet_pyconfig_Trimmed_tau32", "h_jet_pyconfig_Trimmed_tau32",100,0,10);

  h_jet_pyconfig_Pruned_pt       = new TH1D("h_jet_pyconfig_Pruned_pt",    "h_jet_pyconfig_Pruned_pt",100,0,1000);
  h_jet_pyconfig_Pruned_m        = new TH1D("h_jet_pyconfig_Pruned_m",     "h_jet_pyconfig_Pruned_m",100,0,1000);
  h_jet_pyconfig_Pruned_d2       = new TH1D("h_jet_pyconfig_Pruned_d2",    "h_jet_pyconfig_Pruned_d2",100,0,10);
  h_jet_pyconfig_Pruned_tau32    = new TH1D("h_jet_pyconfig_Pruned_tau32", "h_jet_pyconfig_Pruned_tau32",100,0,10);

  h_jet_pyconfig_SoftDrop_pt     = new TH1D("h_jet_pyconfig_SoftDrop_pt",    "h_jet_pyconfig_SoftDrop_pt",100,0,1000);
  h_jet_pyconfig_SoftDrop_m      = new TH1D("h_jet_pyconfig_SoftDrop_m",     "h_jet_pyconfig_SoftDrop_m",100,0,1000);
  h_jet_pyconfig_SoftDrop_d2     = new TH1D("h_jet_pyconfig_SoftDrop_d2",    "h_jet_pyconfig_SoftDrop_d2",100,0,10);
  h_jet_pyconfig_SoftDrop_tau32  = new TH1D("h_jet_pyconfig_SoftDrop_tau32", "h_jet_pyconfig_SoftDrop_tau32",100,0,10);

  // Add them so they automatically appear in output file
  wk()->addOutput (h_jet_pyconfig_Ungroomed_pt);
  wk()->addOutput (h_jet_pyconfig_Ungroomed_m);
  wk()->addOutput (h_jet_pyconfig_Ungroomed_d2);
  wk()->addOutput (h_jet_pyconfig_Ungroomed_tau32);

  wk()->addOutput (h_jet_pyconfig_Trimmed_pt);
  wk()->addOutput (h_jet_pyconfig_Trimmed_m);
  wk()->addOutput (h_jet_pyconfig_Trimmed_d2);
  wk()->addOutput (h_jet_pyconfig_Trimmed_tau32);

  wk()->addOutput (h_jet_pyconfig_Pruned_pt);
  wk()->addOutput (h_jet_pyconfig_Pruned_m);
  wk()->addOutput (h_jet_pyconfig_Pruned_d2);
  wk()->addOutput (h_jet_pyconfig_Pruned_tau32);

  wk()->addOutput (h_jet_pyconfig_SoftDrop_pt);
  wk()->addOutput (h_jet_pyconfig_SoftDrop_m);
  wk()->addOutput (h_jet_pyconfig_SoftDrop_d2);
  wk()->addOutput (h_jet_pyconfig_SoftDrop_tau32);

  //////////////////////////
  // Tree
  // You need to do this process to link the tree to the output
  //////////////////////////
  //std::cout<<"Trying to initialize tree stuff"<<std::endl;
  // get the file we created already
  // get the file we created already
  TFile* treeFile = wk()->getOutputFile ("tree_pyconfig");
  treeFile->mkdir("TreeDirectory");
  treeFile->cd("TreeDirectory");

  outTree = new TTree(m_TreeName.c_str(),m_TreeName.c_str());

  outTree->Branch("tvar_jet_Ungroomed_pt",    &tvar_jet_Ungroomed_pt);
  outTree->Branch("tvar_jet_Ungroomed_m",     &tvar_jet_Ungroomed_m);
  outTree->Branch("tvar_jet_Ungroomed_d2",    &tvar_jet_Ungroomed_d2);
  outTree->Branch("tvar_jet_Ungroomed_tau32", &tvar_jet_Ungroomed_tau32);
  outTree->Branch("tvar_jet_Trimmed_pt",      &tvar_jet_Trimmed_pt);
  outTree->Branch("tvar_jet_Trimmed_m",       &tvar_jet_Trimmed_m);
  outTree->Branch("tvar_jet_Trimmed_d2",      &tvar_jet_Trimmed_d2);
  outTree->Branch("tvar_jet_Trimmed_tau32",   &tvar_jet_Trimmed_tau32);
  outTree->Branch("tvar_jet_Pruned_pt",       &tvar_jet_Pruned_pt);
  outTree->Branch("tvar_jet_Pruned_m",        &tvar_jet_Pruned_m);
  outTree->Branch("tvar_jet_Pruned_d2",       &tvar_jet_Pruned_d2);
  outTree->Branch("tvar_jet_Pruned_tau32",    &tvar_jet_Pruned_tau32);
  outTree->Branch("tvar_jet_SoftDrop_pt",     &tvar_jet_SoftDrop_pt);
  outTree->Branch("tvar_jet_SoftDrop_m",      &tvar_jet_SoftDrop_m);
  outTree->Branch("tvar_jet_SoftDrop_d2",     &tvar_jet_SoftDrop_d2);
  outTree->Branch("tvar_jet_SoftDrop_tau32",  &tvar_jet_SoftDrop_tau32);

  //add the TTree to the output
  outTree->SetDirectory( treeFile->GetDirectory("TreeDirectory") );






  return EL::StatusCode::SUCCESS;

}

EL::StatusCode JSSTutorialPythonConfigAlgo :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed


  m_event = wk()->xaodEvent();
  // get the MetaData tree once a new file is opened, with
  TTree *MetaData = dynamic_cast<TTree*>(wk()->inputFile()->Get("MetaData"));
  if (!MetaData) {
    ATH_MSG_ERROR( "MetaData not found! Exiting.");
    return EL::StatusCode::FAILURE;
  }
  MetaData->LoadTree(0);
  //check if file is from a DxAOD
  bool m_isDerivation = !MetaData->GetBranch("StreamAOD");

  if(m_isDerivation ){
    // check for corruption
    const xAOD::CutBookkeeperContainer* incompleteCBC = nullptr;
    if(!m_event->retrieveMetaInput(incompleteCBC, "IncompleteCutBookkeepers").isSuccess()){
      ATH_MSG_WARNING("Failed to retrieve IncompleteCutBookkeepers from MetaData! Exiting.");
      //return EL::StatusCode::FAILURE;
    }
    if ( incompleteCBC->size() != 0 ) {
      ATH_MSG_WARNING("Found incomplete Bookkeepers! Check file for corruption.");
      //return EL::StatusCode::FAILURE;
    }

    // Now, let's find the actual information
    const xAOD::CutBookkeeperContainer* completeCBC = 0;
    if(!m_event->retrieveMetaInput(completeCBC, "CutBookkeepers").isSuccess()){
      ATH_MSG_WARNING("Failed to retrieve CutBookkeepers from MetaData! Exiting.");
      //return EL::StatusCode::FAILURE;
    }
    // Now, let's actually find the right one that contains all the needed info...
    const xAOD::CutBookkeeper* allEventsCBK = 0;
    int maxCycle = -1;
    for (const auto& cbk: *completeCBC) {
      if (cbk->cycle() > maxCycle && cbk->name() == "AllExecutedEvents" && cbk->inputStream() == "StreamAOD") {
        allEventsCBK = cbk;
        maxCycle = cbk->cycle();
      }
    }
    uint64_t nEventsProcessed  = allEventsCBK->nAcceptedEvents();
    double sumOfWeights        = allEventsCBK->sumOfEventWeights();
    double sumOfWeightsSquared = allEventsCBK->sumOfEventWeightsSquared();

    h_EventCounter->Fill(0.0,(int)nEventsProcessed);
    h_EventCounter->Fill(1.0,sumOfWeights);
    h_EventCounter->Fill(2.0,sumOfWeightsSquared);
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode JSSTutorialPythonConfigAlgo :: changeInput (bool /*firstFile*/)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.

  return EL::StatusCode::SUCCESS;

}

EL::StatusCode JSSTutorialPythonConfigAlgo :: initialize ()
{
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the outputif you have no
  // input events.

  ATH_MSG_INFO( m_name.c_str());
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();

  // count number of events
  m_eventCounter = 0;

  ATH_MSG_INFO( "JSSTutorialPythonConfigAlgo Interface succesfully initialized!" );
  return EL::StatusCode::SUCCESS;
}


EL::StatusCode JSSTutorialPythonConfigAlgo :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.

  if ( m_debug ) { ATH_MSG_INFO( "Executing JSSTutorialPythonConfigAlgo..."); }

  if ( (m_eventCounter % 5000) == 0 ) {
    ATH_MSG_INFO("processed " << m_eventCounter <<"  events");
  }

  ++m_eventCounter;


  // Let's retrieve them

  const xAOD::JetContainer* ungroomedJets;
  RETURN_CHECK("JSSTutorialAlgo::execute()", HelperFunctions::retrieve(ungroomedJets, "AntiKt10LCTopoJets2", m_event, m_store,  m_verbose), "");

  const xAOD::JetContainer* trimmedJets;
  RETURN_CHECK("JSSTutorialAlgo::execute()", HelperFunctions::retrieve(trimmedJets, "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets2", m_event, m_store,  m_verbose), "");

  const xAOD::JetContainer* prunedJets;
  RETURN_CHECK("JSSTutorialAlgo::execute()", HelperFunctions::retrieve(prunedJets, "AntiKt10LCTopoPrunedZcut1Rcut5Jets2", m_event, m_store,  m_verbose), "");
  


  // define some accessors (faster way to retrieve attributes)
  SG::AuxElement::Accessor<float> tau32_acc("Tau32_wta");
  SG::AuxElement::Accessor<float> D2_acc("D2");

  //loop over the  jets
  for(const xAOD::Jet* jet : * ungroomedJets ){

    //only examine jets above 200 GeV
    // if(jet->pt()<200000.0)
    //  continue;
    // NOT necessary since we configured a pt filter in the jet rec sequence above
    
    std::cout<<std::endl<<"JetPt : "<<jet->pt()<<std::endl;
    
    // Ungroomed
    std::cout<<"Ungroomed(mass):         "<<jet->m()<<std::endl;
    
    h_jet_pyconfig_Ungroomed_pt   ->Fill(jet->pt()/1000.0);
    h_jet_pyconfig_Ungroomed_m    ->Fill(jet->m()/1000.0);
    h_jet_pyconfig_Ungroomed_d2   ->Fill(D2_acc(*jet) );
    h_jet_pyconfig_Ungroomed_tau32->Fill(tau32_acc(*jet) );

    tvar_jet_Ungroomed_pt    = jet->pt()/1000.0;
    tvar_jet_Ungroomed_m     = jet->m()/1000.0;
    tvar_jet_Ungroomed_d2    = D2_acc(*jet);
    tvar_jet_Ungroomed_tau32 = tau32_acc(*jet);
    
  }

  //loop over the trimmed  jets
  for(const xAOD::Jet* jet : * trimmedJets ){

    // //only examine jets above 200 GeV
    // if(jet->pt()<200000.0)
    //  continue;
    // NOT necessary since we configured a pt filter in the jet rec sequence above
    
    std::cout<<"Trimmed(mass):         "<<jet->m()<<std::endl;

    float d2 = D2_acc(*jet);
    float tau32 = tau32_acc(*jet);

    h_jet_pyconfig_Trimmed_pt   ->Fill(jet->pt()/1000.0);
    h_jet_pyconfig_Trimmed_m    ->Fill(jet->m()/1000.0);
    h_jet_pyconfig_Trimmed_d2   ->Fill(d2);
    h_jet_pyconfig_Trimmed_tau32->Fill(tau32);
    
    tvar_jet_Trimmed_pt    = jet->pt()/1000.0;
    tvar_jet_Trimmed_m     = jet->m()/1000.0;
    tvar_jet_Trimmed_d2    = d2;
    tvar_jet_Trimmed_tau32 = tau32;
    
    // if needed, we could retrieve the parent jet of this trimmed 
    // jet by :
    // const xAOD::Jet* parent = jet->getAssociatedObject<xAOD::Jet>("Parent");

    
  }

  //loop over the runed  jets
  for(const xAOD::Jet* jet : * prunedJets ){

    // //only examine jets above 200 GeV
    // if(jet->pt()<200000.0)
    //  continue;
    // NOT necessary since we configured a pt filter in the jet rec sequence above
    
    std::cout<<"Pruned(mass):         "<<jet->m()<<std::endl;

    float d2 = D2_acc(*jet);
    float tau32 = tau32_acc(*jet);

    h_jet_pyconfig_Pruned_pt   ->Fill(jet->pt()/1000.0);
    h_jet_pyconfig_Pruned_m    ->Fill(jet->m()/1000.0);
    h_jet_pyconfig_Pruned_d2   ->Fill(d2);
    h_jet_pyconfig_Pruned_tau32->Fill(tau32);
    
    tvar_jet_Pruned_pt    = jet->pt()/1000.0;
    tvar_jet_Pruned_m     = jet->m()/1000.0;
    tvar_jet_Pruned_d2    = d2;
    tvar_jet_Pruned_tau32 = tau32;
    
    // if needed, we could retrieve the parent jet of this pruned 
    // jet by :
    // const xAOD::Jet* parent = jet->getAssociatedObject<xAOD::Jet>("Parent");

    
  }

  



  //   //SoftDrop
  //   fastjet::PseudoJet jet_SoftDrop;
  //   jet_SoftDrop = tool_SoftDrop(jet_Ungroomed);
  //   std::cout<<"SoftDrop(mass):      "<<jet_SoftDrop.m()<<std::endl;
  //   std::cout<<"ECF1:      "<<ECF1(jet_SoftDrop)<<std::endl;
  //   std::cout<<"ECF2:      "<<ECF2(jet_SoftDrop)<<std::endl;
  //   std::cout<<"ECF3:      "<<ECF3(jet_SoftDrop)<<std::endl;
  //   std::cout<<"Tau1WTA:   "<<nSub1_beta1(jet_SoftDrop)<<std::endl;
  //   std::cout<<"Tau2WTA:   "<<nSub2_beta1(jet_SoftDrop)<<std::endl;
  //   std::cout<<"Tau3WTA:   "<<nSub3_beta1(jet_SoftDrop)<<std::endl;

  //   d2    = ECF3(jet_SoftDrop) * pow(ECF1(jet_SoftDrop),3) / pow(ECF2(jet_SoftDrop),3);
  //   tau32 = nSub3_beta1(jet_SoftDrop)/nSub2_beta1(jet_SoftDrop);

  //   h_jet_pyconfig_SoftDrop_pt   ->Fill(jet_SoftDrop.pt()/1000.0);
  //   h_jet_pyconfig_SoftDrop_m    ->Fill(jet_SoftDrop.m()/1000.0);
  //   h_jet_pyconfig_SoftDrop_d2   ->Fill(d2);
  //   h_jet_pyconfig_SoftDrop_tau32->Fill(tau32);

  //   tvar_jet_SoftDrop_pt    = jet_SoftDrop.pt()/1000.0;
  //   tvar_jet_SoftDrop_m     = jet_SoftDrop.m()/1000.0;
  //   tvar_jet_SoftDrop_d2    = d2;
  //   tvar_jet_SoftDrop_tau32 = tau32;




  //   //////////////////////////////
  //   //Fill the jet as a new entry into the output tree
  //   //////////////////////////////
     outTree->Fill();

  // }


  return EL::StatusCode::SUCCESS;

}

EL::StatusCode JSSTutorialPythonConfigAlgo :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.

  return EL::StatusCode::SUCCESS;

}

EL::StatusCode JSSTutorialPythonConfigAlgo :: finalize ()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.

  ATH_MSG_INFO( "Deleting tool instances...");

  return EL::StatusCode::SUCCESS;

}

EL::StatusCode JSSTutorialPythonConfigAlgo :: histFinalize ()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.

  return EL::StatusCode::SUCCESS;

}

