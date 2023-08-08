################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/TFTDisplay/TFTDisplay.cpp 

CPP_DEPS += \
./src/TFTDisplay/TFTDisplay.d 

OBJS += \
./src/TFTDisplay/TFTDisplay.o 


# Each subdirectory must supply rules for building sources it contributes
src/TFTDisplay/%.o: ../src/TFTDisplay/%.cpp src/TFTDisplay/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-buildroot-linux-uclibcgnueabihf-g++ -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/usr/include -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-TFTDisplay

clean-src-2f-TFTDisplay:
	-$(RM) ./src/TFTDisplay/TFTDisplay.d ./src/TFTDisplay/TFTDisplay.o

.PHONY: clean-src-2f-TFTDisplay

