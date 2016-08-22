#ifndef TRACKSELECTORHELPER_H
#define TRACKSELECTORHELPER_H

#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"
#include "TrackVertexAssociationTool/TightTrackVertexAssociationTool.h"

// needed only because there is no RootCore dict in InDetTrackSelectionTool
InDet::InDetTrackSelectionTool* buildTrackSelectionTool(const std::string & name, const std::string & cutLevel);

CP::TightTrackVertexAssociationTool * buildTightTrackVertexAssociationTool(const std::string & name);

#endif
