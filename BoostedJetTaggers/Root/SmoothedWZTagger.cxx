/**************************************************************
//
// Created:       12 August    2016
// Last Updated:   5 May       2017
//
// Samuel Meehan
// samuel.meehan@cern.ch
//
// Implementation of simple smoothed W and Z taggers based on cut on mass and
// a single substructure observables
***************************************************************/
#include "BoostedJetTaggers/SmoothedWZTagger.h"

#include "PathResolver/PathResolver.h"

#include "TEnv.h"
#include "TF1.h"
#include "TSystem.h"

SmoothedWZTagger::SmoothedWZTagger( const std::string& name ) :
  JSSTaggerBase( name ),
  m_jetPtMin(200000.),
  m_jetPtMax(10000000.),
  m_jetEtaMax(2.0){

  declareProperty( "WorkingPoint", m_wkpt="" );
  declareProperty( "TaggerType",   m_tagType="");
  declareProperty( "ConfigFile",   m_configFile="");

  declareProperty( "JetPtMin",              m_jetPtMin = 200000.0);
  declareProperty( "JetPtMax",              m_jetPtMax = 3000000.0);
  declareProperty( "JetEtaMax",             m_jetEtaMax = 2.0);

  declareProperty( "MassCutLowFunc",      m_strMassCutLow="" , "");
  declareProperty( "MassCutHighFunc",     m_strMassCutHigh="" , "");
  declareProperty( "D2CutFunc",           m_strD2Cut="" , "");
}

SmoothedWZTagger::~SmoothedWZTagger() {}


StatusCode SmoothedWZTagger::initialize(){
  /* Initialize the BDT tagger tool */
  ATH_MSG_INFO( "Initializing SmoothedWZTagger tool" );

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

    if( m_wkpt.empty() ){
      m_strMassCutLow=configReader.GetValue("MassCutLow" ,"");
      m_strMassCutHigh=configReader.GetValue("MassCutHigh" ,"");
      m_strD2Cut=configReader.GetValue("D2Cut" ,"");
    } else {
      m_strMassCutLow=configReader.GetValue((m_wkpt+".MassCutLow").c_str() ,"");
      m_strMassCutHigh=configReader.GetValue((m_wkpt+".MassCutHigh").c_str() ,"");
      m_strD2Cut=configReader.GetValue((m_wkpt+".D2Cut").c_str() ,"");
    }

  } else { // no config file
    // Assume the cut functions have been set through properties.
    // check they are non empty
    if( m_strD2Cut.empty() || m_strMassCutLow.empty() || m_strMassCutHigh.empty()) {
      ATH_MSG_ERROR( "No config file provided AND no cut function specified." ) ;
      ATH_MSG_ERROR( "Either set property ConfigFile or both MassCutLowFunc, MassCutHighFunc and D2CutFunc " );
      return StatusCode::FAILURE;
    }

  }

  // transform these strings into functions
  m_funcMassCutLow   = new TF1("strMassCutLow",  m_strMassCutLow.c_str(),  0, 14000);
  m_funcMassCutHigh  = new TF1("strMassCutHigh", m_strMassCutHigh.c_str(), 0, 14000);
  m_funcD2Cut        = new TF1("strD2Cut",       m_strD2Cut.c_str(),       0, 14000);

  ATH_MSG_INFO( ": Smoothed WZ Tagger tool initialized" );
  ATH_MSG_INFO( "  Mass cut low   : "<< m_strMassCutLow );
  ATH_MSG_INFO( "  Mass cut High  : "<< m_strMassCutHigh );
  ATH_MSG_INFO( "  D2 cut low : "<< m_strD2Cut );


  return StatusCode::SUCCESS;
} // end initialize()



SmoothedWZTagger::Result SmoothedWZTagger::result(const xAOD::Jet& jet, bool decorate) const {

  ATH_MSG_DEBUG( ": Obtaining Smooth WZ result" );

  // check basic kinematic selection
  if (std::fabs(jet.eta()) > m_jetEtaMax) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (|eta| < " << m_jetEtaMax << "). Jet eta = " << jet.eta());
    return OutOfRangeEta;
  }
  if (jet.pt() < m_jetPtMin) {
    ATH_MSG_DEBUG("Jet does not pass basic kinematic selection (pT > " << m_jetPtMin << "). Jet pT = " << jet.pt()/1.e3);
    return OutOfRangeLowPt;
  }
  if (jet.pt() > m_jetPtMax) {
    ATH_MSG_WARNING("Jet does not pass basic kinematic selection (pT < " << m_jetPtMax << "). Jet pT = " << jet.pt()/1.e3);
    return OutOfRangeHighPt;
  }

  // get the relevant attributes of the jet
  // mass and pt - note that this will depend on the configuration of the calibration used
  float jet_pt   = jet.pt()/1000.0;
  float jet_mass = jet.m()/1000.0;

  // d2 - check if its there, otherwise, write it out
  float jet_d2;

  static SG::AuxElement::ConstAccessor<float>    acc_D2   ("D2");
  static SG::AuxElement::ConstAccessor<float>    acc_ECF1 ("ECF1");
  static SG::AuxElement::ConstAccessor<float>    acc_ECF2 ("ECF2");
  static SG::AuxElement::ConstAccessor<float>    acc_ECF3 ("ECF3");

  if(acc_D2.isAvailable(jet)){
    jet_d2 = acc_D2(jet);
  } else {
    if((!acc_ECF1.isAvailable(jet) || !acc_ECF2.isAvailable(jet) || !acc_ECF3.isAvailable(jet))){
      ATH_MSG_VERBOSE(": D2 is not available in the file your using, neither are the ECF# variables" );
      return InvalidJet;
    }
    if( acc_ECF2(jet)>0.0 ){
      jet_d2 = acc_ECF3(jet) * pow(acc_ECF1(jet), 3.0) / pow(acc_ECF2(jet), 3.0);
    }
    else{
      // set d2 artificially to 0.  This is probably because the jet has one constituent, so it will fail the mass cut anyways
      jet_d2 = 0;
    }

  }

  // evaluate the values of the upper and lower mass bounds and the d2 cut
  float cut_mass_low  = m_funcMassCutLow ->Eval(jet_pt);
  float cut_mass_high = m_funcMassCutHigh->Eval(jet_pt);
  float cut_d2        = m_funcD2Cut      ->Eval(jet_pt);

  // decorate the cut value if needed;
  if(decorate){
    static SG::AuxElement::Decorator<float>    dec_mcutL ("WZLowWMassCut");
    static SG::AuxElement::Decorator<float>    dec_mcutH ("WZHighWMassCut");
    static SG::AuxElement::Decorator<float>    dec_d2cut ("WZD2Cut");

    dec_d2cut(jet) = cut_d2;
    dec_mcutH(jet) = cut_mass_high;
    dec_mcutL(jet) = cut_mass_low;
  }

  // evaluate the cut criteria on mass and d2

  ATH_MSG_VERBOSE(": CutsValues : MassWindow=["<<std::to_string(cut_mass_low)<<","<<std::to_string(cut_mass_high)<<"]  ,  D2Cut="<<std::to_string(cut_d2) );
  ATH_MSG_VERBOSE(": CutsValues : JetMass="<<std::to_string(jet_mass)<<"  ,  D2="<<std::to_string(jet_d2) );

  if( jet_mass<cut_mass_low ) {
    if ( jet_d2 > cut_d2) return LowMassFailD2Fail;
    else return LowMassFailD2Pass;

  } else if ( jet_mass>cut_mass_high) {
    if ( jet_d2 > cut_d2) return HighMassFailD2Fail;
    else return HighMassFailD2Pass;

  } else  if ( jet_d2 > cut_d2) return MassPassD2Fail;

  return MassPassD2Pass;
}



StatusCode SmoothedWZTagger::finalize(){
  /* Delete or clear anything */
  return StatusCode::SUCCESS;
}


// the end
