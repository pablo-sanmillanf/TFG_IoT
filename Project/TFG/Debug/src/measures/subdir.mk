################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/measures/measures.cpp 

CPP_DEPS += \
./src/measures/measures.d 

OBJS += \
./src/measures/measures.o 


# Each subdirectory must supply rules for building sources it contributes
src/measures/%.o: ../src/measures/%.cpp src/measures/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-buildroot-linux-gnueabihf-g++ -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/usr/include -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/arm-buildroot-linux-gnueabihf/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-measures

clean-src-2f-measures:
	-$(RM) ./src/measures/measures.d ./src/measures/measures.o

.PHONY: clean-src-2f-measures

