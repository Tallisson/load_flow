################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../admitt.cc \
../bar.cc \
../charge_flow.cc \
../graph.cc \
../load_flow.cc \
../node.cc \
../test_flow.cc 

OBJS += \
./admitt.o \
./bar.o \
./charge_flow.o \
./graph.o \
./load_flow.o \
./node.o \
./test_flow.o 

CC_DEPS += \
./admitt.d \
./bar.d \
./charge_flow.d \
./graph.d \
./load_flow.d \
./node.d \
./test_flow.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


