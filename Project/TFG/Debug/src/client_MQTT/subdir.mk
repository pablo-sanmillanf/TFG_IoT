################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/client_MQTT/client_MQTT.cpp 

CPP_DEPS += \
./src/client_MQTT/client_MQTT.d 

OBJS += \
./src/client_MQTT/client_MQTT.o 


# Each subdirectory must supply rules for building sources it contributes
src/client_MQTT/%.o: ../src/client_MQTT/%.cpp src/client_MQTT/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-buildroot-linux-uclibcgnueabihf-g++ -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/usr/include -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-client_MQTT

clean-src-2f-client_MQTT:
	-$(RM) ./src/client_MQTT/client_MQTT.d ./src/client_MQTT/client_MQTT.o

.PHONY: clean-src-2f-client_MQTT

