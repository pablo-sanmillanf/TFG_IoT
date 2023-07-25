################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/TFTDisplay/display_driver.cpp \
../src/TFTDisplay/spi_master.cpp 

C_SRCS += \
../src/TFTDisplay/arial_20v2.c 

CPP_DEPS += \
./src/TFTDisplay/display_driver.d \
./src/TFTDisplay/spi_master.d 

C_DEPS += \
./src/TFTDisplay/arial_20v2.d 

OBJS += \
./src/TFTDisplay/arial_20v2.o \
./src/TFTDisplay/display_driver.o \
./src/TFTDisplay/spi_master.o 


# Each subdirectory must supply rules for building sources it contributes
src/TFTDisplay/%.o: ../src/TFTDisplay/%.c src/TFTDisplay/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-buildroot-linux-gnueabihf-gcc -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/usr/include -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/arm-buildroot-linux-gnueabihf/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/TFTDisplay/%.o: ../src/TFTDisplay/%.cpp src/TFTDisplay/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-buildroot-linux-gnueabihf-g++ -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/usr/include -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/arm-buildroot-linux-gnueabihf/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-TFTDisplay

clean-src-2f-TFTDisplay:
	-$(RM) ./src/TFTDisplay/arial_20v2.d ./src/TFTDisplay/arial_20v2.o ./src/TFTDisplay/display_driver.d ./src/TFTDisplay/display_driver.o ./src/TFTDisplay/spi_master.d ./src/TFTDisplay/spi_master.o

.PHONY: clean-src-2f-TFTDisplay

