from BoostedJetTaggers.BoostedJetTaggersConf import STTHelpers__TopTagSmoothMassCut, STTHelpers__TopTagSmoothTau32Cut , SubstructureTopTagger
from JetRec.JetRecStandardToolManager import jtm

jtm += STTHelpers__TopTagSmoothTau32Cut("smoothTau32TopTag50",
                                        PtValues=[250000.000,325000.000,375000.000,425000.000,475000.000,525000.000,575000.000,625000.000,675000.000,725000.000,775000.000,850000.000,950000.000,1100000.000,1300000.000,1680000.000 ],
                                        Tau32Values= [0.773,0.713,0.672,0.637,0.610,0.591,0.579,0.574,0.573,0.574,0.576,0.578,0.580,0.580,0.577,0.571] )

jtm += STTHelpers__TopTagSmoothTau32Cut("smoothTau32TopTag80",
                                        PtValues=[250000.000,325000.000,375000.000,425000.000,475000.000,525000.000,575000.000,625000.000,675000.000,725000.000,775000.000,850000.000,950000.000,1100000.000,1300000.000,1680000.000 ],
                                        Tau32Values= [0.879,0.831,0.799,0.770,0.746,0.727,0.714,0.706,0.701,0.698,0.698,0.699,0.700,0.701,0.699,0.696] )


jtm += STTHelpers__TopTagSmoothMassCut("smoothMassTopTag50",
                                       PtValues=[250000.000,325000.000,375000.000,425000.000,475000.000,525000.000,575000.000,625000.000,675000.000,725000.000,775000.000,850000.000,950000.000,1100000.000,1300000.000,1680000.000 ],
                                       MassValues= [85052.983,98705.422,107807.048,115186.721,120365.410,123510.000,125010.000,125662.377,126075.960,126389.113,126537.840,126803.137,127322.903,128379.386,130241.032,133778.159] )


jtm += STTHelpers__TopTagSmoothMassCut("smoothMassTopTag80",
                                       PtValues=[250000.000,325000.000,375000.000,425000.000,475000.000,525000.000,575000.000,625000.000,675000.000,725000.000,775000.000,850000.000,950000.000,1100000.000,1300000.000,1680000.000 ],
                                       MassValues= [67888.967,72014.026,74764.066,76769.667,78354.344,79170.000,79530.000,80158.525,81195.851,82779.245,84890.965,88747.162,94262.629,102710.787,113868.253,135067.438  ] )




# the recommended Taggers :
jtm += SubstructureTopTagger("substrucTopTagger50",
                             TaggerName = "SubTopTag50",
                             SelectionTools =  [jtm.smoothTau32TopTag50 , jtm.smoothMassTopTag50 ],
                             OutputLevel = 2,
                             )


jtm += SubstructureTopTagger("substrucTopTagger80",
                             TaggerName = "SubTopTag80",
                             SelectionTools = [jtm.smoothTau32TopTag80 , jtm.smoothMassTopTag80 ]
                             )
