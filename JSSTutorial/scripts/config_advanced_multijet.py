from xAH_config import xAH_config
import ROOT

c = xAH_config()
c.setalg("BasicEventSelection", {"m_name": "test", "m_useMetaData": False, "m_derivationName": "JETM8"})

# *******************************************************
# Preparation for tracks
# Schedule Track-Vertex association
c.setalg("JetExecuteToolAlgo", dict(m_configScript='xAHjetconfig_example.py',                                    
                                    m_className="TrackVertexAssociationTool", m_name="TrkVtxAssoc",
                                    m_configCall="buildJetTrackVtxAssoc(tool)")
         )

# Schedule Track filtering : the standard is to keep loose track. This creates a dedicated track container.
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
                                m_configCall="jetConfig.jetFindingSequence('AntiKt10PV0Track',outputName='AntiKt10PV0TrackJets2',jetTool=tool)",
                                m_name="buildTrkJet" ))

# -------------------------------
# trimm the Track jets  we just build :
c.setalg("JetRecToolAlgo", dict(m_configScript="xAHjetconfig_example.py",
                                m_configCall="jetConfig.jetGroomingSequence('AntiKt10PV0TrackJets2', 'Trim', modifierList='cut50+substr',jetTool=tool)",
                                m_name="buildtrkTrim" ))

# -------------------------------
# rebuild cluster jets, ghost-associating tracks.
c.setalg("JetRecToolAlgo", dict(m_configScript="xAHjetconfig_example.py", 
                                 m_configCall="jetConfig.jetFindingSequence('AntiKt10LCTopo_full',outputName='AntiKt10LCTopoJets2', jetTool=tool)",
                                 m_name="buildfull" ))

# -------------------------------
# trimm the LCTopo jets  we just build :
c.setalg("JetRecToolAlgo", dict(m_configScript="xAHjetconfig_example.py",
                                m_configCall="jetConfig.jetGroomingSequence('AntiKt10LCTopoJets2', 'Trim', modifierList='cut50+substr',jetTool=tool)",
                                m_name="buildlctopTrim" ))

# -------------------------------
# build truth jets.
c.setalg("JetRecToolAlgo", dict(m_configScript="xAHjetconfig_example.py", 
                                m_configCall="jetConfig.jetFindingSequence('AntiKt10Truth', jetTool=tool,outputName='AntiKt10TruthJets2')",
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

# *******************************************************
# Write out a Mini xAOD
# -------------------------------

# prepare a vector of string.
selectedVars = ROOT.vector('string')()
containers = [ "AntiKt10LCTopoJets","AntiKt10TruthJets" , ] 
containers2 = [jc+"2" for jc in containers] + [ "AntiKt10PV0TrackJets2"]
containers += [ "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets", "AntiKt10PV0TrackTrimmedPtFrac5SmallR20Jets"]
containers2 += ["AntiKt10LCTopoTrimPtFrac5SmallR2Jets2","AntiKt10PV0TrackTrimPtFrac5SmallR2Jets2"]

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
                             m_selectedAuxVars= selectedVars,
                             #m_deepCopyKeys
                             )
         )

