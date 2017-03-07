Table of Contents
=========================

**[Description](#description)**

**[Quick Setup](#quick-setup)**

**[Tutorial 1 : Native fastjet in ATLAS](#tutorial-1-native-fastjet-in-atlas)**

**[Tutorial 2 : JetRec in ATLAS](#tutorial-2-jetrec-in-atlas)**

**[Tutorial 3 : Advanced JetRec Tools via jobOptions](#tutorial-3-advanced-jetrec-tools-via-joboptions)**

**[Tutorials Explained](#tutorials-explained)**

**[Run an Example on the Grid](#run-an-example-on-the-grid)**

**[ToDo List](#todo-list)**



# Authors : 
These are the main authors and initial developers of this.  If you are following this tutorial to familiarize yourself with jet software in ATLAS, 
then you _should_ have questions, and you should *_ask_* these people for help.

Sam Meehan <samuel.meehan@cern.ch>

Pierre-Antoine Delsart <delsart@in2p3.fr>



Description
=======================
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


Quick Setup
=====================================

If you don't already have [kerberos](http://linux.web.cern.ch/linux/docs/kerberos-access.shtml) authentication set up, do that first -- this is important if you want to develop code from the copy of the code you check out:

```
kinit <username>@CERN.CH
```

To check out and set up the code, simply do:

```
git clone https://:@gitlab.cern.ch:8443/atlas-jetetmiss/JetRecoTutorial.git
cd JetRecoTutorial/
source setup_tool.sh
```

note that the choice of the URL depends on the authentication method you are using.  Since we guided you to set up kerberos, it is the KRB5 url from the above dropdown menu.

## Additionnal steps with AnalysisBase 2.4.18

NOT NEEDED anymore every needed package should be in setup_tool.sh


## Get the test file :
```
setupATLAS
localSetupRucioClients
localSetupPandaClient
voms-proxy-init -voms atlas

rucio get --nrandom 1 mc15_13TeV.361024.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ4W.merge.DAOD_JETM8.e3668_s2576_s2132_r7267_r6282_p2528
```

Tutorial 1 : Native fastjet in ATLAS
=====================================

Being familiar with [fastjet](http://fastjet.fr/) and learning how to use it in your analysis is important, 
especially if you will be working within the JetEtMiss (because your advisor said so)
or because you want to be more creative than the ATLAS tools will allow. Therefore, if you are not 
familiar with this set of tools please follow this tutorial, it will benefit you greatly. To simply execute the tutorial 
(after successfully compiling and getting the test file), run the following command :

```
xAH_run.py --files /afs/cern.ch/work/m/meehan/public/JSSTutorial2016/mc15_13TeV.361024.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ4W.merge.DAOD_JETM8.e3668_s2576_s2132_r7267_r6282_p2528/DAOD_JETM8* \
--nevents 1 \
--config config_Tutorial1.py \
-v \
--submitDir OutputDirectory_Tutorial1_fastjet \
direct
```

note that you will have to point to the local file that you downloaded and not Sam's public directory.  If this does not work,
please try to debug it yourself for about 30 minutes, but then **__email the authorsfor help__**!!!.  If you make it this far, then 
look at the output in a TBrowser and then go ahead and read the source code and see if you can figure out how it works.

```
JSSTutorial/scripts/config_Tutorial1.py
JSSTutorial/JSSTutorial/JSSTutorialAlgo.h
JSSTutorial/Root/JSSTutorialAlgo.cxx
```

Check you understanding of the tutorial by answering these questions :
- How does the command know to execute the `EventLoop` algorithm in `JSSTutorialAlgo.cxx`?
- What are the different components that I need to include in my header files and `cmt/MakeFile.RootCore` to allow me to have
access to fastjet?
- Where is jet finding done?  What kind of jets am I building?  What if I only wanted to examine jets that have a minimum pT of 20 GeV when produced by fastjet?  How could I do that?
- What do I need to do in particular to get access to the fastjet contrib packages (https://fastjet.hepforge.org/contrib/)?  Can you identify an example 
of a fastjet contrib used here?  

Tutorial 2 : JetRec in ATLAS
=====================================
If you are already confident with using native fastjet then you've come to the right place.  The next step to being a jet expert is learning
how to use [JetRec](http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Reconstruction/Jet/JetRec/).  This is the software that is used to perform
official ATLAS reconstruction, and once you come up with an amazing new idea, you will need to implement it here, if the functionality to execute it
does not already exist, before it can be used within your publishable analysis.

To get you started, this tutorial is intended to do *the exact same thing* as Tutorial 1, but using the official ATLAS tools.  Therefore, the task
here is to read the code in parallel with the last tutorial and find the similarities and differences.  But first, execute this tutorial
with the following command:

```
xAH_run.py --files /afs/cern.ch/work/m/meehan/public/JSSTutorial2016/mc15_13TeV.361024.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ4W.merge.DAOD_JETM8.e3668_s2576_s2132_r7267_r6282_p2528/DAOD_JETM8* \
--nevents 1 \
--config config_Tutorial2.py \
-v \
--submitDir OutputDirectory_Tutorial2_JetRec \
direct
```

note that you will have to point to the local file that you downloaded and not Sam's public directory.  Next, go 
ahead and read the source code and see if you can figure out how it works.

```
JSSTutorial/scripts/config_Tutorial2_JetRec.py
JSSTutorial/JSSTutorial/JSSTutorialJetRecAlgo.h
JSSTutorial/Root/JSSTutorialJetRecAlgo.cxx
```


Check you understanding of the tutorial by answering these questions
(you might find help in the "Tutorials explained" section below):

- What are the ATLAS jet classes used in this tutorial ? 
- Where are they used in the `execute()` step ? 
- What header files need to be included ? What dependency do they
  imply (check `cmt/Makefile.RootCore`)
- How did you "get the clusters" from the `xAOD::CalCaloTopoCluster` container this time?
- More generally, please take 10-15 minutes to make the correspondences and draw analogs between __Tutorial 1__ and this one
   * how/where are clusters retrieved & prepared for fastjet ?
   * how/where is fastjet called ?
   * how/where are substructure variables calculated ?
- Compare how the final jets and the substrucure info is retrieved
  here wit respect to Tutorial 1.
- How would you add a new variable calculation here ?




Tutorial 3 : Advanced JetRec Tools via jobOptions
=====================================
Now we want maximal flexibility in our ability to perform jet finding in an ATLAS official way. For this we want to do all
the configuration of the same jet tools we used in Tutorial 2, but this time within python. This will allow us to change *any* 
`JetRec` parameters without re-compiling our analysis and to easily configure many jet algorithms at once. This also has the
advantage of being very close to how configuration works in Athena and how jets reconstruction is performed at Tier0. 

Let's run it : 

```
xAH_run.py --files /afs/cern.ch/work/m/meehan/public/JSSTutorial2016/mc15_13TeV.361024.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ4W.merge.DAOD_JETM8.e3668_s2576_s2132_r7267_r6282_p2528/DAOD_JETM8* \
--nevents 1 \
--config config_Tutorial3.py \
-v \
--submitDir OutputDirectory_Tutorial3_JetRecJobOptions \
direct
```

The source code is here 

```
JSSTutorial/scripts/config_Tutorial3_JetRec.py
JSSTutorial/JSSTutorial/JSSTutorialPythonConfigAlgo.h
JSSTutorial/Root/JSSTutorialPythonConfigAlgo.cxx
```

Check you understanding of the tutorial by answering these questions
(you might find help in the [Tutorials Explained](#tutorials-explained) section below):

* Do you see any jet reconstruction related code in
  `JSSTutorialPythonConfigAlgo.cxx` ? 
* Can you find where the atlas jet tools are configured ? Can you make
  the correspondence with the Tutorial 2 ?
* How would you add an other attribute calculation to the trimmed jets?
* Can you locate the configuration functions? If so, try to play with the configuration functions to change the
  jet finding parameters like: radius, finding algorithm, or input type.





Tutorials Explained
=====================================

## Jet Reconstruction Overview
In Atlas the jet reco code works in 3 distinct steps, which can be seen in the code for [Tutorial 2 : JetRec in ATLAS](#tutorial-2-jetrec-in-atlas) :
1. Prepare input to jet finding. This means translating the
 clusters/tracks/truth particles from the xAOD format to the fastjet
 format. This is done via the `PseudoJetGetter` tool like 
 [in Tutorial 2 here](https://gitlab.cern.ch/JetSubstructure/Tutorial_HCW2016/blob/master/JSSTutorial/Root/JSSTutorialJetToolsAlgo.cxx#L184). 
 In this case, this is stored and passed to jet finding via a `GetterArray` so that multiple types of inputs can be used for the same type of jet finding.
2. Call fastjet using a `JetRecTool` using the input from step 1 and translate final jets from
fastjet to xAOD::Jet [like you can see here](https://gitlab.cern.ch/JetSubstructure/Tutorial_HCW2016/blob/master/JSSTutorial/Root/JSSTutorialJetToolsAlgo.cxx#L267).
3. Calculate additional quantities on final jets. Ex: calibration,
filtering, sorting, substructure quantities.  This is done via a number of `JetModifier`'s that are 
[set up before jet finding](https://gitlab.cern.ch/JetSubstructure/Tutorial_HCW2016/blob/master/JSSTutorial/Root/JSSTutorialJetToolsAlgo.cxx#L231)
and then passed to the JetRecoTool that will build and modify that jet collection [during jet finding](https://gitlab.cern.ch/JetSubstructure/Tutorial_HCW2016/blob/master/JSSTutorial/Root/JSSTutorialJetToolsAlgo.cxx#L271).

Each step are performed by dedicated dual-use tools (i.e tools which
run in `Athena` and `RootCore`).

1. `PseudoJetGetter` ([LXR Link](http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Reconstruction/Jet/JetRec/JetRec/PseudoJetGetter.h)) and 
all of the inheritted classes **(PA to provide examples.)**
2. `JetFinder` ([LXR Link](http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Reconstruction/Jet/JetRec/JetRec/JetFinder.h)) or in the case of trimming, `JetTrimmer`
3. Many tools, all inheriting from `IJetModifier` ([LXR Link](http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Reconstruction/Jet/JetInterface/JetInterface/IJetModifier.h))
an example of which is the tool used to calculate N-subjettiness ([LXR Link](http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Reconstruction/Jet/JetSubStructureMomentTools/JetSubStructureMomentTools/NSubjettinessTool.h)).

All these tools are driven by a top-level tool : `JetRecTool`. This last
tool runs the 3 steps and records the final `JetContainer` in the evt store such
that it can be used thereafter in the event analysis.

In summary, running a jet alg requires to configure tools for the 3 steps
and to associate them to a `JetRecTool`. **(PA do we even need this summary statement?)**


## Jet Reconstruction in This Package
(*PA This portion gets a little abstract for me.  Can it be made a bit more concrete? Or maybe we just put a "WARNING : Proceed only if you think you want to learn the nitty gritty!" statement*)
In this package a jet building procedure is done by a `JetRecToolAlgo` (inheriting from
`xAH::Algorithm` which inherits from `EL::Algorithm`).
This package holds a single instance of a `JetRecTool` and will call
it's `execute()` method once per event.

The configuration is done through python scripts which contain
configuration instructions. These scripts are interpreted during
`JetRecToolAlgo::initialize()`. 

During the `initialize()` method, `JetRecToolAlgo` passes its `JetRecTool` instance into
the python interpreter under the name "_tool_". The `JetRecToolAlgo` algorithm then
tells the interpreter to execute a user given python script and a user
function call. And, since (1) the user has written python instructions to
configure the variable "_tool_" in this script and function and (2) "_tool_" in the 
python interpreter really is the `JetRecTool` c++ instance, it is effectively configured "in c++" too. 

So, in the simplest scenario, the user writes a python script (say
myjetscript.py) which sets the desired properties to "tool". ex :

```
python
tool.OutputContainer = "MyJetContainer"
tool.JetFinder = JetFinder("AntiKt12", Radius=0.12, ... )
tool.PseudoJetGetters = [ ... ] 
```  

The user then configures a JetRecToolAlgo to use myjetscript.py (by
setting its m_configScript member). Nothing else is needed : on each
event JetRecToolAlgo will call the configured JetRecTool and thus make
its result available in the evt store.

However when running with several jet algs in the same job, it will be
painfull to maintain a version of myjetscript.py for each jet
alg. An other workflow is to define configuration functions in the
script and then configure JetRecToolAlgo to also call one of this
function. Ex :
```python
  def configMyJet(tool, R):
      # configure tool to run a jet alg with radius R
      tool.JetFinder = JetFinder("somname", Radius = R, ...)
      ... other configs ....
```  

Then all JetRecToolAlgo instance will use the same m_configScript, but
they can have different m_configCall to configure various jet
algs. Typically : `m_configCall="configMyJet(tool, 0.9)"`


## Answers

### Tutorial 1
- How does the command know to execute the EventLoop algorithm in
```JSSTutorialAlgo.cxx```?
  * in the driving script config_Tutorial1.py, we add an giving the
  class name : `c.setalg("JSSTutorialAlgo",...)`

- What are the different components that I need to include in my header files and ```cmt/MakeFile.RootCore``` to allow me to have
access to fastjet?
   * see all the `#include "fastjet/XXX.h"` in the .h files and the
   PACKAGE_DEP line in cmt/Makefile.RootCore must contain Asg_FastJet
   
- Where is jet finding done?  What kind of jets are being built? What if I only wanted to examine jets
that have a minimum pT of 20 GeV when produced by fastjet?  How could
I do that?
  * jet finding call : `fastjet::ClusterSequence(jet_inputs,
  jet_def);`, Anti-Kt R=1.0 jets trimmed with Rsub=0.2 and fcut=0.05, change 0.0 in 
  `fastjet::sorted_by_pt(clust_seq.inclusive_jets(0.0) )`  to 20000.

- What do I need to do in particular to get access to the fastjet
contrib packages (https://fastjet.hepforge.org/contrib/)?  Can you
identify an example of a fastjet contrib used here?  
  * fastjet-contrib is included in the Asg_FastJet package. Make sure
  you have the latest one. Then you need to add links instructions on
  PACKAGE_LDFLAGS in the Makefile.RootCore. Example : EnergyCorrelator


### Tutorial 2

- Which are the Atlas jet classes used in this tutorial ? 
  * all the classes included from JetRec/ JetSubStructureMomentTools/
  and JetCalibTools/ there are > 10 such classes.
- Where are they used in the execute() step ? 
  * within  `JetRecTool::execute()` : this driving tool is in charge
  of calling all its sub-tools in the right sequence.
- What header files need to be included ? What dependency do they
  impliy (check Makefile.RootCore)
  * JetRec JetSubStructureMomentTools   JetCalibTools, add them to the
  PACKAGE_DEP line in Makefile.RootCore
- How did you "get the clusters" from the xAOD::CalCaloTopoCluster
container this time?
  * this is the job of the PseudoJetGetter tool (see the initialize()
  part). The actual retieval and conversion to fastjet::PseudoJet is
  done within `JetRecTool::execute()`. 
- More generally make the correspondances between Tutorial1 and this one
   * how/where clusters are retrieved & prepared for fastjet ?
     + PseudoJetGetter
   * how/where is fastjet called ?
     + By the JetFinderTool called within  `JetRecTool::execute()`. 
   * how/where substructure variables are calculated ?
     + by the various JetModifierTools
- Compare how the final jets and the substrucure info is retrieved
  here w.r.t tutorial 1.
  * in tutorial 1 we use directly the output final
  fastjet::PseudoJet. Here we retrieve the xAOD::JetContainer produced
  by the JetRecTool
- How would you add a new variable calculation here ?
  * Instanciate a new JetModifier (ex: JetWidthTool), configure it,
  and append it to the list of modifiers : ` modArray.push_back(ToolHandle<IJetModifier>(myTool) )`


### Tutorial 3
* Do you see any jet reconstruction related code in
  JSSTutorialPythonConfigAlgo.cxx ? 
  - There is none, only a call to the evt store to retrieve the final
  jets. Everything is done in other algs scheduled before the JSSTutorialPythonConfigAlgo.
* Can you find where the atlas jet tools are configured ? Can you make
  the correspondance with the tutorial 2 ?
  - all is done in JSSTutorial/scripts/xAHjetconfig_example.py, see
  the simpleJetConfig() function, there's almost 1-by-1 correspondence
  with the c++ config.
* how would you add an other attribute calculation to the trimmed jets
  ?
* Try to play with the configuration functions (can you locate them ?) to change the
  parameters : radius, alg, or input type.


Run an Example on the Grid
=====================================
Hopefully you have the basic (and advanced) tools to start being creative with jets.  So now you want to run over all of the dijet samples
(DSID 361020-361032 [here](https://svnweb.cern.ch/trac/atlasoff/browser/Generators/MC15JobOptions/trunk/share/DSID361xxx)).  Outlined here is
how you can do that for Tutorial 3 above.

Open a fresh session, go to the working directory and setup the environment for the grid:

```
source grid_env.sh
```

Next, make a file called `sample_list_for_grid.list` which contains a newline delimited list of samples that you want to run over like

```
mc15_13TeV.361020.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ0W.merge.DAOD_JETM8.e3569_s2576_s2132_r7725_r7676_p2794  
mc15_13TeV.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.merge.DAOD_JETM8.e3569_s2576_s2132_r7725_r7676_p2794  
mc15_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.merge.DAOD_JETM8.e3668_s2576_s2132_r7725_r7676_p2794  
mc15_13TeV.361023.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ3W.merge.DAOD_JETM8.e3668_s2576_s2132_r7725_r7676_p2794  
mc15_13TeV.361024.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ4W.merge.DAOD_JETM8.e3668_s2576_s2132_r7725_r7676_p2794  
mc15_13TeV.361025.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ5W.merge.DAOD_JETM8.e3668_s2576_s2132_r7725_r7676_p2794  
mc15_13TeV.361026.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ6W.merge.DAOD_JETM8.e3569_s2608_s2183_r7725_r7676_p2794  
mc15_13TeV.361027.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ7W.merge.DAOD_JETM8.e3668_s2608_s2183_r7725_r7676_p2794  
mc15_13TeV.361028.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ8W.merge.DAOD_JETM8.e3569_s2576_s2132_r7772_r7676_p2794  
mc15_13TeV.361029.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ9W.merge.DAOD_JETM8.e3569_s2576_s2132_r7772_r7676_p2794  
mc15_13TeV.361030.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ10W.merge.DAOD_JETM8.e3569_s2576_s2132_r7772_r7676_p2794 
mc15_13TeV.361031.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ11W.merge.DAOD_JETM8.e3569_s2608_s2183_r7772_r7676_p2794 
mc15_13TeV.361032.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ12W.merge.DAOD_JETM8.e3668_s2608_s2183_r7772_r7676_p2794 
```

and then run the following command which will submit the analysis code in the Tutorial 3 example to run over those dataset containers.

```
xAH_run.py --files sample_list_for_grid.list --inputList --nevents 5 --config config_Tutorial3.py -f --submitDir OutputDirectory_Tutorial3_JetRec --inputDQ2 prun --optGridExpress 1 --optGridNFilesPerJob 1 --optSubmitFlags='--nFiles=1 --useNewCode'
```

*NOTE* : It is very important to note that when using this framework, all of the files intended to be
sent to the grid (e.g. python configuration files) must be part of a RootCore package in your local space.
For instance, if your python configuration uses the same style as config_Tutorial3.py, that is calling configuration 
functions defined in another python script, the python script containing the function definitions should be stored 
in the `<package-name>/scripts` directory and this same path should be used in the main python config file:

```
m_configScript="JSSTutorial/xAHjetconfig_example.py" 
```

which means the system will search for the script 'xAHjetconfig_example.py' within the `scripts/` directory of the `JSSTutorial` package.




ToDo List 
=====================================
The list below are additions and modifications to this tutorial that would benefit users.  Contributing to the incorporation of examples of these
capabilities in this framework is OTP-worthy, so please contact the authors if you are interested.
- [JET] how to write a simple modifier themselves
- [JET] playing with variable-R jet reconstruction
- [MET] example of how to use the custom jets output from the example to create a new set of MET maps and then use these for MET rebuilding

