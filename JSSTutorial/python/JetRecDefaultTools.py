
from ROOT import PseudoJetGetter, JetFinder, JetWidthTool, JetFromPseudojet, JetCalibrationTool
from ROOT import JetSorter, JetFilterTool, PseudoJetGetter, TrackPseudoJetGetter

from ROOT import EnergyCorrelatorTool, EnergyCorrelatorRatiosTool, NSubjettinessRatiosTool, NSubjettinessTool, KtMassDropTool
from ROOT import JetChargeTool, KTSplittingScaleTool, CenterOfMassShapesTool, JetPullTool, DipolarityTool, AngularityTool, PlanarFlowTool
from ROOT import JetRecTool, JetTrimmer, JetPruner, JetPseudojetRetriever

from JSSTutorial.JetRecConfig import jetConfig

## ********************************************************
## List of standard configuration aliases
## ********************************************************

class ConfigConstants(object):
    """Simply put all config constants in a common place """
    ghostScale = 1e-40

    vtxContainer = "PrimaryVertices"
    trkContainer = "InDetTrackParticles"

cst = ConfigConstants()
    


## ********************************************************
## top level tool
## ********************************************************
## knownJetBuilders has the form
##
##  JetContName : ( alias_for_input , alias_for_modifier )
##
## where
##  * alias_for_input  is a string or a list :
##                       'alias' -> refers to an entry in *knownInputLists*
##     ['alias1','alias2', tool] -> strings refer to entries in knownInputTools, tool is a configured PseudoJetGetter instance. 
##  * alias_for_modifier is  string or a list :
##                           'alias' -> refer to an entry in *knownModifierList*
##                   'alias1+alias2' -> refer to 2 entries in knownModifierList to be concatenated
##     ['alias1', 'alias2,' , tool ] -> strings refer to entries in knownModifierTools, tool is a configured tool instance.
##
jetConfig.knownJetBuilders = { 
    'AntiKt4EMTopo'         : ( 'emtopoInputs', 'calib+cut5' ),
    'AntiKt4LCTopo'         : ( 'lctopoInputs', 'calib+cut5' ),
    'AntiKt4EMTopo_full'    : ( 'emtopoInputs+ghosts', 'calib+cut5' ),
    'AntiKt4LCTopo_full'    : ( 'lctopoInputs+ghosts', 'calib+cut5' ),

    'AntiKt4PV0Track'       : ( 'trackInputs', ['ptMin5GeV','width'] ),
    'AntiKt4Truth'          : ( 'truthInputs',  ['ptMin5GeV','width'] ),    

    'AntiKt10LCTopo'        : ( 'lctopoInputs', 'cut50+substr' ),
    'AntiKt10LCTopo_full'   : ( 'lctopoInputs+ghosts', 'cut50+substr' ),

    'AntiKt10PV0Track'      : ( 'trackInputs', ['ptMin5GeV','width'] ),
    'AntiKt10Truth'         : ( 'truthInputs', ['ptMin5GeV','width'] ),        
    }


## ********************************************************
## Inputs
## ********************************************************
## knownInputLists : maps an alias to a list of alias for individual input tool as defined in knownInputTools.
## format is 'alias' : [ 'alias1', 'alias2',...] where strings in the list are keys in knownInputTools
jetConfig.knownInputLists = {
    'lctopoInputs' : [ 'lctopo', ],
    'emtopoInputs' : [ 'emtopo',  ] ,

    'trackInputs'  : ['track'],
    'truthInputs'  : ['truth'],   

    'ghosts'       : ['gtrack', 'gtruth'],
    }


## knownInputTools : The map of known/standard PseudoJetGetter tools.
## This a simple python dict whicg format is  :
##   alias : ( class , dict_of_properties )  
jetConfig.knownInputTools = {
    'lctopo' : (PseudoJetGetter, dict(InputContainer="CaloCalTopoClusters",Label="LCTopo",SkipNegativeEnergy=True, OutputContainer="LCTopoPseudoJetVec") ),
    'emtopo' : (PseudoJetGetter, dict(InputContainer="CaloCalTopoClusters",Label="EMTopo",SkipNegativeEnergy=True, OutputContainer="EMTopoPseudoJetVec") ),

    'track'  : (TrackPseudoJetGetter, dict(InputContainer="JetSelectedTracks_LooseTrackJets", Label="Track",
                                           TrackVertexAssociation  = "JetTrackVtxAssoc",OutputContainer="TrackPseudoJetVec") ),
    'gtrack' : (TrackPseudoJetGetter, dict(InputContainer="JetSelectedTracks_LooseTrackJets", Label="GhostTrack",
                                           TrackVertexAssociation  = "JetTrackVtxAssoc" , GhostScale=cst.ghostScale,OutputContainer="GTrackPseudoJetVec") ),

    'truth'  : (PseudoJetGetter, dict(InputContainer="JetInputTruthParticles",Label="Truth",OutputContainer="TruthPseudoJetVec") ),
    'gtruth' : (PseudoJetGetter, dict(InputContainer="JetInputTruthParticles",Label="GhostTruth" , GhostScale=cst.ghostScale,OutputContainer="TruthPseudoJetVec") ), 
    }





## ********************************************************
## Jet Modifiers
## ********************************************************
## knownModifierList : maps an alias to a list of alias for individual modifier tool as defined in knownModifierTools.
## format is 'alias' : [ 'alias1', 'alias2',...] where strings in the list are keys in knownModifierTools
jetConfig.knownModifierList = {
    'calib'  : ["calib",] ,
    'cut5'   : ["ptMin5GeV", "sort"],
    'cut50'  : ["ptMin50GeV", "sort"],
    'substr' : ["encorr", "encorrR", "nsubjet", "nsubjetR", "ktsplit"],
    # ... etc ...
    }

## knownModifierTools format  :
##   alias : ( class/callable , dict_of_properties )
## class is expected to be a JetModifierBase class
## callable is a function returning a JetModifierBase instance. if 'context' is part of its dict_of_properties, this means
##  a JetConfigContext object will be passed to the call as the 'context' argument.
jetConfig.knownModifierTools = {
    # standard modifier tools
    'width'      : ( JetWidthTool, {}),
    'sort'       : ( JetSorter, {} ),
    'ptMin5GeV'  : ( JetFilterTool, dict(PtMin= 5000) ), 
    'ptMin50GeV' : ( JetFilterTool, dict(PtMin= 50000) ), 

    # calib tool : there is no default properties for calibration since it depends on the jet alg
    # being calibrated. The relevant info will be passed through the 'context' argument (which will be set accordingly by the helper functions)
    'calib' : ( jetConfig.getCalibTool, dict(context=None) ), 

    # substructure tools
    'encorr'   : (EnergyCorrelatorTool, dict(Beta=1) ),
    'encorrR'  : (EnergyCorrelatorRatiosTool, dict() ),
    'nsubjet'  : (NSubjettinessTool, dict(Alpha=1) ),
    'nsubjetR' : (NSubjettinessRatiosTool, dict() ),
    'ktsplit'  : (KTSplittingScaleTool, dict() ),
    
    'angularity': (AngularityTool, dict() ),
    'dipolarity': (DipolarityTool, dict( SubJetRadius = 0.3) ),
    'planarflow': (PlanarFlowTool, dict() ),
    'ktmdrop'   : (KtMassDropTool, dict() ),
    'comshapes' : (CenterOfMassShapesTool, dict() ),
    'pull'      : (JetPullTool, dict(UseEtaInsteadOfY = False, IncludeTensorMoments = True) ),
    'charge'    : (JetChargeTool, dict( K=1.0) ),

    }


## ********************************************************
## Jet groomers
## ********************************************************
def _buildTrimName(RClus=0, PtFrac=0):
    print "RClus ",RClus, "PtFrac",PtFrac
    return "TrimmedPtFrac%sSmallR%s"%( str(int(PtFrac*100) ), str(int(RClus*100)) )

def _buildPrunName(ZCut=0, RCut=0):
    return "PrunedZCut%sRCut%s"%(str(int(ZCut*100)), str(int(RCut*100)) )
                                    
jetConfig.knownJetGroomers = {
    "Trim" : (JetTrimmer, dict(RClus=0.2, PtFrac=0.05), _buildTrimName ),
    "Prun" : (JetPruner,  dict(ZCut = 0.1, RCut=0.5),   _buildPrunName), 
    }

## ********************************************************
## Jet Calibration options
## ********************************************************
## calibOptions format is in the form
##  ('JetAlgName', 'dataType') : ('calib_config_file','calib_sequence')
jetConfig.calibOptions = { 
        ("AntiKt10LCTopoTrimmedPtFrac5SmallR20", 'FullS') : ("JES_MC15recommendation_FatJet_track_assisted_January2016.config", "EtaJES_JMS"),
        ("AntiKt10LCTopoTrimmedPtFrac5SmallR20", 'FastS') : ("JES_MC15recommendation_FatJet_track_assisted_January2016.config", "EtaJES_JMS"),
        ("AntiKt10LCTopoTrimmedPtFrac5SmallR20", 'data')  : ("JES_MC15recommendation_FatJet_track_assisted_January2016.config", "EtaJES_JMS"),
    
        ("AntiKt4EMTopo",'FullS') : ( "JES_MC15cRecommendation_May2016.config",  "JetArea_Residual_Origin_EtaJES_GSC")  ,
        ("AntiKt4EMTopo",'FastS') : ( "JES_MC15Prerecommendation_AFII_June2015.config",  "JetArea_Residual_Origin_EtaJES_GSC")  ,
        ("AntiKt4EMTopo",'data' ) : ( "JES_MC15cRecommendation_May2016.config", "JetArea_Residual_Origin_EtaJES_GSC_Insitu")  ,

        }
