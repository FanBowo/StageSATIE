# StageSATIE Version  on board
Note:


The battery used has two ports USB, one supplies the module of GPS and the rest for the Motherboard. 
For the reason that the GPS costs few minutes to search for satellites  when restart it, it’s better not to plug out the cable USB of GPS or to cut off the power supply by pressing the button of battery. 
Meanwhile, if we need to restart the  Motherboard, we  plug out  and then re-plug in its  own cable USB instead. 

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
We can set that value from 1 till 100, while the larger that value is, the less stable our program is. 
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


 
