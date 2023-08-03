################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/measures/i2c_master/i2c_master.cpp 

CPP_DEPS += \
./src/measures/i2c_master/i2c_master.d 

OBJS += \
./src/measures/i2c_master/i2c_master.o 


# Each subdirectory must supply rules for building sources it contributes
src/measures/i2c_master/%.o: ../src/measures/i2c_master/%.cpp src/measures/i2c_master/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-buildroot-linux-uclibcgnueabihf-g++ -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/usr/include -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-measures-2f-i2c_master

clean-src-2f-measures-2f-i2c_master:
	-$(RM) ./src/measures/i2c_master/i2c_master.d ./src/measures/i2c_master/i2c_master.o

.PHONY: clean-src-2f-measures-2f-i2c_master

