################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/TFTDisplay/display_driver/display_driver.cpp 

CPP_DEPS += \
./src/TFTDisplay/display_driver/display_driver.d 

OBJS += \
./src/TFTDisplay/display_driver/display_driver.o 


# Each subdirectory must supply rules for building sources it contributes
src/TFTDisplay/display_driver/%.o: ../src/TFTDisplay/display_driver/%.cpp src/TFTDisplay/display_driver/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-buildroot-linux-uclibcgnueabihf-g++ -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/usr/include -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-TFTDisplay-2f-display_driver

clean-src-2f-TFTDisplay-2f-display_driver:
	-$(RM) ./src/TFTDisplay/display_driver/display_driver.d ./src/TFTDisplay/display_driver/display_driver.o

.PHONY: clean-src-2f-TFTDisplay-2f-display_driver

