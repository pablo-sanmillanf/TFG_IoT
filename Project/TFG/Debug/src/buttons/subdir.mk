################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/buttons/buttons.cpp 

CPP_DEPS += \
./src/buttons/buttons.d 

OBJS += \
./src/buttons/buttons.o 


# Each subdirectory must supply rules for building sources it contributes
src/buttons/%.o: ../src/buttons/%.cpp src/buttons/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-buildroot-linux-uclibcgnueabihf-g++ -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/usr/include -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-buttons

clean-src-2f-buttons:
	-$(RM) ./src/buttons/buttons.d ./src/buttons/buttons.o

.PHONY: clean-src-2f-buttons

