################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../bar.cc \
../charge_flow.cc \
../grafo.cc \
../graph.cc \
../load_flow.cc \
../node.cc \
../test_flow.cc 

OBJS += \
./bar.o \
./charge_flow.o \
./grafo.o \
./graph.o \
./load_flow.o \
./node.o \
./test_flow.o 

CC_DEPS += \
./bar.d \
./charge_flow.d \
./grafo.d \
./graph.d \
./load_flow.d \
./node.d \
./test_flow.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


