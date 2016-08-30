sleep 10
echo "Enter Assemble.sh" >> /home/firefly/Desktop/Assemble.log
cd /home/firefly/Desktop/StageSATIE/code/modules/Assemble/Assemble/bin/Debug/
export GENICAM_GENTL64_PATH=/opt/Vimba_3_0/VimbaUSBTL/CTI/arm_64bit
sudo -E /home/firefly/Desktop/StageSATIE/code/modules/Assemble/Assemble/bin/Debug/Assemble &
echo "Quit Assemble.sh" >> /home/firefly/Desktop/Assemble.log	


