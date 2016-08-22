from xAH_config import xAH_config
    
c = xAH_config()
c.setalg("BasicEventSelection", {"m_name": "test", "m_useMetaData": False, "m_derivationName": "JETM8"})

# -------------------------------
# re-build a AntiKt10LCTopoJets : 
c.setalg("JetRecToolAlgo", dict(m_configScript="xAHjetconfig_example.py", 
                                 m_configCall="simpleJetConfig(tool, 'AntiKt10LCTopoJets2')",
                                 m_name="buildalg" ))
# trimm the  AntiKt10LCTopoJets  we just build :
c.setalg("JetRecToolAlgo", dict(m_configScript="xAHjetconfig_example.py",
                                m_configCall="minimalJetTrimming(tool, 'AntiKt10LCTopoJets2', 0.2, 0.05)",
                                m_name="buildtrimm" ))


# -------------------------------
# plot containers we build.
c.setalg("IParticleHistsAlgo", dict(m_debug= False,
                                    m_inContainerName= "AntiKt10LCTopoJets2",
                                    m_detailStr= "kinematic",
                                    m_histPrefix="Jet_",
                                    )
         )

c.setalg("IParticleHistsAlgo", dict(m_debug= False,
                                    m_inContainerName= "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets2",
                                    m_detailStr= "kinematic",
                                    m_histPrefix="Jet2_",
                                    ) )

