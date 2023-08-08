################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/TFTDisplay/spi_master/spi_master.cpp 

CPP_DEPS += \
./src/TFTDisplay/spi_master/spi_master.d 

OBJS += \
./src/TFTDisplay/spi_master/spi_master.o 


# Each subdirectory must supply rules for building sources it contributes
src/TFTDisplay/spi_master/%.o: ../src/TFTDisplay/spi_master/%.cpp src/TFTDisplay/spi_master/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-buildroot-linux-uclibcgnueabihf-g++ -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/usr/include -I/home/ubuntu/Documents/buildroot-2022.11.1/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-TFTDisplay-2f-spi_master

clean-src-2f-TFTDisplay-2f-spi_master:
	-$(RM) ./src/TFTDisplay/spi_master/spi_master.d ./src/TFTDisplay/spi_master/spi_master.o

.PHONY: clean-src-2f-TFTDisplay-2f-spi_master

