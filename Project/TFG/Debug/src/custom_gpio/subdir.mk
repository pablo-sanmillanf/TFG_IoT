################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/custom_gpio/custom_gpio.cpp 

CPP_DEPS += \
./src/custom_gpio/custom_gpio.d 

OBJS += \
./src/custom_gpio/custom_gpio.o 


# Each subdirectory must supply rules for building sources it contributes
src/custom_gpio/%.o: ../src/custom_gpio/%.cpp src/custom_gpio/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-buildroot-linux-gnueabihf-g++ -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/usr/include -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/arm-buildroot-linux-gnueabihf/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-custom_gpio

clean-src-2f-custom_gpio:
	-$(RM) ./src/custom_gpio/custom_gpio.d ./src/custom_gpio/custom_gpio.o

.PHONY: clean-src-2f-custom_gpio

