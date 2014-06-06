################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../bkp/charge_flow.cc \
../bkp/test_flow.cc 

OBJS += \
./bkp/charge_flow.o \
./bkp/test_flow.o 

CC_DEPS += \
./bkp/charge_flow.d \
./bkp/test_flow.d 


# Each subdirectory must supply rules for building sources it contributes
bkp/%.o: ../bkp/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


