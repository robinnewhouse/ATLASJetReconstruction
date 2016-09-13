#include <JSSTutorial/JSSTutorialAlgo.h>
#include <JSSTutorial/JSSTutorialJetToolsAlgo.h>
#include <JSSTutorial/JSSTutorialPythonConfigAlgo.h>
#include <JSSTutorial/JetBuildingAlgs.h>
#include <JSSTutorial/TrackSelecToolHelper.h>
#include <JSSTutorial/ToolConfigHelper.h>
#include <JSSTutorial/JSSMinixAOD.h>
#include <JSSTutorial/JetGlobalOptionsAlgo.h>
#include <JSSTutorial/FixVertexAlgo.h>

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#endif

#ifdef __CINT__
#pragma link C++ class JSSTutorialAlgo+;
#pragma link C++ class JSSTutorialJetToolsAlgo+;
#pragma link C++ class JSSTutorialPythonConfigAlgo+;

#pragma link C++ class JetExecuteToolAlgo+;
#pragma link C++ class ToolWrapper+;
#pragma link C++ class JetRecToolAlgo+;
#pragma link C++ class JSSMinixAOD+;
#pragma link C++ class JetGlobalOptionsAlgo+;
#pragma link C++ class FixVertexAlgo+;

#pragma link C++ function buildTrackSelectionTool;
#pragma link C++ function buildTightTrackVertexAssociationTool;

#endif
