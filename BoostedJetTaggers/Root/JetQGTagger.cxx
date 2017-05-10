#include "BoostedJetTaggers/JetQGTagger.h"

#include <TRandom3.h>
#include "TEnv.h"
#include "TSystem.h"

#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"
#include "InDetTrackSystematicsTools/InDetTrackTruthFilterTool.h"
#include "InDetTrackSystematicsTools/InDetTrackTruthOriginTool.h"
#include "InDetTrackSystematicsTools/JetTrackFilterTool.h"
#include "PathResolver/PathResolver.h"

#include "CxxUtils/make_unique.h"
using CxxUtils::make_unique;

namespace CP {

  JetQGTagger::JetQGTagger( const std::string& name): asg::AsgTool( name ),
                  m_appliedSystEnum(QG_NONE),
                  m_hquark(nullptr),
                  m_hgluon(nullptr),
                  m_topo_hquark(nullptr),
                  m_exp_hquark_up(nullptr),
                  m_exp_hquark_down(nullptr),
                  m_exp_hgluon_up(nullptr),
                  m_exp_hgluon_down(nullptr),
                  m_me_hquark_up(nullptr),
                  m_me_hquark_down(nullptr),
                  m_me_hgluon_up(nullptr),
                  m_me_hgluon_down(nullptr),
                  m_pdf_hquark_up(nullptr),
                  m_pdf_hquark_down(nullptr),
                  m_pdf_hgluon_up(nullptr),
                  m_pdf_hgluon_down(nullptr),
                  m_trkSelectionTool(name+"_trackselectiontool", this),
                  m_trkTruthFilterTool(name+"_trackfiltertool",this),
                  m_trkFakeTool(name+"_trackfaketool",this),
                  m_jetTrackFilterTool(name+"_jettrackfiltertool",this),
                  m_originTool(name+"_origintool",this)
  {
    declareProperty( "Tagger", m_taggername = "ntrack");
    declareProperty( "CalibArea",     m_calibarea = "BoostedJetTaggers/QGTagger/Moriond2017/");
    declareProperty( "TopoWeightFile", m_topofile = "");
    declareProperty( "ExpWeightFile", m_expfile = "qgsyst_exp.root");
    declareProperty( "MEWeightFile",  m_mefile  = "qgsyst_me.root");
    declareProperty( "PDFWeightFile", m_pdffile = "qgsyst_pdf.root");
    declareProperty( "MinPt", m_minpt = 50e3);
    declareProperty( "MaxEta", m_maxeta = 2.1);
    declareProperty( "WeightDecorationName", m_weight_decoration_name = "qgTaggerWeight");
    declareProperty( "TaggerDecorationName", m_tagger_decoration_name = "qgTagger");

    declareProperty( "ConfigFile",   m_configFile="");
    declareProperty( "NTrackCut",    m_NTrackCut=-1);

    applySystematicVariation(SystematicSet()).ignore();
  }

  StatusCode JetQGTagger::initialize(){

    ATH_MSG_INFO( "Initializing QuarkGluonTagger tool" );

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

      // read in the specified track cut in the config file
      m_NTrackCut=configReader.GetValue("NTrackCut" ,-1);

      ATH_MSG_VERBOSE( "NTrackCut by config file : "<<m_NTrackCut );

    }
    else {
      // no config file
      // Assume the cut functions have been set through properties.
      // check they are non empty
      if( m_NTrackCut!=-1){
        ATH_MSG_VERBOSE( "NTrackCut by manual setting of property : "<<m_NTrackCut );
      }
      else {
        ATH_MSG_WARNING( "No config file provided AND no NTrackCut specified." ) ;
      }
    }

    // decorators used to store
    // 1) ntracks
    // 2) tagger weight
    m_taggerdec  = new SG::AuxElement::Decorator< float>(m_tagger_decoration_name);
    m_weightdec = new SG::AuxElement::Decorator< float>(m_weight_decoration_name);

    // set up InDet selection tool
    assert( ASG_MAKE_ANA_TOOL( m_trkSelectionTool,  InDet::InDetTrackSelectionTool ) );
    assert( m_trkSelectionTool.setProperty( "CutLevel", "Loose" ) );
    assert( m_trkSelectionTool.retrieve() );

    // set up InDet truth track selection tools
    assert( ASG_MAKE_ANA_TOOL( m_trkTruthFilterTool, InDet::InDetTrackTruthFilterTool ) );
    assert( ASG_MAKE_ANA_TOOL( m_trkFakeTool, InDet::InDetTrackTruthFilterTool ) );

    assert( ASG_MAKE_ANA_TOOL( m_originTool, InDet::InDetTrackTruthOriginTool ) );
    assert( m_originTool.retrieve() );

    assert( m_trkTruthFilterTool.setProperty( "Seed", 1234 ) );
    assert( m_trkTruthFilterTool.setProperty( "trackOriginTool", m_originTool ) );
    assert( m_trkTruthFilterTool.retrieve() );
    CP::SystematicSet systSetTrk = {
      InDet::TrackSystematicMap[InDet::TRK_EFF_LOOSE_GLOBAL],
      InDet::TrackSystematicMap[InDet::TRK_EFF_LOOSE_IBL],
      InDet::TrackSystematicMap[InDet::TRK_EFF_LOOSE_PP0],
      InDet::TrackSystematicMap[InDet::TRK_EFF_LOOSE_PHYSMODEL]
    };
    assert( m_trkTruthFilterTool->applySystematicVariation(systSetTrk) );

    // set up tools used for systematic variations of tracks
    assert( m_trkFakeTool.setProperty( "Seed", 1234 ) );
    assert( m_trkFakeTool.setProperty( "trackOriginTool", m_originTool ) );
    assert( m_trkFakeTool.retrieve() );
    CP::SystematicSet systSetTrkFake = {
      InDet::TrackSystematicMap[InDet::TRK_FAKE_RATE_LOOSE]
    };
    assert( m_trkFakeTool->applySystematicVariation(systSetTrkFake) );

    assert( ASG_MAKE_ANA_TOOL( m_jetTrackFilterTool, InDet::JetTrackFilterTool ) );
    assert( m_jetTrackFilterTool.setProperty( "Seed", 1234 ) );
    assert( m_jetTrackFilterTool.retrieve() );
    CP::SystematicSet systSetJet = {
      InDet::TrackSystematicMap[InDet::TRK_EFF_LOOSE_TIDE]
    };
    assert( m_jetTrackFilterTool->applySystematicVariation(systSetJet) );

    // specify systematic variations relevant for this tool
    if (!addAffectingSystematic(QGntrackSyst::trackfakes,true) ||
        !addAffectingSystematic(QGntrackSyst::trackefficiency,true) ||
        !addAffectingSystematic(QGntrackSyst::nchargedtopo,false /*for topology differences */) ||
        !addAffectingSystematic(QGntrackSyst::nchargedexp_up,true) ||
        !addAffectingSystematic(QGntrackSyst::nchargedme_up,true) ||
        !addAffectingSystematic(QGntrackSyst::nchargedpdf_up,true) ||
        !addAffectingSystematic(QGntrackSyst::nchargedexp_down,true) ||
        !addAffectingSystematic(QGntrackSyst::nchargedme_down,true) ||
        !addAffectingSystematic(QGntrackSyst::nchargedpdf_down,true)
      )
    {
      ATH_MSG_ERROR("failed to set up JetQGTagger systematics");
      return StatusCode::FAILURE;
    }

    // load in the histograms that store the ntrack systematics
    if(m_topofile!="")//load topology file only if explicitly configured (default is "")
      assert( this->loadHist(m_topo_hquark,  m_topofile,"h2dquark") );
    assert( this->loadHist(m_exp_hquark_up,  m_expfile,"h2dquark_up")  );
    assert( this->loadHist(m_exp_hquark_down,m_expfile,"h2dquark_down"));
    assert( this->loadHist(m_exp_hgluon_up,  m_expfile,"h2dgluon_up")  );
    assert( this->loadHist(m_exp_hgluon_down,m_expfile,"h2dgluon_down"));
    assert( this->loadHist(m_me_hquark_up,   m_mefile, "h2dquark_up")  );
    assert( this->loadHist(m_me_hquark_down, m_mefile, "h2dquark_down"));
    assert( this->loadHist(m_me_hgluon_up,   m_mefile, "h2dgluon_up")  );
    assert( this->loadHist(m_me_hgluon_down, m_mefile, "h2dgluon_down"));
    assert( this->loadHist(m_pdf_hquark_up,  m_pdffile,"h2dquark_up")  );
    assert( this->loadHist(m_pdf_hquark_down,m_pdffile,"h2dquark_down"));
    assert( this->loadHist(m_pdf_hgluon_up,  m_pdffile,"h2dgluon_up")  );
    assert( this->loadHist(m_pdf_hgluon_down,m_pdffile,"h2dgluon_down"));

    ATH_MSG_INFO( ": JetQGTagger tool initialized" );
    ATH_MSG_INFO( "  NTrackCut   : "<< m_NTrackCut );

    return StatusCode::SUCCESS;
  }

  StatusCode JetQGTagger::finalize(){

    delete m_topo_hquark;
    delete m_exp_hquark_up;
    delete m_exp_hquark_down;
    delete m_exp_hgluon_up;
    delete m_exp_hgluon_down;
    delete m_me_hquark_up;
    delete m_me_hquark_down;
    delete m_me_hgluon_up;
    delete m_me_hgluon_down;
    delete m_pdf_hquark_up;
    delete m_pdf_hquark_down;
    delete m_pdf_hgluon_up;
    delete m_pdf_hgluon_down;

    delete m_hquark;
    delete m_hgluon;

    return StatusCode::SUCCESS;
  }

  CP::JetQGTagger::Result JetQGTagger::result(const xAOD::Jet& jet, const bool decorate, const xAOD::Vertex * _pv) const {

    ATH_MSG_DEBUG( "Obtaining JetQGTagger decision" );

    // if no primary vertex is specified, then the 0th primary vertex is used
    const xAOD::Vertex *pv = _pv ;
    if(! pv){
      const xAOD::VertexContainer* vxCont = 0;
      if(evtStore()->retrieve( vxCont, "PrimaryVertices" ).isFailure()){
        ATH_MSG_WARNING("Unable to retrieve primary vertex container PrimaryVertices");
        return InvalidJet;
      }
      else if(vxCont->empty()){
        ATH_MSG_WARNING("Event has no primary vertices!");
        return InvalidJet;
      }
      else{
        for(const auto& vx : *vxCont){
          // take the first vertex in the list that is a primary vertex
          if(vx->vertexType()==xAOD::VxType::PriVtx){
            pv = vx;
            break;
          }
        }
      }
    }

    // obtain the relevant information for tagging
    // 1) the number of tracks
    // 2) jet-by-jet event weight
    double jetWeight = -1;
    int    jetNTrack = -1;

    if(jet.pt()<m_minpt || fabs(jet.eta())>m_maxeta){
      ATH_MSG_INFO("Jet pT or eta is out of allowable range");
      ATH_MSG_INFO( ": Bounds : pt=["<<m_minpt<<",Inf] , |eta|<"<<m_maxeta<<" | ThisJet : pt="<<std::to_string(jet.pt()/1000.0)<<" , eta="<<std::to_string(jet.eta()) ) ;
      return OutOfRange;
    }
    else{
      assert( getNTrack(&jet, pv, jetNTrack)  );
      assert( getNTrackWeight(&jet, jetWeight));
    }

    // decorate the cut value if specified
    if(decorate){
      (*m_taggerdec)(jet) = jetNTrack;
      (*m_weightdec)(jet) = jetWeight;
    }

    // determine the tagging type
    if(jetNTrack<0){
      ATH_MSG_WARNING("This jet has a negative number of tracks");
      return InvalidJet;
    }
    else if(jetNTrack<m_NTrackCut){
      return QuarkTag;
    }
    else{
      return GluonTag;
    }

  }

  StatusCode JetQGTagger::getNTrack(const xAOD::Jet * jet, const xAOD::Vertex * pv, int &ntracks) const {

    ATH_MSG_DEBUG( "Counting the number of tracks in the jet" );

    ntracks = 0;

    // loop over the tracks associated to the jet of interest
    std::vector<const xAOD::IParticle*> jettracks;
    jet->getAssociatedObjects<xAOD::IParticle>(xAOD::JetAttribute::GhostTrack,jettracks);
    for (size_t i = 0; i < jettracks.size(); i++) {

      const xAOD::TrackParticle* trk = static_cast<const xAOD::TrackParticle*>(jettracks[i]);

      // if you are applying a systematic variation then
      // FRANCESCO ADD COMMENT

      bool acceptSyst = true;

      if ( m_appliedSystEnum==QG_TRACKEFFICIENCY )
        acceptSyst = ( m_trkTruthFilterTool->accept(trk) && m_jetTrackFilterTool->accept(trk,jet) );
      else if ( m_appliedSystEnum==QG_TRACKFAKES )
        acceptSyst = m_trkFakeTool->accept(trk);

      if (!acceptSyst)
        continue;

      // only count tracks with selections
      // 1) pt>500 MeV
      // 2) accepted track from InDetTrackSelectionTool with CutLevel==Loose
      // 3) associated to primary vertex OR within 3mm of the primary vertex
      bool accept = (trk->pt()>500 &&
                     m_trkSelectionTool->accept(*trk) &&
                     (trk->vertex()==pv || (!trk->vertex() && fabs((trk->z0()+trk->vz()-pv->z())*sin(trk->theta()))<3.))
                    );

      if (!accept)
        continue;

      ntracks++;
    }

    return StatusCode::SUCCESS;
  }


  StatusCode JetQGTagger::getNTrackWeight(const xAOD::Jet * jet, double &weight) const {

    ATH_MSG_DEBUG( "Getting the jet weight for systematic variation " << m_appliedSystEnum );

    // initially set the weight to unity
    // this is the weight returned if you are *not* dealing with a systematic variation
    weight = 1.0;

    // if you are not dealing with a systematic variation, then exit
    if ( m_appliedSystEnum!=QG_NCHARGEDTOPO &&
         m_appliedSystEnum!=QG_NCHARGEDEXP_UP &&
         m_appliedSystEnum!=QG_NCHARGEDME_UP &&
         m_appliedSystEnum!=QG_NCHARGEDPDF_UP &&
         m_appliedSystEnum!=QG_NCHARGEDEXP_DOWN &&
         m_appliedSystEnum!=QG_NCHARGEDME_DOWN &&
         m_appliedSystEnum!=QG_NCHARGEDPDF_DOWN
       )
      return StatusCode::SUCCESS;

    // if pdgid<0
    int pdgid = jet->getAttribute<int>("PartonTruthLabelID");
    if ( pdgid<0 ) {
      ATH_MSG_INFO("Undefined pdg ID: setting weight to 1");
      return StatusCode::SUCCESS;
    }

    // getting the associated truth jet
    // FRANCESCO COMMENT
    const xAOD::Jet* tjet;
    if(jet->isAvailable< ElementLink<xAOD::JetContainer> >("GhostTruthAssociationLink") ){
      ATH_MSG_DEBUG("Accessing GhostTruthAssociationLink: is available");
      if(jet->auxdata< ElementLink<xAOD::JetContainer> >("GhostTruthAssociationLink").isValid() ){
        ATH_MSG_DEBUG("Accessing GhostTruthAssociationLink: is valid");
	ElementLink<xAOD::JetContainer> truthlink = jet->auxdata< ElementLink<xAOD::JetContainer> >("GhostTruthAssociationLink");
	// std::cout << "truthlink " << truthlink << std::endl;
	if(truthlink)
	  tjet = * truthlink;
	else{
	  ATH_MSG_WARNING("Cannot access truth: setting weight to 1");
	  return StatusCode::SUCCESS;
	}//endelse NULL pointer
      }
      else {
        ATH_MSG_WARNING("Cannot access truth: setting weight to 1");
        return StatusCode::SUCCESS;
      } //endelse isValid
    } //endif isAvailable
    else {
      ATH_MSG_WARNING("Cannot access truth: setting weight to 1");
      return StatusCode::SUCCESS;
    }//endelse isAvailable
    
    // if the jet is outside of the measurement fiducial region
    // the systematic uncertainty is set to 0
    double tjetpt = tjet->pt()*0.001;
    double tjeteta = tjet->eta();
    if( tjetpt<50 || fabs(tjeteta)>2.1){
      ATH_MSG_INFO("Outside of fiducial region: setting weight to 1");
      return StatusCode::SUCCESS;
    }

    // compute truth ntrk
    int tntrk = 0;
    for (size_t ind = 0; ind < tjet->numConstituents(); ind++) {
      const xAOD::TruthParticle *part = static_cast<const xAOD::TruthParticle*>(tjet->rawConstituent(ind));

      // dont count invalid truth particles
      if (!part) continue;
      // require the particle in the final state
      if( ! (part->status() == 1) ) continue;
      // require that the particle type (e.g. production type) be valid (e.g. not primaries)
      if ((part->barcode())>2e5) continue;
      // pt>500 MeV
      if( ! (part->pt()>500.) )  continue;
      // charged
      if( !(part->isCharged()) ) continue;
      // this seems redundant
      // FRANCESCO COMMENT
      double pt = part->pt();
      if( pt>500 ) tntrk++;

    }

    // use the lookup tables loaded in initialize() to find the systematically shifted weights
    if ( pdgid==21 && m_appliedSystEnum!=QG_NCHARGEDTOPO){
      int ptbin = m_hgluon->GetXaxis()->FindBin(tjetpt);
      int ntrkbin = m_hgluon->GetYaxis()->FindBin(tntrk);
      weight = m_hgluon->GetBinContent(ptbin,ntrkbin);
    }// gluon
    else if ( pdgid<5 ){
      int ptbin = m_hquark->GetXaxis()->FindBin(tjetpt);
      int ntrkbin = m_hquark->GetYaxis()->FindBin(tntrk);
      weight = m_hquark->GetBinContent(ptbin,ntrkbin);
    }//quarks
    else{
      ATH_MSG_INFO("Neither quark nor gluon jet: setting weight to 1");
    }

    return StatusCode::SUCCESS;
  }

  SystematicCode JetQGTagger::sysApplySystematicVariation(const SystematicSet& systSet){

    // FRANCESCO COMMENT

    ATH_MSG_DEBUG( "Applying systematic variation by weight" );

    // by default no systematics are applied
    m_appliedSystEnum = QG_NONE;

    if (systSet.size()==0) {
      ATH_MSG_DEBUG("No affecting systematics received.");
      return SystematicCode::Ok;
    }
    else if (systSet.size()>1) {
      ATH_MSG_WARNING("Tool does not support multiple systematics, returning unsupported" );
      return CP::SystematicCode::Unsupported;
    }
    SystematicVariation systVar = *systSet.begin();
    if (systVar == SystematicVariation(""))
      m_appliedSystEnum = QG_NONE;
    else if (systVar == QGntrackSyst::nchargedtopo){
      m_appliedSystEnum = QG_NCHARGEDTOPO;
      m_hquark=m_topo_hquark;
    }
    else if (systVar == QGntrackSyst::trackefficiency)
      m_appliedSystEnum = QG_TRACKEFFICIENCY;
    else if (systVar == QGntrackSyst::trackfakes)
      m_appliedSystEnum = QG_TRACKFAKES;
    else if (systVar == QGntrackSyst::nchargedexp_up){
      m_appliedSystEnum = QG_NCHARGEDEXP_UP;
      m_hquark=m_exp_hquark_up;
      m_hgluon=m_exp_hgluon_up;
    }
    else if (systVar == QGntrackSyst::nchargedme_up){
      m_appliedSystEnum = QG_NCHARGEDME_UP;
      m_hquark=m_me_hquark_up;
      m_hgluon=m_me_hgluon_up;
    }
    else if (systVar == QGntrackSyst::nchargedpdf_up){
      m_appliedSystEnum = QG_NCHARGEDPDF_UP;
      m_hquark=m_pdf_hquark_up;
      m_hgluon=m_pdf_hgluon_up;
    }
    else if (systVar == QGntrackSyst::nchargedexp_down){
      m_appliedSystEnum = QG_NCHARGEDEXP_DOWN;
      m_hquark=m_exp_hquark_down;
      m_hgluon=m_exp_hgluon_down;
    }
    else if (systVar == QGntrackSyst::nchargedme_down){
      m_appliedSystEnum = QG_NCHARGEDME_DOWN;
      m_hquark=m_me_hquark_down;
      m_hgluon=m_me_hgluon_down;
    }
    else if (systVar == QGntrackSyst::nchargedpdf_down){
      m_appliedSystEnum = QG_NCHARGEDPDF_DOWN;
      m_hquark=m_pdf_hquark_down;
      m_hgluon=m_pdf_hgluon_down;
    }
    else {
      ATH_MSG_WARNING("unsupported systematic applied");
      return SystematicCode::Unsupported;
    }

    ATH_MSG_DEBUG("applied systematic is " << m_appliedSystEnum);
    return SystematicCode::Ok;
  }

  StatusCode JetQGTagger::loadHist(TH2D *&hist,std::string fname,std::string histname){

    std::string filename = PathResolverFindCalibFile( (m_calibarea+fname).c_str() );
    if (filename.empty()){
      ATH_MSG_WARNING ( "Could NOT resolve file name " << fname);
      return StatusCode::FAILURE;
    }
    else{
      ATH_MSG_DEBUG(" Path found = "<<filename);
    }
    TFile* infile = TFile::Open(filename.c_str());
    hist = dynamic_cast<TH2D*>(infile->Get(histname.c_str()));

    hist->SetDirectory(0);
    return StatusCode::SUCCESS;
  }


} /* namespace CP */
