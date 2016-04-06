################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../InitialStep.cpp 

OBJS += \
./InitialStep.o 

CPP_DEPS += \
./InitialStep.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-hisiv100nptl-linux-g++ -I/home/fatsun/Library/OpenCV/install-opencv-2.4.12-hisiv100/include/opencv -I/home/fatsun/Library/OpenCV/install-opencv-2.4.12-hisiv100/include -I/home/fatsun/Library/OpenCV/install-opencv-2.4.12-hisiv100/include/opencv2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


