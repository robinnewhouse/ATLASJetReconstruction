### Setup Tool and Compile: ###


export USER=`whoami`

if [ "$1" != "" ] ; then 
    export USER=$1
fi

echo "USER is " $USER


git clone https://github.com/UCATLAS/xAODAnaHelpers.git
cd xAODAnaHelpers

# To freeze the version by the commit
git checkout 9ab194fb180d208767c353536876859293c62788  
cd ..


# Get the SVN repo of Shower Deconstruction
svn co svn+ssh://$USER@svn.cern.ch/reps/atlas-dferreir/dferreir/ShowerDeconstruction/tags/ShowerDeconstruction-01-00-00 ShowerDeconstruction
#svn co svn+ssh://svn.cern.ch/reps/atlasoff/Reconstruction/Jet/BoostedJetTaggers/tags/BoostedJetTaggers-00-00-25 BoostedJetTaggers
#svn co svn+ssh://svn.cern.ch/reps/atlasoff/Reconstruction/PFlow/PFlowUtils/tags/PFlowUtils-00-00-10 PFlowUtils
svn co svn+ssh://svn.cern.ch/reps/atlasoff/Reconstruction/Jet/JetRecTools/tags/JetRecTools-00-03-63 JetRecTools


#if you want the subjet calibration:
# source subjet_calibration.sh

#setup analysis base version and get packages consistent with that version
setupATLAS
Release=2.4.24
rcSetup Base,$Release


# do this if you need btagging in 2.4.24
# python xAODAnaHelpers/scripts/checkoutASGtags.py $Release

rc clean
rc find_packages
rc compile

mkdir run
cd run
ln -s ../JSSTutorial/scripts/config_Tutorial*.py .
ln -s ../JSSTutorial/scripts/xAHjetconfig_example.py .
