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
# re-build a AntiKt10LCTopoJets : 
c.setalg("JetRecToolAlgo", dict(m_configScript="xAHjetconfig_example.py", 
                                 m_configCall="jetConfig.jetFindingSequence('AntiKt10LCTopo',outputName='AntiKt10LCTopoJets2', jetTool=tool)",
                                 m_name="buildalg" ))
# trimm the  AntiKt10LCTopoJets  we just build :
c.setalg("JetRecToolAlgo", dict(m_configScript="xAHjetconfig_example.py",
                                m_configCall="jetConfig.jetGroomingSequence('AntiKt10LCTopoJets2', 'Trim', modifierList='substr',jetTool=tool)",
                                m_name="buildtrimm" ))

# prune the  AntiKt10LCTopoJets  we just build :
c.setalg("JetRecToolAlgo", dict(m_configScript="xAHjetconfig_example.py",
                                m_configCall="jetConfig.jetGroomingSequence('AntiKt10LCTopoJets2', 'Prun', modifierList='substr',jetTool=tool)",                                
                                m_name="buildprun" ))


c.setalg("JSSTutorialPythonConfigAlgo", {"m_debug": True,
                                         "m_name": "JSSTutorialPythonConfigAlgo",
                                         "m_trimmedContName" : "AntiKt10LCTopoTrimPtFrac5SmallR2Jets2",
                                         "m_prunedContName" : "AntiKt10LCTopoPrunZCut10RCut5Jets2",
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

