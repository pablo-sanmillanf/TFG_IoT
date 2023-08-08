################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/TFTDisplay/icon_manager/icon_manager.cpp 

CPP_DEPS += \
./src/TFTDisplay/icon_manager/icon_manager.d 

OBJS += \
./src/TFTDisplay/icon_manager/icon_manager.o 


# Each subdirectory must supply rules for building sources it contributes
src/TFTDisplay/icon_manager/%.o: ../src/TFTDisplay/icon_manager/%.cpp src/TFTDisplay/icon_manager/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-buildroot-linux-uclibcgnueabihf-g++ -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/usr/include -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-TFTDisplay-2f-icon_manager

clean-src-2f-TFTDisplay-2f-icon_manager:
	-$(RM) ./src/TFTDisplay/icon_manager/icon_manager.d ./src/TFTDisplay/icon_manager/icon_manager.o

.PHONY: clean-src-2f-TFTDisplay-2f-icon_manager

