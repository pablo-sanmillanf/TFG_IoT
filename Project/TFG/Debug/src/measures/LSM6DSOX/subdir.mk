################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/measures/LSM6DSOX/LSM6DSOX.cpp 

CPP_DEPS += \
./src/measures/LSM6DSOX/LSM6DSOX.d 

OBJS += \
./src/measures/LSM6DSOX/LSM6DSOX.o 


# Each subdirectory must supply rules for building sources it contributes
src/measures/LSM6DSOX/%.o: ../src/measures/LSM6DSOX/%.cpp src/measures/LSM6DSOX/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-buildroot-linux-gnueabihf-g++ -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/usr/include -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/arm-buildroot-linux-gnueabihf/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-measures-2f-LSM6DSOX

clean-src-2f-measures-2f-LSM6DSOX:
	-$(RM) ./src/measures/LSM6DSOX/LSM6DSOX.d ./src/measures/LSM6DSOX/LSM6DSOX.o

.PHONY: clean-src-2f-measures-2f-LSM6DSOX

