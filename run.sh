cd run
xAH_run.py --files data/mc15_13TeV.301329.Pythia8EvtGen_A14NNPDF23LO_zprime2000_tt.merge.DAOD_JETM6.e4061_s2608_s2183_r7772_r7676_p2949/DAOD_JETM6.10517604._000014.pool.root.1  --nevents 10 --config config_DNN_Test.py -v --submitDir output/OutputDirectory_DNN_Test_JetRecJobOptions direct
rm -rf output/OutputDirectory_DNN_Test_JetRecJobOptions
cd ..

