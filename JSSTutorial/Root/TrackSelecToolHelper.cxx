#include "JSSTutorial/TrackSelecToolHelper.h"


InDet::InDetTrackSelectionTool* buildTrackSelectionTool(const std::string & name, const std::string & cutLevel){
  InDet::InDetTrackSelectionTool* tool = new InDet::InDetTrackSelectionTool(name);
  tool->setProperty("CutLevel", cutLevel);
  tool->initialize();
  return tool;
}

CP::TightTrackVertexAssociationTool * buildTightTrackVertexAssociationTool(const std::string & name){
  CP::TightTrackVertexAssociationTool * tool = new CP::TightTrackVertexAssociationTool(name);
  tool->setProperty("dzSinTheta_cut",3);
  tool->setProperty("doPV",true);
  tool->initialize();
  return tool;
}
