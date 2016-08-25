# Authors : 
Sam Meehan <samuel.meehan@cern.ch>

# Description
This package is intended to be used to perform jet reconstruction
starting from a DAOD with clusters.  It should be transparent, so that
a newcomer has examples and a good starting point for working on an
interesting topic. If this is not the case, please consider saying
something! 

The code is based on
[xAODAnaHelpers](https://github.com/UCATLAS/xAODAnaHelpers) with a
link to the example that this package is based on found
[here](https://github.com/UCATLAS/ASG_AnalysisFrameworkReview). 
xAODAnaHelpers is essentially a set of scripts and a few convenience
c++ classes based on the official EventLoop framework.

## Tutorial organisation

This tutorial shows how to run jet finding, grooming and calculating
substructure.
This task is done in 3 different ways giving identical results.

1. JSSTutorialAlgo implements the task using only C++ and calls to
fastjet. It is straightforward readable but rather monolithic.
2. JSSTutorialJetToolsAlgo implements the task using the Atlas JetRec
C++ classes. It shows how to configure these various classes and put
them together to perform the tasks. Most of the complexity is moved to
the initialization step, but then the execution code becomes much
simple :just a call to the 'excecute()' method of the top-level tools.
3. JSSTutorialPythonConfigAlgo implements the task using the same
Atlas JetRec classes as in 2, but configuring them entireley from
python scripts. This offers maximal flexibility and good readability
at the cost of abstraction and knowledge of the Atlas classes.

## basics on EventLoop and xAODAnaHelpers
In EventLoop, tasks are implemented in c++ classes inheriting
EL::Algorithm.

# Tutorial implementation in pure c++ 

# Using the Atlas JetRec classes

## Jet Reconstruction overview
In Atlas the jet reco code works in 3 distinct steps :
1. Prepare input to jet finding. This means translating the
 clusters/tracks/truth particles from the xAOD format to the fastjet
 format. 
2. Call fastjet using the input from 1. Translate final jets from
fastjet to xAOD::Jet.
3. Calculate additional quantities on final jets. Ex: calibration,
filtering, sorting, substructure quantities.

Each step are performed by dedicated dual-use tools (i.e tools which
run in Athena and RootCore).

1. PseudoJetGetter (and inherited)
2. JetFinder (or JetTrimmer in the trimming case)
3. Many tools, all inheriting IJetModifier : we call them
JetModifiers.

All these tools are driven by a top-level tool : JetRecTool. This last
tool runs the 3 steps and record the final JetContainer in the evt store.

Running a jet alg requires to configure tools for the 3 steps
and to associate them to a JetRecTool.

## Tutorial implementation with JetRec, C++ only

## Tutorial implementation with JetRec, python config


### Jet Reconstruction in this package
Here a jet building procedure is done by a JetRecToolAlgo (inherits
xAH::Algorithm which inherits EL::Algorithm).
JetRecToolAlgo holds a single instance of a JetRecTool and will call
it's execute() method once per event.

The configuration is done through python scripts : during
initialize(), JetRecToolAlgo will pass its JetRecTool instance into
the python interpreter under the name "tool". It will then execute
user given python script and function call. This script and function call
are then expected to configure "tool" as required.

In the simplest scenario, the user writes a python script (say
myjetscript.py) which sets the desired properties to "tool". ex :
```  
  tool.OutputContainer = "MyJetContainer"
  tool.JetFinder = JetFinder("AntiKt12", Radius=0.12, ... )
  tool.PseudoJetGetters = [ ... ] 
```  
The user then configures a JetRecToolAlgo to use myjetscript.py (by
setting its m_configScript member). Nothing else is needed.

However when running with several jet algs in the same job, it will be
painfull to maintain a version of myjetscript.py for each jet
alg. An other workflow is to define configuration functions in the
script and then configure JetRecToolAlgo to also call one of this
function. Ex :
```  
  def configMyJet(tool, R):
      # configure tool to run a jet alg with radius R
      tool.JetFinder = JetFinder("somname", Radius = R, ...)
      ... other configs ....
```  

Then all JetRecToolAlgo instance will use the same m_configScript, but
they can have different m_configCall to configure various jet
algs. Typically : m_configCall="configMyJet(tool, 0.9)"

## Jet reco example



## Quick Setup :

If you don't already have [kerberos](http://linux.web.cern.ch/linux/docs/kerberos-access.shtml) authentication set up, do that first -- this is important if you want to develop code from the copy of the code you check out:

```
kinit <username>@CERN.CH
```

To check out and set up the code, simply do:

```
git clone https://:@gitlab.cern.ch:8443/JetSubstructure/Tutorial_HCW2016.git
cd Tutorial_HCW2016/
source setup_tool.sh
```

note that the choice of the URL depends on the authentication method you are using.  Since we guided you to set up kerberos, it is the KRB5 url from the above dropdown menu.

## Get the test file :
```
setupATLAS
localSetupRucioClients
localSetupPandaClient
voms-proxy-init -voms atlas

rucio get --nrandom 1 mc15_13TeV.361024.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ4W.merge.DAOD_JETM8.e3668_s2576_s2132_r7267_r6282_p2528
```

## Run locally
You will first need to modify the ```Run_local.sh``` script to point to the directory where you downloaded the file from the grid previously

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

