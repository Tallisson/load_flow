################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../examples/case_14_bus.cc \
../examples/case_2_bus.cc \
../examples/case_5_bus.cc \
../examples/case_6_bus.cc \
../examples/test_time.cc 

OBJS += \
./examples/case_14_bus.o \
./examples/case_2_bus.o \
./examples/case_5_bus.o \
./examples/case_6_bus.o \
./examples/test_time.o 

CC_DEPS += \
./examples/case_14_bus.d \
./examples/case_2_bus.d \
./examples/case_5_bus.d \
./examples/case_6_bus.d \
./examples/test_time.d 


# Each subdirectory must supply rules for building sources it contributes
examples/%.o: ../examples/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


