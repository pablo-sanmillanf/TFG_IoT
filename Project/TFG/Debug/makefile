################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/measures/i2c_master/subdir.mk
-include src/measures/LSM6DSOX/subdir.mk
-include src/measures/IAQTracker/subdir.mk
-include src/measures/BME688/subdir.mk
-include src/measures/subdir.mk
-include src/custom_gpio/subdir.mk
-include src/client_MQTT/subdir.mk
-include src/buttons/subdir.mk
-include src/app_data_storage/subdir.mk
-include src/TFTDisplay/spi_master/subdir.mk
-include src/TFTDisplay/icon_manager/subdir.mk
-include src/TFTDisplay/display_driver/subdir.mk
-include src/TFTDisplay/subdir.mk
-include src/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := TFG
BUILD_ARTIFACT_EXTENSION :=
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: main-build

# Main-build Target
main-build: TFG

# Tool invocations
TFG: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	arm-buildroot-linux-uclibcgnueabihf-g++ -L/home/ubuntu/Documents/buildroot-2022.11.1/output/host/usr/lib -L/home/ubuntu/Documents/buildroot-2022.11.1/output/host/arm-buildroot-linux-uclibcgnueabihf -L/home/ubuntu/Documents/buildroot-2022.11.1/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr/lib -o "TFG" $(OBJS) $(USER_OBJS) $(LIBS) -ljsoncpp -lpaho-mqtt3c -lpaho-mqttpp3
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) TFG
	-@echo ' '

.PHONY: all clean dependents main-build

-include ../makefile.targets
