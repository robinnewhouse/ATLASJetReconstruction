import JSSTutorial.RootCoreConfigInit
from ROOT import PseudoJetGetter, JetFinder, JetWidthTool, JetFromPseudojet, JetCalibrationTool
from ROOT import JetSorter, JetFilterTool


def buildClusterGetter():
    """A very simple function returning a configured PseudoJetGetter for LCTopo clusters"""
    getter =  PseudoJetGetter("lcget"
                              , InputContainer = "CaloCalTopoClusters"
                              , OutputContainer = "PseudoJetLCTopo"
                              , Label = "LCTopo"
                              , SkipNegativeEnergy = True
                              , GhostScale = 0.0 )
    return getter



    

def buildPseudoJetGetter(algName=None, **userOptions):
    """returns a configured PseudoJetGetter. userOptions are either the Properties of PseudoJetGetter or
      - algName : alg, radius and input type are interperted from this name (ex: algName='AntiKt10LCTopo')

    """
    # set some default for our options
    theOptions = dict( SkipNegativeEnergy=True, GhostScale=0.0 )

    if algName is not None:
        alg, R, input = interpretJetName(algName)
    else:
        input = userOptions['input']


    # prepare a mapping of default arguments to  known inputs
    optionsDict = dict( LCTopo = dict(InputContainer="CaloCalTopoClusters", Label="LCTopo"),
                        EMTopo = dict(InputContainer="CaloCalTopoClusters", Label="EMTopo"),
                        Track  = dict(InputContainer="JetSelectedTracks_LooseTrackJets", Label="Track", TrackVertexAssociation  = "JetTrackVtxAssoc"),
                      )


    getterOptions = optionsDict[input]
    getterOptions.update(theOptions)
    getterOptions.update(userOptions)

    toolName = algName+input+"get"
    if getterOptions["GhostScale"] > 0. :
        toolName = 'g'+toolName
        
    if input == "Track" :
        return TrackPseudoJetGetter(toolName, **getterOptions)
    elif "Topo" in input:
        return PseudoJetGetter(toolName, **getterOptions)



def buildJetFinder(algName=None,**userOptions):
    """returns a configured JetFinder. userOptions are either the Properties of JetFinder (ex: JetRadius=1.2 )
    or special keywords :
      - algName : if set the alg and radius are interpreted from this name (ex : algName='AntiKt10') """

    # set some default for our options
    theOptions = dict( PtMin = 5000, GhostArea = 0.01, RandomOption = 1, )
    
    if algName is None:
        algName = buildJetAlgName(userOptions["JetAlgorithm"],userOptions[JetRadius])
    else:
        alg, R, i = interpretJetName( algName , input="anything")
        userOptions.update( JetAlgorithm=alg, JetRadius= R)
    toolName = algName+"Finder"

    # Technically we need this tool to translate fastjet to xAOD::Jet
    jetFromPJ = JetFromPseudojet(toolName+"jetbuild",
                                 Attributes = ["ActiveArea", "ActiveAreaFourVector"] )
    userOptions['JetBuilder'] = jetFromPJ

    # overwrite with userOptions
    theOptions.update(userOptions)

    # pass all the options to the constructor :
    return JetFinder(toolName, **theOptions)


def buildJetCalibModifiers(algName, isFastSim = False, **userOptions):
    """returns a list of  configured JetModifiers [calib, sort, filter]. userOptions are either the Properties of JetCalibrationTool 
    or special keywords :
      - PtMin : cut after calibration
      - isFastSim : a boolean defaulting to False
      algName is the algorithm name, like AntiKt10LCTopoJets.
      """
    # set some default for our options
    theOptions = dict( IsData=False , PtMin=15000)


    ## define default options for calibrations :
    akt10trimmOptions = dict( JetCollection="AntiKt10LCTopoTrimmedPtFrac5SmallR20", ConfigFile = "JES_MC15recommendation_FatJet_track_assisted_January2016.config", CalibSequence = "EtaJES_JMS")
    
    atk4emOptions_FullS = dict( JetCollection="AntiKt4EMTopo", ConfigFile = "JES_MC15cRecommendation_May2016.config", CalibSequence = "JetArea_Residual_Origin_EtaJES_GSC")  
    atk4emOptions_FastS = dict( JetCollection="AntiKt4EMTopo", ConfigFile = "JES_MC15Prerecommendation_AFII_June2015.config", CalibSequence = "JetArea_Residual_Origin_EtaJES_GSC")  
    atk4emOptions_data = dict( JetCollection="AntiKt4EMTopo", ConfigFile = "JES_MC15cRecommendation_May2016.config", CalibSequence = "JetArea_Residual_Origin_EtaJES_GSC_Insitu")  

    calibDic =dict( AntiKt4EMTopo_FullS = atk4emOptions_FullS,
                    AntiKt4EMTopo_FastS = atk4emOptions_FastS,
                    AntiKt4EMTopo_data = atk4emOptions_data,
                    AntiKt10LCTopoTrimmedPtFrac5SmallR20_FullS = akt10trimmOptions,
                    AntiKt10LCTopoTrimmedPtFrac5SmallR20_FastS = akt10trimmOptions,
                    AntiKt10LCTopoTrimmedPtFrac5SmallR20_data  = akt10trimmOptions,
                    AntiKt10LCTopo_FullS = akt10trimmOptions,
                    AntiKt10LCTopo_FastS = akt10trimmOptions,
                    AntiKt10LCTopo_data  = akt10trimmOptions,
                     )


    theOptions.update(userOptions)
    ptMin = theOptions.pop("PtMin") # removes it

    # use all info to build the proper calib tag :
    alg, R, input = interpretJetName( algName )
    calOptTag = buildJetAlgName(alg,R)+input
    calOptTag = calOptTag+ ( "_data" if theOptions["IsData"] else ( "_FastS" if isFastSim else "_FullS" ) )
    # now get the relevant options from the dict :
    calOptions = calibDic.get(calOptTag, None)
    
    if calOptions is None :
        print " Config  ERROR : can't retrieve calibration for ",algName," with IsData=",theOptions["IsData"], "and isFastSim=",isFastSim
        return None
    

    modifierList = [JetCalibrationTool(algName+"Calib", **calOptions),
                    JetSorter("jetsorter"),
                    JetFilterTool("ptfilter",PtMin=ptMin),
                    ]
    return modifierList





def buildJetTrackVtxAssoc(tool=None):
    from ROOT import TrackVertexAssociationTool, CP__TightTrackVertexAssociationTool
    cpTVa = CP__TightTrackVertexAssociationTool("jetTighTVAtool", dzSinTheta_cut=3, doPV=True)
    if tool is None:
        tool = TrackVertexAssociationTool("tvassoc")
        
    tool.TrackParticleContainer  = "InDetTrackParticles"
    tool.TrackVertexAssociation  = "JetTrackVtxAssoc"
    tool.VertexContainer         = "PrimaryVertices"
    tool.TrackVertexAssoTool     = cpTVa
    return tool


def buildJetTrackSelection(tool=None):
    # for now we call our onw defined buildTrackSelectionTool (TrackSelecToolHelper.h) because there's no
    # dictionnary for InDet__InDetTrackSelectionTool.
    inDetSel = ROOT.buildTrackSelectionTool("TrackSelForJet", "Loose")

    if tool is None:
        tool = JetTrackSelectionTool("trackselloose_trackjets")
        
    tool.InputContainer  = "InDetTrackParticles"
    tool.OutputContainer = "JetSelectedTracks_LooseTrackJets"
    tool.Selector        = inDetSel

    return tool






## **************************************************************************
## Helper functions 
## recopied from JetRec (they could be put in common)
## **************************************************************************
def buildJetAlgName(finder, mainParam):
    return finder + str(int(mainParam*10))

def buildJetContName(finder, mainParam, input):
    return buildJetAlgName(finder, mainParam) +input+"Jets" # could be more elaborated...

def interpretJetName(jetcollName,  finder = None,input=None, mainParam=None):
    # first step : guess the finder, input , mainParam, if needed
    if finder is None:
        for a in [ 'AntiKt','CamKt','Kt', 'Cone','SISCone','CMSCone']:
            if jetcollName.startswith(a):
                finder = a
                break
        if finder is None:
            jetrec.info( "Error could not guess jet finder type in ",jetcollName )
            return 

    if input is None:
        for i in ['LCTopo','Tower','EMTopo', "Truth", "ZTrack", 'PV0Track']:
            if i in jetcollName:
                input = i
                if i== "Tower":
                    if 'Ghost' in jetcollName:
                        input = 'Tower'
                    else:
                        input = "TopoTower"
                break
        if input is None:
            jetrec.info( "Error could not guess input type in ",jetcollName )
            return
        
    if mainParam is None:
        # get the 2 chars following finder :
        mp = jetcollName[len(finder):len(finder)+2]
        mp = mp[0] if not mp[1] in '0123456789' else mp
        try :
            mainParam = float(mp)/10.
        except ValueError :
            jetrec.info( "Error could not guess main parameter in ",jetcollName )
            return

    return finder, mainParam, input
