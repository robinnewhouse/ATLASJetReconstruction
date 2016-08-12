
export USER=`whoami`
export DATE=`date +%Y%m%d`

echo $USER
echo $DATE

export OUTPUTCONTAINER="user."$USER".grid_running."$DATE

echo $OUTPUTCONTAINER

# xAH_run.py --files\
#  sample_list_for_grid.list --inputList\
#  --config config_example.py\
#  -v --inputDQ2 --submitDir Output_wprime_list\
#  prun --optGridNFilesPerJob 1 --optGridOutputSampleName $OUTPUTCONTAINER




