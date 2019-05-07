
CC=g++
CFLAGS=`pkg-config --cflags --libs opencv`

TopDir=~/Desktop/STAGE_SATIE/code/modules/mpu9250

TestMPU9250:TestMPU9250.o SparkFunMPU9250-DMP.o 
	$(CC)  -o $@ $< 

IncludeDir= ./src

TestMPU9250.o: TestMPU9250.cpp SparkFunMPU9250-DMP.o
	$(CC)  -c -I$(IncludeDir) TestMPU9250.cpp 
	
	
IncludeDir= . ./src ./src/util	

SparkFunMPU9250-DMP.o: SparkFunMPU9250-DMP.cpp SparkFunMPU9250-DMP.h MPU9250_RegisterMap.h inv_mpu.o
	$(CC)  -c -I$(IncludeDir) SparkFunMPU9250-DMP.cpp

IncludeDir= ./src/util	

inv_mpu.o:	inv_mpu.c  inv_mpu.h
	$(CC)  -c -I $(IncludeDir) inv_mpu.c	

clean:
	rm -f *.o 
