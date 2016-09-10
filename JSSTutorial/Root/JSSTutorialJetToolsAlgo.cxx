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

#include "JetRec/JetRecTool.h"
#include "JetRec/PseudoJetGetter.h"
#include "JetRec/JetFromPseudojet.h"
#include "JetRec/JetFinder.h"
#include "JetCalibTools/JetCalibrationTool.h"

#include "JetSubStructureMomentTools/EnergyCorrelatorTool.h"
#include "JetSubStructureMomentTools/EnergyCorrelatorRatiosTool.h"
#include "JetSubStructureMomentTools/NSubjettinessTool.h"
#include "JetSubStructureMomentTools/NSubjettinessRatiosTool.h"
#include "JetRec/JetFilterTool.h"
#include "JetRec/JetTrimmer.h"
#include "JetRec/JetPseudojetRetriever.h"

#include <xAODAnaHelpers/HelperFunctions.h>
#include <xAODAnaHelpers/tools/ReturnCheck.h>



#include <JSSTutorial/JSSTutorialJetToolsAlgo.h>

// c++ include(s)
//#include <stdexcept>

// this is needed to distribute the algorithm to the workers
ClassImp(JSSTutorialJetToolsAlgo)

/// This macro simplify the StatusCode checking
#define SC_CHECK( exp ) RETURN_CHECK("JSSTutorialJetToolsAlgo", exp , "" )
//#define SC_CHECK( exp ) { EL::StatusCode sc= (EL::StatusCode) exp; if(sc==EL::StatusCode::FAILURE) {s//td::cout<< "Config  ERROR   StatusCode failure. "<< std::endl; return sc;}  }



JSSTutorialJetToolsAlgo :: JSSTutorialJetToolsAlgo ()
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
  msg().setName( "JSSTutorialJetToolsAlgo" );

  ATH_MSG_INFO( "Calling constructor");

  m_MyNewVariable = "";
  m_TreeName = "";

}

EL::StatusCode JSSTutorialJetToolsAlgo :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.

  job.useXAOD();
  xAOD::Init( "JSSTutorialJetToolsAlgo" ).ignore(); // call before opening first file

  EL::OutputStream outForTree("tree_jetrec");
  job.outputAdd (outForTree);

  return EL::StatusCode::SUCCESS;

}

EL::StatusCode JSSTutorialJetToolsAlgo :: histInitialize ()
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
  h_jet_jetrec_Ungroomed_pt    = new TH1D("h_jet_jetrec_Ungroomed_pt",   "h_jet_jetrec_Ungroomed_pt",100,0,1000);
  h_jet_jetrec_Ungroomed_m     = new TH1D("h_jet_jetrec_Ungroomed_m",    "h_jet_jetrec_Ungroomed_m",100,0,1000);
  h_jet_jetrec_Ungroomed_d2    = new TH1D("h_jet_jetrec_Ungroomed_d2",   "h_jet_jetrec_Ungroomed_d2",100,0,10);
  h_jet_jetrec_Ungroomed_tau32 = new TH1D("h_jet_jetrec_Ungroomed_tau32","h_jet_jetrec_Ungroomed_tau32",100,0,10);

  h_jet_jetrec_Trimmed_pt      = new TH1D("h_jet_jetrec_Trimmed_pt",    "h_jet_jetrec_Trimmed_pt",100,0,1000);
  h_jet_jetrec_Trimmed_m       = new TH1D("h_jet_jetrec_Trimmed_m",     "h_jet_jetrec_Trimmed_m",100,0,1000);
  h_jet_jetrec_Trimmed_d2      = new TH1D("h_jet_jetrec_Trimmed_d2",    "h_jet_jetrec_Trimmed_d2",100,0,10);
  h_jet_jetrec_Trimmed_tau32   = new TH1D("h_jet_jetrec_Trimmed_tau32", "h_jet_jetrec_Trimmed_tau32",100,0,10);

  h_jet_jetrec_Pruned_pt       = new TH1D("h_jet_jetrec_Pruned_pt",    "h_jet_jetrec_Pruned_pt",100,0,1000);
  h_jet_jetrec_Pruned_m        = new TH1D("h_jet_jetrec_Pruned_m",     "h_jet_jetrec_Pruned_m",100,0,1000);
  h_jet_jetrec_Pruned_d2       = new TH1D("h_jet_jetrec_Pruned_d2",    "h_jet_jetrec_Pruned_d2",100,0,10);
  h_jet_jetrec_Pruned_tau32    = new TH1D("h_jet_jetrec_Pruned_tau32", "h_jet_jetrec_Pruned_tau32",100,0,10);

  h_jet_jetrec_SoftDrop_pt     = new TH1D("h_jet_jetrec_SoftDrop_pt",    "h_jet_jetrec_SoftDrop_pt",100,0,1000);
  h_jet_jetrec_SoftDrop_m      = new TH1D("h_jet_jetrec_SoftDrop_m",     "h_jet_jetrec_SoftDrop_m",100,0,1000);
  h_jet_jetrec_SoftDrop_d2     = new TH1D("h_jet_jetrec_SoftDrop_d2",    "h_jet_jetrec_SoftDrop_d2",100,0,10);
  h_jet_jetrec_SoftDrop_tau32  = new TH1D("h_jet_jetrec_SoftDrop_tau32", "h_jet_jetrec_SoftDrop_tau32",100,0,10);

  // Add them so they automatically appear in output file
  wk()->addOutput (h_jet_jetrec_Ungroomed_pt);
  wk()->addOutput (h_jet_jetrec_Ungroomed_m);
  wk()->addOutput (h_jet_jetrec_Ungroomed_d2);
  wk()->addOutput (h_jet_jetrec_Ungroomed_tau32);

  wk()->addOutput (h_jet_jetrec_Trimmed_pt);
  wk()->addOutput (h_jet_jetrec_Trimmed_m);
  wk()->addOutput (h_jet_jetrec_Trimmed_d2);
  wk()->addOutput (h_jet_jetrec_Trimmed_tau32);

  wk()->addOutput (h_jet_jetrec_Pruned_pt);
  wk()->addOutput (h_jet_jetrec_Pruned_m);
  wk()->addOutput (h_jet_jetrec_Pruned_d2);
  wk()->addOutput (h_jet_jetrec_Pruned_tau32);

  wk()->addOutput (h_jet_jetrec_SoftDrop_pt);
  wk()->addOutput (h_jet_jetrec_SoftDrop_m);
  wk()->addOutput (h_jet_jetrec_SoftDrop_d2);
  wk()->addOutput (h_jet_jetrec_SoftDrop_tau32);

  //////////////////////////
  // Tree
  // You need to do this process to link the tree to the output
  //////////////////////////
  //std::cout<<"Trying to initialize tree stuff"<<std::endl;
  // get the file we created already
  // get the file we created already
  TFile* treeFile = wk()->getOutputFile ("tree_jetrec");
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






  // Here we configure the jet tools to reproduce what is done in JSSTutorialAlgo
  
  // *********************************************************
  // Step 1 configure input building.
  // PseudoJetGetter is the tool translating xAOD inputs into fastjet
  // there can be several of them (when doing ghost-association) so we use
  // a 'ToolHandleArray' rather than just a 'ToolHandle'

  ToolHandleArray<IPseudoJetGetter> getterArray;
  // Create a PseudoJet builder.
  //cout << "Creating pseudojet builder." << endl;

  PseudoJetGetter* lcgetter = new PseudoJetGetter("lcget");
  SC_CHECK(lcgetter->setProperty("InputContainer", "CaloCalTopoClusters"));
  SC_CHECK(lcgetter->setProperty("OutputContainer", "PseudoJetLCTopo"));
  SC_CHECK(lcgetter->setProperty("Label", "LCTopo"));
  SC_CHECK(lcgetter->setProperty("SkipNegativeEnergy", true));
  SC_CHECK(lcgetter->setProperty("GhostScale", 0.0));
  SC_CHECK(lcgetter->initialize()); // IMPORTANT !! 

  getterArray.push_back( ToolHandle<IPseudoJetGetter>(lcgetter) );



  // *********************************************************
  // Step 2. Configure Jet Finder
  // JetFinder is the tool calling fastjet. We configure it with the relevant parameters

  // Technically we need this tool to translate fastjet to xAOD::Jet
  JetFromPseudojet* jetFromPJ = new JetFromPseudojet("jetbuild");
    std::vector<std::string> areatts = {"ActiveArea", "ActiveAreaFourVector"};
  jetFromPJ->setProperty("Attributes", areatts);
  //  jetFromPJ->msg().setLevel(MSG::ERROR);
  SC_CHECK(jetFromPJ->initialize());

  // JetFinder 
  //cout << "Creating jet finder." << endl;
  JetFinder* finder = new JetFinder("AntiKt4Finder");
  //ToolStore::put(finder);
  SC_CHECK(finder->setProperty("JetAlgorithm", "AntiKt"));
  SC_CHECK(finder->setProperty("JetRadius", 1.0));
  SC_CHECK(finder->setProperty("PtMin", 2000.0));
  SC_CHECK(finder->setProperty("GhostArea", 0.01)); // if non-null will run ActiveArea calculation
  SC_CHECK(finder->setProperty("RandomOption", 1));
  SC_CHECK(finder->setProperty("JetBuilder", ToolHandle<IJetFromPseudojet>(jetFromPJ))); // associate

  finder->msg().setLevel(MSG::INFO);
  SC_CHECK(finder->initialize());

  // *********************************************************
  // Step 3. Configure Jet modifiers

  // Create list of modifiers.
  ToolHandleArray<IJetModifier> modArray;

  JetFilterTool * ptfilt = new JetFilterTool("ptfilt");
  SC_CHECK(ptfilt->setProperty("PtMin", 200000.) ); // In atlas all tools work with MeV!!!
  SC_CHECK(ptfilt->initialize());
  modArray.push_back(ToolHandle<IJetModifier>(ptfilt) );
  
  // this creates the ECF1/2/3 moments
  EnergyCorrelatorTool * eccor = new EnergyCorrelatorTool("encorr");
  SC_CHECK(eccor->setProperty("Beta", 1.0) );
  SC_CHECK(eccor->initialize());
  modArray.push_back(ToolHandle<IJetModifier>(eccor) );

  // this creates the C1, C2, D2  moments
  EnergyCorrelatorRatiosTool * eccorR = new EnergyCorrelatorRatiosTool("encorrR");
  SC_CHECK(eccorR->initialize());
  modArray.push_back(ToolHandle<IJetModifier>(eccorR) );

  NSubjettinessTool* nsubj = new NSubjettinessTool("nsubjettiness");
  SC_CHECK(nsubj->setProperty("Alpha", 1.0) );
  SC_CHECK(nsubj->initialize());
  modArray.push_back(ToolHandle<IJetModifier>(nsubj) );

  NSubjettinessRatiosTool* nsubjR = new NSubjettinessRatiosTool("nsubjettinessR");
  SC_CHECK(nsubjR->initialize());
  modArray.push_back(ToolHandle<IJetModifier>(nsubjR) );



  // *********************************************************
  // Finally put everithing together :

  //cout << "Creating jetrec tool." << endl;
  m_jetBuilder = new JetRecTool("jetbuilder");
  SC_CHECK(m_jetBuilder->setProperty("OutputContainer", "AntiKt10LCTopoJets2"));
  SC_CHECK(m_jetBuilder->setProperty("PseudoJetGetters", getterArray));
  SC_CHECK(m_jetBuilder->setProperty("JetFinder", ToolHandle<IJetFinder>(finder)));
  SC_CHECK(m_jetBuilder->setProperty("JetModifiers", modArray));

  //m_jetBuilder->msg().setLevel(MSG::DEBUG);
  SC_CHECK(m_jetBuilder->initialize());


  // *********************************************************


  // *********************************************************
  // Now create a trimming tool. 
  //
  // We don't need PseudoJetGetters because the input of grooming
  // is simply an existing JetContainer
  
  // Create the trimming tool 
  JetTrimmer* trimmer = new JetTrimmer("trimmer");
  SC_CHECK(trimmer->setProperty("RClus" , 0.2) );
  SC_CHECK(trimmer->setProperty("PtFrac", 0.05 ) );
  SC_CHECK(trimmer->setProperty("JetBuilder", ToolHandle< IJetFromPseudojet >(jetFromPJ) ) ); // re-use jetFromPJ
  SC_CHECK(trimmer->initialize() );

  // We'll use the same modifiers except the pt filter.
  //  thus jets in the trimmed container will correspond 1-to-1 to those from the input container.
  modArray.clear();
  modArray.push_back(ToolHandle<IJetModifier>(eccor) );
  modArray.push_back(ToolHandle<IJetModifier>(eccorR) );
  modArray.push_back(ToolHandle<IJetModifier>(nsubj) );
  modArray.push_back(ToolHandle<IJetModifier>(nsubjR) );
  

  // Put the trimming tool to a top level JetRecTool
  m_jetTrimmer = new JetRecTool("jettrimmer");

  // configure the top-level tool
  SC_CHECK(m_jetTrimmer->setProperty("InputContainer", "AntiKt10LCTopoJets2"));
  SC_CHECK(m_jetTrimmer->setProperty("OutputContainer", "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets2"));
  SC_CHECK(m_jetTrimmer->setProperty("JetGroomer", ToolHandle<IJetGroomer>(trimmer)));
  SC_CHECK(m_jetTrimmer->setProperty("JetModifiers", modArray));
  // we also need an additional technical tool :
  SC_CHECK(m_jetTrimmer->setProperty("JetPseudojetRetriever", ToolHandle<IJetPseudojetRetriever>(new JetPseudojetRetriever("pjretriever") ) ));


  //m_jetTrimmer->msg().setLevel(MSG::DEBUG);
  SC_CHECK(m_jetTrimmer->initialize());
  
           


  return EL::StatusCode::SUCCESS;

}

EL::StatusCode JSSTutorialJetToolsAlgo :: fileExecute ()
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

EL::StatusCode JSSTutorialJetToolsAlgo :: changeInput (bool /*firstFile*/)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.

  return EL::StatusCode::SUCCESS;

}

EL::StatusCode JSSTutorialJetToolsAlgo :: initialize ()
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

  ATH_MSG_INFO( "JSSTutorialJetToolsAlgo Interface succesfully initialized!" );
  return EL::StatusCode::SUCCESS;
}


EL::StatusCode JSSTutorialJetToolsAlgo :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.

  if ( m_debug ) { ATH_MSG_INFO( "Executing JSSTutorialJetToolsAlgo..."); }

  if ( (m_eventCounter % 5000) == 0 ) {
    ATH_MSG_INFO( m_eventCounter);
  }

  ++m_eventCounter;

  // Run jet building 
  int ret = m_jetBuilder->execute();
  std::cout << "  executed JetBuilder "<< ret << std::endl;
  // Run jet trimming 
  ret = m_jetTrimmer->execute();
  std::cout << "  executed jet trimmer "<< ret << std::endl;

  // Both jet containers are now availabe in the event store.
  // Let's retrieve them

  const xAOD::JetContainer* ungroomedJets;
  RETURN_CHECK("JSSTutorialAlgo::execute()", HelperFunctions::retrieve(ungroomedJets, "AntiKt10LCTopoJets2", m_event, m_store,  m_verbose), "");

  const xAOD::JetContainer* trimmedJets;
  RETURN_CHECK("JSSTutorialAlgo::execute()", HelperFunctions::retrieve(trimmedJets, "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets2", m_event, m_store,  m_verbose), "");
  


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
    
    h_jet_jetrec_Ungroomed_pt   ->Fill(jet->pt()/1000.0);
    h_jet_jetrec_Ungroomed_m    ->Fill(jet->m()/1000.0);
    h_jet_jetrec_Ungroomed_d2   ->Fill(D2_acc(*jet) );
    h_jet_jetrec_Ungroomed_tau32->Fill(tau32_acc(*jet) );

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

    h_jet_jetrec_Trimmed_pt   ->Fill(jet->pt()/1000.0);
    h_jet_jetrec_Trimmed_m    ->Fill(jet->m()/1000.0);
    h_jet_jetrec_Trimmed_d2   ->Fill(d2);
    h_jet_jetrec_Trimmed_tau32->Fill(tau32);
    
    tvar_jet_Trimmed_pt    = jet->pt()/1000.0;
    tvar_jet_Trimmed_m     = jet->m()/1000.0;
    tvar_jet_Trimmed_d2    = d2;
    tvar_jet_Trimmed_tau32 = tau32;
    
    // if needed, we could retrieve the parent jet of this trimmed 
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

  //   h_jet_jetrec_SoftDrop_pt   ->Fill(jet_SoftDrop.pt()/1000.0);
  //   h_jet_jetrec_SoftDrop_m    ->Fill(jet_SoftDrop.m()/1000.0);
  //   h_jet_jetrec_SoftDrop_d2   ->Fill(d2);
  //   h_jet_jetrec_SoftDrop_tau32->Fill(tau32);

  //   tvar_jet_SoftDrop_pt    = jet_SoftDrop.pt()/1000.0;
  //   tvar_jet_SoftDrop_m     = jet_SoftDrop.m()/1000.0;
  //   tvar_jet_SoftDrop_d2    = d2;
  //   tvar_jet_SoftDrop_tau32 = tau32;


  //   //Pruned
  //   fastjet::PseudoJet jet_Pruned;
  //   jet_Pruned = tool_Pruning(jet_Ungroomed);
  //   std::cout<<"Pruned(mass):         "<<jet_Pruned.m()<<std::endl;
  //   std::cout<<"ECF1:      "<<ECF1(jet_Pruned)<<std::endl;
  //   std::cout<<"ECF2:      "<<ECF2(jet_Pruned)<<std::endl;
  //   std::cout<<"ECF3:      "<<ECF3(jet_Pruned)<<std::endl;
  //   std::cout<<"Tau1WTA:   "<<nSub1_beta1(jet_Pruned)<<std::endl;
  //   std::cout<<"Tau2WTA:   "<<nSub2_beta1(jet_Pruned)<<std::endl;
  //   std::cout<<"Tau3WTA:   "<<nSub3_beta1(jet_Pruned)<<std::endl;

  //   d2    = ECF3(jet_Pruned) * pow(ECF1(jet_Pruned),3) / pow(ECF2(jet_Pruned),3);
  //   tau32 = nSub3_beta1(jet_Pruned)/nSub2_beta1(jet_Pruned);

  //   h_jet_jetrec_Pruned_pt   ->Fill(jet_Pruned.pt()/1000.0);
  //   h_jet_jetrec_Pruned_m    ->Fill(jet_Pruned.m()/1000.0);
  //   h_jet_jetrec_Pruned_d2   ->Fill(d2);
  //   h_jet_jetrec_Pruned_tau32->Fill(tau32);

  //   tvar_jet_Pruned_pt    = jet_Pruned.pt()/1000.0;
  //   tvar_jet_Pruned_m     = jet_Pruned.m()/1000.0;
  //   tvar_jet_Pruned_d2    = d2;
  //   tvar_jet_Pruned_tau32 = tau32;


  //   //////////////////////////////
  //   //Fill the jet as a new entry into the output tree
  //   //////////////////////////////
     outTree->Fill();

  // }


  return EL::StatusCode::SUCCESS;

}

EL::StatusCode JSSTutorialJetToolsAlgo :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.

  return EL::StatusCode::SUCCESS;

}

EL::StatusCode JSSTutorialJetToolsAlgo :: finalize ()
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

EL::StatusCode JSSTutorialJetToolsAlgo :: histFinalize ()
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

