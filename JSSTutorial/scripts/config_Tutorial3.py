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
c.setalg("JetRecToolAlgo", dict(m_configScript="JSSTutorial/xAHjetconfig_example.py", 
                                 m_configCall="simpleJetConfig(tool, 'AntiKt10LCTopoJets2')",
                                 m_name="buildalg" ))
# trimm the  AntiKt10LCTopoJets  we just build :
c.setalg("JetRecToolAlgo", dict(m_configScript="JSSTutorial/xAHjetconfig_example.py",
                                m_configCall="minimalJetTrimming(tool, 'AntiKt10LCTopoJets2', 0.2, 0.05)",
                                m_name="buildtrimm" ))

# prune the  AntiKt10LCTopoJets  we just build :
c.setalg("JetRecToolAlgo", dict(m_configScript="JSSTutorial/xAHjetconfig_example.py",
                                m_configCall="minimalJetPruning(tool, 'AntiKt10LCTopoJets2', 0.1, 0.5)",
                                m_name="buildprun" ))


c.setalg("JSSTutorialPythonConfigAlgo", {"m_debug": True,
    "m_name": "JSSTutorialPythonConfigAlgo",
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

