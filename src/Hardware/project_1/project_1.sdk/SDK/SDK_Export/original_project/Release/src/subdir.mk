################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/TFT.cpp \
../src/angles.cpp \
../src/physics.cpp \
../src/position_locator.cpp 

LD_SRCS += \
../src/lscript.ld 

OBJS += \
./src/TFT.o \
./src/angles.o \
./src/physics.o \
./src/position_locator.o 

CPP_DEPS += \
./src/TFT.d \
./src/angles.d \
./src/physics.d \
./src/position_locator.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MicroBlaze g++ compiler'
	mb-g++ -Wall -O2 -c -fmessage-length=0 -I../../original_project_bsp/microblaze_0/include -mlittle-endian -mcpu=v9.3 -mxl-soft-mul -Wl,--no-relax -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


