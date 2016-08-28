from xAH_config import xAH_config
    
c = xAH_config()
c.setalg("BasicEventSelection", {"m_name": "test", "m_useMetaData": False, "m_derivationName": "JETM8"})

# *******************************************************
# Preparation for tracks
# Schedule Track-Vertex association
c.setalg("JetExecuteToolAlgo", dict(m_configScript='xAHjetconfig_example.py',                                    
                                    m_className="TrackVertexAssociationTool", m_name="TrkVtxAssoc",
                                    m_configCall="buildJetTrackVtxAssoc(tool)")
         )

# Schedule Track filtering : the standard is to keep loose track. This created a dedicated track container.
c.setalg("JetExecuteToolAlgo", dict(m_configScript='xAHjetconfig_example.py',                                    
                                    m_className="JetTrackSelectionTool", m_name="JetTrkSelec",
                                    m_configCall="buildJetTrackSelection(tool)")
         )

# -------------------------------
# Preparation for Truth
# Schedule truth particle filtering : filter stable, non-neutrino particles into their own container

c.setalg("JetExecuteToolAlgo", dict(m_configScript='xAHjetconfig_example.py',                                    
                                    m_className="CopyTruthJetParticles", m_name="filtertruth",
                                    m_configCall="buildJetInputTruthParticles(tool)")
         )
# *******************************************************



# *******************************************************
# Jet building  
# -------------------------------
# Track jets
c.setalg("JetRecToolAlgo", dict(m_configScript="xAHjetconfig_example.py", 
                                 m_configCall="minimalJetReco(tool, 'AntiKt10PV0TrackJets2')",
                                 m_name="buildTrkJet" ))
# -------------------------------
# trimm the Track jets  we just build :
c.setalg("JetRecToolAlgo", dict(m_configScript="xAHjetconfig_example.py",
                                m_configCall="minimalJetTrimming(tool, 'AntiKt10PV0TrackJets2', 0.2, 0.05)",
                                m_name="buildtrkTrim" ))

# -------------------------------
# rebuild cluster jets, ghost-associating tracks.
#  (read how this is done in minimalJetRecoWithGhosts() from xAHjetconfig_example
c.setalg("JetRecToolAlgo", dict(m_configScript="xAHjetconfig_example.py", 
                                 m_configCall="minimalJetRecoWithGhosts(tool, 'AntiKt10LCTopoJets2')",
                                 m_name="buildLCwithghosts" ))

# -------------------------------
# build truth jets.
c.setalg("JetRecToolAlgo", dict(m_configScript="xAHjetconfig_example.py", 
                                 m_configCall="minimalJetReco(tool, 'AntiKt10TruthJets2');tool.JetModifiers[1].PtMin =40000",
                                 m_name="buildTruth" ))


# *******************************************************
# plot some of the containers we build.
# -------------------------------

c.setalg("IParticleHistsAlgo", dict(m_debug= False,
                                    m_inContainerName= "AntiKt10TruthJets",
                                    m_detailStr= "kinematic",
                                    m_histPrefix="Jet_",
                                    )
         )
c.setalg("IParticleHistsAlgo", dict(m_debug= False,
                                    m_inContainerName= "AntiKt10TruthJets2",
                                    m_detailStr= "kinematic",
                                    m_histPrefix="Jet2_",
                                    )
         )

