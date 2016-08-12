// EL include(s):
#include <EventLoop/Job.h>
#include <EventLoop/Worker.h>
#include <EventLoop/OutputStream.h>

// EDM include(s):
#include <xAODEventInfo/EventInfo.h>
#include <xAODJet/JetContainer.h>

// Add the truth container!
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"

// package include(s):
#include <xAODAnaHelpers/HelperFunctions.h>
#include <xAODAnaHelpers/HelperClasses.h>
#include <xAODAnaHelpers/tools/ReturnCheck.h>
#include <xAODAnaHelpers/tools/ReturnCheckConfig.h>
#include <JSSTutorial/JSSTutorialAlgo.h>

// ROOT include(s):
#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>

// c++ include(s)
#include <stdexcept>


// this is needed to distribute the algorithm to the workers
ClassImp(JSSTutorialAlgo)


JSSTutorialAlgo :: JSSTutorialAlgo ()
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().

  Info("JSSTutorialAlgo()", "Calling constructor");

  m_MyNewVariable = "";
  m_TreeName = "";

}

EL::StatusCode JSSTutorialAlgo :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.

  job.useXAOD();
  xAOD::Init( "JSSTutorialAlgo" ).ignore(); // call before opening first file

  EL::OutputStream outForTree("tree");
  job.outputAdd (outForTree);

  return EL::StatusCode::SUCCESS;

}

EL::StatusCode JSSTutorialAlgo :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.
  Info("histInitialize()", "Calling histInitialize");

  std::cout<<"Printing the observable you loaded in (m_MyNewVariable) : "<<m_MyNewVariable<<std::endl;

  // Number of events
  h_EventCounter = new TH1D("h_EventCounter","h_EventCounter",10,0,10);
  wk()->addOutput (h_EventCounter);

  //////////////////////////
  // Histograms
  //////////////////////////
  h_jet_Ungroomed_pt    = new TH1D("h_jet_Ungroomed_pt",   "h_jet_Ungroomed_pt",100,0,1000);
  h_jet_Ungroomed_m     = new TH1D("h_jet_Ungroomed_m",    "h_jet_Ungroomed_m",100,0,1000);
  h_jet_Ungroomed_d2    = new TH1D("h_jet_Ungroomed_d2",   "h_jet_Ungroomed_d2",100,0,10);
  h_jet_Ungroomed_tau32 = new TH1D("h_jet_Ungroomed_tau32","h_jet_Ungroomed_tau32",100,0,10);

  h_jet_Trimmed_pt      = new TH1D("h_jet_Trimmed_pt",    "h_jet_Trimmed_pt",100,0,1000);
  h_jet_Trimmed_m       = new TH1D("h_jet_Trimmed_m",     "h_jet_Trimmed_m",100,0,1000);
  h_jet_Trimmed_d2      = new TH1D("h_jet_Trimmed_d2",    "h_jet_Trimmed_d2",100,0,10);
  h_jet_Trimmed_tau32   = new TH1D("h_jet_Trimmed_tau32", "h_jet_Trimmed_tau32",100,0,10);

  h_jet_Pruned_pt       = new TH1D("h_jet_Pruned_pt",    "h_jet_Pruned_pt",100,0,1000);
  h_jet_Pruned_m        = new TH1D("h_jet_Pruned_m",     "h_jet_Pruned_m",100,0,1000);
  h_jet_Pruned_d2       = new TH1D("h_jet_Pruned_d2",    "h_jet_Pruned_d2",100,0,10);
  h_jet_Pruned_tau32    = new TH1D("h_jet_Pruned_tau32", "h_jet_Pruned_tau32",100,0,10);

  h_jet_SoftDrop_pt     = new TH1D("h_jet_SoftDrop_pt",    "h_jet_SoftDrop_pt",100,0,1000);
  h_jet_SoftDrop_m      = new TH1D("h_jet_SoftDrop_m",     "h_jet_SoftDrop_m",100,0,1000);
  h_jet_SoftDrop_d2     = new TH1D("h_jet_SoftDrop_d2",    "h_jet_SoftDrop_d2",100,0,10);
  h_jet_SoftDrop_tau32  = new TH1D("h_jet_SoftDrop_tau32", "h_jet_SoftDrop_tau32",100,0,10);

  // Add them so they automatically appear in output file
  wk()->addOutput (h_jet_Ungroomed_pt);
  wk()->addOutput (h_jet_Ungroomed_m);
  wk()->addOutput (h_jet_Ungroomed_d2);
  wk()->addOutput (h_jet_Ungroomed_tau32);

  wk()->addOutput (h_jet_Trimmed_pt);
  wk()->addOutput (h_jet_Trimmed_m);
  wk()->addOutput (h_jet_Trimmed_d2);
  wk()->addOutput (h_jet_Trimmed_tau32);

  wk()->addOutput (h_jet_Pruned_pt);
  wk()->addOutput (h_jet_Pruned_m);
  wk()->addOutput (h_jet_Pruned_d2);
  wk()->addOutput (h_jet_Pruned_tau32);

  wk()->addOutput (h_jet_SoftDrop_pt);
  wk()->addOutput (h_jet_SoftDrop_m);
  wk()->addOutput (h_jet_SoftDrop_d2);
  wk()->addOutput (h_jet_SoftDrop_tau32);

  //////////////////////////
  // Tree
  // You need to do this process to link the tree to the output
  //////////////////////////
  //std::cout<<"Trying to initialize tree stuff"<<std::endl;
  // get the file we created already
  // get the file we created already
  TFile* treeFile = wk()->getOutputFile ("tree");
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

EL::StatusCode JSSTutorialAlgo :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed


  m_event = wk()->xaodEvent();
  // get the MetaData tree once a new file is opened, with
  TTree *MetaData = dynamic_cast<TTree*>(wk()->inputFile()->Get("MetaData"));
  if (!MetaData) {
    Error("fileExecute()", "MetaData not found! Exiting.");
    return EL::StatusCode::FAILURE;
  }
  MetaData->LoadTree(0);
  //check if file is from a DxAOD
  bool m_isDerivation = !MetaData->GetBranch("StreamAOD");

  if(m_isDerivation ){
    // check for corruption
    const xAOD::CutBookkeeperContainer* incompleteCBC = nullptr;
    if(!m_event->retrieveMetaInput(incompleteCBC, "IncompleteCutBookkeepers").isSuccess()){
      Warning("initializeEvent()","Failed to retrieve IncompleteCutBookkeepers from MetaData! Exiting.");
      //return EL::StatusCode::FAILURE;
    }
    if ( incompleteCBC->size() != 0 ) {
      Warning("initializeEvent()","Found incomplete Bookkeepers! Check file for corruption.");
      //return EL::StatusCode::FAILURE;
    }

    // Now, let's find the actual information
    const xAOD::CutBookkeeperContainer* completeCBC = 0;
    if(!m_event->retrieveMetaInput(completeCBC, "CutBookkeepers").isSuccess()){
      Warning("initializeEvent()","Failed to retrieve CutBookkeepers from MetaData! Exiting.");
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

EL::StatusCode JSSTutorialAlgo :: changeInput (bool /*firstFile*/)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.

  return EL::StatusCode::SUCCESS;

}

EL::StatusCode JSSTutorialAlgo :: initialize ()
{
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the outputif you have no
  // input events.

  Info("initialize()", m_name.c_str());
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();

  // count number of events
  m_eventCounter = 0;

  ////////////////////////////////////////////////////
  // Create/configure the input builder
  ////////////////////////////////////////////////////
  lcgetter = new PseudoJetGetter("lcget");
  lcgetter->setProperty("InputContainer", "CaloCalTopoClusters");
  lcgetter->setProperty("OutputContainer", "PseudoJetLCTopo");
  lcgetter->setProperty("Label", "LCTopo");
  lcgetter->setProperty("SkipNegativeEnergy", true);
  lcgetter->setProperty("GhostScale", 0.0);
  lcgetter->initialize();

  getterArray.push_back( ToolHandle<IPseudoJetGetter>(lcgetter) );

  ////////////////////////////////////////////////////
  // Create/configure Jet Finder
  // Technically we need this tool to translate fastjet to xAOD::Jet
  ////////////////////////////////////////////////////
  jetFromPJ = new JetFromPseudojet("jetbuild");
  std::vector<std::string> areatts = {"ActiveArea", "ActiveAreaFourVector"};
  jetFromPJ->setProperty("Attributes", areatts);
  jetFromPJ->msg().setLevel(MSG::ERROR);
  jetFromPJ->initialize();

  finder = new JetFinder("AntiKt10Finder");
  finder->setProperty("JetAlgorithm", "AntiKt");
  finder->setProperty("JetRadius", 1.0);
  finder->setProperty("PtMin", 2000.0);
  finder->setProperty("GhostArea", 0.01); // if non-null will run ActiveArea calculation
  finder->setProperty("RandomOption", 1);
  finder->setProperty("JetBuilder", ToolHandle<IJetFromPseudojet>(jetFromPJ)); // associate
  finder->msg().setLevel(MSG::INFO);
  finder->initialize();

  ////////////////////////////////////////////////////
  // Create/configure Jet modifiers
  ////////////////////////////////////////////////////
  s_simpleJetBuilder = new JetRecTool("FullJetRecTool");
  s_simpleJetBuilder->setProperty("OutputContainer", "AntiKt10LCTopoJetsSAM");
  s_simpleJetBuilder->setProperty("PseudoJetGetters", getterArray);
  s_simpleJetBuilder->setProperty("JetFinder", ToolHandle<IJetFinder>(finder));
  s_simpleJetBuilder->setProperty("JetModifiers", modArray);
  s_simpleJetBuilder->msg().setLevel(MSG::INFO);
  s_simpleJetBuilder->initialize();

  Info("initialize()", "JSSTutorialAlgo Interface succesfully initialized!" );
  return EL::StatusCode::SUCCESS;
}


EL::StatusCode JSSTutorialAlgo :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.

  if ( m_debug ) { Info("execute()", "Executing JSSTutorialAlgo..."); }

  if ( (m_eventCounter % 5000) == 0 ) {
    Info("execute()", "processed %i events", m_eventCounter);
  }

  ++m_eventCounter;


  ///////////////////////////
  // Build jets
  // this means that the container of jets specified
  // with the "OutputContainer" parameter of the JetRecTool
  // is now available for use in your analysis
  ///////////////////////////
  int i_jet_build_check = s_simpleJetBuilder->execute();
  if(i_jet_build_check != 0 ){ // error
    std::cout<<"JetBuildingFailed"<<std::endl;
  }


  ///////////////////////////
  // Get Jets for Use
  // use the xAH helper functions
  ///////////////////////////
  const xAOD::JetContainer*      jetsSAM(nullptr);
  RETURN_CHECK("JSSTutorialAlgo::execute()", HelperFunctions::retrieve(jetsSAM, "AntiKt10LCTopoJetsSAM", m_event, m_store,  m_verbose), "");
  std::cout<<"jetsSAM size: "<<jetsSAM->size()<<std::endl;


  /////////////////////////////////////////
  // Loop over those jets and perform a few basic calculation
  //
  // GROOMING
  // 1) Trimming
  // 2) SoftDrop
  // 3) Pruning
  //
  // JSS MOMENT CALCULATION
  // 1) D2 from ECFs
  // 2) Tau32 from NSubjettiness
  /////////////////////////////////////////


  ////////////////////////
  //configure pruning tool
  //http://fastjet.fr/repo/doxygen-3.0.3/classfastjet_1_1Pruner.html
  ////////////////////////
  fastjet::Pruner tool_Pruning(fastjet::cambridge_algorithm, 0.1, 0.5);

  ////////////////////////
  //configure soft drop tool
  //http://fastjet.hepforge.org/svn/contrib/contribs/RecursiveTools/tags/1.0.0/SoftDrop.hh
  ////////////////////////
  fastjet::contrib::SoftDrop tool_SoftDrop(0.0, 0.1);

  ////////////////////////
  //configure trimming tool
  //http://fastjet.fr/repo/doxygen-3.2.1/classfastjet_1_1Filter.html
  ////////////////////////
  fastjet::Transformer *trimmer = new fastjet::Filter(fastjet::JetDefinition(fastjet::kt_algorithm, 0.2), fastjet::SelectorPtFractionMin(0.05) );
  const fastjet::Transformer &tool_Trimming = *trimmer;

  ////////////////////////
  //D2 calculation from ECF
  //http://fastjet.hepforge.org/svn/contrib/contribs/EnergyCorrelator/tags/1.1.0/example.cc
  ////////////////////////
  fastjet::contrib::EnergyCorrelator ECF1(1, 1.0, fastjet::contrib::EnergyCorrelator::pt_R);
  fastjet::contrib::EnergyCorrelator ECF2(2, 1.0, fastjet::contrib::EnergyCorrelator::pt_R);
  fastjet::contrib::EnergyCorrelator ECF3(3, 1.0, fastjet::contrib::EnergyCorrelator::pt_R);

  ////////////////////////
  //Define Nsubjettiness functions for beta = 1.0 using one-pass WTA KT axes
  //http://fastjet.hepforge.org/svn/contrib/contribs/Nsubjettiness/tags/2.2.4/example_basic_usage.cc
  ////////////////////////
  fastjet::contrib::Nsubjettiness nSub1_beta1(1, fastjet::contrib::OnePass_WTA_KT_Axes(), fastjet::contrib::UnnormalizedMeasure(1.0));
  fastjet::contrib::Nsubjettiness nSub2_beta1(2, fastjet::contrib::OnePass_WTA_KT_Axes(), fastjet::contrib::UnnormalizedMeasure(1.0));
  fastjet::contrib::Nsubjettiness nSub3_beta1(3, fastjet::contrib::OnePass_WTA_KT_Axes(), fastjet::contrib::UnnormalizedMeasure(1.0));

  //temporary variables
  float d2    = 0.0;
  float tau32 = 0.0;

  for(auto myJet : *jetsSAM){

    //only examine jets above 200 GeV
    if(myJet->pt()<200000.0)
      continue;

    std::cout<<std::endl<<"JetPt : "<<myJet->pt()<<std::endl;

    //Retrieve original jet
    std::vector<fastjet::PseudoJet> constit_pseudojets = jet::JetConstituentFiller::constituentPseudoJets(*myJet);
    fastjet::JetDefinition jet_def = fastjet::JetDefinition(fastjet::antikt_algorithm, 1.0, fastjet::E_scheme, fastjet::Best);
    fastjet::ClusterSequence clust_seq = fastjet::ClusterSequence(constit_pseudojets, jet_def);
    std::vector<fastjet::PseudoJet> pjets = clust_seq.inclusive_jets(0.0);
    auto jet_Ungroomed = pjets[0];

    //Ungroomed
    std::cout<<"Ungroomed(mass):         "<<jet_Ungroomed.m()<<std::endl;
    std::cout<<"ECF1:      "<<ECF1(jet_Ungroomed)<<std::endl;
    std::cout<<"ECF2:      "<<ECF2(jet_Ungroomed)<<std::endl;
    std::cout<<"ECF3:      "<<ECF3(jet_Ungroomed)<<std::endl;
    std::cout<<"Tau1WTA:   "<<nSub1_beta1(jet_Ungroomed)<<std::endl;
    std::cout<<"Tau2WTA:   "<<nSub2_beta1(jet_Ungroomed)<<std::endl;
    std::cout<<"Tau3WTA:   "<<nSub3_beta1(jet_Ungroomed)<<std::endl;

    d2    = ECF3(jet_Ungroomed) * pow(ECF1(jet_Ungroomed),3) / pow(ECF2(jet_Ungroomed),3);
    tau32 = nSub3_beta1(jet_Ungroomed)/nSub2_beta1(jet_Ungroomed);

    h_jet_Ungroomed_pt   ->Fill(jet_Ungroomed.pt()/1000.0);
    h_jet_Ungroomed_m    ->Fill(jet_Ungroomed.m()/1000.0);
    h_jet_Ungroomed_d2   ->Fill(d2);
    h_jet_Ungroomed_tau32->Fill(tau32);

    tvar_jet_Ungroomed_pt    = jet_Ungroomed.pt()/1000.0;
    tvar_jet_Ungroomed_m     = jet_Ungroomed.m()/1000.0;
    tvar_jet_Ungroomed_d2    = d2;
    tvar_jet_Ungroomed_tau32 = tau32;


    //Trimmed
    fastjet::PseudoJet jet_Trimmed;
    jet_Trimmed = tool_Trimming(jet_Ungroomed);
    std::cout<<"Trimmed(mass):         "<<jet_Trimmed.m()<<std::endl;
    std::cout<<"ECF1:      "<<ECF1(jet_Trimmed)<<std::endl;
    std::cout<<"ECF2:      "<<ECF2(jet_Trimmed)<<std::endl;
    std::cout<<"ECF3:      "<<ECF3(jet_Trimmed)<<std::endl;
    std::cout<<"Tau1WTA:   "<<nSub1_beta1(jet_Trimmed)<<std::endl;
    std::cout<<"Tau2WTA:   "<<nSub2_beta1(jet_Trimmed)<<std::endl;
    std::cout<<"Tau3WTA:   "<<nSub3_beta1(jet_Trimmed)<<std::endl;

    d2    = ECF3(jet_Trimmed) * pow(ECF1(jet_Trimmed),3) / pow(ECF2(jet_Trimmed),3);
    tau32 = nSub3_beta1(jet_Trimmed)/nSub2_beta1(jet_Trimmed);

    h_jet_Trimmed_pt   ->Fill(jet_Trimmed.pt()/1000.0);
    h_jet_Trimmed_m    ->Fill(jet_Trimmed.m()/1000.0);
    h_jet_Trimmed_d2   ->Fill(d2);
    h_jet_Trimmed_tau32->Fill(tau32);

    tvar_jet_Trimmed_pt    = jet_Trimmed.pt()/1000.0;
    tvar_jet_Trimmed_m     = jet_Trimmed.m()/1000.0;
    tvar_jet_Trimmed_d2    = d2;
    tvar_jet_Trimmed_tau32 = tau32;


    //SoftDrop
    fastjet::PseudoJet jet_SoftDrop;
    jet_SoftDrop = tool_SoftDrop(jet_Ungroomed);
    std::cout<<"SoftDrop(mass):      "<<jet_SoftDrop.m()<<std::endl;
    std::cout<<"ECF1:      "<<ECF1(jet_SoftDrop)<<std::endl;
    std::cout<<"ECF2:      "<<ECF2(jet_SoftDrop)<<std::endl;
    std::cout<<"ECF3:      "<<ECF3(jet_SoftDrop)<<std::endl;
    std::cout<<"Tau1WTA:   "<<nSub1_beta1(jet_SoftDrop)<<std::endl;
    std::cout<<"Tau2WTA:   "<<nSub2_beta1(jet_SoftDrop)<<std::endl;
    std::cout<<"Tau3WTA:   "<<nSub3_beta1(jet_SoftDrop)<<std::endl;

    d2    = ECF3(jet_SoftDrop) * pow(ECF1(jet_SoftDrop),3) / pow(ECF2(jet_SoftDrop),3);
    tau32 = nSub3_beta1(jet_SoftDrop)/nSub2_beta1(jet_SoftDrop);

    h_jet_SoftDrop_pt   ->Fill(jet_SoftDrop.pt()/1000.0);
    h_jet_SoftDrop_m    ->Fill(jet_SoftDrop.m()/1000.0);
    h_jet_SoftDrop_d2   ->Fill(d2);
    h_jet_SoftDrop_tau32->Fill(tau32);

    tvar_jet_SoftDrop_pt    = jet_SoftDrop.pt()/1000.0;
    tvar_jet_SoftDrop_m     = jet_SoftDrop.m()/1000.0;
    tvar_jet_SoftDrop_d2    = d2;
    tvar_jet_SoftDrop_tau32 = tau32;


    //Pruned
    fastjet::PseudoJet jet_Pruned;
    jet_Pruned = tool_Pruning(jet_Ungroomed);
    std::cout<<"Pruned(mass):         "<<jet_Pruned.m()<<std::endl;
    std::cout<<"ECF1:      "<<ECF1(jet_Pruned)<<std::endl;
    std::cout<<"ECF2:      "<<ECF2(jet_Pruned)<<std::endl;
    std::cout<<"ECF3:      "<<ECF3(jet_Pruned)<<std::endl;
    std::cout<<"Tau1WTA:   "<<nSub1_beta1(jet_Pruned)<<std::endl;
    std::cout<<"Tau2WTA:   "<<nSub2_beta1(jet_Pruned)<<std::endl;
    std::cout<<"Tau3WTA:   "<<nSub3_beta1(jet_Pruned)<<std::endl;

    d2    = ECF3(jet_Pruned) * pow(ECF1(jet_Pruned),3) / pow(ECF2(jet_Pruned),3);
    tau32 = nSub3_beta1(jet_Pruned)/nSub2_beta1(jet_Pruned);

    h_jet_Pruned_pt   ->Fill(jet_Pruned.pt()/1000.0);
    h_jet_Pruned_m    ->Fill(jet_Pruned.m()/1000.0);
    h_jet_Pruned_d2   ->Fill(d2);
    h_jet_Pruned_tau32->Fill(tau32);

    tvar_jet_Pruned_pt    = jet_Pruned.pt()/1000.0;
    tvar_jet_Pruned_m     = jet_Pruned.m()/1000.0;
    tvar_jet_Pruned_d2    = d2;
    tvar_jet_Pruned_tau32 = tau32;


    //////////////////////////////
    //Fill the jet as a new entry into the output tree
    //////////////////////////////
    outTree->Fill();

  }


  return EL::StatusCode::SUCCESS;

}

EL::StatusCode JSSTutorialAlgo :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.

  return EL::StatusCode::SUCCESS;

}

EL::StatusCode JSSTutorialAlgo :: finalize ()
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

  Info("finalize()", "Deleting tool instances...");

  return EL::StatusCode::SUCCESS;

}

EL::StatusCode JSSTutorialAlgo :: histFinalize ()
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

//=======================================================
//
//=======================================================
const xAOD::TruthParticle* JSSTutorialAlgo::GetParticleAfterFSR(const xAOD::TruthParticle* inputParticle){
  // follow decay chain along X -> X (+ Y) until X decays into something else, return last X
  if( !inputParticle ) return inputParticle;

  const xAOD::TruthParticle* after_fsr = inputParticle;
  int pid = inputParticle->pdgId();

  bool fsr_loop = true;
  while(fsr_loop){
    fsr_loop = false; // only continue loop if we find suitable child...
    for(unsigned int i_child=0; i_child<after_fsr->nChildren(); i_child++){
      if( after_fsr->child(i_child)->pdgId() == pid ){
        // child of same type, continue fsr loop child
        fsr_loop = true;
        after_fsr = after_fsr->child(i_child);
        break;
      }
    }
  }

  return after_fsr;
}
//=======================================================
//
//=======================================================
const xAOD::TruthParticle* JSSTutorialAlgo::GetHadronicTopParticle(const xAOD::TruthParticle* inputParticle)
{
  // if input top: follow through FSR, test if decays hadronically, try to match
  // if input W: test if decays hadronically and follow upwards to top
  // return top after fsr or null if no top or no hadronic W
  // FIXME: someone should check this and can one do this more elegantly?
  if( !inputParticle ) return inputParticle;

  const xAOD::TruthParticle* top_after_fsr = nullptr;
  const xAOD::TruthParticle* W_after_fsr = nullptr;

  // get top after FSR
  if( inputParticle->absPdgId() == 6 ){
    top_after_fsr = GetParticleAfterFSR(inputParticle);
    // get W child
    for(unsigned int i_child=0; i_child<top_after_fsr->nChildren(); i_child++){
      if( top_after_fsr->child(i_child)->absPdgId() == 24 ){ // found W, stop
        W_after_fsr = top_after_fsr->child(i_child);
        break;
      }
    }
  }
  // check if W comes from top and get top parent
  else if( inputParticle->absPdgId() == 24){
    top_after_fsr = inputParticle;
    W_after_fsr = inputParticle;
    bool parent_loop = true;
    bool found_top = false;
    while(parent_loop){
      parent_loop = false; // only continue loop if we find suitable parent.
      for(unsigned int i_parent=0; i_parent<top_after_fsr->nParents(); i_parent++){
        if( top_after_fsr->parent(i_parent)->absPdgId() == 6 ){ // found t parent
          parent_loop = false;
          found_top = true;
          top_after_fsr = top_after_fsr->parent(i_parent);
          break;
        }
        else if(  top_after_fsr->parent(i_parent)->absPdgId() == 24 ){ // found another W, continue
          parent_loop = true;
          top_after_fsr = top_after_fsr->parent(i_parent);
          break;
        }
      }
    }
    if(!found_top) top_after_fsr = nullptr;
  }

  // no good top or W found, we can already abort
  if( !top_after_fsr || !W_after_fsr ) return nullptr;

  // check if W decays hadronically
  if( GetHadronicWParticle(W_after_fsr) )
    return top_after_fsr;
  else
    return nullptr;
}
//=======================================================
//
//=======================================================
const xAOD::TruthParticle* JSSTutorialAlgo::GetHadronicWParticle(const xAOD::TruthParticle* inputParticle)
{
  if( !inputParticle || (inputParticle->absPdgId() != 24) ) return nullptr;

  const xAOD::TruthParticle* W_after_fsr = GetParticleAfterFSR(inputParticle);
  if(W_after_fsr->nChildren()>0 && W_after_fsr->child(0)->absPdgId() < 7)
    return W_after_fsr;
  else
    return nullptr;
}
//=======================================================
//
//=======================================================
const xAOD::TruthParticle* JSSTutorialAlgo::GetHadronicZParticle(const xAOD::TruthParticle* inputParticle)
{
  if( !inputParticle || (inputParticle->absPdgId() != 23) ) return nullptr;

  const xAOD::TruthParticle* Z_after_fsr = GetParticleAfterFSR(inputParticle);
  if(Z_after_fsr->nChildren()>0 && Z_after_fsr->child(0)->absPdgId() < 7)
     return Z_after_fsr;
  else
    return nullptr;
}
//=======================================================
//
//=======================================================
// characterize matching between truth jet and hadronic top particle
// 1. is jet dR matched to truth top particle         x 100
// 2. is jet dR matched to truth b particle           x  10
// 3. is jet dR matched to n truth W daughter quarks  x   1
// returns 100*1. + 10*2. + 3.
// to decode: 1. = (returnCode/100)%10, 2. = (returnCode/10)%10, 3. = returnCode%10
int JSSTutorialAlgo::CharacterizeHadronicTopJet(const  xAOD::TruthParticle* truthtop, const xAOD::Jet* fatjet, double dRmax)
{
  int isTopMatched=0;
  int isBMatched=0;
  int nMatchedWChildren = 0;
  int returnCode = 0;

  const xAOD::TruthParticle* truthtop_afterFSR = GetHadronicTopParticle(truthtop);
  if(!truthtop_afterFSR) // not a hadronic top...
    return 0;

  const xAOD::TruthParticle* truthW_afterFSR = nullptr;
  const xAOD::TruthParticle* truthB = nullptr;
  for(unsigned int i_child=0; i_child<truthtop_afterFSR->nChildren(); i_child++)
  {
    if( truthtop_afterFSR->child(i_child)->absPdgId() == 24) // W
      truthW_afterFSR = GetHadronicWParticle(truthtop_afterFSR->child(i_child));
    else if( truthtop_afterFSR->child(i_child)->absPdgId() ==  5 ) // b
      truthB = truthtop_afterFSR->child(i_child);
  }

  TLorentzVector fatjet_vector;
  fatjet_vector.SetPtEtaPhiM(fatjet->pt(), fatjet->eta(), fatjet->phi(), fatjet->m());

  TLorentzVector top_vector;
  top_vector.SetPtEtaPhiM(truthtop_afterFSR->pt(), truthtop_afterFSR->eta(), truthtop_afterFSR->phi(), truthtop_afterFSR->m());
  if( fatjet_vector.DeltaR(top_vector) < dRmax )
    isTopMatched = 1;

  if(truthB){ // FIXME: what to do about tops that don't decay into b but other quark (~5%?!) ?
    TLorentzVector b_vector;
    b_vector.SetPtEtaPhiM(truthB->pt(), truthB->eta(), truthB->phi(), truthB->m());
    if( fatjet_vector.DeltaR(b_vector) < dRmax )
      isBMatched = 1;
  }

  if(truthW_afterFSR){
    for(unsigned int i_child=0; i_child<truthW_afterFSR->nChildren(); i_child++){
      const xAOD::TruthParticle* truthWchild = truthW_afterFSR->child(i_child);
      TLorentzVector WChild_vector;
      WChild_vector.SetPtEtaPhiM(truthWchild->pt(), truthWchild->eta(), truthWchild->phi(), truthWchild->m());
      if( truthWchild->absPdgId() < 7 && fatjet_vector.DeltaR(WChild_vector) < dRmax )
        nMatchedWChildren += 1;
    }
  }

  returnCode = 100*isTopMatched + 10*isBMatched + nMatchedWChildren;
  return returnCode;
}
//=======================================================
//
//=======================================================
// returns pdgid of highest energy parton the jet can be matched to
int JSSTutorialAlgo::QuarkGluonLabelJet(const xAOD::TruthParticleContainer* truthparticles, const xAOD::Jet* fatjet, double dRmax)
{
  double Emax = 0;
  TLorentzVector fatjet_vector;
  fatjet_vector.SetPtEtaPhiM(fatjet->pt(), fatjet->eta(), fatjet->phi(), fatjet->m());
  int label = 0;

  for(auto parton : *truthparticles){
    if( parton->pt() < 5000) continue; // avoid errors for pt = 0 particles
    if( parton->absPdgId() > 9 && parton->absPdgId() != 21 ) continue; // not quark or gluon(9 or 21)
    if( parton->e() < Emax) continue; // want the one with the highest energy
    TLorentzVector parton_vector;
    parton_vector.SetPtEtaPhiM(parton->pt(), parton->eta(), parton->phi(), parton->m());
    if( fatjet_vector.DeltaR(parton_vector) < dRmax ){
      label = parton->pdgId();
      Emax = parton->e();
    }
  }
  return label;
}

