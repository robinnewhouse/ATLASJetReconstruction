### Setup Tool and Compile: ###


git clone https://github.com/UCATLAS/xAODAnaHelpers.git
cd xAODAnaHelpers

# To freeze the version by the commit
git checkout f88f771

cd ..

# Get the SVN repo of Shower Deconstruction
export USER=`whoami`
svn co svn+ssh://$USER@svn.cern.ch/reps/atlas-dferreir/dferreir/ShowerDeconstruction/tags/ShowerDeconstruction-01-00-00 ShowerDeconstruction
svn co svn+ssh://svn.cern.ch/reps/atlasoff/Reconstruction/Jet/BoostedJetTaggers/trunk BoostedJetTaggers

# trunk of these packages needed in 2.4.18 
svn co svn+ssh://svn.cern.ch/reps/atlasoff/Reconstruction/Jet/JetMomentTools/tags/JetMomentTools-00-03-42 JetMomentTools
svn co svn+ssh://svn.cern.ch/reps/atlasoff/PhysicsAnalysis/AnalysisCommon/ParticleJetTools/trunk ParticleJetTools
svn co svn+ssh://svn.cern.ch/reps/atlasoff/Reconstruction/Jet/JetSubStructureMomentTools/trunk JetSubStructureMomentTools
svn co svn+ssh://svn.cern.ch/reps/atlasoff/Reconstruction/Jet/JetRec/tags/JetRec-03-00-83 JetRec
svn co svn+ssh://svn.cern.ch/reps/atlasoff/Reconstruction/PFlow/PFlowUtils/tags/PFlowUtils-00-00-08 PFlowUtils
svn co svn+ssh://svn.cern.ch/reps/atlasoff/Reconstruction/Jet/JetInterface/trunk JetInterface
svn co svn+ssh://svn.cern.ch/reps/atlasoff/Reconstruction/Jet/JetRecTools/tags/JetRecTools-00-03-48 JetRecTools
svn co svn+ssh://svn.cern.ch/reps/atlasoff/AsgExternal/Asg_FastJet/trunk Asg_FastJet
# This is huge !! hopefully not needed with 2.4.19 !

patch -p0 -i utils/JetMomentTools_patch_2.4.18.txt
patch -p0 -i utils/JetRecTools_patch_2.4.18.txt
patch -p0 -i utils/PFlowUtils_patch_2.4.18.txt

#if you want the subjet calibration:
# source subjet_calibration.sh

#setup analysis base version and get packages consistent with that version
setupATLAS
Release=2.4.18
rcSetup Base,$Release
python xAODAnaHelpers/scripts/checkoutASGtags.py $Release

rc clean
rc find_packages
rc compile

mkdir run
cd run
ln -s ../JSSTutorial/scripts/config_Tutorial*.py .
ln -s ../JSSTutorial/scripts/xAHjetconfig_example.py .
