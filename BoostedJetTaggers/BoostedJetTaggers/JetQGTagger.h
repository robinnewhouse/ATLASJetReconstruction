// this file is -*- C++ -*-
#ifndef JETQGTAGGER_H
#define JETQGTAGGER_H

#include "JetAnalysisInterfaces/IJetSelectorTool.h"
#include "BoostedJetTaggers/IJetQGTagger.h"
#include "BoostedJetTaggers/JSSTaggerBase.h"

#include "PATInterfaces/SystematicsTool.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"

#include "PATCore/TAccept.h"


#include <TH2D.h>

namespace InDet { class IInDetTrackSelectionTool; }
namespace InDet { class IInDetTrackTruthFilterTool; }
namespace InDet { class IJetTrackFilterTool; }
namespace InDet { class IInDetTrackTruthOriginTool; }

namespace CP {

  enum QGSystApplied {
    QG_NONE,
    QG_TRACKEFFICIENCY,
    QG_TRACKFAKES,
    QG_NCHARGEDTOPO,
    QG_NCHARGEDEXP_UP,
    QG_NCHARGEDME_UP,
    QG_NCHARGEDPDF_UP,
    QG_NCHARGEDEXP_DOWN,
    QG_NCHARGEDME_DOWN,
    QG_NCHARGEDPDF_DOWN
  };


  class JetQGTagger: public IJetQGTagger, public asg::AsgTool, public SystematicsTool{
    ASG_TOOL_CLASS( JetQGTagger, IJetQGTagger )

    public:

      JetQGTagger( const std::string& name);

      ///////////////// =======================
      // these are available in JSSTaggerBase and they should be taken from there
      bool m_decorate;
      mutable Root::TAccept m_accept;
      void showCuts() const;
      float m_jetPtMin;
      float m_jetPtMax;
      float m_jetEtaMax;
      ///////////////// =======================

      StatusCode initialize();
      StatusCode finalize();

      // Implement IJetSelectorTool interface
      Root::TAccept tag(const xAOD::Jet& jet, const xAOD::Vertex * _pv = NULL) const;

      Root::TAccept tag(const xAOD::Jet& jet) const {return tag(jet,NULL); }

      // functions for systematic variations
      bool isAffectedBySystematic(const SystematicVariation& var) const{return SystematicsTool::isAffectedBySystematic(var);}
      SystematicSet affectingSystematics() const {return SystematicsTool::affectingSystematics();}
      SystematicSet recommendedSystematics() const {return SystematicsTool::recommendedSystematics();}
      SystematicCode applySystematicVariation(const SystematicSet& set) {return SystematicsTool::applySystematicVariation(set);}
      SystematicCode sysApplySystematicVariation(const SystematicSet&);

    private:
      JetQGTagger();
      StatusCode getNTrack(const xAOD::Jet * jet, const xAOD::Vertex * pv, int &ntracks) const ;
      StatusCode getNTrackWeight(const xAOD::Jet * jet, double &weight) const ;

      QGSystApplied m_appliedSystEnum;

      TH2D* m_hquark;
      TH2D* m_hgluon;

      TH2D* m_topo_hquark;

      TH2D* m_exp_hquark_up;
      TH2D* m_exp_hquark_down;
      TH2D* m_exp_hgluon_up;
      TH2D* m_exp_hgluon_down;

      TH2D* m_me_hquark_up;
      TH2D* m_me_hquark_down;
      TH2D* m_me_hgluon_up;
      TH2D* m_me_hgluon_down;

      TH2D* m_pdf_hquark_up;
      TH2D* m_pdf_hquark_down;
      TH2D* m_pdf_hgluon_up;
      TH2D* m_pdf_hgluon_down;

      StatusCode loadHist(TH2D *&hist,std::string filename,std::string histname);

      std::string m_taggername;
      std::string m_calibarea;
      std::string m_topofile;
      std::string m_expfile;
      std::string m_mefile;
      std::string m_pdffile;
      std::string m_weight_decoration_name;
      std::string m_tagger_decoration_name;
      float m_minpt;
      float m_maxeta;
      SG::AuxElement::Decorator< float >* m_taggerdec;
      SG::AuxElement::Decorator< float >* m_weightdec;

      std::string m_configFile;
      int m_NTrackCut;

      asg::AnaToolHandle<InDet::IInDetTrackSelectionTool> m_trkSelectionTool;
      asg::AnaToolHandle<InDet::IInDetTrackTruthFilterTool> m_trkTruthFilterTool;
      asg::AnaToolHandle<InDet::IInDetTrackTruthFilterTool> m_trkFakeTool;
      asg::AnaToolHandle<InDet::IJetTrackFilterTool> m_jetTrackFilterTool;
      asg::AnaToolHandle<InDet::IInDetTrackTruthOriginTool> m_originTool;

  };

} /* namespace CP */

#endif /* JETQGTAGGER_H */
