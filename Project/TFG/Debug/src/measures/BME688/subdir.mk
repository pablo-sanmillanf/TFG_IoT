################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/measures/BME688/BME688.cpp 

CPP_DEPS += \
./src/measures/BME688/BME688.d 

OBJS += \
./src/measures/BME688/BME688.o 


# Each subdirectory must supply rules for building sources it contributes
src/measures/BME688/%.o: ../src/measures/BME688/%.cpp src/measures/BME688/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-buildroot-linux-uclibcgnueabihf-g++ -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/usr/include -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-measures-2f-BME688

clean-src-2f-measures-2f-BME688:
	-$(RM) ./src/measures/BME688/BME688.d ./src/measures/BME688/BME688.o

.PHONY: clean-src-2f-measures-2f-BME688

