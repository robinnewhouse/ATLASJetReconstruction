#########################################
##
## JetRecConfig
##
## This file is a prototype module for a jet configuration system compatible
## with RootCore.
## The system is based on a hierarchy of keywords which describe top-level full configuration
## down to individual tool configuration
## An example of keyword hiearchy could be :
##  
## 'AntiKt4EMTopo'                       # a top-level keyword. Refers to :
##    ('emtopoInputs', 'calib+cut5' )    # a pair (keyword for inputs, keyword(s) for modifiers). They refer to
##
##       'emtopoInputs' --> [ 'emtopo' ] # a list of keywords for input tools (see below for 'emtopo')
##
##       'calib'        --> ['calib']             # a list of keywords for input tools
##       'cut5'         --> ['ptMin5GeV', 'sort'] # an other list of keywords for input tools
##                                                # (then calib+cut5 is interpreted as ['calib','ptMin5GeV', 'sort']
##
##         'emtopo' --> (PseudoJetGetter, dict(InputContainer="CaloCalTopoClusters",Label="EMTopo",SkipNegativeEnergy=True, OutputContainer="EMTopoPseudoJetVec") ) # specification for an input tool in the form (class, dict_of_properties)
##
##         'ptMin5GeV' --> ( JetFilterTool, dict(PtMin= 5000) ), $ specification for a modifier tool in the form (class, dict_of_properties)
##        ... etc ....
##
## Together with this hiearchy, some helper functions can interpret the keywords and return the corresponding
## configured tool.
## As examples :
##
## # re-create AntiKt10LCTopoJets exactly as the standard ones (just changing the name) :
## jetConfig.jetFindingSequence('AntiKt10LCTopo',  outputName="AntiKt10LCTopoJets2", jetTool=jetTool)
## 
## # re-create AntiKt10LCTopoJets exactly as the standard ones but a special list of modifiers
## jetConfig.jetFindingSequence('AntiKt10LCTopo', modifierList=['ptMin50GeV','sort','width'],outputName="AntiKt10LCTopoJets2", jetTool=jetTool)
## 
## 
##
##
import JSSTutorial.RootCoreConfigInit

from ROOT import JetFromPseudojet, JetFinder, JetPseudojetRetriever



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




###############################################3
## Below is config experimentations. 
###############################################3


class JetConfigException(Exception):
    pass

from collections import namedtuple
JetConfigContext  = namedtuple( 'JetConfigContext', 'algName, alg, R, input, dataType, output' )

class JetConfigurator(object):        
    
    dataType = 'FullS'

    ## ********************************************************
    ## Dictionnary definitions
    ## ********************************************************

    ## knownJetBuilders = { 'top_level_alias' : ( alias_for_input, alias_for_modifier) }
    ##    where alias_for_xxx is either a string or a list of alias
    ## knownJetBuilders thus maps top aliase to a full jet alg configuration (in the form of alias to input and modifiers).
    knownJetBuilders = dict( )

    ## ------------------------
    ## knownJetGroomers maps a string (groomer alias) to (klass, dict_of_properties, namebuilding_function)
    knownJetGroomers = dict( )

    ## ------------------------
    ## map alias to list of alias = {'alias_for_input' : ['alias1', 'alias2', ...] }
    ##   where 'aliasX' is an entry in knownInputTools (i.e is an alias to a tool configuration)
    knownInputLists = dict()

    ## The map of known/standard PseudoJetGetter tools  : 'alias' : ( class , dict_of_properties )  
    knownInputTools = dict( )

    ## ------------------------
    ## map alias to list of alias = {'alias_for_modifier' : ['alias1', 'alias2', ...] }
    ##   where 'aliasX' is an entry in knownModifierTools (i.e is an alias to a tool configuration)
    knownModifierList = dict( )
    ## The map of known/standard modifier tools  : 'alias' : ( class , dict_of_properties )
    ##  (class can also be a function, see )
    knownModifierTools = dict( )

    ## ------------------------
    ## define default options for calibrations.    
    ## calibOptions format is in the form
    ##  ('JetAlgName', 'dataType') : ('calib_config_file','calib_sequence')
    calibOptions = { }

    ##
    ## All standard content of these dictionnaries is done in JetRecDefaultTools.py
    ##


    ## ********************************************************
    ## top level tools
    ## ********************************************************

    def jetFindingSequence(self, topAlias, inputList=None, finder=None, modifierList=None, jetTool=None, outputName=None ):
        """jetFindingSequence returns a JetRecTool (or configure jetTool if given) to run a full jet finding sequence.
        topAlias will be used to retrieve the full configuration from the alias dictionnary knownJetBuilders.
        If given, the other arguments will be use to overwrite default config as retrieved from knownJetBuilders.

        topAlias : str, a key in knownJetBuilders in the form 'AlgRadInputSuffix' like 'CamKt11PV0TrackJets' (i.e Alg=Cam ,Rad=11, Input=PV0Track)

        optional arguments :
        inputList : str (key in knownInputLists) or a list of (key in knownInputTools or configured PseudoJetGetter instance)
        modifierList :str (key in knownModifierList) or list of (key in knownModifierTools or configured JetModifier instance)        
        finder : a configured JetFinder instance
        jetTool : a JetRecTool instance : will be configured by this function
        outputName : name of final JetContainer. if None, will be build from topAlias    

        Examples :
        # re-create AntiKt10LCTopoJets exactly as the standard ones :
        jetConfig.jetFindingSequence('AntiKt10LCTopoJets2', jetTool=jetTool)
        # re-create AntiKt10LCTopoJets exactly as the standard ones but a special list of modifiers
        jetConfig.jetFindingSequence('AntiKt10LCTopoJets2', modifierList=['ptMin50GeV','sort','width'],jetTool=jetTool)

        # create AntiKt12LCTopoJets. As this key is not in knownJetBuilders, specify aliases for input and modifiers.
        jetConfig.jetFindingSequence('AntiKt12LCTopoJets', inputList='lctopoInputs', modifierList='cut50+substr',jetTool=jetTool)

        # create AntiKt12LCTopoJets. same as above, but no ghosts (as implied by the  'lctopo' input list alias)
        jetConfig.jetFindingSequence('AntiKt12LCTopoJets', inputList=['lctopo'], modifierList='cut50+substr',jetTool=jetTool)
        #    -----> in this case the 'lctopo' is in the list, so it refers to the PseudoJetGetter tool directly

        """
        alg, R, input = interpretJetName(topAlias)
        algName = buildJetAlgName( alg, R)+ input 
        if outputName is None : outputName = algName+"Jets"

        # context is used to pass around usefull information to sub-tools configuration 
        context = JetConfigContext(algName, alg, R, input, self.dataType, outputName)

        if jetTool is None:
            jetTool = JetRecTool(outputName )
        else:
            jetTool.setName( outputName )
        

        inputAlias, modifAlias = self.knownJetBuilders.get( topAlias, (None,None) )

        # prepare the inputs --------------
        if inputList is None:
            if inputAlias is None:
                print "JetConfigurator.jetFindingSequence ERROR can't retrieve input tools for ", topAlias , " interpreted as ",alg, r, input
                raise JetConfigException("Bad input specification")
        else:
            inputAlias = inputList # consider the user given inputList as an alias 
        # interpret the inputAlias :
        inputList, inputAliasList = self.getInputList( inputAlias ,context=context)

        # prepare the modifiers --------------
        if modifierList is None :
            if modifAlias is None:
                print "JetConfigurator.jetFindingSequence ERROR can't retrieve modifier tools for ", topAlias , " interpreted as ",alg, r, input
                raise JetConfigException("Bad modif specification")
        else:
            modifAlias = modifierList
        modifierList, modifAliasList = self.getModifList( modifAlias, context)

        # prepare the finder --------------
        if finder is None:
            finder = self.getJetFinderTool(algName=algName, context=context )


        jetTool.PseudoJetGetters = inputList
        jetTool.JetFinder = finder
        jetTool.JetModifiers = modifierList
        jetTool.OutputContainer = outputName
        

        print " *********************************** "
        print " JetConfigurator : Configured jet finder for ", topAlias, "  -> ", outputName
        print "   --> alg name    : ",algName.ljust(20) , '(',alg,R,input,')'
        print "   --> inputs      : ", str(inputAlias).ljust(20), '=',inputAliasList
        print "   --> modifiers   : ", str(modifAlias).ljust(20), '=', modifAliasList
        print " *********************************** "
        
                
        return jetTool



    def jetGroomingSequence(self, inputJets, groomAlias,  modifierList=None, jetTool=None, outputJets=None, **userParams ):
        """ """

        # retrieve class, parameters and name from the dict :
        groomerKlass, groomerParams, nameBuildingFunc = self.knownJetGroomers.get( groomAlias, (None,None,None) )

        if groomerKlass is None :
            print "JetConfigurator.jetGroomingSequence ERROR can't retrieve groomer for ", groomAlias
            raise JetConfigException("Bad groomer specification")

        # take user parameters into account
        if userParams != {}:
            groomerParams = dict(groomerParams) # copy
            groomerParams.update(userParams)

        algName = nameBuildingFunc(**groomerParams)
        if outputJets is None:
            alg,R,input = interpretJetName(inputJets)
            outputJets = inputJets.replace(input, input+algName)

        context = JetConfigContext(algName, groomAlias, -1, inputJets, self.dataType, outputJets)

        if jetTool is None:
            jetTool = JetRecTool(algName )
        else:
            jetTool.setName( algName )
        
        if modifierList is None :
            # use the same as for input :
            inputAlias, modifAlias = self.knownJetBuilders.get( inputJets, (None,None) )
            if modifAlias is None :
                print "ERROR JetConfigurator.jetGroomingSequence : can't guess a modifier list from ",inputJets
                raise JetConfigException("Bad modifier specification")
        else:
            modifAlias = modifierList

        modifierList, modifAliasList = self.getModifList( modifAlias, context)

        # needed for technical reasons
        jetBuilder=JetFromPseudojet(outputJets+"jetbuild", Attributes = [] )
                                    

        jetTool.JetGroomer = groomerKlass( algName, JetBuilder=jetBuilder,**groomerParams)
        jetTool.InputContainer = inputJets
        jetTool.OutputContainer = outputJets
        jetTool.JetPseudojetRetriever = JetPseudojetRetriever("pjretriever")
        jetTool.JetModifiers = modifierList        

        print " *********************************** "
        print " JetConfigurator : Configured jet groomer for ", groomAlias, " from ", inputJets, "to", outputJets
        print "   --> groom class : ", groomerKlass
        print "   --> groom params: ", groomerParams
        print "   --> modifiers   : ", modifAlias.ljust(20), '=', modifAliasList
        print " *********************************** "

        return jetTool
    


    ## ********************************************************
    ## Jet finding
    ## ********************************************************

    def getJetFinderTool(self, alg="AntiKt", R=0.4, input="LCTopo", algName=None, doArea=True, context=None, **userProp):
        """returns a configured JetFinder tool.

        The JetFinderTool is configured according to the input arguments.
        if algName is not None, it is interpreted and OVERWRITES alg,R and input.
        
        """
        # Use some default properties
        defaultProps = dict( PtMin = 5000, GhostArea = 0.01 if doArea else 0., RandomOption = 1, )

        if algName is None:
            algName = buildJetAlgName(alg,R)
        else:
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
            vertexIndex = 0 # configure it to use PV0 (we could interpret input)
            vfinder = JetByVertexFinder(toolName + "ByVertex",
                                        JetFinder = finder,
                                        Vertex =  vertexIndex) 
            finder = vfinder

        return finder


    ## ********************************************************
    ## Inputs
    ## ********************************************************

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
        """Interpret the given alias and returns (toolList, aliasList).
        input :
        alias : str (key in knownInputLists) or a list of (key in knownInputTools or configured PseudoJetGetter instance)

        returns 
          - toolList : a list of configured instances
          - aliasList : a list of strings (alias or tool names)
          """
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
    ## Jet Modifiers
    ## ********************************************************

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
    ## Helpers
    ## ********************************************************
    def aliasToListOfAliases(self, alias, aliasDict):
        """ Given alias (a string or a list of strings), returns a list of aliases as defined by those mapped in aliasDict.
            The following forms are allowed for alias :
             * 'alias'                 --> aliasDict['alias']
             * ['alias0','alias1',...] --> ['alias0','alias1',...] (same list)
             * 'aliasX+aliasY+aliasZ'  --> aliasDict['aliasX']+aliasDict['aliasY']+aliasDict['aliasZ']
        """
        if isinstance(alias, list):
            return alias, ''
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
        if userProp != {} : # copy default and update
            finalProp = dict(defaultProp)
            finalProp.update(userProp)
        else:
            finalProp = defaultProp

        if 'context' in finalProp: # then klass is actually a function which needs a context
            finalProp['context'] = context
        if context:
            tname = context.output+'.'+alias
        print '***',klass
        modif = klass(tname, **finalProp)
        return modif



jetConfig = JetConfigurator()

import JetRecDefaultTools



## **************************************************************************
## Helper functions 
## 
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
