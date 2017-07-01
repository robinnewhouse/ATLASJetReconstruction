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
  msg().setName( "JSSTutorialAlgo" );

  ATH_MSG_INFO( "Calling constructor");

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
  ATH_MSG_INFO( "Calling histInitialize");

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

  ATH_MSG_INFO( m_name.c_str());
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();

  // count number of events
  m_eventCounter = 0;

  //jet calibration
  const std::string name = "MyxAODAnalysis"; //string describing the current thread, for logging
  TString jetAlgo = "AntiKt10LCTopoTrimmedPtFrac5SmallR20";  //String describing your jet collection, for example AntiKt4EMTopo or AntiKt4LCTopo (see below)
  TString config = "JES_MC15recommendation_FatJet_Nov2016_QCDCombinationUncorrelatedWeights.config"; //Path to global config used to initialize the tool (see below)
  TString calibSeq = "EtaJES_JMS" ; //String describing the calibration sequence to apply (see below)
  bool isData = false; //bool describing if the events are data or from simulation

  m_jetCalibration = new JetCalibrationTool("myJetCalibration");
  m_jetCalibration->setProperty("JetCollection",jetAlgo.Data());
  m_jetCalibration->setProperty("ConfigFile",config.Data());
  m_jetCalibration->setProperty("CalibSequence",calibSeq.Data());
  m_jetCalibration->setProperty("IsData",isData);

  // Initialize the tool
  m_jetCalibration->initializeTool(name);


  // Instantiation (if not using some ToolHandle)
  std::cout<<"Initializing WZ Tagger"<<std::endl;
  ASG_SET_ANA_TOOL_TYPE( m_smoothedWZTagger, SmoothedWZTagger);
  m_smoothedWZTagger.setName("SmoothedWZTagger");
  m_smoothedWZTagger.setProperty( "ConfigFile",   "SmoothedWZTaggers/SmoothedContainedWTagger_AntiKt10LCTopoTrimmed_FixedSignalEfficiency50_MC15c_20161215.dat");
  m_smoothedWZTagger.retrieve();

  // Instantiation (if not using some ToolHandle)
  std::cout<<"Initializing top Tagger"<<std::endl;
  ASG_SET_ANA_TOOL_TYPE( m_smoothedTopTagger, SmoothedTopTagger);
  m_smoothedTopTagger.setName("SmoothedTopTagger");
  m_smoothedTopTagger.setProperty( "ConfigFile",   "SmoothedTopTaggers/SmoothedTopTagger_AntiKt10LCTopoTrimmed_MassTau32FixedSignalEfficiency50_MC15c_20161209.dat");
  m_smoothedTopTagger.retrieve();

  // Instantiation (if not using some ToolHandle )
  std::cout<<"Initializing Xbb Tagger"<<std::endl;
  ASG_SET_ANA_TOOL_TYPE( m_BoostedXbbTagger, BoostedXbbTagger);
  m_BoostedXbbTagger.setName("BoostedXbbTagger");
  m_BoostedXbbTagger.setProperty( "ConfigFile",   "XbbTagger/XbbTagger_AntiKt10LCTopoTrimmed_1BTag_MC15c_20161118.dat");
  m_BoostedXbbTagger.retrieve();

  // Instantiation (if not using some ToolHandle )
  std::cout<<"Initializing DNN Tagger"<<std::endl;
  ASG_SET_ANA_TOOL_TYPE( m_JSSWTopTaggerDNN, JSSWTopTaggerDNN);
  m_JSSWTopTaggerDNN.setName("JSSWTopTaggerDNN");
  m_JSSWTopTaggerDNN.setProperty( "ConfigFile",   "JSSWTopTaggerDNN/JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuark_MC15c_20170511_NOTFORANALYSIS.dat");
  m_JSSWTopTaggerDNN.retrieve();

  //Instantiation (if not using some ToolHandle )
  std::cout<<"Initializing BDT Tagger"<<std::endl;
  ASG_SET_ANA_TOOL_TYPE( m_JSSWTopTaggerBDT, JSSWTopTaggerBDT);
  m_JSSWTopTaggerBDT.setName("JSSWTopTaggerBDT");
  m_JSSWTopTaggerBDT.setProperty( "ConfigFile",   "JSSWTopTaggerBDT/JSSBDTTagger_AntiKt10LCTopoTrimmed_TopQuarkInclusive_MC15c_20170511_NOTFORANALYSIS.dat");
  m_JSSWTopTaggerBDT.retrieve();

  //Instantiation (if not using some ToolHandle )
  std::cout<<"Initializing Topocluster Top Tagger"<<std::endl;
  ASG_SET_ANA_TOOL_TYPE( m_TopoclusterTopTagger, TopoclusterTopTagger);
  m_TopoclusterTopTagger.setName("TopoclusterTopTagger");
  m_TopoclusterTopTagger.setProperty( "ConfigFile",   "TopoclusterTopTagger/TopoclusterTopTagger_AntiKt10LCTopoTrimmed_TopQuark_MC15c_20170511_NOTFORANALYSIS.dat");
  m_TopoclusterTopTagger.retrieve();

  //Instantiation (if not using some ToolHandle )
//   std::cout<<"Initializing q/g Tagger"<<std::endl;
//   ASG_SET_ANA_TOOL_TYPE( m_JetQGTagger, JetQGTagger);
//   m_JetQGTagger.setName("JetQGTagger");
//   m_JetQGTagger.setProperty( "NTrackCut",   20);
//   m_JetQGTagger.retrieve();


  ATH_MSG_INFO( "JSSTutorialAlgo Interface succesfully initialized!" );
  return EL::StatusCode::SUCCESS;
}


EL::StatusCode JSSTutorialAlgo :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.

  if ( m_debug ) { ATH_MSG_INFO( "Executing JSSTutorialAlgo..."); }

  if ( (m_eventCounter % 5000) == 0 ) {
    ATH_MSG_INFO( m_eventCounter);
  }

  ++m_eventCounter;

  std::cout<<std::endl<<std::endl<<std::endl<<"Executing : SAM"<<std::endl;


  // Obtain the set of clusters from StoreGate
  const xAOD::JetContainer* myJets;
  RETURN_CHECK("JSSTutorialAlgo::execute()", HelperFunctions::retrieve(myJets, "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets", m_event, m_store,  m_verbose), "");
  std::cout<<"NumJets: "<<myJets->size()<<std::endl;

  int ijet=0;
  for(const xAOD::Jet* jet : * myJets ){
    ijet++;

    std::cout<<"Jet # "<<ijet<<"   ";
    // try{
    //   std::vector<xAOD::JetConstituent> clusters = jet->getConstituents().asSTLVector();
    //   std::cout<<"Num clusters # "<<clusters.size()<<std::endl;
    // } catch (...){
    //   std::cout<<"Failed "<< std::endl;
    //   continue;
    // }
    // continue;

    static SG::AuxElement::ConstAccessor<float>    acc_D2   ("D2");
    static SG::AuxElement::ConstAccessor<float>    acc_ECF1 ("ECF1");
    static SG::AuxElement::ConstAccessor<float>    acc_ECF2 ("ECF2");
    static SG::AuxElement::ConstAccessor<float>    acc_ECF3 ("ECF3");


    std::cout<<std::endl<<"Uncalibrated Jet : "<<ijet<<std::endl;
    std::cout<<jet->pt()<<"  "<<jet->eta()<<std::endl;

    xAOD::Jet * caljet = 0;
    std::cout<<"Applying Calibration"<<std::endl;
    m_jetCalibration->calibratedCopy(*jet,caljet); //make a calibrated copy, assuming a copy hasn't been made already, alternative is:

    float D2 = acc_ECF3(*caljet) * pow(acc_ECF1(*caljet), 3.0) / pow(acc_ECF2(*caljet), 3.0);


    std::cout<<std::endl<<"Calibrated Jet : "<<ijet<<std::endl;
    std::cout<<caljet->pt()<<"  "<<caljet->eta()<<"  "<<caljet->m()<<"  "<<D2<<std::endl;

    std::cout<<"Testing W Tagger "<<std::endl;
    const Root::TAccept& res_wz = m_smoothedWZTagger->tag( *caljet );
    std::cout<<"RunningTag : "<<res_wz<<std::endl;
    std::cout<<"result(W) d2pass       = "<<res_wz.getCutResult("PassD2")<<std::endl;
    std::cout<<"result(W) masspasslow  = "<<res_wz.getCutResult("PassMassLow")<<std::endl;
    std::cout<<"result(W) masspasshigh = "<<res_wz.getCutResult("PassMassHigh")<<std::endl;

    std::cout<<"Testing top Tagger "<<std::endl;
    const Root::TAccept& res_top = m_smoothedTopTagger->tag( *caljet );
    std::cout<<"RunningTag : "<<res_top<<std::endl;
    std::cout<<"result(top) var1= "<<res_top.getCutResult("PassMass")<<std::endl;
    std::cout<<"result(top) var2= "<<res_top.getCutResult("PassTau32")<<std::endl;


    std::cout<<"Testing Xbb Tagger "<<std::endl;
    const Root::TAccept& res_xbb = m_BoostedXbbTagger->tag( *caljet );
    std::cout<<"RunningTag : "<<res_xbb<<std::endl;
    std::cout<<"result(Xbb) masslow  = "<<res_xbb.getCutResult("PassMassLow")<<std::endl;
    std::cout<<"result(Xbb) masshigh = "<<res_xbb.getCutResult("PassMassHigh")<<std::endl;
    std::cout<<"result(Xbb) btag     = "<<res_xbb.getCutResult("PassBTag")<<std::endl;
    std::cout<<"result(Xbb) jss      = "<<res_xbb.getCutResult("PassJSS")<<std::endl;


    std::cout<<"Testing DNN Tagger "<<std::endl;
    const Root::TAccept& res_dnn = m_JSSWTopTaggerDNN->tag( *caljet );
    std::cout<<"RunningTag = "<<res_dnn<<std::endl;
    std::cout<<"result(DNN) mass  = "<<res_dnn.getCutResult("PassMass")<<std::endl;
    std::cout<<"result(DNN) score = "<<res_dnn.getCutResult("PassScore")<<std::endl;
    static SG::AuxElement::ConstAccessor<float>    acc_DNNScore("DNNTaggerScoreTopQuark");
    if(!acc_DNNScore.isAvailable(*caljet)){
      std::cout<<"DNN moment not existent"<<std::endl;
    }
    std::cout<<"value(DNN)  = "<<acc_DNNScore(*caljet)<<std::endl;


    std::cout<<"Testing BDT Tagger "<<std::endl;
    const Root::TAccept& res_bdt = m_JSSWTopTaggerBDT->tag( *caljet );
    std::cout<<"RunningTag = "<<res_bdt<<std::endl;
    std::cout<<"result(BDT) mass  = "<<res_bdt.getCutResult("PassMass")<<std::endl;
    std::cout<<"result(BDT) score = "<<res_bdt.getCutResult("PassScore")<<std::endl;
    static SG::AuxElement::ConstAccessor<float>    acc_BDTScore("BDTTaggerScoreTopQuark");
    if(!acc_BDTScore.isAvailable(*caljet)){
      std::cout<<"BDT moment not existent"<<std::endl;
    }
    std::cout<<"value(BDT)  = "<<acc_BDTScore(*caljet)<<std::endl;


    std::cout<<"Testing TTT Tagger "<<std::endl;
    const Root::TAccept& res_ttt = m_TopoclusterTopTagger->tag( *caljet );
    std::cout<<"RunningTag = "<<res_ttt<<std::endl;
    std::cout<<"result(TTT) mass  = "<<res_ttt.getCutResult("PassMass")<<std::endl;
    std::cout<<"result(TTT) score = "<<res_ttt.getCutResult("PassScore")<<std::endl;
    static SG::AuxElement::ConstAccessor<float>    acc_TTTScore("TopoclusterTopTaggerScoreTopQuark");
    if(!acc_TTTScore.isAvailable(*caljet)){
      std::cout<<"TTT moment not existent"<<std::endl;
    }
    std::cout<<"value(TTT)  = "<<acc_TTTScore(*caljet)<<std::endl;


//     std::cout<<"Testing JetQG Tagger "<<std::endl;
//     const Root::TAccept& res_qg = m_JetQGTagger->tag( *caljet );
//     std::cout<<"RunningTag : "<<res_qg<<std::endl;
//     std::cout<<"result(qg) q = "<<res_qg.getCutResult("QuarkJetTag")<<std::endl;
//     std::cout<<"result(qg) g = "<<res_qg.getCutResult("GluonJetTag")<<std::endl;

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

  ATH_MSG_INFO( "Deleting tool instances...");

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

