/**************************************************************
//
// Created:       12 August    2016
// Last Updated:   8 November  2016
//
// Daniel Marley
// daniel.edison.marley@cern.ch
//
// Work by: Tatsumi Nitta & Tobias Kupfer
// Original svn directory:
//   https://svnweb.cern.ch/trac/atlasperf/browser/CombPerf/JetETMiss/TopBosonTagAnalysis2016/BDTtagger/
// Gitlab directory:
//   https://gitlab.cern.ch/tnitta/BosonTopTaggingBDT.git
//
// BDT Tagging of Large-R jets as W/top
//
//   There are two components to the tagger:
//    1. Load the weights from the trained tagger
//    2. Determine if jet passes working point
//       (use trained tagger info to get discriminant for new jet)
//
//    :: NONE -> 0
//    :: BDT  -> 1
//    :: MASS -> 2
//    :: BOTH -> 3
//    :: OUT OF KINEMATIC RANGE -> -5
//    :: BAD CONFIGURATION      -> -9
//
//
// FitFunctions*.root accessed on 8 Nov 2016 19:00 (EST)
***************************************************************/
#include "BoostedJetTaggers/JSSWTopTaggerBDT.h"

#include "PathResolver/PathResolver.h"

#include "TEnv.h"
#include "TF1.h"
#include "TSystem.h"

#define APP_NAME "JSSWTopTaggerBDT"

#define CERRD std::cout<<"SAM Error : "<<__FILE__<<"  "<<__LINE__<<std::endl;


JSSWTopTaggerBDT::JSSWTopTaggerBDT( const std::string& name ) :
  JSSTaggerBase( name ),
  m_name(name),
  m_APP_NAME(APP_NAME),
  m_inputVariableSet("NvarM"),
  m_BDTmethod("BDTG method")
  {

    declareProperty( "ConfigFile",   m_configFile="");

    declareProperty( "Decoration",   m_decorationName="XX");
    declareProperty( "DecorateJet",           m_decorate = true);

    declareProperty( "JetPtMin",              m_jetPtMin = 200000.0);
    declareProperty( "JetPtMax",              m_jetPtMax = 3000000.0);
    declareProperty( "JetEtaMax",             m_jetEtaMax = 2.0);

    declareProperty( "TaggerType",    m_tagType="XXX");

    declareProperty( "CalibArea",      m_calibarea = "BoostedJetTaggers/JSSWTopTaggerBDT/Moriond2017/");
    declareProperty( "TMVAConfigFile", m_tmvaConfigFileName="XXX");


}

JSSWTopTaggerBDT::~JSSWTopTaggerBDT() {}


StatusCode JSSWTopTaggerBDT::initialize(){
  /* Initialize the BDT tagger tool */
  ATH_MSG_INFO( (m_APP_NAME+": Initializing JSSWTopTaggerBDT tool").c_str() );
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

    // get the CVMFS calib area where stuff is stored
    m_calibarea = configReader.GetValue("CalibArea" ,"");

    // get the name/path of the JSON config
    m_tmvaConfigFileName = configReader.GetValue("TMVAConfigFile" ,"");

    // input parameter setting type
    m_inputVariableSet = configReader.GetValue("InputVariableSet" ,"");

    // get the configured cut values
    m_strMassCutLow  = configReader.GetValue("MassCutLow" ,"");
    m_strMassCutHigh = configReader.GetValue("MassCutHigh" ,"");
    m_strScoreCut    = configReader.GetValue("ScoreCut" ,"");

    // get the decoration name
    m_decorationName = configReader.GetValue("DecorationName" ,"");

    std::cout<<"Configurations Loaded  :"<<std::endl
             <<"tagType                : "<<m_tagType <<std::endl
             <<"calibarea              : "<<m_calibarea <<std::endl
             <<"tmvaConfigFileName     : "<<m_tmvaConfigFileName <<std::endl
             <<"inputVariableSet       : "<<m_inputVariableSet<<std::endl
             <<"strMassCutLow          : "<<m_strMassCutLow  <<std::endl
             <<"strMassCutHigh         : "<<m_strMassCutHigh <<std::endl
             <<"strScoreCut            : "<<m_strScoreCut      <<std::endl
             <<"decorationName         : "<<m_decorationName <<std::endl
    <<std::endl;

  }
  else { // no config file
    // Assume the cut functions have been set through properties.
    // check they are non empty
    if( m_tmvaConfigFileName.empty() ||
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
  m_funcScoreCut       = new TF1("strScoreCut",      m_strScoreCut.c_str(),      0, 14000);

  ATH_MSG_INFO( ": BDT Tagger tool initialized" );
  ATH_MSG_INFO( "  Mass cut low   : "<< m_strMassCutLow );
  ATH_MSG_INFO( "  Mass cut High  : "<< m_strMassCutHigh );
  ATH_MSG_INFO( "  Score cut low    : "<< m_strScoreCut );

  // get the config file from CVMFS
  // necessary because xml files are too large to house on the data space
  m_tmvaConfigFilePath = PathResolverFindCalibFile( (m_calibarea+m_tmvaConfigFileName).c_str() );

  // read json file for DNN weights
  ATH_MSG_INFO( (m_APP_NAME+": BDT Tagger configured with: "+m_tmvaConfigFilePath.c_str() ));

  // -- Initialize TMVA for BDTs
  TMVA::Tools::Instance();
  m_bdtTagger = std::unique_ptr<TMVA::Reader> ( new TMVA::Reader( "!Color:!Silent" ) );

  std::string taggerType("");
  if (m_tagType.compare("WQuark")==0){
    ATH_MSG_DEBUG( "W Boson BDT Tagger Selected" );
    if(!m_inputVariableSet.compare("NvarS") || !m_inputVariableSet.compare("NvarM") || !m_inputVariableSet.compare("NvarL") || !m_inputVariableSet.compare("NvarAll")){
      m_bdtTagger->AddVariable( "fjet_D2",         &m_D2);
      m_bdtTagger->AddVariable( "fjet_ECF3",       &m_ECF3);
      m_bdtTagger->AddVariable( "fjet_C2",         &m_C2);
      m_bdtTagger->AddVariable( "fjet_ThrustMin",  &m_ThrustMin);
      m_bdtTagger->AddVariable( "fjet_Angularity", &m_Angularity);
      m_bdtTagger->AddVariable( "fjet_PlanarFlow", &m_PlanarFlow);
      m_bdtTagger->AddVariable( "fjet_Tau21_wta",  &m_Tau21_wta);

      if(!m_inputVariableSet.compare("NvarM") || !m_inputVariableSet.compare("NvarL") || !m_inputVariableSet.compare("NvarAll")){
        m_bdtTagger->AddVariable( "fjet_Aplanarity",   &m_Aplanarity);
        m_bdtTagger->AddVariable( "fjet_FoxWolfram20", &m_FoxWolfram20);
        m_bdtTagger->AddVariable( "fjet_Split12",      &m_Split12);
        m_bdtTagger->AddVariable( "fjet_ZCut12",       &m_ZCut12);
        m_bdtTagger->AddVariable( "fjet_KtDR",         &m_KtDR);
        m_bdtTagger->AddVariable( "fjet_ECF2",         &m_ECF2);

        if(!m_inputVariableSet.compare("NvarL") || !m_inputVariableSet.compare("NvarAll")){
          m_bdtTagger->AddVariable( "fjet_Tau1_wta",   &m_Tau1_wta);
          m_bdtTagger->AddVariable( "fjet_Sphericity", &m_Sphericity);
          m_bdtTagger->AddVariable( "fjet_Dip12",      &m_Dip12);

          if(!m_inputVariableSet.compare("NvarAll")){
            m_bdtTagger->AddVariable( "fjet_ThrustMaj", &m_ThrustMaj);
            m_bdtTagger->AddVariable( "fjet_Tau2_wta",  &m_Tau2_wta);
            m_bdtTagger->AddVariable( "fjet_ECF1",      &m_ECF1);
          }
        }
      }
    }
    else{
      ATH_MSG_ERROR( "You have chosen a variable set that is un-supported. Exiting." );
      return StatusCode::FAILURE;
    }
  }
  else if (m_tagType.compare("TopQuarkInclusive")==0){

    ATH_MSG_DEBUG( "Top Quark (Inclusive) BDT Tagger Selected" );
    if(!m_inputVariableSet.compare("NvarS") || !m_inputVariableSet.compare("NvarM") || !m_inputVariableSet.compare("NvarL") || !m_inputVariableSet.compare("NvarAll")){
      m_bdtTagger->AddVariable( "fjet_Tau32_wta", &m_Tau32_wta  );
      m_bdtTagger->AddVariable( "fjet_Qw",        &m_Qw  );
      m_bdtTagger->AddVariable( "fjet_ECF3",      &m_ECF3  );
      m_bdtTagger->AddVariable( "fjet_D2",        &m_D2  );
      m_bdtTagger->AddVariable( "fjet_Tau21_wta", &m_Tau21_wta  );

      if(!m_inputVariableSet.compare("NvarM") || !m_inputVariableSet.compare("NvarL") || !m_inputVariableSet.compare("NvarAll")){
        m_bdtTagger->AddVariable( "fjet_ECF1",    &m_ECF1  );
        m_bdtTagger->AddVariable( "fjet_Split12", &m_Split12  );

        if(!m_inputVariableSet.compare("NvarL") || !m_inputVariableSet.compare("NvarAll")){
          m_bdtTagger->AddVariable( "fjet_Tau3_wta", &m_Tau3_wta  );
          m_bdtTagger->AddVariable( "fjet_Split23",  &m_Split23  );
          m_bdtTagger->AddVariable( "fjet_C2",       &m_C2  );

          if(!m_inputVariableSet.compare("NvarAll")){
            m_bdtTagger->AddVariable( "fjet_Tau2_wta", &m_Tau2_wta  );
            m_bdtTagger->AddVariable( "fjet_ECF2",     &m_ECF2  );
            m_bdtTagger->AddVariable( "fjet_Tau1_wta", &m_Tau1_wta  );
          }
        }
      }
    }
    else{
      ATH_MSG_ERROR( "You have chosen a variable set that is un-supported. Exiting." );
      return StatusCode::FAILURE;
    }
  }

  else if (m_tagType.compare("TopQuarkContained")==0){
    ATH_MSG_DEBUG( "Top Quark (Fully Contained) BDT Tagger Selected" );
    if(!m_inputVariableSet.compare("NvarS") || !m_inputVariableSet.compare("NvarM") || !m_inputVariableSet.compare("NvarL") || !m_inputVariableSet.compare("NvarAll")){
      m_bdtTagger->AddVariable( "fjet_Tau32_wta", &m_Tau32_wta  );
      m_bdtTagger->AddVariable( "fjet_ECF3",      &m_ECF3  );
      m_bdtTagger->AddVariable( "fjet_Qw",        &m_Qw  );
      m_bdtTagger->AddVariable( "fjet_D2",        &m_D2  );
      m_bdtTagger->AddVariable( "fjet_ECF2",      &m_ECF2  );
      m_bdtTagger->AddVariable( "fjet_Split23",   &m_Split23  );
      m_bdtTagger->AddVariable( "fjet_Tau21_wta", &m_Tau21_wta  );

      if(!m_inputVariableSet.compare("NvarM") || !m_inputVariableSet.compare("NvarL") || !m_inputVariableSet.compare("NvarAll")){
        m_bdtTagger->AddVariable( "fjet_Split12",  &m_Split12  );
        m_bdtTagger->AddVariable( "fjet_Tau3_wta", &m_Tau3_wta  );

        if(!m_inputVariableSet.compare("NvarL") || !m_inputVariableSet.compare("NvarAll")){
          m_bdtTagger->AddVariable( "fjet_ECF1",     &m_ECF1  );
          m_bdtTagger->AddVariable( "fjet_C2",       &m_C2  );
          m_bdtTagger->AddVariable( "fjet_Tau2_wta", &m_Tau2_wta  );
          m_bdtTagger->AddVariable( "fjet_Tau1_wta", &m_Tau1_wta  );

          if(!m_inputVariableSet.compare("NvarAll")){
            m_bdtTagger->AddVariable( "fjet_Tau2_wta", &m_Tau2_wta  );
            m_bdtTagger->AddVariable( "fjet_ECF2",     &m_ECF2  );
            m_bdtTagger->AddVariable( "fjet_Tau1_wta", &m_Tau1_wta  );
          }
        }
      }
    }
    else{
      ATH_MSG_ERROR( "You have chosen a variable set that is un-supported. Exiting." );
      return StatusCode::FAILURE;
    }
  }

  // configure the bdt
  m_bdtTagger->BookMVA( m_BDTmethod.c_str(), m_tmvaConfigFilePath.c_str() );

  //setting the possible states that the tagger can be left in after the JSSTaggerBase::tag() function is called
  m_accept.addCut( "ValidPtRangeHigh"    , "True if the jet is not too high pT"  );
  m_accept.addCut( "ValidPtRangeLow"     , "True if the jet is not too low pT"   );
  m_accept.addCut( "ValidEtaRange"       , "True if the jet is not too forward"     );
  m_accept.addCut( "ValidJetContent"     , "True if the jet is alright technicall (e.g. all attributes necessary for tag)"        );

  if(m_tagType.compare("WBoson")==0 || m_tagType.compare("ZBoson")==0){
    m_accept.addCut( "PassMassLow"         , "mJet > mCutLow"       );
    m_accept.addCut( "PassMassHigh"        , "mJet < mCutHigh"      );
    m_accept.addCut( "PassScore"           , "ScoreJet > ScoreCut"         );
  }
  if(m_tagType.compare("TopQuark")==0){
    m_accept.addCut( "PassMassLow"         , "mJet > mCutLow"       );
    m_accept.addCut( "PassScore"           , "ScoreJet > ScoreCut"         );
  }
  //loop over and print out the cuts that have been configured
  ATH_MSG_INFO( "After tagging, you will have access to the following cuts as a Root::TAccept : (<NCut>) <cut> : <description>)" );
  showCuts();


  ATH_MSG_INFO( "BDT Tagger tool initialized" );

  return StatusCode::SUCCESS;
}



Root::TAccept JSSWTopTaggerBDT::tag(const xAOD::Jet& jet) const {
  // helpful execute message
  ATH_MSG_DEBUG("Obtaining BDT result" );

  //clear all accept values
  m_accept.clear();

  // set the jet validity bits to 1 by default
  m_accept.setCutResult( "ValidPtRangeHigh", true);
  m_accept.setCutResult( "ValidPtRangeLow" , true);
  m_accept.setCutResult( "ValidEtaRange"   , true);
  m_accept.setCutResult( "ValidJetContent" , true);

  // check basic kinematic selection
  if (std::fabs(jet.eta()) > m_jetEtaMax) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (|eta| < " << m_jetEtaMax << "). Jet eta = " << jet.eta());
    m_accept.setCutResult("ValidEtaRange", false);
    if(m_decorate)
      decorateJet(jet, -1., -1., -1., -666.);
  }
  if (jet.pt() < m_jetPtMin) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (pT > " << m_jetPtMin << "). Jet pT = " << jet.pt()/1.e3);
    m_accept.setCutResult("ValidPtRangeLow", false);
    if(m_decorate)
      decorateJet(jet, -1., -1., -1., -666.);
  }
  if (jet.pt() > m_jetPtMax) {
    ATH_MSG_WARNING("Jet does not pass basic kinematic selection (pT < " << m_jetPtMax << "). Jet pT = " << jet.pt()/1.e3);
    m_accept.setCutResult("ValidPtRangeHigh", false);
    if(m_decorate)
      decorateJet(jet, -1., -1., -1., -666.);
  }

  // get the relevant attributes of the jet
  // mass and pt - note that this will depend on the configuration of the calibration used
  float jet_pt   = jet.pt()/1000.0;
  float jet_mass = jet.m()/1000.0;

  // get BDT score
  float jet_score = getScore(jet);

  // evaluate the values of the upper and lower mass bounds and the d2 cut
  float cut_mass_low  = m_funcMassCutLow ->Eval(jet_pt);
  float cut_mass_high = m_funcMassCutHigh->Eval(jet_pt);
  float cut_score     = m_funcScoreCut   ->Eval(jet_pt);

  // decorate the cut value if needed;
  if(m_decorate){
    std::cout<<"Decorating with score"<<std::endl;
    decorateJet(jet, cut_mass_high, cut_mass_low, cut_score, jet_score);
  }

  // evaluate the cut criteria on mass and score
  ATH_MSG_VERBOSE(": CutsValues : MassWindow=["<<std::to_string(cut_mass_low)<<","<<std::to_string(cut_mass_high)<<"]  ,  scoreCut="<<std::to_string(cut_score) );
  ATH_MSG_VERBOSE(": JetValues  : JetMass="<<std::to_string(jet_mass)<<"  ,  score="<<std::to_string(jet_score) );

  //set the TAccept depending on whether it is a W/Z or a top tag
  if(m_tagType.compare("WBoson")==0 || m_tagType.compare("ZBoson")==0){
    ATH_MSG_VERBOSE("Determining WZ tag return");
    if( jet_mass>cut_mass_low )
      m_accept.setCutResult( "PassMassLow"  , true );
    if( jet_mass<cut_mass_high )
      m_accept.setCutResult( "PassMassHigh" , true );
    if( jet_score > cut_score )
      m_accept.setCutResult( "PassScore"    , true );
  }
  else if(m_tagType.compare("TopQuark")==0){
    ATH_MSG_VERBOSE("Determining TopQuark tag return");
    if( jet_mass>cut_mass_low )
      m_accept.setCutResult( "PassMassLow"  , true );
    if( jet_score > cut_score )
      m_accept.setCutResult( "PassScore"    , true );
  }

  // return the TAccept object that you created and filled
  return m_accept;
}


float JSSWTopTaggerBDT::getScore(const xAOD::Jet& jet) const{

    // load the new values of the variables for this jet
    getJetProperties(jet);

    // evaluate bdt
    float bdt_score(-666.);
    bdt_score = m_bdtTagger->EvaluateMVA( m_BDTmethod.c_str() );

    return bdt_score;
}


void JSSWTopTaggerBDT::decorateJet(const xAOD::Jet& jet, float mcutH, float mcutL, float scoreCut, float scoreValue) const{
    /* decorate jet with attributes */

    // decorators to be used throughout
    static SG::AuxElement::Decorator<float>    dec_mcutL ("BDTTagCut_mlow");
    static SG::AuxElement::Decorator<float>    dec_mcutH ("BDTTagCut_mlow");
    static SG::AuxElement::Decorator<float>    dec_scoreCut("BDTTagCut_score");
    static SG::AuxElement::Decorator<float>    dec_scoreValue(m_decorationName.c_str());

    dec_mcutH(jet)      = mcutH;
    dec_mcutL(jet)      = mcutL;
    dec_scoreCut(jet)   = scoreCut;
    dec_scoreValue(jet) = scoreValue;

}


void JSSWTopTaggerBDT::getJetProperties(const xAOD::Jet& jet) const{
    /* Update the jet substructure variables for this jet */
    // Splitting Scales
    m_Split12 = jet.getAttribute<float>("Split12");
    m_Split23 = jet.getAttribute<float>("Split23");

    // Energy Correlation Functions
    jet.getAttribute("ECF1", m_ECF1);
    jet.getAttribute("ECF2", m_ECF2);
    jet.getAttribute("ECF3", m_ECF3);

    if (!jet.isAvailable<float>("C2"))
        m_C2 = m_ECF3 * m_ECF1 / pow(m_ECF2, 2.0);
    else
        m_C2 = jet.getAttribute<float>("C2");
    if (!jet.isAvailable<float>("D2"))
        m_D2 = m_ECF3 * pow(m_ECF1, 3.0) / pow(m_ECF2, 3.0);
    else
        m_D2 = jet.getAttribute<float>("D2");

    // N-subjettiness
    m_Tau1_wta = jet.getAttribute<float>("Tau1_wta");
    m_Tau2_wta = jet.getAttribute<float>("Tau2_wta");
    m_Tau3_wta = jet.getAttribute<float>("Tau3_wta");

    if (!jet.isAvailable<float>("Tau21_wta"))
        m_Tau21_wta = m_Tau2_wta / m_Tau1_wta;
    else
        m_Tau21_wta = jet.getAttribute<float>("Tau21_wta");

    if (!jet.isAvailable<float>("Tau32_wta"))
        m_Tau32_wta = m_Tau3_wta/ m_Tau2_wta;
    else
        m_Tau32_wta = jet.getAttribute<float>("Tau32_wta");


    // Other Substructure Variables
    m_Qw           = jet.getAttribute<float>("Qw");
    m_KtDR         = jet.getAttribute<float>("KtDR");
    m_Dip12        = jet.getAttribute<float>("Dip12");
    m_ZCut12       = jet.getAttribute<float>("ZCut12");
    m_ThrustMin    = jet.getAttribute<float>("ThrustMin");
    m_ThrustMaj    = jet.getAttribute<float>("ThrustMaj");
    m_Angularity   = jet.getAttribute<float>("Angularity");
    m_Aplanarity   = jet.getAttribute<float>("Aplanarity");
    m_Sphericity   = jet.getAttribute<float>("Sphericity");
    m_PlanarFlow   = jet.getAttribute<float>("PlanarFlow");
    m_FoxWolfram20 = jet.getAttribute<float>("FoxWolfram2") / jet.getAttribute<float>("FoxWolfram0");

    return;
}


StatusCode JSSWTopTaggerBDT::finalize(){
    /* Delete or clear anything */
    return StatusCode::SUCCESS;
}


// the end
