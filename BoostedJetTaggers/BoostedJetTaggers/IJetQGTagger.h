// this file is -*- C++ -*- 
#ifndef IJETQGTAGGER_H
#define IJETQGTAGGER_H

#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "BoostedJetTaggers/IJSSTagger.h"

#include "xAODJet/Jet.h"

namespace CP {

  namespace QGntrackSyst {
    const static SystematicVariation trackefficiency("JET_QG_trackEfficiency");
    const static SystematicVariation trackfakes("JET_QG_trackFakes");
    const static SystematicVariation nchargedtopo("JET_QG_nchargedTopo");
    const static SystematicVariation nchargedexp_up("JET_QG_nchargedExp__1up");
    const static SystematicVariation nchargedme_up("JET_QG_nchargedME__1up");
    const static SystematicVariation nchargedpdf_up("JET_QG_nchargedPDF__1up");
    const static SystematicVariation nchargedexp_down("JET_QG_nchargedExp__1down");
    const static SystematicVariation nchargedme_down("JET_QG_nchargedME__1down");
    const static SystematicVariation nchargedpdf_down("JET_QG_nchargedPDF__1down");
  } //namespace QGntrackSyst

  class IJetQGTagger : public virtual CP::ISystematicsTool, public virtual IJSSTagger {
    
    ASG_TOOL_INTERFACE( CP::IJetQGTagger )
    
    public:
    
    virtual ~IJetQGTagger() {}
    
  };

} // namespace CP

#endif /* IJETQGTAGGER_H */