################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/measures/IAQTracker/IAQTracker.cpp 

CPP_DEPS += \
./src/measures/IAQTracker/IAQTracker.d 

OBJS += \
./src/measures/IAQTracker/IAQTracker.o 


# Each subdirectory must supply rules for building sources it contributes
src/measures/IAQTracker/%.o: ../src/measures/IAQTracker/%.cpp src/measures/IAQTracker/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-buildroot-linux-uclibcgnueabihf-g++ -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/usr/include -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-measures-2f-IAQTracker

clean-src-2f-measures-2f-IAQTracker:
	-$(RM) ./src/measures/IAQTracker/IAQTracker.d ./src/measures/IAQTracker/IAQTracker.o

.PHONY: clean-src-2f-measures-2f-IAQTracker

