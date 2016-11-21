
import ROOT
from JSSTutorial.JetRecConfig import buildJetTrackVtxAssoc , buildJetTrackSelection, buildJetInputTruthParticles

from JSSTutorial.JetRecConfig import jetConfig
from JSSTutorial.JetConstitSeqConfig import constitConfig



def addClusterPseudoJetGetter(name):
    from ROOT import PseudoJetGetter

    # this is a helper function to automatically declare a
    # PseudoJetGetter for input clusters named = 'AbCdTopoClusters'
    # and record it as 'AbCd'
    keyword = name[:name.index('TopoClusters')]
    jetConfig.knownInputTools[keyword] = (PseudoJetGetter, dict(InputContainer=name,Label="LCTopo",SkipNegativeEnergy=True,  ) ) 
    

#xAH_run.py --files ../../Tutorial_HCW2016_18/run/HCWDAOD_JETM8.output.pool.root  --nevents 1  --config config_Tutorial5.py -v --submitDir OutputDirectory_Tutorial5_JetRec 
