from xAH_config import xAH_config
import ROOT
from JSSTutorial.JetRecConfig import globalOptionBuilder

c = xAH_config()
c.setalg("BasicEventSelection", {"m_name": "test", "m_useMetaData": False, "m_derivationName": "JETM8"})


# *******************************************************
# This alg allows to set global jet configuration options from this submission script (ex: options related to the dataset to be processed).
# they will be set in the 'jetConfig.globalOptions' dictionnary.
#  Why use this algo and not set options directly to jetConfig ?
#   --> because this script is executed *only* at submission time and not on the worker nodes.
#   --> this is how EventLoop works.
#   --> We thus encode the options into a string which we passes to this alg. The string is saved and accessible on the worker nodes.
#   --> this alg only does 1 thing in the initialize() : decode the string and set back the options into jetConfig.globalOptions
#   --> it must run before any other algs using jetConfig.
c.setalg("JetGlobalOptionsAlgo", dict(m_optionString=globalOptionBuilder(mystring="AAA", dataType="FullS"),                                    
                                      ) )

# *******************************************************
# The algs below run the pre-selection tools for truth and track jet building
# They MUST run before attempting to use tracks or truth with jets

# Preparation for tracks
# Schedule Track-Vertex association
c.setalg("JetExecuteToolAlgo", dict(m_configScript='JSSTutorial/xAHjetconfig_example.py',                                    
                                    m_className="TrackVertexAssociationTool", m_name="TrkVtxAssoc", # give the actuall class and name of the tool we configure
                                    m_configCall="buildJetTrackVtxAssoc(tool)")
         )

# Schedule Track filtering : the standard is to keep loose track. This creates a dedicated track container.
c.setalg("JetExecuteToolAlgo", dict(m_configScript='JSSTutorial/xAHjetconfig_example.py',                                    
                                    m_className="JetTrackSelectionTool", m_name="JetTrkSelec", # give the actuall class and name of the tool we configure
                                    m_configCall="buildJetTrackSelection(tool)")
         )

# -------------------------------
# Preparation for Truth
# Schedule truth particle filtering : filter stable, non-neutrino particles into their own container

c.setalg("JetExecuteToolAlgo", dict(m_configScript='JSSTutorial/xAHjetconfig_example.py',                                    
                                    m_className="CopyTruthJetParticles", m_name="filtertruth", # give the actuall class and name of the tool we configure
                                    m_configCall="buildJetInputTruthParticles(tool)")
         )
# *******************************************************



# *******************************************************
# Jet building  
# -------------------------------
# Track jets
c.setalg("JetRecToolAlgo", dict(m_configScript="JSSTutorial/xAHjetconfig_example.py", 
                                m_configCall="jetConfig.jetFindingSequence('AntiKt10PV0Track',outputName='AntiKt10PV0TrackJets2',jetTool=tool)",
                                m_name="buildTrkJet" ))

# -------------------------------
# trimm the Track jets  we just build :
c.setalg("JetRecToolAlgo", dict(m_configScript="JSSTutorial/xAHjetconfig_example.py",
                                m_configCall="jetConfig.jetGroomingSequence('AntiKt10PV0TrackJets2', 'Trim', modifierList='cut50+substr',jetTool=tool)",
                                m_name="buildtrkTrim" ))

# -------------------------------
# rebuild cluster jets, ghost-associating tracks : the 'AntiKt10LCTopo_full' key word includes ghost tracks and truth
c.setalg("JetRecToolAlgo", dict(m_configScript="JSSTutorial/xAHjetconfig_example.py", 
                                m_configCall="jetConfig.jetFindingSequence('AntiKt10LCTopo_full',outputName='AntiKt10LCTopoJets2', jetTool=tool)",
                                m_name="buildfull" ))

# -------------------------------
# trimm the LCTopo jets  we just build :
c.setalg("JetRecToolAlgo", dict(m_configScript="JSSTutorial/xAHjetconfig_example.py",
                                m_configCall="jetConfig.jetGroomingSequence('AntiKt10LCTopoJets2', 'Trim', modifierList='cut50+substr',jetTool=tool)",
                                m_name="buildlctopTrim" ))

# -------------------------------
# build truth jets.
c.setalg("JetRecToolAlgo", dict(m_configScript="JSSTutorial/xAHjetconfig_example.py", 
                                m_configCall="jetConfig.jetFindingSequence('AntiKt10Truth', jetTool=tool,outputName='AntiKt10TruthJets2')",
                                m_name="buildTruth" ))


# *******************************************************
# plot some of the containers we build.
# -------------------------------

## c.setalg("IParticleHistsAlgo", dict(m_debug= False,
##                                     m_inContainerName= "AntiKt10TruthJets",
##                                     m_detailStr= "kinematic",
##                                     m_histPrefix="Jet_",
##                                     )
##          )
## c.setalg("IParticleHistsAlgo", dict(m_debug= False,
##                                     m_inContainerName= "AntiKt10TruthJets2",
##                                     m_detailStr= "kinematic",
##                                     m_histPrefix="Jet2_",
##                                     )
##          )

# *******************************************************
# Write out a Mini xAOD
# -------------------------------

# prepare a vector of string.
selectedVars = ROOT.vector('string')()

containers = [ "AntiKt10LCTopoJets","AntiKt10TruthJets" , "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets", "AntiKt10PV0TrackTrimmedPtFrac5SmallR20Jets" ] # containers from input file
containers2 = [jc+"2" for jc in containers] + [ "AntiKt10PV0TrackJets2"]
#containers += [ ]

# each entry specify which entry we keep in the xAOD (one can instead veto variables by appending a - before each var)
def selectJV(cont,vect):
    vect.push_back(cont+"Aux.pt.eta.phi.m.Width.Tau32_wta")

for jc in containers+containers2:
    selectJV(jc,selectedVars)

c.setalg("JSSMinixAOD", dict(m_debug= False,
                             m_outputFileName="JSSxAOD.root",
                             m_createOutputFile=True,
                             m_copyFileMetaData=True,m_copyCutBookkeeper=True,
                             m_simpleCopyKeys = ' '.join(containers),
                             m_storeCopyKeys=  ' '.join(containers2),
                             ) )

