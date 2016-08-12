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

# c.setalg("TopWBosonSubstructureAlgo", {"m_debug": False,
#     "m_name": "TopWBosonSubstructureAlgo",
#     "m_MyNewVariable" : "ThisGotLoadedIn",
#     "m_TreeName" : "JetTree"
# })

# You can add new parameters to your algorithm that will be loaded automatically.  
# Note that they will be loaded into the algorithms member variable that has the same name, only if it is public.
# The example above it the "m_MyNewVariable"