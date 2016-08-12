
#####################################

Authors : 
Sam Meehan <samuel.meehan@cern.ch>

#######################################
This document walks you through the steps of:
- Downloading the package which consists of the xAODAnaHelpers and TopWBosonSubstructure

xAODAnaHelpers: https://github.com/UCATLAS/xAODAnaHelpers
Link to the example that this package is based on: https://github.com/UCATLAS/ASG_AnalysisFrameworkReview

#######################################
Quick Setup :

git clone https://:@gitlab.cern.ch:8443/meehan/HCW2016Tutorial.git
cd HCW2016Tutorial
source setup_tool.sh

########################################
Get the test file :
setupATLAS
localSetupRucioClients
localSetupPandaClient
voms-proxy-init -voms atlas

rucio get mc15_13TeV.301256.Pythia8EvtGen_A14NNPDF23LO_Wprime_WZqqqq_m800.merge.DAOD_JETM8.e3743_s2608_s2183_r7772_r7676_p2613

#######################################
Run locally
source Run_local.sh

#######################################
Run an example on the grid :
NOT TESTED

# Open a fresh session, go to the working directory and setup the environment for the grid:
source grid_env.sh

# Run an example on the grid by:
source Run_grid_zprime_list.sh

########################################

