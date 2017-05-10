/**************************************************************
//
// Created:       12 August    2016
// Last Updated:   5 May       2017
//
// Samuel Meehan
// samuel.meehan@cern.ch
//
// Implementation of standard smoothed top tagger based on cuts on
// two substructure observables
***************************************************************/
#include "BoostedJetTaggers/SmoothedTopTagger.h"

#include "PathResolver/PathResolver.h"

#include <TEnv.h>
#include <TF1.h>
#include "TSystem.h"



SmoothedTopTagger::SmoothedTopTagger( const std::string& name ) :
  JSSTaggerBase( name ),
  m_jetPtMin(350000.),
  m_jetPtMax(3000000.),
  m_jetEtaMax(2.0){

  declareProperty( "WorkingPoint", m_wkpt="" );
  declareProperty( "ConfigFile",   m_configFile="");

  declareProperty( "JetPtMin",              m_jetPtMin = 350000.0);
  declareProperty( "JetPtMax",              m_jetPtMax = 3000000.0);
  declareProperty( "JetEtaMax",             m_jetEtaMax = 2.0);

  declareProperty( "Var1CutFunc", m_var1CutExpr="", "") ;
  declareProperty( "Var2CutFunc", m_var2CutExpr="", "") ;

  declareProperty( "TaggerMode",   m_modeName="");

}


SmoothedTopTagger::~SmoothedTopTagger(){
  if( m_var1CutFunc) delete m_var1CutFunc;
  if( m_var2CutFunc) delete m_var2CutFunc;
}


StatusCode SmoothedTopTagger::initialize(){
  /* Initialize the BDT tagger tool */
  ATH_MSG_INFO( "Initializing SmoothedTopTagger tool" );


  // Define known modes :
  std::map<std::string, Mode> knownModes = {
    {"MassTau32" , MassTau32 } ,
    {"MassSplit23" , MassSplit23},
    {"Tau32Split23" , Tau32Split23},
    {"QwTau32" , QwTau32}
  };


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
      ATH_MSG_ERROR("Recommendations file could not be found : ");
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

    TString prefix = "";
    if ( ! m_wkpt.empty() ) prefix = m_wkpt+".";

    // load the variables to be used.
    m_var1CutName = configReader.GetValue( prefix+"Var1", "");
    if(m_var1CutName == "") { ATH_MSG_ERROR("Config file does not specify Var1 !") ; return StatusCode::FAILURE; }
    m_var2CutName = configReader.GetValue(prefix+"Var2", "");
    if(m_var2CutName == "") { ATH_MSG_ERROR("Config file does not specify Var2 !") ; return StatusCode::FAILURE; }


    // Now verify we know this variables :
    auto it = knownModes.find( m_var1CutName+m_var2CutName);
    if (it == knownModes.end() ) {
      ATH_MSG_ERROR("Config file has unknown couple of variable : "<< m_var1CutName << " , "<< m_var2CutName) ;
      return StatusCode::FAILURE;
    } else {
      m_mode = it->second;
      m_modeName = it->first;
    }

    // load the expression of cut functions
    m_var1CutExpr=configReader.GetValue(prefix+m_var1CutName+"Cut" ,"");
    m_var2CutExpr=configReader.GetValue(prefix+m_var2CutName+"Cut" ,"");
    if(m_var1CutExpr == "") { ATH_MSG_ERROR("Config file does not specify formula for "<< prefix+m_var1CutName+"Cut") ; return StatusCode::FAILURE; }
    if(m_var2CutExpr == "") { ATH_MSG_ERROR("Config file does not specify formula for "<< prefix+m_var2CutName+"Cut") ; return StatusCode::FAILURE; }


  } else { // no config file

    // Assume the cut functions have been set through properties.
    // check they are non empty
    if( m_var1CutExpr.empty() || m_var2CutExpr.empty() ) {
      ATH_MSG_ERROR( "No config file provided AND no cut function specified." ) ;
      ATH_MSG_ERROR( "Either set property ConfigFile or both MassCutFunc and Tau32CutFunc " );
      return StatusCode::FAILURE;
    }
    // also check we have a valid mode :
    auto it = knownModes.find( m_modeName);
    if (it == knownModes.end() ) {
      ATH_MSG_ERROR(" No valid mode specified : "<< m_modeName ) ;
      return StatusCode::FAILURE;
    } else {
      m_mode = it->second;
      m_modeName = it->first;
    }

  } // if config file


  m_var1CutFunc =  new TF1(m_var1CutName.c_str(),  m_var1CutExpr.c_str());
  m_var2CutFunc =  new TF1(m_var2CutName.c_str(),  m_var2CutExpr.c_str());

  ATH_MSG_INFO( "Smoothed top Tagger tool initialized in mode "<< m_modeName );
  ATH_MSG_INFO( m_var1CutName+"   cut : "<< m_var1CutExpr );
  ATH_MSG_INFO( m_var2CutName+"   cut : " << m_var2CutExpr );

  return StatusCode::SUCCESS;
} // end initialize()



SmoothedTopTagger::Result SmoothedTopTagger::result(const xAOD::Jet& jet, bool decorate) const {

  ATH_MSG_DEBUG( ": Obtaining Smooth top result" );

  // check basic kinematic selection
  if (std::fabs(jet.eta()) > m_jetEtaMax) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (|eta| < " << m_jetEtaMax << "). Jet eta = " << jet.eta());
    return Result(Result::OutOfRangeEta, m_mode);
  }
  if (jet.pt() < m_jetPtMin) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (pT > " << m_jetPtMin << "). Jet pT = " << jet.pt()/1.e3);
    return Result(Result::OutOfRangeLowPt, m_mode);
  }
  if (jet.pt() > m_jetPtMax) {
    ATH_MSG_WARNING("Jet does not pass basic kinematic selection (pT < " << m_jetPtMax << "). Jet pT = " << jet.pt()/1.e3);
    return Result(Result::OutOfRangeHighPt, m_mode);
  }

  // get the relevant attributes of the jet
  // mass and pt - note that this will depend on the configuration of the calibration used
  float jet_pt   = jet.pt()/1000.0;
  float jet_mass = jet.m()/1000.0;

  // evaluate the cut values on each variables
  float cut_var1  = m_var1CutFunc->Eval(jet_pt);
  float cut_var2  = m_var2CutFunc->Eval(jet_pt);

  static SG::AuxElement::Decorator<float>    dec_m ("TopTagMassCut");
  static SG::AuxElement::Decorator<float>    dec_tau32 ("TopTagTau32Cut");
  static SG::AuxElement::Decorator<float>    dec_s23 ("TopTagSplit23Cut");
  static SG::AuxElement::Decorator<float>    dec_qw ("TopTagQwCut");

  static SG::AuxElement::Accessor<float>    acc_s23 ("Split23");
  static SG::AuxElement::Accessor<float>    acc_qw ("Qw");


  switch(m_mode) {
  case MassTau32 :{
    if(decorate) { dec_m(jet) = cut_var1; dec_tau32(jet) = cut_var2;}
    float tau32 = buildTau32(jet);
    return Result( (jet_mass > cut_var1) ,  (0<=tau32) && ( tau32 < cut_var2) , m_mode);
  }
  case MassSplit23:{
    if(decorate) { dec_m(jet) = cut_var1; dec_s23(jet) = cut_var2;}
    return Result( ( jet_mass > cut_var1) , ( acc_s23(jet)/1000. > cut_var2), m_mode );
  }
  case Tau32Split23:{
    if(decorate) { dec_tau32(jet) = cut_var1; dec_s23(jet) = cut_var2;}
    float tau32 = buildTau32(jet);
    return Result(  (0<=tau32) && ( tau32 < cut_var1)  , ( acc_s23(jet)/1000.> cut_var2), m_mode );
  }
  case QwTau32:{
    if(decorate) { dec_qw(jet) = cut_var1; dec_tau32(jet) = cut_var2;}
    float tau32 = buildTau32(jet);
    return Result( ( acc_qw(jet)/1000. > cut_var1)  , (0<=tau32) && (tau32 < cut_var2), m_mode );
  }
  default: break;
  }

  // we should not arrive there !
  return Result(false, false, m_mode);

}



float SmoothedTopTagger::buildTau32(const xAOD::Jet& jet) const {

  static SG::AuxElement::ConstAccessor<float>    acc_Tau2 ("Tau2_wta");
  static SG::AuxElement::ConstAccessor<float>    acc_Tau3 ("Tau3_wta");
  static SG::AuxElement::ConstAccessor<float>    acc_Tau32 ("Tau32_wta");
  float jet_tau32;
  if((!acc_Tau2.isAvailable(jet) || !acc_Tau3.isAvailable(jet))){
    ATH_MSG_VERBOSE( ": The Tau# variables are not available in your file" );
    return -1.; // this will always fail
  }

  if( acc_Tau32.isAvailable( jet ) ) {
    jet_tau32= acc_Tau32( jet ) ;
  } else if( acc_Tau2(jet)>0.0 ){
    jet_tau32 = acc_Tau3(jet) / acc_Tau2(jet);

  } else {
    // set tau32 artificially to 0.  This is probably because the jet has one constituent, so it will fail the mass cut anyways
    jet_tau32 = 0;
  }
  return jet_tau32;
}


StatusCode SmoothedTopTagger::finalize(){
  /* Delete or clear anything */
  return StatusCode::SUCCESS;
}


bool SmoothedTopTagger::Result::massPassed() const {
  switch(m_mode) {
  case MassTau32: case MassSplit23: return m_v1;
  default:return false;
  }
}

bool SmoothedTopTagger::Result::tau32Passed() const {
  switch(m_mode) {
  case MassTau32:
  case QwTau32:       return m_v2;
  case Tau32Split23:  return m_v1;
  default:return false;
  }
}

bool SmoothedTopTagger::Result::split23Passed() const {
  switch(m_mode) {
  case MassSplit23:   return m_v2;
  case Tau32Split23:  return m_v2;
  default:return false;
  }
}

bool SmoothedTopTagger::Result::qwPassed() const {
  switch(m_mode) {
  case QwTau32:   return m_v1;
  default:return false;
  }
}


// the end
