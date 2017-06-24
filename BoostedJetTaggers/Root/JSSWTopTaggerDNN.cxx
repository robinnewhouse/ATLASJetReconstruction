/**************************************************************
//
// Created:        8 November  2016
// Last Updated:   11 May 2017
//
// Daniel Marley
// daniel.edison.marley@cern.ch
//
// Work by: Ece Akilli, Dan Guest, Oliver Majersky, Sam Meehan
//
// DNN Tagging of Large-R jets as W/top
//
//   There are two components to the tagger:
//    1. Load the weights from the trained tagger
//    2. Determine if jet passes working point
//       (use trained tagger info to get discriminant for new jet)
//
***************************************************************/
#include "BoostedJetTaggers/JSSWTopTaggerDNN.h"

#include "PathResolver/PathResolver.h"

#include "TEnv.h"
#include "TF1.h"
#include "TSystem.h"

#define APP_NAME "JSSWTopTaggerDNN"

#define N_CONSTITUENTS 10
#define EMPTY_CONSTITUENT 0.0

#define CERRD std::cout<<"SAM Error : "<<__FILE__<<"  "<<__LINE__<<std::endl;

JSSWTopTaggerDNN::JSSWTopTaggerDNN( const std::string& name ) :
  JSSTaggerBase( name ),
  m_name(name),
  m_APP_NAME(APP_NAME),
  m_lwnn(nullptr),
  m_jetPtMin(200000.),
  m_jetPtMax(300000.),
  m_jetEtaMax(2.0)
  {

    declareProperty( "ConfigFile",   m_configFile="");
    declareProperty( "Decoration",   m_decorationName="XX");

    declareProperty( "JetPtMin",              m_jetPtMin = 200000.0);
    declareProperty( "JetPtMax",              m_jetPtMax = 3000000.0);
    declareProperty( "JetEtaMax",             m_jetEtaMax = 2.0);

    declareProperty( "TaggerType",    m_tagType="XXX");

    declareProperty( "KerasConfigFile", m_kerasConfigFileName="XXX");
    declareProperty( "KerasOutput",     m_kerasConfigOutputName="XXX");

}

JSSWTopTaggerDNN::~JSSWTopTaggerDNN() {}

StatusCode JSSWTopTaggerDNN::initialize(){
  std::remove("jet_data_final.csv");
  std::remove("jet_data_scaled_shifted_rotated_flipped.csv");
  std::remove("jet_data_scaled_shifted_rotated.csv");
  std::remove("jet_data_scaled_shifted.csv");
  std::remove("jet_data_scaled.csv");
  std::remove("jet_data_untransformed.csv");

  /* Initialize the DNN tagger tool */
  ATH_MSG_INFO( (m_APP_NAME+": Initializing JSSWTopTaggerDNN tool").c_str() );
  ATH_MSG_INFO( (m_APP_NAME+": Using config file :"+m_configFile).c_str() );

  if( ! m_configFile.empty() ) {
    ATH_MSG_INFO( "Using config file : "<< m_configFile );
    // check for the existence of the configuration file
    std::string configPath;
    int releaseSeries = atoi(getenv("ROOTCORE_RELEASE_SERIES"));
    if(releaseSeries>=25) configPath = PathResolverFindDataFile(("BoostedJetTaggers/"+m_configFile).c_str());
    else {
      #ifdef ROOTCORE
          configPath = gSystem->ExpandPathName(("$ROOTCOREBIN/data/BoostedJetTaggers/"+m_configFile).c_str());
      #else
          configPath = PathResolverFindXMLFile(("$ROOTCOREBIN/data/BoostedJetTaggers/"+m_configFile).c_str());
      #endif
    }
    /* https://root.cern.ch/root/roottalk/roottalk02/5332.html */
    FileStat_t fStats;
    int fSuccess = gSystem->GetPathInfo(configPath.c_str(), fStats);
    if(fSuccess != 0){
      ATH_MSG_ERROR("Recommendations file could not be found : " << configPath);
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_DEBUG("Recommendations file was found : "<<configPath);
    }


    TEnv configReader;
    if(configReader.ReadFile( configPath.c_str(), EEnvLevel(0) ) != 0 ) {
      ATH_MSG_ERROR( "Error while reading config file : "<< configPath );
      return StatusCode::FAILURE;
    }

    // get tagger type
    m_tagType = configReader.GetValue("TaggerType" ,"");

    // get the name/path of the JSON config
    m_kerasConfigFileName = configReader.GetValue("KerasConfigFile" ,"");

    // get the name of the Keras output node
    m_kerasConfigOutputName = configReader.GetValue("KerasOutput" ,"");

    // get the configured cut values
    m_strMassCutLow  = configReader.GetValue("MassCutLow" ,"");
    m_strMassCutHigh = configReader.GetValue("MassCutHigh" ,"");
    m_strScoreCut    = configReader.GetValue("DNNCut" ,"");

    // get the decoration name
    m_decorationName = configReader.GetValue("DecorationName" ,"");


    std::cout<<"Configurations Loaded  :"<<std::endl
             <<"tagType                : "<<m_tagType <<std::endl
             <<"kerasConfigFileName    : "<<m_kerasConfigFileName <<std::endl
             <<"kerasConfigOutputName  : "<<m_kerasConfigOutputName <<std::endl
             <<"strMassCutLow          : "<<m_strMassCutLow  <<std::endl
             <<"strMassCutHigh         : "<<m_strMassCutHigh <<std::endl
             <<"strScoreCut              : "<<m_strScoreCut      <<std::endl
             <<"decorationName      : "<<m_decorationName <<std::endl
    <<std::endl;

  }
  else { // no config file
    // Assume the cut functions have been set through properties.
    // check they are non empty
    if( m_kerasConfigFileName.empty() ||
        m_kerasConfigOutputName.empty() ||
        m_strScoreCut.empty() ||
        m_strMassCutLow.empty() ||
        m_strMassCutHigh.empty() ||
        m_decorationName.empty()) {
      ATH_MSG_ERROR( "No config file provided AND you haven't manually specified all needed parameters" ) ;
      ATH_MSG_ERROR( "Please read the TWiki for this tool" );
      return StatusCode::FAILURE;
    }

  }

  // transform these strings into functions
  m_funcMassCutLow   = new TF1("strMassCutLow",  m_strMassCutLow.c_str(),  0, 14000);
  m_funcMassCutHigh  = new TF1("strMassCutHigh", m_strMassCutHigh.c_str(), 0, 14000);
  m_funcScoreCut     = new TF1("strDNNCut",      m_strScoreCut.c_str(),      0, 14000);

  ATH_MSG_INFO( ": DNN Tagger tool initialized" );
  ATH_MSG_INFO( "  Mass cut low   : "<< m_strMassCutLow );
  ATH_MSG_INFO( "  Mass cut High  : "<< m_strMassCutHigh );
  ATH_MSG_INFO( "  DNN cut low    : "<< m_strScoreCut );



// convert the JSON config file name to the full path
#ifdef ROOTCORE
    m_kerasConfigFilePath = gSystem->ExpandPathName(("$ROOTCOREBIN/data/BoostedJetTaggers/JSSWTopTaggerDNN/"+m_kerasConfigFileName).c_str());
#else
    m_kerasConfigFilePath   = PathResolverFindDataFile("BoostedJetTaggers/data/"+m_kerasConfigFileName);
#endif

    // read json file for DNN weights
    ATH_MSG_INFO( (m_APP_NAME+": DNN Tagger configured with: "+m_kerasConfigFilePath.c_str() ));

    std::ifstream input_cfg( m_kerasConfigFilePath.c_str() );

    if(input_cfg.is_open()==false){
      ATH_MSG_INFO( (m_APP_NAME+": Error openning config file : "+m_kerasConfigFilePath.c_str() ));
      ATH_MSG_INFO( (m_APP_NAME+": Are you sure that the file exists at this path?" ));
      return StatusCode::FAILURE;
    }

    lwt::JSONConfig cfg = lwt::parse_json( input_cfg );

    ATH_MSG_INFO( (m_APP_NAME+": Keras Network NLayers : "+std::to_string(cfg.layers.size()).c_str() ));

    m_lwnn = std::unique_ptr<lwt::LightweightNeuralNetwork>
                (new lwt::LightweightNeuralNetwork(cfg.inputs, cfg.layers, cfg.outputs) );

    ATH_MSG_INFO( (m_APP_NAME+": DNN Tagger tool initialized").c_str() );

    return StatusCode::SUCCESS;
}

JSSWTopTaggerDNN::Result JSSWTopTaggerDNN::result(const xAOD::Jet& jet, bool decorate) const{

  ATH_MSG_DEBUG( ": Obtaining DNN result" );

  // decorators to be used throughout
  static SG::AuxElement::Decorator<float>    dec_mcutL ("DNNTagCut_mlow");
  static SG::AuxElement::Decorator<float>    dec_mcutH ("DNNTagCut_mlow");
  static SG::AuxElement::Decorator<float>    dec_scoreCut("DNNTagCut_dnn");
  static SG::AuxElement::Decorator<float>    dec_scoreValue(m_decorationName.c_str());

  // check basic kinematic selection
  if (std::fabs(jet.eta()) > m_jetEtaMax) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (|eta| < " << m_jetEtaMax << "). Jet eta = " << jet.eta());
    if(decorate){
      std::cout<<"Decorating with DNN"<<std::endl;
      decorateJet(jet, -1., -1., -1., -666.);
    }
    return OutOfRangeEta;
  }
  if (jet.pt() < m_jetPtMin) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (pT > " << m_jetPtMin << "). Jet pT = " << jet.pt()/1.e3);
    if(decorate){
      std::cout<<"Decorating with DNN"<<std::endl;
      decorateJet(jet, -1., -1., -1., -666.);
    }
    return OutOfRangeLowPt;
  }
  if (jet.pt() > m_jetPtMax) {
    ATH_MSG_WARNING("Jet does not pass basic kinematic selection (pT < " << m_jetPtMax << "). Jet pT = " << jet.pt()/1.e3);
    if(decorate){
      std::cout<<"Decorating with DNN"<<std::endl;
      decorateJet(jet, -1., -1., -1., -666.);
    }
    return OutOfRangeHighPt;
  }

  // get the relevant attributes of the jet
  // mass and pt - note that this will depend on the configuration of the calibration used
  float jet_pt   = jet.pt()/1000.0;
  float jet_mass = jet.m()/1000.0;

  // get DNN score for the jet
  float jet_score = getScore(jet);

  // evaluate the values of the upper and lower mass bounds and the d2 cut
  float cut_mass_low  = m_funcMassCutLow ->Eval(jet_pt);
  float cut_mass_high = m_funcMassCutHigh->Eval(jet_pt);
  float cut_score     = m_funcScoreCut   ->Eval(jet_pt);

  // decorate the cut value if needed;
  if(decorate){
    std::cout<<"Decorating with score"<<std::endl;
    decorateJet(jet, cut_mass_high, cut_mass_low, cut_score, jet_score);
  }

  // evaluate the cut criteria on mass and score
  ATH_MSG_VERBOSE(": CutsValues : MassWindow=["<<std::to_string(cut_mass_low)<<","<<std::to_string(cut_mass_high)<<"]  ,  scoreCut="<<std::to_string(cut_score) );
  ATH_MSG_VERBOSE(": JetValues  : JetMass="<<std::to_string(jet_mass)<<"  ,  score="<<std::to_string(jet_score) );


  // w tagging
  if(m_tagType.compare("WBoson")==0 || m_tagType.compare("ZBoson")==0){
    ATH_MSG_VERBOSE("Determining WZ tag return");
    if( jet_mass<cut_mass_low ) {
      if ( jet_score < cut_score)
        return LowMassFailMVAFail;
      else
        return LowMassFailMVAPass;
    }
    else if ( jet_mass>cut_mass_high) {
      if ( jet_score < cut_score)
        return HighMassFailMVAFail;
      else
        return HighMassFailMVAPass;
    }
    else  if ( jet_score > cut_score){
      return MassPassMVAFail;
    }
    else{
      return MassPassMVAPass;
    }
  }
  else if(m_tagType.compare("TopQuark")==0){
    ATH_MSG_VERBOSE("Determining TopQuark tag return");
    if( jet_mass<cut_mass_low ) {
      if ( jet_score < cut_score)
        return LowMassFailMVAFail;
      else
        return LowMassFailMVAPass;
    }
    else{
      if ( jet_score < cut_score){
        return MassPassMVAFail;
      }
      else{
        return MassPassMVAPass;
      }
    }
  }

  // you should never arrive here
  return InvalidJet;
}

double JSSWTopTaggerDNN::getScore(const xAOD::Jet& jet) const{

    // create input dictionary map<string,double> for argument to lwtnn
    // std::map<std::string,double> DNN_inputValues = getJetProperties(jet);
    std::cout<<"Loading constituents from individual jet"<<std::endl;
    std::map<std::string,double> DNN_inputValues_clusters = getJetConstituents(jet); // RN
    
    std::cout<<"Preprocessing constituents"<<std::endl;
    preprocess(DNN_inputValues_clusters, jet);

    // print to a file to compare with python-based preprocessing
    store_jet_data(DNN_inputValues_clusters, jet, "jet_data_final.csv");

    for (int i = 0; i < N_CONSTITUENTS; ++i)
    {
      std::cout << "constit_"<<i <<
      "   pt  :  " << DNN_inputValues_clusters["clust_"+std::to_string(i)+"_pt"] <<
      "   eta :  " << DNN_inputValues_clusters["clust_"+std::to_string(i)+"_eta"] <<
      "   phi :  " << DNN_inputValues_clusters["clust_"+std::to_string(i)+"_phi"] << std::endl;
    }

    // evaluate the network
    lwt::ValueMap discriminant = m_lwnn->compute(DNN_inputValues_clusters);

    // obtain the output associated with the single output node
    double DNNscore(-666.);
    DNNscore = discriminant[m_kerasConfigOutputName];

    return DNNscore;
}

// RN
void JSSWTopTaggerDNN::preprocess(std::map<std::string,double> &clusters, const xAOD::Jet jet) const {
    /* Adapted from Jannicke Pearkes */
    // We assume these constituents are sorted by pt

    // Make new cluster map
    std::map<std::string,double> T_clusters;

    // Extract jet properties
    // double jet_pt = jet.pt(); // unused
    // double jet_eta = jet.eta();
    // double jet_phi = jet.phi();
    // double prim_pt = clusters["clust_0_pt"];
    double prim_eta = clusters["clust_0_eta"];
    double prim_phi = clusters["clust_0_phi"];


    store_jet_data(clusters, jet, "jet_data_untransformed.csv"); 

    // Instructions from Jannicke
    //- min max scaling (this is actually has a hard-coded min and max) 
    for (int i = 0; i < N_CONSTITUENTS; ++i) {
      clusters["clust_"+std::to_string(i)+"_pt"] = Transform::pt_min_max_scale(clusters["clust_"+std::to_string(i)+"_pt"], 0);
    }
    store_jet_data(clusters, jet, "jet_data_scaled.csv"); 

    // -  shift prim (translation about primary jet constituent)
    for (int i = 0; i < N_CONSTITUENTS; ++i) {
      clusters["clust_"+std::to_string(i)+"_eta"] = Transform::eta_shift(clusters["clust_"+std::to_string(i)+"_eta"], prim_eta);
      clusters["clust_"+std::to_string(i)+"_phi"] = Transform::phi_shift(clusters["clust_"+std::to_string(i)+"_phi"], prim_phi);
    }

    store_jet_data(clusters, jet, "jet_data_scaled_shifted.csv"); 

    // - rotate 
    // //// Code under "Calculating thetas for rotation” and “Rotating”, 
    // //// the rotation part is just a python translation of TLorentzVector’s rotate method  
    // //// https://root.cern.ch/doc/master/classTLorentzVector.html
    // Calculate axes of rotation
    double theta = 0.0;
    if (jet.getConstituents().size() >= 2){ // need at least 2 constituents 
      theta = Transform::calculate_theta_for_rotations(clusters);
    }
    // Perform the rotation // TODO do we rotate if the theta == 0.0 ?
    Transform::rotate_about_primary(clusters, theta);

    store_jet_data(clusters, jet, "jet_data_scaled_shifted_rotated.csv"); 


    // - flip     
    // Code under  elif "flip" in eta_phi_prep_type:
    Transform::flip(clusters);

    store_jet_data(clusters, jet, "jet_data_scaled_shifted_rotated_flipped.csv"); 

    // clusters = T_clusters;
    return;
}

void JSSWTopTaggerDNN::store_jet_data(std::map<std::string,double> clusters, const xAOD::Jet jet, std::string filename) const {
  


  std::ofstream jetData;
  jetData.open (filename, std::ios_base::app); // append
  jetData << 0.0 << ","; // weight
  jetData << 1.0 << ","; // label
  jetData << jet.m()/1000.0 << ","; // jet mass GeV
  jetData << jet.pt()/1000.0 << ","; // jet pt GeV
  jetData << jet.eta() << ","; // jet eta
  jetData << jet.phi() << ","; // jet phi
  jetData << 0.0 << ","; // Tau32_wta
  jetData << 0.0 << ","; // Split23
  jetData << 0.0 << ","; // NPV
  jetData << 0.0 << ","; // C2
  jetData << 0.0 << ","; // D2
  jetData << 0.0 << ","; // subjet start
  jetData << 0.0 << ","; // b eta
  jetData << 0.0 << ","; // b phi
  jetData << 0.0 << ","; // W_1 pt
  jetData << 0.0 << ","; // W_1 eta
  jetData << 0.0 << ","; // W_1 phi
  jetData << 0.0 << ","; // W_2 pt
  jetData << 0.0 << ","; // W_2 eta
  jetData << 0.0 << ","; // W_2 phi
  // jetData << 0.0 << ","; // constit start pt
  // jetData << 0.0 << ","; // 1 const eta
  // jetData << 0.0 << ","; // 1 const phi
  // jetData << 0.0 << ","; // 2 const pt
  // jetData << 0.0 << ","; // 2 const eta
  // jetData << 0.0 << ","; // 2 const phi
  // jetData << 0.0 << ","; // 3 const pt
  // jetData << 0.0 << ","; // 3 const eta
  // jetData << 0.0 << ","; // 4 const phi
  for (int i = 0; i < N_CONSTITUENTS; ++i)
  {
    jetData << clusters["clust_"+std::to_string(i)+"_pt"]  << ","; // is this is GeV ???
    jetData << clusters["clust_"+std::to_string(i)+"_eta"] << ",";
    jetData << clusters["clust_"+std::to_string(i)+"_phi"] << ",";

  }

  jetData << std::endl; 
  jetData.close();

}


void JSSWTopTaggerDNN::decorateJet(const xAOD::Jet& jet, float mcutH, float mcutL, float scoreCut, float scoreValue) const{
    /* decorate jet with attributes */

    // decorators to be used throughout
    static SG::AuxElement::Decorator<float>    dec_mcutL ("BDTTagCut_mlow");
    static SG::AuxElement::Decorator<float>    dec_mcutH ("BDTTagCut_mlow");
    static SG::AuxElement::Decorator<float>    dec_scoreCut("BDTTagCut_dnn");
    static SG::AuxElement::Decorator<float>    dec_scoreValue(m_decorationName.c_str());

    dec_mcutH(jet)      = mcutH;
    dec_mcutL(jet)      = mcutL;
    dec_scoreCut(jet)   = scoreCut;
    dec_scoreValue(jet) = scoreValue;

}

// RN
std::map<std::string,double> JSSWTopTaggerDNN::getJetConstituents(const xAOD::Jet& jet) const{
    // Update the jet constituents for this jet
    std::map<std::string,double> DNN_inputValues;

    // Initialize map with empty values. Truncate at N_CONSTITUENTS
    for (int i = 0; i < N_CONSTITUENTS; ++i)
    {
      DNN_inputValues["clust_"+std::to_string(i)+"_pt"] = EMPTY_CONSTITUENT;
      DNN_inputValues["clust_"+std::to_string(i)+"_eta"] = EMPTY_CONSTITUENT;
      DNN_inputValues["clust_"+std::to_string(i)+"_phi"] = EMPTY_CONSTITUENT;
    }

    // Extract jet constituents from jet
    std::vector<xAOD::JetConstituent> clusters = jet.getConstituents().asSTLVector();
    // Sort them by pt (using lambda function for sorting)
    std::sort(clusters.begin(), clusters.end(), 
      [](const xAOD::JetConstituent & a, const xAOD::JetConstituent & b) -> bool
      { 
          return a.pt() > b.pt(); 
      });

    ATH_MSG_INFO("clusters size: " << clusters.size());
    int count = std::min(N_CONSTITUENTS, int(clusters.size()));
    for (int i = 0; i < count; ++i)
    {
      DNN_inputValues["clust_"+std::to_string(i)+"_pt"] = clusters.at(i)->pt() / 1000.0; // convert to GeV
      // std::cout << clusters.at(i)->pt();
      DNN_inputValues["clust_"+std::to_string(i)+"_eta"] = clusters.at(i)->eta();
      // std::cout << clusters.at(i)->eta();
      DNN_inputValues["clust_"+std::to_string(i)+"_phi"] = clusters.at(i)->phi();
      // std::cout << clusters.at(i)->phi();
    }

    return DNN_inputValues;
}

std::map<std::string,double> JSSWTopTaggerDNN::getJetProperties(const xAOD::Jet& jet) const{
    // Update the jet substructure variables for this jet
    std::map<std::string,double> DNN_inputValues;

    // Splitting Scales
    DNN_inputValues["Split12"] = jet.getAttribute<float>("Split12");
    DNN_inputValues["Split23"] = jet.getAttribute<float>("Split23");

    // Energy Correlation Functions
    float ecf1(0.0);
    float ecf2(0.0);
    float ecf3(0.0);
    jet.getAttribute("ECF1", ecf1);
    jet.getAttribute("ECF2", ecf2);
    jet.getAttribute("ECF3", ecf3);
    DNN_inputValues["ECF1"] = ecf1;
    DNN_inputValues["ECF2"] = ecf2;
    DNN_inputValues["ECF3"] = ecf3;
    if (!jet.isAvailable<float>("C2"))
        DNN_inputValues["C2"] = ecf3 * ecf1 / pow(ecf2, 2.0);
    else
        DNN_inputValues["C2"] = jet.getAttribute<float>("C2");
    if (!jet.isAvailable<float>("D2"))
        DNN_inputValues["D2"] = ecf3 * pow(ecf1, 3.0) / pow(ecf2, 3.0);
    else
        DNN_inputValues["D2"] = jet.getAttribute<float>("D2");

    // N-subjettiness
    float tau1wta = jet.getAttribute<float>("Tau1_wta");
    float tau2wta = jet.getAttribute<float>("Tau2_wta");
    float tau3wta = jet.getAttribute<float>("Tau3_wta");
    DNN_inputValues["Tau1_wta"] = tau1wta;
    DNN_inputValues["Tau2_wta"] = tau2wta;
    DNN_inputValues["Tau3_wta"] = tau3wta;
    if (!jet.isAvailable<float>("Tau21_wta"))
        DNN_inputValues["Tau21_wta"] = tau2wta / tau1wta;
    else
        DNN_inputValues["Tau21_wta"] = jet.getAttribute<float>("Tau21_wta");
    if (!jet.isAvailable<float>("Tau32_wta"))
        DNN_inputValues["Tau32_wta"] = tau3wta/ tau2wta;
    else
        DNN_inputValues["Tau32_wta"] = jet.getAttribute<float>("Tau32_wta");

    // Qw observable for top tagging
    DNN_inputValues["Qw"] = jet.getAttribute<float>("Qw");

    return DNN_inputValues;
}

StatusCode JSSWTopTaggerDNN::finalize(){
    // Delete or clear anything
    return StatusCode::SUCCESS;
}
