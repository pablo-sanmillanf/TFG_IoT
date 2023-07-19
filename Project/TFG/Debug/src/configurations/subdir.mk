################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/configurations/configurations.cpp 

CPP_DEPS += \
./src/configurations/configurations.d 

OBJS += \
./src/configurations/configurations.o 


# Each subdirectory must supply rules for building sources it contributes
src/configurations/%.o: ../src/configurations/%.cpp src/configurations/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-buildroot-linux-gnueabihf-g++ -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/usr/include -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/arm-buildroot-linux-gnueabihf/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-configurations

clean-src-2f-configurations:
	-$(RM) ./src/configurations/configurations.d ./src/configurations/configurations.o

.PHONY: clean-src-2f-configurations

