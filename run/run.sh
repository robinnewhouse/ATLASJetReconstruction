source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh

xAH_run.py --files data/DAOD_EXOT7.08607211._000001.pool.root.1  --config config_DNN_Test.py -v --submitDir output/OutputDirectory_DNN_Test_JetRecJobOptions direct
rm -rf output/OutputDirectory_DNN_Test_JetRecJobOptions
