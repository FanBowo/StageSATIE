# StageSATIE Version  on board
##Note:

If we start with a blank board, it might be useful to glance over https://roc-rk3328-cc.readthedocs.io/en/latest/intro.html to get know how to intall systemes in emmc module, 
or read StageSATIE/doc/rock3328/roc-rk3328-cc-zh-cn.pdf in chinese.
We can find ofially imag file in https://libre.computer/products/boards/roc-rk3328-cc/ 
or one backup in my googledrive https://drive.google.com/drive/folders/1wMkQ4YDOPzxj1toXsm-93rsR1YK7MMoE?usp=sharing 

To connect the module IMU with the Motherboard, 4 pins used: SDA1 SCL1 5V(VCC) GND, to connect the trigger pin of camera, two pins used: SDO1(as Trigger pin) and gnd. 

If we install a new ubuntu on one blank board, we need to update all library by sudo apt-get update and sudo apt-get upgrade. 
Then the library eigen3 should be installed and also the driver of camera, Vimba. 
Vimba can be found in https://www.alliedvision.com/en/products/software.html . We install the driver in default folder /opt as mentioned in release note of Vimba. 

The battery used has two ports USB, one supplies the module of GPS and the rest for the Motherboard.
For the reason that the GPS costs few minutes to search for satellites  when restart it, it’s better not to plug out the cable USB of GPS or to cut off the power supply by pressing the button of battery.
Meanwhile, if we need to restart the  Motherboard, we  plug out  and then re-plug in its  own cable USB instead.

When plug in the cable USB of Motherboard, the LED of camera will blink one time, it means that the motherboard get the power supply.
Then it will rest unlit for about 10 seconds. During that period, the system is starting itself.
After that, il will remain bright, about 3 seconds, which depends on the value of sleep time defined in StageSATIE/code/modules/Assemble/Assemble.sh.
Then we will notice that it will blink one time, it means our main program begin to execute.
When watched that phenomenon, we let our platform rest quiet about 3 seconds to calibrate the gyroscope,
then we move it in the space with the track of  '' 8 '' to calibrate the magnetometer.
Once the IMU gets to know the real NORTH, it will begin to takes photos and the LED keeps blinking.
When the LED does not blink any longer, it means that our programm lost the control of the camera, we can restart the Motherboard if needed.
According to the tests I have done, with frequency of 15 Hz of taking photo, we won't lost lost the control of the camera in 20- 45 minutes.

When the GPS hasn’t found enough satellites, more than 3, it can not tell us the timestamp global.
Therefore, it’s better not to turn on our platform until the GPS can tell us a  stable  timestamp.
When it has reached enough  satellites, the LED   on the module of GPS  blinks blinks once every 15 seconds.   
Contrariwise, when searching the  satellites, it blinks about 1 Hz.
So it’s better to wait till this phenomenon and then plug in the cable USB of  the Motherboard, if we do not want to use the wrong timestamp to name our files.
By the way, the timestamp used to name the photo is the current time GMT (Greenwich Mean Time), in the unit of nanoseconde.

When browsing the code, it has five macro we might be interested.
 - StageSATIE/code/modules/CameraMako130/Src/CameraTimer.cpp #define ExternTriggerFre 15:
     Or in layout of codeblocks: Sources/CameraMako130/Src/CameraTimer.cpp
It’s where we define the frequency that we take photos, the default value is 15.
Each time we take a photo, we read also the three angles of pose of IMU.
We can set that value from 1 till 100 , while the larger that value is, the less stable our program is.
100 is the max frequency allowed by the IMU.
 
- StageSATIE/code/modules/Assemble/src/Tasks.h #define TimerIMUFre 80
     Or in layout of codeblocks: Headers/Assemble/src/Tasks.h
It’s where we define the frequency that we read the raw data of IMU, the default value is 80.
We can set that value from 1 till 100.

- StageSATIE/code/modules/Assemble/src/Tasks.h  #define TimerDeviceFre 157
    Or in layout of codeblocks: : Headers/Assemble/src/Tasks.h
It’s where we define the frequency of timer who aim at calculating the local timestamp.
The default value is 157, a prime number adopted to avoid errors coupled with other timer.
It has little influence when we change this value.

- StageSATIE/code/modules/Assemble/src/Tasks.cpp #define KALIBR
    Or in layout of codeblocks: : Sources/Assemble/src/Tasks.cpp
When we declare KALIBR, it means that we want to record the raw value of IMU, which will be used in algorithm of Kalibr to calibrate the relation geometric bettween the camera and the IMU.
By default, we do not declare this macro.

- StageSATIE/code/modules/Assemble/src/Tasks.cpp#define RECORD_IMU_ANG_DATA
    Or in layout of codeblocks: : Sources/Assemble/src/Tasks.cpp
 When we declare RECORD_IMU_ANG_DATA, it means that we want to record the three angles of pose of IMU.
 Usually, we un-declare this macro when we declare  KALIBR, for the Kalibr use only raw value of IMU.

There are two shell files launched when we power on our system. It’s declares in file /etc/rc.local

-  StageSATIE/code/modules/GPIO/MonuntGPIO.sh
In this file, we mount the GPIO who trigger  the camera to Linux file system, and the that GPIO is set to be 0 as initial value.  

-  StageSATIE/code/modules/Assemble/Assemble.sh
In this file, we launch our main program.
Before launching, there is 1 second of delay left to system for preparing.
We can change this value as we want.


 






