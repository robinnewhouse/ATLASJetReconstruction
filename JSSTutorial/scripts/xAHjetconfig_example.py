
import ROOT
from JSSTutorial.JetRecConfig import buildClusterGetter

if 'wrapper' in dir():
    jetTool = wrapper.m_tool


def simpleJetConfig(jetTool, output="AntiKt10LCTopoJets2"):
    from ROOT import PseudoJetGetter, JetFinder, JetWidthTool, JetFromPseudojet, JetCalibrationTool
    from ROOT import JetSorter, JetFilterTool, JetPseudojetRetriever, JetTrimmer

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
    #lcgetter.initialize()

    getterList = [lcgetter]


    # *********************************************************
    # Step 2. Configure Jet Finder

    # Technically we need this tool to translate fastjet to xAOD::Jet
    jetFromPJ = JetFromPseudojet("jetbuild",
                                 Attributes = ["ActiveArea", "ActiveAreaFourVector"] )
    
    finder = JetFinder("AntiKt10Finder"
                       ,JetAlgorithm = "AntiKt"
                       ,JetRadius = 1.0
                       ,PtMin = 50000.0
                       ,GhostArea = 0.01 # if non-null will run ActiveArea calculation
                       ,RandomOption = 1 
                       ,JetBuilder = jetFromPJ )# associate
    #finder.initialize()



    # *********************************************************
    # Step 3. Configure Jet modifiers
    #      We create a list of JetModifiers    

    # we can create modifiers like this :
    widthT = JetWidthTool("Width")
    # or directly in a list :
    modifierList = [
        JetCalibrationTool("Akt10Calib" # there's no AntiKt10 calib : use the trimmed one for this example.
                           , JetCollection="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
                           , ConfigFile = "JES_MC15recommendation_FatJet_June2015.config"
                           , CalibSequence = "EtaJES_JMS"
                           , IsData = True
                           ),
        JetSorter("jetsorter"),
        JetFilterTool("ptfilter",PtMin=15000),
        widthT,
        ]


    # ***********************************
    # finally put everything together :
    
    jetTool.PseudoJetGetters = getterList
    jetTool.JetFinder        = finder
    jetTool.JetModifiers     = modifierList
    jetTool.OutputContainer  = output

    #jetTool.initialize()
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


def minimalJetTrimming(jetTool, inputJets,rclus,ptfrac):
    from ROOT import JetPseudojetRetriever, JetTrimmer, JetFromPseudojet
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
    jetTool.JetModifiers = buildJetCalibModifiers(outputJets)
    jetTool.JetPseudojetRetriever = JetPseudojetRetriever("pjretriever")


    


