import JSSTutorial.RootCoreConfigInit
from ROOT import PseudoJetGetter, JetFinder, JetWidthTool, JetFromPseudojet, JetCalibrationTool
from ROOT import JetSorter, JetFilterTool, PseudoJetGetter, TrackPseudoJetGetter
from ROOT import EnergyCorrelatorTool, EnergyCorrelatorRatiosTool, NSubjettinessRatiosTool, NSubjettinessTool



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



from collections import namedtuple
JetConfigContext  = namedtuple( 'JetConfigContext', 'algName, alg, R, input, dataType, output' )

class JetConfigurator(object):
    
    
    
    dataType = 'FullS'
    ## ********************************************************
    ## Inputs
    ## ********************************************************
    
    ## The map of known/standard PseudoJetGetter tools.
    ## This a simple python dict which format is  :
    ##   alias : ( class , dict_of_properties )  
    knownInputTools = dict( )

    knownInputLists = dict()
    # These dicts are populated below


    

    def addKnownInput(self, alias, klass, **properties):
        if alias in self.knownInputTools:
            print "ERROR in PseudoJetInput::addKnownInput can't add already existing ",alias
            return
        self.knownInputTools[alias] = (klass, properties)
            
    def getInputTool(self, alias,context=None, **userProp):
        tool = self.aliasToTool( alias, self.knownInputTools, context = context, **userProp)
        if tool is None:
            print "ERROR. JetConfigurator.getInputTool unknown modifer  ",alias
            return None
        return tool

    def getInputToolFromAlgName(self, algname, **userProp):
        alg, R, input = interpretJetName(algName)
        if 'Track' in input:
            input='Track' # (because 'PV0Track' is the same input as 'Track')
        return self.getInputTool(input.lower(), **userProp)

    def getInputList(self, alias, context=None):
        aliasList, failed = self.aliasToListOfAliases(alias, self.knownInputLists)
        if aliasList is None:
            print "ERROR JetConfigurator.getInputList unknown alias", failed
            print "            --> add it to JetConfigurator.knownInputLists ? "
            return

        toolList = []
        aliasList_str = []
        for a in aliasList:            
            if isinstance(a, str):
                t = self.getInputTool(a, context=context)
            else: # assume a is alaready a tool:
                t=a
                a = t.name()
            toolList.append( t )
            aliasList_str.append( a )            
        return toolList, aliasList_str

    ## ********************************************************
    ## Jet finding
    ## ********************************************************

    def getJetFinderTool(self, alg="AntiKt", R=0.4, input="LCTopo", algName=None, doArea=True, context=None, **userProp):
        defaultProps = dict( PtMin = 5000, GhostArea = 0.01 if doArea else 0., RandomOption = 1, )

        if algName is not None:
            alg, R, input = interpretJetName( algName )

        if context:
            toolName = context.output+'.Finder'
        else:
            toolName = algName+"Finder"

        # Technically we need this tool to translate fastjet to xAOD::Jet
        jetFromPJ = JetFromPseudojet(toolName.replace('Finder',"jetbuild"),
                                     Attributes = ["ActiveArea", "ActiveAreaFourVector"] )
        defaultProps['JetBuilder'] = jetFromPJ

        # overwrite with userProp
        defaultProps.update(userProp)
        defaultProps.update( JetAlgorithm =  alg, JetRadius = R )

        # pass all the options to the constructor :
        finder = JetFinder(toolName, **defaultProps)

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
    # These dicts are populated below

    def getModifTool(self, alias, context=None, **userProp ):
        tool = self.aliasToTool( alias, self.knownModifierTools, context = context, **userProp)
        if tool is None:
            print "ERROR. JetConfigurator.getModifTool unknown modifer  ",alias
            return None
        return tool

    def getModifList(self, alias, context=None):
        aliasList, failed = self.aliasToListOfAliases(alias, self.knownModifierList)
        if aliasList is None:
            print "ERROR JetConfigurator.getModifList unknown alias", failed
            print "            --> add it to JetConfigurator.knownModifierList ? "
            return

        toolList = []
        aliasList_str = []
        for a in aliasList:            
            if isinstance(a, str):
                t = self.getModifTool(a, context=context)
            else: # assume a is alaready a tool:
                t=a
                a=t.name()
            aliasList_str.append( a )
            toolList.append( t )
        return toolList, aliasList_str


    ## ********************************************************
    ## Jet Calibration
    ## ********************************************************
    ## define default options for calibrations.    
    ## calibOptions format is in the form
    ##  ('JetAlgName', 'dataType') : ('calib_config_file','calib_sequence')
    calibOptions = { }
    # calibOptions is populated below


    def getCalibTool(self, algName="AntiKt4EMTopo", dataType='FullS', context=None, **userProp):

        if context is not None:
            # take algName and dataType from context and ignore other algs
            algName, dataType = context.algName, context.dataType

        if (algName,dataType) not in self.calibOptions:
            print "ERROR JetConfigurator.getCalibTool  can't retrieve calib config for ",algName, dataType
            return None
        
        confFile, seq = self.calibOptions[ (algName,dataType) ]
        tool=JetCalibrationTool(algName+"calib", IsData= (dataType=='data') , ConfigFile=confFile, CalibSequence=seq, JetCollection=algName)

        return tool

    ## ********************************************************
    ## top level tools
    ## ********************************************************
    ## knownJetBuilders maps a string (an jet container name) to an alias for input and an alias (or list of aliases) for JetModifiers
    ## knownJetBuilders thus maps standard collection names to their configuration (in the form of alias to input and modifiers).
    knownJetBuilders = dict( )
    # knownJetBuilders is populated below


    def jetFindingSequence(self, output, inputList=None, finder=None, modifierList=None, jetTool=None, ):
        """jetFindingSequence returns a JetRecTool (or configure jetTool if given) to run a full jet finding sequence.
        output is the final jet container name.
        output will be used to retrieve the full configuration from the alias dictionnary knownJetBuilders.
        If given, the other arguments will be use to overwrite default config as retrieved from knownJetBuilders.

        output : str (key in knownJetBuilders or something interpretable as alg,radius, input type like 'CamKt11PV0TrackJets')
        optional arguments :
        inputList : str (key in knownInputLists) or a list (str as in knownInputTools or configured PseudoJetGetter instance)
        modifierList :str (key in knownModifierList) or list (str as in knownModifierTools or configured JetModifier instance)        
        finder : a configured JetFinder instance
        jetTool : a JetRecTool instance : will be configured by this function
            

        Examples :
        # re-create AntiKt10LCTopoJets exactly as the standard ones :
        jetConfig.jetFindingSequence('AntiKt10LCTopoJets2', jetTool=jetTool)
        # re-create AntiKt10LCTopoJets exactly as the standard ones but a special list of modifier
        jetConfig.jetFindingSequence('AntiKt10LCTopoJets2', modifierList=['ptMin50GeV','sort','width'],jetTool=jetTool)

        # create AntiKt12LCTopoJets. As this key is not in knownJetBuilders, specify alias for input and modifiers.
        jetConfig.jetFindingSequence('AntiKt12LCTopoJets', inputList='lctopo', modifierList='cut50+substr',jetTool=jetTool)

        # create AntiKt12LCTopoJets. same as above, but no ghosts (as implied by the  'lctopo' input list alias)
        jetConfig.jetFindingSequence('AntiKt12LCTopoJets', inputList=['lctopo'], modifierList='cut50+substr',jetTool=jetTool)
        #    -----> in this case the 'lctopo' is in the list, so it refers to the PseudoJetGetter tool directly

        """
        alg, R, input = interpretJetName(output)
        algName = buildJetAlgName( alg, R)+ input 

        context = JetConfigContext(algName, alg, R, input, self.dataType, output)

        if jetTool is None:
            jetTool = JetRecTool(output )
        else:
            jetTool.setName( output )

        inputAlias, modifAlias = self.knownJetBuilders.get( algName, (None,None) )

        if inputList is None:
            if inputAlias is None:
                print "JetConfigurator.jetFindingSequence ERROR can't retrieve input tools for ", output , " interpreted as ",alg, r, input
                return None
        else:
            inputAlias = inputList # then the user given inputList is an alias 
        inputList, inputAliasList = self.getInputList( inputAlias ,context=context)

        if modifierList is None :
            if modifAlias is None:
                print "JetConfigurator.jetFindingSequence ERROR can't retrieve modifier tools for ", output , " interpreted as ",alg, r, input
                return
        else:
            modifAlias = modifierList
        modifierList, modifAliasList = self.getModifList( modifAlias, context)

        if finder is None:
            finder = self.getJetFinderTool(algName=algName, context=context )

        jetTool.PseudoJetGetters = inputList
        jetTool.JetFinder = finder
        jetTool.JetModifiers = modifierList
        jetTool.OutputContainer = output

        print " *********************************** "
        print " JetConfigurator : Configured jet for ",output
        print "   --> alg name    : ",algName.ljust(20) , '(',alg,R,input,')'
        print "   --> inputs      : ", inputAlias.ljust(20), '=',inputAliasList
        print "   --> modifiers   : ", modifAlias.ljust(20), '=', modifAliasList
        print " *********************************** "
        
                
        return jetTool


    ## ********************************************************
    ## Helpers
    ## ********************************************************
    def aliasToListOfAliases(self, alias, aliasDict):
        """ Given alias (a string or a list of strings), returns a list of aliases as defined by those mapped in aliasDict.
            The following forms are allowed for alias :
            'alias'  --> aliasDict['alias']
            ['alias0','alias1',...] --> ['alias0','alias1',...] (same list)
            'aliasX+aliasY+aliasZ'  --> aliasDict['aliasX']+aliasDict['aliasY']+aliasDict['aliasZ']
        """
        if isinstance(alias, list):
            return alias
        aL = alias.split('+')
        finalAliases = []
        for a in aL :
            a_list = aliasDict.get(a,None)
            if a_list is None:
                return None, a # return no list and the offender
            finalAliases += a_list
        return finalAliases, ''

    def aliasToTool(self, alias,  aliasDict, context=None, **userProp ):
        klass, defaultProp = aliasDict.get(alias, (None,None) )
        tname = alias
        if klass is None:
            return None,
        if userProp != {} :
            finalProp = dict(defaultProp)
            finalProp.update(userProp)
        else:
            finalProp = defaultProp
        if 'context' in finalProp:
            finalProp['context'] = context
        if context:
            tname = context.output+'.'+alias
        print '***',klass
        modif = klass(tname, **finalProp)
        return modif



jetConfig = JetConfigurator()


class ConfigConstants(object):
    """Simply put all config constants in a common place """
    ghostScale = 1e-40

    vtxContainer = "PrimaryVertices"
    trkContainer = "InDetTrackParticles"

cst = ConfigConstants()
    

## ********************************************************
## List of standard configuration aliases
## ********************************************************

## ********************************************************
## top level tool
## ********************************************************
## knownJetBuilders has the form
##  JetContName : ( alias_for_input , alias_for_modifier )
## where
##  * alias_for_input  is a string or a list :
##      'alias'  -> refers to an entry in knownInputLists
##       ['alias1','alias2', tool] -> strings refer to entries in knownInputTools, tool are configured PseudoJetGetter instance. 
##  * alias_for_modifier is  string or a list :
##     'alias' -> refer to an entry in knownModifierList
##     'alias1+alias2' -> refer to 2 entries in knownModifierList to be concatenated
##     ['alias1', 'alias2,' , tool ] -> strings refer to entries in knownModifierTools, tool is a configured tool instance.
##
jetConfig.knownJetBuilders = { 
    'AntiKt4EMTopo'    : ( 'emtopo', 'calib' ),
    'AntiKt4LCTopo'    : ( 'lctopo', 'calib+cut5' ),
    'AntiKt4PV0Track'  : ( 'track', ['ptMin5GeV','width'] ),
    'AntiKt4Truth'     : ( 'truth',  ['ptMin5GeV','width'] ),    

    'AntiKt10LCTopo'   : ( 'lctopo', 'cut50+substr' ),
    'AntiKt10PV0Track' : ( 'track', ['ptMin5GeV','width'] ),
    'AntiKt10Truth'    : ( 'truth', ['ptMin5GeV','width'] ),        
    }


## ********************************************************
## Inputs
## ********************************************************

## knownInputTools : The map of known/standard PseudoJetGetter tools.
## This a simple python dict whicg format is  :
##   alias : ( class , dict_of_properties )  
jetConfig.knownInputTools = {
    'lctopo' : (PseudoJetGetter, dict(InputContainer="CaloCalTopoClusters",Label="LCTopo") ),
    'emtopo' : (PseudoJetGetter, dict(InputContainer="CaloCalTopoClusters",Label="EMTopo") ),
    'track'  : (TrackPseudoJetGetter, dict(InputContainer="JetSelectedTracks_LooseTrackJets", Label="Track",
                                           TrackVertexAssociation  = "JetTrackVtxAssoc") ),
    'gtrack' : (TrackPseudoJetGetter, dict(InputContainer="JetSelectedTracks_LooseTrackJets", Label="Track",
                                           TrackVertexAssociation  = "JetTrackVtxAssoc" , GhostScale=cst.ghostScale) ),
    'truth'  : (PseudoJetGetter, dict(InputContainer="JetInputTruthParticles",Label="Truth") ),
    'gtruth' : (PseudoJetGetter, dict(InputContainer="JetInputTruthParticles",Label="Truth" , GhostScale=cst.ghostScale) ), 
    }

## knownInputLists : maps an alias to a list of alias for individual input tool as defined in knownInputTools.
## format is 'alias' : [ 'alias1', 'alias2',...] where strings in the list are keys in knownInputTools
jetConfig.knownInputLists = {
    'lctopo' : [ 'lctopo', 'gtrack', 'gtruth' ] ,
    'emtopo' : [ 'lctopo', 'gtrack', 'gtruth' ] ,
    'track'  : ['track']
    }




## ********************************************************
## Jet Modifiers
## ********************************************************
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
    }

## knownModifierList : maps an alias to a list of alias for individual modifier tool as defined in knownModifierTools.
## format is 'alias' : [ 'alias1', 'alias2',...] where strings in the list are keys in knownModifierTools
jetConfig.knownModifierList = {
    'calib'  : ["calib",] ,
    'cut5'   : ["ptMin5GeV", "sort"],
    'cut50'  : ["ptMin50GeV", "sort"],
    'substr' : ["encorr", "encorrR", "nsubjet", "nsubjetR"],
    # ... etc ...
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
