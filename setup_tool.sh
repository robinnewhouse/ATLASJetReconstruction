### Setup Tool and Compile: ###
git clone https://github.com/UCATLAS/xAODAnaHelpers.git
cd xAODAnaHelpers

# To freeze the version by the commit
git checkout f88f771

cd ..

# Get the SVN repo of Shower Deconstruction
export USER=`whoami`
svn co svn+ssh://$USER@svn.cern.ch/reps/atlas-dferreir/dferreir/ShowerDeconstruction/tags/ShowerDeconstruction-01-00-00 ShowerDeconstruction
svn co svn+ssh://svn.cern.ch/reps/atlasoff/Reconstruction/Jet/BoostedJetTaggers/tags/BoostedJetTaggers-00-00-21 BoostedJetTaggers

#if you want the subjet calibration:
source subjet_calibration.sh

#setup analysis base version and get packages consistent with that version
rcSetup Base,2.4.9
python xAODAnaHelpers/scripts/checkoutASGtags.py 2.4.9
rc clean
rc find_packages
rc compile


