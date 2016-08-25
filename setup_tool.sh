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
cp ../JSSTutorial/scripts/config_Tutorial*.py .
