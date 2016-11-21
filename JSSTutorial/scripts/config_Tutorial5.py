## ************************************
## This example demonstrates how to run a cluster modifier sequence
## and how to use the output sequence to run a jet finding alg on top of that.
## 
## ************************************
from xAH_config import xAH_config
    
c = xAH_config()
c.setalg("BasicEventSelection", {"m_debug": False,
                                 "m_isMC" : True,
                                 "m_truthLevelOnly": False,
                                 "m_applyGRLCut": False,
                                 "m_doPUreweighting": False,
                                 "m_vertexContainerName": "PrimaryVertices",
                                 "m_PVNTrack": 2,
                                 "m_applyPrimaryVertexCut": True,
                                 "m_useMetaData": False,
                                 "m_derivationName": "JETM8"
                                })

# -------------------------------
# needed to fix an issue with Vertex missing their x and y attributes.
#  (NOT needed when input file is a full xAOD since xAOD do have the x and y info)
c.setalg("FixVertexAlgo",dict(m_debug=True , m_name="FixVertexAlgo") ) 

# run a cluster modifier sequence : see JSSTutorial/python/JetConstitSeqConfig.py
# this produces a new cluster container in the EvtStore named OrigSKLCTopoClusters
c.setalg("JetConstitSeqAlgo", dict(m_configScript="xAHjetconfig_example.py", 
                                   m_configCall="constitConfig.constitModifSequence('OrigSKLCTopoClusters',jetTool=tool)",
                                   m_name="sklcoriginAlg" ))

# re-build a AntiKt10LCTopoJets : 
c.setalg("JetRecToolAlgo", dict(m_configScript="xAHjetconfig_example.py", 
                                 m_configCall="t=jetConfig.jetFindingSequence('AntiKt10LCTopo',outputName='AntiKt10LCTopoJets2', jetTool=tool)",
                                 m_name="buildalg" ))

# re-build a AntiKt10 alg using modified constits OrigSKLCTopoClusters:
# Here we use an alternate config scripts where we added a helper function addClusterPseudoJetGetter.
# this function justs add a dedicated PseudoJetGetter tool to the list of known input tools.
# In the following call to jetFindingSequence we use the top level keyword 'AntiKt10LCTopo' and we overwrite the inputList arguments
# this way we run the same jet sequence as above, just changing the input.
c.setalg("JetRecToolAlgo", dict(m_configScript="xAHjetconfig_example_constiMod.py",  # special config file !!
                                 m_configCall="addClusterPseudoJetGetter('OrigSKLCTopoClusters');jetConfig.jetFindingSequence('AntiKt10LCTopo',outputName='AntiKt10OrigSKLCTopoJets', jetTool=tool,inputList=['OrigSKLC']);",
                                 m_name="buildalg" ))

# An alternative would have been to encode the new inputs directly in xAHjetconfig_example_constiMod.py
# by adding lines like
# jetConfig.knownJetBuilders['AntiKt10OrigSKLCTopo'] = ( ['OrigSKLC'], 'calib+cut5' )
# jetConfig.knownInputLists['OrigSKLC'] = (PseudoJetGetter, dict(InputContainer='OrigSKLCTopoClusters',Label="LCTopo",SkipNegativeEnergy=True,  ) )
# and then using the call :
# m_configCall="jetConfig.jetFindingSequence('AntiKt10OrigSKLCTopo',outputName='AntiKt10OrigSKLCTopoJets', jetTool=tool)" 
#
# The advantage of the above method is we can create a PseudoJetGetter and run with it in 1 line without having to
# modify xAHjetconfig_example_constiMod.py each time we want a new input container.


# dump some histos from above jets
c.setalg("JSSTutorialPythonConfigAlgo", {"m_debug": True,
                                         "m_name": "JSSTutorialPythonConfigAlgo",
                                         "m_trimmedContName" : 'AntiKt10OrigSKLCTopoJets', # just pretend it's trimmed.
                                         "m_prunedContName" : "AntiKt10OrigSKLCTopoJets",  # just pretend it's pruned
                                         "m_TreeName" : "JetTree_pyconfig"
                                         })

# -------------------------------
# plot containers we build.
## c.setalg("IParticleHistsAlgo", dict(m_debug= False,
##                                     m_inContainerName= "AntiKt10LCTopoJets2",
##                                     m_detailStr= "kinematic",
##                                     m_histPrefix="Jet_",
##                                     )
##          )

## c.setalg("IParticleHistsAlgo", dict(m_debug= False,
##                                     m_inContainerName= "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets2",
##                                     m_detailStr= "kinematic",
##                                     m_histPrefix="Jet2_",
##                                     ) )

