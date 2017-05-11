from xAH_config import xAH_config
from ROOT import vector 
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

c.setalg("JSSTutorialAlgo", {"m_debug": False,
    "m_name": "JSSTutorialAlgo",
    "m_MyNewVariable" : "ThisGotLoadedIn",
    "m_TreeName" : "JetTree"
})



