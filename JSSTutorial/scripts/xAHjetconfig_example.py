
import ROOT
from JSSTutorial.JetRecConfig import buildClusterGetter
from JSSTutorial.JetRecConfig import buildJetTrackVtxAssoc , buildJetTrackSelection, buildJetInputTruthParticles

if 'wrapper' in dir():
    jetTool = wrapper.m_tool


def simpleJetConfig(jetTool, output="AntiKt10LCTopoJets2"):
    """This function demonstrate how to configure a JetRecTool expliciting each sub-tool.
    jetTool : is an instance of a JetRecTpp;
    output  : the final collection name.
    """

    # import atlas jet tools. 
    # these imports are specific to RootCore. In Athena tools are imported differently.
    from ROOT import PseudoJetGetter, JetFinder, JetWidthTool, JetFromPseudojet, JetCalibrationTool
    from ROOT import JetSorter, JetFilterTool, JetPseudojetRetriever, JetTrimmer
    from ROOT import EnergyCorrelatorTool, EnergyCorrelatorRatiosTool, NSubjettinessRatiosTool, NSubjettinessTool

    # *********************************************************
    # Step 1. Configure input
    #  We create a list of PseudoJetGetter tools
    # (in more complex configs there are more than 1 PseudoJetGetter)

    lcgetter =  PseudoJetGetter("lcget"
                                , InputContainer = "CaloCalTopoClusters"
                                , OutputContainer = "PseudoJetLCTopo"
                                , Label = "LCTopo"
                                , SkipNegativeEnergy = True
                                , GhostScale = 0.0 )

    getterList = [lcgetter]


    # *********************************************************
    # Step 2. Configure Jet Finder

    # Technically we need this tool to translate fastjet to xAOD::Jet
    jetFromPJ = JetFromPseudojet("jetbuild",
                                 Attributes = ["ActiveArea", "ActiveAreaFourVector"] )
    
    finder = JetFinder("AntiKt10Finder"
                       ,JetAlgorithm = "AntiKt"
                       ,JetRadius = 1.0
                       ,PtMin = 2000.0
                       ,GhostArea = 0.01 # if non-null will run ActiveArea calculation
                       ,RandomOption = 1 
                       ,JetBuilder = jetFromPJ )# associate


    # *********************************************************
    # Step 3. Configure Jet modifiers
    #      We create a list of JetModifiers    

    # we can create modifiers like this :
    widthT = JetWidthTool("Width")
    # or directly in a list :
    modifierList = [        
        JetFilterTool("ptfilter",PtMin=200000),
        JetSorter("jetsorter"), # sorts by pT
        widthT,
        EnergyCorrelatorTool("ecorr", Beta=1.),
        EnergyCorrelatorRatiosTool("ecorrR"),
        NSubjettinessTool("nsubjettiness",Alpha=1.),
        NSubjettinessRatiosTool("nsubjettinessR",),        
        ]


    # ***********************************
    # finally put everything together :
    
    jetTool.PseudoJetGetters = getterList
    jetTool.JetFinder        = finder
    jetTool.JetModifiers     = modifierList
    jetTool.OutputContainer  = output

    # jetTool.OutputLevel = 2    


def simpleJetConfig2(jetTool, output="AntiKt10LCTopoJets2"):
    """Exactly as above but making use of some helper functions.
    Shorter to write/read."""
    from JSSTutorial.JetRecConfig import buildClusterGetter, buildJetFinder, buildJetCalibModifiers

    jetTool.PseudoJetGetters = [ buildClusterGetter() ] # (this property is a list)
    jetTool.JetFinder        = buildJetFinder("AntiKt10")
    jetTool.JetModifiers     = buildJetCalibModifiers("AntiKt10LCTopo") + [ROOT.JetWidthTool("width")]
    jetTool.OutputContainer  = output
    

def minimalJetReco(jetTool, jetContName):
    """Almost as above. Will configure jetTool so it builds jetContName.
    jetContName must be a standard-like name (ex : 'AntKt5EMTopoJets').
    """

    from JSSTutorial.JetRecConfig import buildClusterGetter, buildJetFinder, buildJetCalibModifiers, buildPseudoJetGetter

    jetTool.PseudoJetGetters = [ buildPseudoJetGetter(jetContName) ] # (this property is a list)
    jetTool.JetFinder        = buildJetFinder(jetContName)
    jetTool.JetModifiers     = buildJetCalibModifiers(jetContName) + [ROOT.JetWidthTool("width")]
    jetTool.OutputContainer  = jetContName

def minimalJetRecoWithGhosts(jetTool, jetContName):
    """Demonstrate how to add ghosts objects
    """
    from JSSTutorial.JetRecConfig import buildClusterGetter, buildJetFinder, buildJetCalibModifiers, buildPseudoJetGetter

    # first configure without ghosts
    minimalJetReco(jetTool, jetContName)

    ASDS
    getters = jetTool.PseudoJetGetters
    ghostTrackgetter = buildPseudoJetGetter(algname=None, input="Track", asGhost=True)
    jetTool.PseudoJetGetters = getters+[ ghostTrackgetter]



def minimalJetTrimming(jetTool, inputJets,rclus,ptfrac):
    """configures jetTool (a JetRecTool instance) to run trimming on inputJets """
    from ROOT import JetPseudojetRetriever, JetTrimmer, JetFromPseudojet
    from ROOT import EnergyCorrelatorTool, EnergyCorrelatorRatiosTool, NSubjettinessRatiosTool, NSubjettinessTool

    from JSSTutorial.JetRecConfig import buildClusterGetter, buildJetFinder, buildJetCalibModifiers, buildPseudoJetGetter

    outputJets = inputJets.replace('Jets', 'TrimmedPtFrac'+str(int(ptfrac*100))+"SmallR"+str(int(rclus*100))+"Jets")


    groomer = JetTrimmer(outputJets + "Tool"
                         ,RClus = rclus
                         ,PtFrac = ptfrac
                         ,JetBuilder=     JetFromPseudojet(outputJets+"jetbuild",
                                                           Attributes = [] )
                         )

    jetTool.JetGroomer = groomer
    jetTool.InputContainer = inputJets
    jetTool.OutputContainer = outputJets
    jetTool.JetPseudojetRetriever = JetPseudojetRetriever("pjretriever")
    jetTool.JetModifiers = [
        # JetFilterTool("ptfilter",PtMin=200000), no need for filter : we just read input jets.
        EnergyCorrelatorTool("ecorr", Beta=1.),
        EnergyCorrelatorRatiosTool("ecorrR"),
        NSubjettinessTool("nsubjettiness",Alpha=1.),
        NSubjettinessRatiosTool("nsubjettinessR",),        
        ]

def minimalJetPruning(jetTool, inputJets,zcut,rcut):
    """configures jetTool (a JetRecTool instance) to run pruning on inputJets """
    from ROOT import JetPseudojetRetriever, JetPruner, JetFromPseudojet
    from ROOT import EnergyCorrelatorTool, EnergyCorrelatorRatiosTool, NSubjettinessRatiosTool, NSubjettinessTool

    from JSSTutorial.JetRecConfig import buildClusterGetter, buildJetFinder, buildJetCalibModifiers, buildPseudoJetGetter

    outputJets = inputJets.replace('Jets', 'PrunedZcut'+str(int(zcut*10))+"Rcut"+str(int(rcut*10))+"Jets")


    groomer = JetPruner(outputJets + "Tool"
                        ,RCut = rcut
                        ,ZCut = zcut
                        ,JetAlgorithm = "CamKt"
                        ,JetBuilder=     JetFromPseudojet(outputJets+"jetbuild",
                                                          Attributes = [] )
                         )

    jetTool.JetGroomer = groomer
    jetTool.InputContainer = inputJets
    jetTool.OutputContainer = outputJets
    jetTool.JetPseudojetRetriever = JetPseudojetRetriever("pjretriever")
    jetTool.JetModifiers = [
        # JetFilterTool("ptfilter",PtMin=200000), no need for filter : we just read input jets.
        EnergyCorrelatorTool("ecorr", Beta=1.),
        EnergyCorrelatorRatiosTool("ecorrR"),
        NSubjettinessTool("nsubjettiness",Alpha=1.),
        NSubjettinessRatiosTool("nsubjettinessR",),        
        ]


    


