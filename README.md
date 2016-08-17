# Authors : 
Sam Meehan <samuel.meehan@cern.ch>

# Description
This package is intended to be used to perform jet reconstruction starting from a DAOD with clusters.  It should be transparent, so that a newcomer has examples and a good starting point for working on an interesting topic. If this is not the case, please consider saying something!

The code is based on [xAODAnaHelpers](https://github.com/UCATLAS/xAODAnaHelpers) with a link to the example that this package is based on found [here](https://github.com/UCATLAS/ASG_AnalysisFrameworkReview).


## Quick Setup :

If you don't already have [kerberos](http://linux.web.cern.ch/linux/docs/kerberos-access.shtml) authentication set up, do that first -- this is important if you want to develop code from the copy of the code you check out:

```
kinit <username>@CERN.CH
```

To check out and set up the code, simply do:

```
git clone https://gitlab.cern.ch/JetSubstructure/Tutorial_HCW2016.git
cd Tutorial_HCW2016/
source setup_tool.sh
```

## Get the test file :
```
setupATLAS
localSetupRucioClients
localSetupPandaClient
voms-proxy-init -voms atlas

rucio get mc15_13TeV.301256.Pythia8EvtGen_A14NNPDF23LO_Wprime_WZqqqq_m800.merge.DAOD_JETM8.e3743_s2608_s2183_r7772_r7676_p2613
```

## Run locally
```
source Run_local.sh
```

## Run an example on the grid :
NOT TESTED

Open a fresh session, go to the working directory and setup the environment for the grid:
```
source grid_env.sh
```

Run an example on the grid by:
```
source Run_grid_zprime_list.sh
```

## ToDo List For Future:
- Make the jet grooming in JetRec
- Make jet moment calculation in JetRec using JetSubstructureUtils but keep one example with fastjet directly and/or fastjet/contrib so that someone can see how to use that
- **new xAH algorithm** for calculation of MET
- **new xAH algorithm** to show how to do jet input modifications
