echo 1084 > /sys/class/gpio/export
echo low > /sys/class/gpio/gpio1084/direction
sudo /home/firefly/Desktop/StageSATIE/code/modules/GPIO/TestGPIO/bin/Debug/TestGPIO 

