#remove the directory where you previously output your test to
rm -r OutputDirectory

#run your new test
#xAH_run.py --files /afs/cern.ch/work/m/meehan/public/HCW2016Tutorial/InputFile/mc15_13TeV.301256.Pythia8EvtGen_A14NNPDF23LO_Wprime_WZqqqq_m800.merge.DAOD_JETM8.e3743_s2608_s2183_r7772_r7676_p2613/DAOD_JETM8* \

xAH_run.py --files /afs/cern.ch/work/m/meehan/public/JSSTutorial2016/mc15_13TeV.361024.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ4W.merge.DAOD_JETM8.e3668_s2576_s2132_r7267_r6282_p2528/DAOD_JETM8* \
--nevents 1 \
--config config_example.py \
-v \
--submitDir OutputDirectory \
direct
