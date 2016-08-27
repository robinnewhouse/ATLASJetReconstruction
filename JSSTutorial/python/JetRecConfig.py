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

    if userOptions.get("asGhost", False):
        theOptions["GhostScale"] = 1e-40

    from ROOT import PseudoJetGetter, TrackPseudoJetGetter

    if algName is not None:
        alg, R, input = interpretJetName(algName)
    else:
        input = userOptions['input']
    

    # prepare a mapping of default arguments to  known inputs
    optionsDict = dict( LCTopo = dict(InputContainer="CaloCalTopoClusters", Label="LCTopo"),
                        EMTopo = dict(InputContainer="CaloCalTopoClusters", Label="EMTopo"),
                        Track  = dict(InputContainer="JetSelectedTracks_LooseTrackJets", Label="Track", TrackVertexAssociation  = "JetTrackVtxAssoc"),
                        PV0Track  = dict(InputContainer="JetSelectedTracks_LooseTrackJets", Label="Track", TrackVertexAssociation  = "JetTrackVtxAssoc"),
                        Truth  = dict(InputContainer="JetInputTruthParticles", Label="Truth"),
                      )


    getterOptions = optionsDict[input]
    getterOptions.update(theOptions)
    getterOptions.update(userOptions)

    toolName = input+"getter"
    if getterOptions["GhostScale"] > 0. :
        toolName = 'g'+toolName
        
    if "Track" in input :
        return TrackPseudoJetGetter(toolName, **getterOptions)
    elif "Topo" in input:
        return PseudoJetGetter(toolName, **getterOptions)
    elif "Truth" == input:
        return PseudoJetGetter(toolName, **getterOptions)
    print "buildPseudoJetGetter ERROR can't interpret ",input
    return None



def buildJetFinder(algName=None,**userOptions):
    """returns a configured JetFinder. userOptions are either the Properties of JetFinder (ex: JetRadius=1.2 )
    or special keywords :
      - algName : if set the alg and radius are interpreted from this name (ex : algName='AntiKt10') """

    # set some default for our options
    theOptions = dict( PtMin = 5000, GhostArea = 0.01, RandomOption = 1, )
    
    if algName is None:
        algName = buildJetAlgName(userOptions["JetAlgorithm"],userOptions[JetRadius])
    else:
        alg, R, input = interpretJetName( algName , input="anything")
        userOptions.update( JetAlgorithm=alg, JetRadius= R)
    toolName = algName+"Finder"

    # Technically we need this tool to translate fastjet to xAOD::Jet
    jetFromPJ = JetFromPseudojet(toolName+"jetbuild",
                                 Attributes = ["ActiveArea", "ActiveAreaFourVector"] )
    userOptions['JetBuilder'] = jetFromPJ

    # overwrite with userOptions
    theOptions.update(userOptions)

    # pass all the options to the constructor :
    finder = JetFinder(toolName, **theOptions)

    # incase of track jet, the finder is a bit more complex
    # we used a dedicated one which will build jets per vertex
    if "Track" in input:
        from ROOT import JetByVertexFinder
        vfinder = JetByVertexFinder(toolName + "ByVertex",
                                    JetFinder = finder,
                                    Vertex = 0 ) # configure it to use ONLY PV0
        finder = vfinder

    return finder



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

    modifierList = []
    # use all info to build the proper calib tag :
    alg, R, input = interpretJetName( algName )

    if "Topo" in  input :
        calOptTag = buildJetAlgName(alg,R)+input
        calOptTag = calOptTag+ ( "_data" if theOptions["IsData"] else ( "_FastS" if isFastSim else "_FullS" ) )
        # now get the relevant options from the dict :
        calOptions = calibDic.get(calOptTag, None)

        if calOptions is None:
            print " Config  ERROR : can't retrieve calibration for ",algName," with IsData=",theOptions["IsData"], "and isFastSim=",isFastSim
            return None
        modifierList += [ JetCalibrationTool(algName+"Calib", **calOptions), ]
        
    modifierList += [
                    JetSorter("jetsorter"),
                    JetFilterTool("ptfilter",PtMin=ptMin),
                    ]
    return modifierList




def buildJetInputTruthParticles(tool=None):
    from ROOT import CopyTruthParticles, CopyTruthJetParticles, MCTruthClassifier
    if tool is None:
        tool = CopyTruthJetParticles("truthpartcopy")

    tool.OutputName = "JetInputTruthParticles"
    tool.BarCodeFromMetadata = False # !!! not sure what this implies !
    tool.MCTruthClassifier  = MCTruthClassifier("jettruthclassifier")
    return tool
    
def buildJetTrackVtxAssoc(tool=None):
    # for now we call our onw defined buildTightTrackVertexAssociationTool() (TrackSelecToolHelper.h) because there's no
    # dictionnary for TightTrackVertexAssociationTool
    from ROOT import TrackVertexAssociationTool, buildTightTrackVertexAssociationTool
    cpTVa = buildTightTrackVertexAssociationTool("jetTighTVAtool")

    if tool is None:
        tool = TrackVertexAssociationTool("tvassoc")
        
    tool.TrackParticleContainer  = "InDetTrackParticles"
    tool.TrackVertexAssociation  = "JetTrackVtxAssoc"
    tool.VertexContainer         = "PrimaryVertices"
    tool.TrackVertexAssoTool     = cpTVa
    return tool


def buildJetTrackSelection(tool=None):
    # for now we call our onw defined buildTrackSelectionTool() (TrackSelecToolHelper.h) because there's no
    # dictionnary for InDet__InDetTrackSelectionTool.
    from ROOT import buildTrackSelectionTool
    inDetSel = buildTrackSelectionTool("TrackSelForJet", "Loose")

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
            print "interpretJetName Error could not guess jet finder type in ", jetcollName
            return 

    if input is None:
        knownInput = ['LCTopo','Tower','EMTopo', "Truth", "ZTrack", 'PV0Track']
        for i in knownInput:
            if i in jetcollName:
                input = i
                if i== "Tower":
                    if 'Ghost' in jetcollName:
                        input = 'Tower'
                    else:
                        input = "TopoTower"
                break
        if input is None:
            print "interpretJetName ERROR could not guess input type in ",jetcollName
            print " Known input :", knownInput
            return
        
    if mainParam is None:
        # get the 2 chars following finder :
        mp = jetcollName[len(finder):len(finder)+2]
        mp = mp[0] if not mp[1] in '0123456789' else mp
        try :
            mainParam = float(mp)/10.
        except ValueError :
            print "interpretJetName Error could not guess main parameter in ",jetcollName 
            return

    return finder, mainParam, input



###############################################3
## Below is config experimentations. 
###############################################3



_ghostScale = 1e-40    
from collections import namedtuple
JetConfigContext = namedtuple( 'JetConfigContext', 'algName, alg, R, input, dataType' )

class JetConfigurator(object):

    ## ********************************************************
    ## Inputs
    ## ********************************************************
    
    ## The map of known/standard PseudoJetGetter tools.
    ## This a simple python dict which format is  :
    ##   alias : ( class , dict_of_properties )  
    knownInputTools = dict( )

    knownInputLists = dict()




    def addKnownInput(self, alias, klass, **properties):
        if alias in self.knownInputTools:
            print "ERROR in PseudoJetInput::addKnownInput can't add already existing ",alias
            return
        self.knownInputTools[alias] = (klass, properties)
            
    def getInputTool(self, alias, **userProp):
        klass, defaultProp = self.knownInputTools.get(alias, (None,None) )
        if klass is None:
            print "ERROR. unknown getter ",alias
            return None
        if userProp != {} :
            finalProp = dict(defaultProp)
            finalProp.update(userProp)
        else:
            finalProp = defaultProp
        getter = klass(alias+'pjGetter', **finalProp)
        return getter

    def getInputToolFromAlgName(self, algname, **userProp):
        alg, R, input = interpretJetName(algName)
        if 'Track' in input:
            input='Track' # (because 'PV0Track' is the same input as 'Track')
        return self.get(alias, **userProp)

    def getInputList(self, alias):
        if isinstance(alias, list):
            aliasList = alias
        else:
            aliasList=self.knownInputLists.get(alias, None)
            if aliasList is None:
                print "ERROR PseudoJetGetter.getList unknown alias", alias
                return
        toolList = []
        for a in aliasList:            
            if isinstance(a, str):
                t = self.getInputTool(a)
            else: # assume a is alaready a tool:
                t=a
            toolList.append( t )
        return toolList

    ## ********************************************************
    ## Jet finding
    ## ********************************************************

    def getJetFinderTool(self, alg="AntiKt", R=0.4, input="LCTopo", algName=None, doArea=True, **userProp):
        defaultProps = dict( PtMin = 5000, GhostArea = 0.01 if doArea else 0., RandomOption = 1, )

        if algName is not None:
            alg, R, input = interpretJetName( algName )

        toolName = algName+"Finder"

        # Technically we need this tool to translate fastjet to xAOD::Jet
        jetFromPJ = JetFromPseudojet(toolName+"jetbuild",
                                     Attributes = ["ActiveArea", "ActiveAreaFourVector"] )
        userOptions['JetBuilder'] = jetFromPJ

        # overwrite with userOptions
        theOptions.update(userOptions)

        # pass all the options to the constructor :
        finder = JetFinder(toolName, **theOptions)

        # incase of track jet, the finder is a bit more complex
        # we used a dedicated one which will build jets per vertex
        if "Track" in input:
            from ROOT import JetByVertexFinder
            vfinder = JetByVertexFinder(toolName + "ByVertex",
                                        JetFinder = finder,
                                        Vertex = 0 ) # configure it to use ONLY PV0
            finder = vfinder

        return finder



    ## ********************************************************
    ## Jet Modifiers
    ## ********************************************************
    ## knownModifierTools : The map of known/standard Modifier tools.
    ## This a simple python dict whih format is  :
    ##   alias : ( class , dict_of_properties )  
    knownModifierTools = dict( )
    ## knownModifierList : maps an alias to a list of alias for individual modifier tool as defined in knownModifierTools.
    knownModifierList = dict( )

    def getModifTool(self, alias, context=None ):
        klass, defaultProp = self.knownModifierList.get(alias, (None,None) )
        if klass is None:
            print "ERROR. unknown modifer ",alias
            return None
        if userProp != {} :
            finalProp = dict(defaultProp)
            finalProp.update(userProp)
        else:
            finalProp = defaultProp
        if 'context' in finalProp:
            finalProp['context'] = context 
        modif = klass(alias+'Tool', **finalProp)
        return modif

    def getModifList(self, alias, context=None):
        if isinstance(alias, list):
            aliasList = alias
        else:
            aliasList=self.knownModifierList.get(alias, None)
            if aliasList is None:
                print "ERROR JetConfigurator.getModifList unknown alias", alias
                return
        toolList = []
        for a in aliasList:            
            if isinstance(a, str):
                t = self.getModifTool(a, context)
            else: # assume a is alaready a tool:
                t=a
            toolList.append( t )
        return toolList


    ## ********************************************************
    ## Jet Calibration
    ## ********************************************************
    ## define default options for calibrations.    
    ## calibOptions format is in the form
    ##  ('JetAlgName', 'dataType') : ('calib_config_file','calib_sequence')
    calibOptions = { }
    # calibOptions is populated below


    def getCalibTool(self, algName="AntiKt4EMTopo", dataType='FullS', context=None, **userOptions):

        if context is not None:
            # take algName and dataType from context and ignore other algs
            algName, dataType = context.algName, context.dataType

        if (algName,dataType) not in calibOptions:
            print "ERROR JetConfigurator.getCalibTool  can't retrieve calib config for ",algname, dataType
            return None
        
        confFile, seq = calibOptions[ (algName,dataType) ]
        tool=JetCalibrationTool(algName+"calib", IsData= (dataType=='data') , ConfigFile=confFile, CalibSequence=seq, JetCollection=algName)

        return tool

    ## ********************************************************
    ## top level tool
    ## ********************************************************
    ## knownJetBuilders maps a string (an jet container name) to an alias for input and an alias (or list of aliases) for JetModifiers
    ## knownJetBuilders thus maps standard collection names to their configuration (in the form of alias to input and modifiers).
    knownJetBuilders = dict( )
    # knownJetBuilders is populated below

    def jetBuildSequence(self, output, inputList=None, finder=None, modifierList=None, jetTool=None, **userProp):
        alg, R, input = interpretJetName(output)
        algName = buildJetAlgName( alg, R)+ input 

        if jetTool is None:
            jetTool = JetRecTool(algName+"seq")

        context = JetConfigContext(algName, alg, R, input, self.dataType)
        if inputList is None or modifierList is None:
            # retrieve  standard one
            inputAlias, modifAlias = self.knownJetBuilders.get( algName, (None,None) )

            if inputList is None:
                if inputAlias is None:
                    print "JetConfigurator.jetBuildSequence ERROR can't retrieve input tools for ", output , " interpreted as ",alg, r, input
                    return None
                inputList = self.getInputList( inputAlias )
            if modifierList is None :
                if modifAlias is None:
                    print "JetConfigurator.jetBuildSequence ERROR can't retrieve modifier tools for ", output , " interpreted as ",alg, r, input
                    return
                modifierList = self.getModifList( modifAlias, context)

        if finder is None:
            finder = self.getJetFinderTool(algName=algName )

        jetTool.PseudoJetGetters = inputList
        jetTool.JetFinder = finder
        jetTool.JetModifiers = modifierList
        jetTool.OutputContainer = output

        return jetTool

jetConfig = JetConfigurator()

## ********************************************************
## List of standard configuration aliases
## ********************************************************

## ********************************************************
## top level tool
## ********************************************************
## knownJetBuilders has the form
##  JetContName : ( 'alias_for_input' , alias_for_modifier )
## where 'alias_for_input' refer to an entry in inputList below
## and alias_for_modifier is either a string or a list :
##  'alias' -> refer to an entry in knownModifierList
##  'alias1_alias2' -> refer to 2 entries in knownModifierList to be concatenated
## ['alias1', 'alias2,' , tool ] -> strings refer to entries in knownModifierTools, tool is a configured tool instance.
##
jetConfig.knownJetBuilders = dict( 
    AntiKt4EMTopo   = ( 'emtopo', 'calib' ),
    AntiKt4LCTopo   = ( 'lctopo', 'calib' ),
    AntiKt4PV0Track = ( 'track', ['ptMin5GeV','width'] ),
    AntiKt4Truth    = ( 'truth',  ['ptMin5GeV','width'] ),
    
    AntiKt10LCTopo = ( 'lctopo', 'calib_substr' ),
    AntiKt10PV0Track = ( 'track', ['ptMin5GeV','width'] ),
    AntiKt10Truth = ( 'truth', ['ptMin5GeV','width'] ),        
    )


## ********************************************************
## Inputs
## ********************************************************

## knownInputTools : The map of known/standard PseudoJetGetter tools.
## This a simple python dict whicg format is  :
##   alias : ( class , dict_of_properties )  
knownInputTools = dict( 
    lctopo = (PseudoJetGetter, dict(InputContainer="CaloCalTopoClusters",Label="LCTopo") ),
    emtopo = (PseudoJetGetter, dict(InputContainer="CaloCalTopoClusters",Label="EMTopo") ),

    track  = (TrackPseudoJetGetter, dict(InputContainer="JetSelectedTracks_LooseTrackJets", Label="Track",TrackVertexAssociation  = "JetTrackVtxAssoc") ),
    gtrack = (TrackPseudoJetGetter, dict(InputContainer="JetSelectedTracks_LooseTrackJets", Label="Track",TrackVertexAssociation  = "JetTrackVtxAssoc") , GhostScale=_ghostScale),

    truth  = (PseudoJetGetter, dict(InputContainer="JetInputTruthParticles",Label="Truth") ),
    gtruth = (PseudoJetGetter, dict(InputContainer="JetInputTruthParticles",Label="Truth") , GhostScale=_ghostScale),
    )

## knownInputLists : maps an alias to a list of alias for individual input tool as defined in knownInputTools.
## format is 'alias' : [ 'alias1', 'alias2',...] where strings in the list are keys in knownInputTools
knownInputLists = dict(
    lctopo = [ 'lctopo', 'gtrack', 'gtruth' ] ,
    emtopo = [ 'lctopo', 'gtrack', 'gtruth' ] ,
    track = ['track']
    )




## ********************************************************
## Jet Modifiers
## ********************************************************
## knownModifierTools format  :
##   alias : ( class/callable , dict_of_properties )
## class is expected to be a JetModifierBase class
## callable is a function returning a JetModifierBase instance. if 'context' is part of its dict_of_properties, this means
##  a JetConfigContext object will be passed to the call as the 'context' argument.
jetConfig.knownModifierTools = dict(
    # standard modifier tools
    width = ( JetWidthTool, {}),
    sort  = ( JetSorter, {} ),
    ptMin5GeV = ( JetFilterTool, dict(PtMin= 5000) ), 

    # calib tool
    calib = ( jetConfig.getCalibTool, dict(context=None) ), # context will be set by the helper functions in jetConfig according to jet alg being configured

    # substructure tools
    encorr   = (EnergyCorrelatorTool, dict(Beta=1) ),
    encorrR  = (EnergyCorrelatorRatiosTool, {} ),
    nsubjet  = (NSubjettinessTool, dict(Alpha=1) ),
    nsubjetR = (NSubjettinessRatiosTool, {} ),
    )

## knownModifierList : maps an alias to a list of alias for individual modifier tool as defined in knownModifierTools.
## format is 'alias' : [ 'alias1', 'alias2',...] where strings in the list are keys in knownModifierTools
jetConfig.knownModifierList = dict(
    calibL = ["calib", "ptMin5GeV", "sort"],
    substr = ["encorr", "encorrR", "nsubjet", "nsubjetR"],
    # ... etc ...
    )


## ********************************************************
## Jet Calibration options
## ********************************************************
## calibOptions format is in the form
##  ('JetAlgName', 'dataType') : ('calib_config_file','calib_sequence')
jetConfig.calibOptions = { 
        ("AntiKt10LCTopoTrimmedPtFrac5SmallR20", 'FullS') = ("JES_MC15recommendation_FatJet_track_assisted_January2016.config", "EtaJES_JMS"),
        ("AntiKt10LCTopoTrimmedPtFrac5SmallR20", 'FastS') = ("JES_MC15recommendation_FatJet_track_assisted_January2016.config", "EtaJES_JMS"),
        ("AntiKt10LCTopoTrimmedPtFrac5SmallR20", 'data')  = ("JES_MC15recommendation_FatJet_track_assisted_January2016.config", "EtaJES_JMS"),
    
        ("AntiKt4EMTopo",'FullS') = ( "JES_MC15cRecommendation_May2016.config",  "JetArea_Residual_Origin_EtaJES_GSC")  ,
        ("AntiKt4EMTopo",'FastS') = ( "JES_MC15Prerecommendation_AFII_June2015.config",  "JetArea_Residual_Origin_EtaJES_GSC")  ,
        ("AntiKt4EMTopo",'data' )=  ( "JES_MC15cRecommendation_May2016.config", "JetArea_Residual_Origin_EtaJES_GSC_Insitu")  ,

        }
