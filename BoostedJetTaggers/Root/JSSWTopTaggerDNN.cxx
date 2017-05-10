/**************************************************************
//
// Created:        8 November  2016
// Last Updated:   8 November  2016
//
// Daniel Marley
// daniel.edison.marley@cern.ch
//
// Work by: Ece Akilli, Dan Guest, & Oliver Majersky
// Gitlab directory:
//   https://gitlab.cern.ch/eakilli/TopBosonTaggingDL
//   https://gitlab.cern.ch/dguest/JetSubstructureTools/tree/master
//
// DNN Tagging of Large-R jets as W/top
//
//   There are two components to the tagger:
//    1. Load the weights from the trained tagger
//    2. Determine if jet passes working point
//       (use trained tagger info to get discriminant for new jet)
//
//    :: NONE -> 0
//    :: DNN  -> 1
//    :: OUT OF KINEMATIC RANGE -> -5
//    :: BAD CONFIGURATION      -> -9
//
***************************************************************/
#include "BoostedJetTaggers/JSSWTopTaggerDNN.h"

#ifndef ROOTCORE
#include "PathResolver/PathResolver.h"
#endif

#define APP_NAME "JSSWTopTaggerDNN"


JSSWTopTaggerDNN::JSSWTopTaggerDNN( const std::string& name ) :
  JSSTaggerBase( name ),
  m_name(name),
  m_APP_NAME(APP_NAME),
  m_wkptFileName("SetMe"),
  m_lwnn(nullptr),
  m_ptMin(200000.),
  m_etaMax(2.0){
    declareProperty( "WorkingPoint", m_wkpt="80" );
    declareProperty( "Decoration",   m_DNNdecorationName="SetMe");   // set below (if not by user)
    declareProperty( "WTagger",      m_wtag=false);
    declareProperty( "ZTagger",      m_ztag=false);
    declareProperty( "TopTagger",    m_toptag=false);
}

JSSWTopTaggerDNN::~JSSWTopTaggerDNN() {}


StatusCode JSSWTopTaggerDNN::initialize(){
    /* Initialize the DNN tagger tool */
    ATH_MSG_INFO( (m_APP_NAME+": Initializing JSSWTopTaggerDNN tool").c_str() );

    // -- Sanity check
    if (!m_toptag && !m_wtag && !m_ztag){
        ATH_MSG_ERROR( (m_APP_NAME+": Selected neither W, Z, nor top tagging DNN.").c_str() );
        return StatusCode::FAILURE;
    }

    // -- Get the working point files
    std::string taggerType("");       // what kind of tagger the user selected
    std::string wkptDataName("");     // the data name to access working points
    if (m_wtag){
        taggerType     = "wtag";
        m_wkptFileName = "BoostedJetTaggers/keras_w_dnn_2.dat";
        wkptDataName   = "DNN_Wtag_"+m_wkpt+"wp"; // dummy
    }
    else if (m_ztag){
        taggerType     = "ztag";
        m_wkptFileName = "BoostedJetTaggers/keras_z_dnn_2.dat";
        wkptDataName   = "DNN_Ztag_"+m_wkpt+"wp"; // dummy
    }
    else if (m_toptag){
        taggerType     = "toptag";
        m_wkptFileName = "BoostedJetTaggers/keras_top_dnn_2.dat";
        wkptDataName   = "DNN_TOPtag_"+m_wkpt+"wp";
    }

    // -- Grab the weights from json files
    m_DNN_weights.clear();
    std::string wtag_filename   = "keras_w_dnn_2.json";    // filler
    std::string ztag_filename   = "keras_z_dnn_2.json";    // filler
    std::string toptag_filename = "keras_top_dnn_2.json";

#ifdef ROOTCORE
    m_wkptFileName = gSystem->ExpandPathName( ("$ROOTCOREBIN/data/"+m_wkptFileName).c_str() );
    m_DNN_weights["wtag"]   = gSystem->ExpandPathName(("$ROOTCOREBIN/data/BoostedJetTaggers/"+wtag_filename).c_str());
    m_DNN_weights["ztag"]   = gSystem->ExpandPathName(("$ROOTCOREBIN/data/BoostedJetTaggers/"+toptag_filename).c_str());
    m_DNN_weights["toptag"] = gSystem->ExpandPathName(("$ROOTCOREBIN/data/BoostedJetTaggers/"+toptag_filename).c_str());
#else
    m_wkptFileName = PathResolverFindDataFile( m_wkptFileName );
    m_DNN_weights["wtag"]   = PathResolverFindDataFile("BoostedJetTaggers/data/"+wtag_filename);
    m_DNN_weights["ztag"]   = PathResolverFindDataFile("BoostedJetTaggers/data/"+ztag_filename);
    m_DNN_weights["toptag"] = PathResolverFindDataFile("BoostedJetTaggers/data/"+toptag_filename);
#endif

    // working point data
    m_wkptFile = TFile::Open(m_wkptFileName.c_str()); // file that contains TF1s for tagging
    m_wkpt_DNN = (TF1*)m_wkptFile->Get( wkptDataName.c_str() );

    // read json file for DNN weights
    std::ifstream input_cfg( m_DNN_weights.at(taggerType).c_str() );
    lwt::JSONConfig cfg = lwt::parse_json( input_cfg );
    m_lwnn = std::unique_ptr<lwt::LightweightNeuralNetwork>
                (new lwt::LightweightNeuralNetwork(cfg.inputs, cfg.layers, cfg.outputs) );

    // -- Set the name for decorated attributes (if not defined by user)
    if (m_DNNdecorationName.compare("SetMe")==0){
        m_DNNdecorationName = "DNNscore_"+taggerType+"_"+m_wkpt;
    }

    ATH_MSG_INFO( (m_APP_NAME+": DNN Tagger tool initialized").c_str() );

    return StatusCode::SUCCESS;
} // end initialize()


int JSSWTopTaggerDNN::result(const xAOD::Jet& jet) const{
    /*
       Return DNN result to user (as decoration)
        1. Update variables needed by DNN from each jet
        2. Decorate jet with new values
        -> Return a value based on working point and if the jet is tagged or not
           0 = Not tagged
           1 = Tagged
    */
    ATH_MSG_DEBUG( (m_APP_NAME+": Obtaining DNN result").c_str() );

    // extra protection
    if (!m_wtag && !m_ztag && !m_toptag){
        ATH_MSG_ERROR( (m_APP_NAME+": No DNN method chosen").c_str() );
        return -9;
    }

    if (jet.pt() < m_ptMin || std::abs(jet.eta()) > m_etaMax){
        ATH_MSG_WARNING( (m_APP_NAME+": Jet out of kinematic range").c_str() );
        return -5;
    }

    // get DNN score
    double DNNscore = getScore(jet);

    // check if tagged (pT,mass,DNN score)
    double jet_DNN_min_cut  = m_wkpt_DNN->Eval(jet.pt()*1e-3);  // GeV

    return (DNNscore > jet_DNN_min_cut);
}


double JSSWTopTaggerDNN::getScore(const xAOD::Jet& jet) const{
    /* Return DNN score */
    std::map<std::string,double> DNN_inputValues = getJetProperties(jet);
    lwt::ValueMap discriminant = m_lwnn->compute(DNN_inputValues);

    double DNNscore(-999.);
    if (m_wtag){
        DNNscore = discriminant["w_dnn_1"];
    }
    else if (m_ztag){
        DNNscore = discriminant["z_dnn_1"];
    }
    else if (m_toptag){
        DNNscore = discriminant["top_dnn_1"];
    }

    return DNNscore;
}


void JSSWTopTaggerDNN::writeDecoration(const xAOD::Jet& jet) const{
    /* Decorate DNN score to the jet */
    jet.auxdecor<float>(m_DNNdecorationName) = getScore(jet);

    return;
}


std::map<std::string,double> JSSWTopTaggerDNN::getJetProperties(const xAOD::Jet& jet) const{
    /* Update the jet substructure variables for this jet */
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

    // Other Substructure Variables
    DNN_inputValues["Qw"] = jet.getAttribute<float>("Qw");

    return DNN_inputValues;
}


StatusCode JSSWTopTaggerDNN::finalize(){
    /* Delete or clear anything */
    return StatusCode::SUCCESS;
}


// THE END
