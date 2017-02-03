# © 2010 - 2016, Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V. All rights reserved.
#
# BSD 3-Clause License
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
# 1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
# 3.  Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# We kindly request you to use one or more of the following phrases to refer to foxBMS in your hardware, software, documentation or advertising materials:
#
# 'This product uses parts of foxBMS®'
#
# 'This product includes parts of foxBMS®'
#
# 'This product is derived from foxBMS®'


# define command for cleanup 

RM := rm -rf

# all .o and .d files created during compilation process are placed into a subfolder structure below this folder
OBJDIR := \
	objs

# components like HAL and FreeRTOS are very unlikely to be modified therefore we build libraries to save some time during compilation
# library files will be placed into this subfolder during compilation process	
LIBDIR := \
	libs
	
# all desired compiler flags can be applied here
CFLAGS := \
	-mcpu=cortex-m4           \
	-mthumb -mlittle-endian   \
	-mfloat-abi=softfp        \
	-mfpu=fpv4-sp-d16         \
	-O0                       \
	-fmessage-length=0        \
	-fsigned-char             \
	-ffunction-sections       \
	-fdata-sections           \
	-ffreestanding            \
	-fno-move-loop-invariants \
	-Wall                     \
	-g3                       \
	-std=gnu11                \
	-DSTM32F429xx             \
	-DDEBUG                   \
	-DUSE_FULL_ASSERT         \
	-DTRACE                   \
	-DOS_USE_TRACE_ITM        \
	-DUSE_HAL_DRIVER          \
	-DHSE_VALUE=8000000

# all assembler flags passed through GCC can be applied here
ASMFLAGS := \
	-mcpu=cortex-m4 			\
	-mthumb -mlittle-endian 	\
	-mfloat-abi=softfp 			\
	-mfpu=fpv4-sp-d16 			\
	-O0							\
	-fmessage-length=0 			\
	-fsigned-char 				\
	-ffunction-sections 		\
	-fdata-sections 			\
	-ffreestanding 				\
	-fno-move-loop-invariants	\
	-Wall						\
	-g3 						\
	-x assembler 				\
	-DSTM32F429xx 				\
	-DDEBUG 					\
	-DUSE_FULL_ASSERT 			\
	-DTRACE 					\
	-DOS_USE_TRACE_ITM 			\
	-DUSE_HAL_DRIVER 			\
	-DHSE_VALUE=8000000
	
# every subdirectory containing source files must be described here to be considered during compilation process
SUBDIRS := \
	src/test										\
	src/test/usb_cdc_lolevel						\
	src/os											\
	src/os/FreeRTOS/Source							\
	src/os/FreeRTOS/Source/portable/MemMang			\
	src/os/FreeRTOS/Source/portable/GCC/ARM_CM4F	\
	src/os/FreeRTOS/Source/CMSIS_RTOS				\
	src/module/uart									\
	src/module/spi									\
	src/module/rcc									\
	src/module/ltc									\
	src/module/irq									\
	src/module/io									\
	src/module/eeprom								\
	src/module/dma									\
	src/module/mcu									\
	src/module/contactor							\
	src/module/config								\
	src/module/com									\
	src/module/rtc									\
	src/module/utils								\
	src/module/timer								\
	src/module/can									\
	src/module/adc									\
	src/module/chksum								\
	src/module/watchdog								\
	src/module/bal									\
	src/hal/STM32F4xx_HAL_Driver/Src				\
	src/general										\
	src/general/config								\
	src/engine/task									\
	src/engine/sysctrl								\
	src/engine/sof									\
	src/engine/soc									\
	src/application/sox								\
	src/module/isoguard								\
	src/engine/diag									\
	src/engine/database								\
	src/engine/config								\
	src/engine/bmsctrl								\
	src/application/task							\
	src/application/config

# global list of include dirs, needed during compilation
INCDIRS := \
	-I"./src/application"                              \
	-I"./src/application/config"                       \
	-I"./src/application/task"                         \
	-I"./src/module/adc"                               \
	-I"./src/module/can"                               \
	-I"./src/module/com"                               \
	-I"./src/module/config"                            \
	-I"./src/module/contactor"                         \
	-I"./src/module/cpuload"                           \
	-I"./src/module/io"                                \
	-I"./src/module/ltc"                               \
	-I"./src/module/spi"                               \
	-I"./src/module/uart"                              \
	-I"./src/module/timer"                             \
	-I"./src/module/eeprom"							   \
	-I"./src/module/utils"                             \
	-I"./src/module/rtc"                               \
	-I"./src/module/chksum"                            \
	-I"./src/module/cansignal"                         \
	-I"./src/module/watchdog"                          \
	-I"./src/module/bal"                               \
	-I"./src/engine/bmsctrl"                           \
	-I"./src/engine/config"                            \
	-I"./src/engine/database"                          \
	-I"./src/engine/diag"                              \
	-I"./src/module/isoguard"                          \
	-I"./src/application/soc"                          \
	-I"./src/application/sof"                          \
	-I"./src/application/sox"                          \
	-I"./src/engine/sysctrl"                           \
	-I"./src/engine/task"                              \
	-I"./src/engine/dbengine"                          \
	-I"./src/general"                                  \
	-I"./src/general/config"                           \
	-I"./src/general/includes"                         \
	-I"./src/hal/CMSIS/Device/ST/STM32F4xx/Include"    \
	-I"./src/hal/CMSIS/Include"                        \
	-I"./src/hal/STM32F4xx_HAL_Driver/Inc"             \
	-I"./src/os"                                       \
	-I"./src/os/FreeRTOS"                              \
	-I"./src/os/FreeRTOS/Source"                       \
	-I"./src/os/FreeRTOS/Source/include"               \
	-I"./src/os/FreeRTOS/Source/CMSIS_RTOS"            \
	-I"./src/os/FreeRTOS/Source/portable/GCC/ARM_CM4F" \
	-I"./src/test"                                     \
	-I"./src/module/mcu"                               \
	-I"./src/module/dma"                               \
	-I"./src/module/irq"                               \
	-I"./src/module/rcc"                               \
	-I"./src/test/usb_cdc_lolevel"

# added startup script source for FoxBMS, will be assembled later using GCC passthrough and linked into final binary, unlikely to be modified 
S_UPPER_SRCS := \
	./src/general/config/startup_stm32f429xx.S

# dependency listing for startup script
S_UPPER_DEPS := \
	./src/general/config/startup_stm32f429xx.d

# macro/function which do a recursive search approach in all defined subfolders, matching the provided pattern
rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

# this part of the makefile adds all *.c source files from the pre-defined $(SUBDIRS)
C_SRCS := \
	$(foreach d,$(SUBDIRS),$(call rwildcard, $d, *.c))

# abuse the sort function to filter out duplicate source paths in $(C_SRCS)
C_SRCS := \
	$(sort $(C_SRCS))
	
# macro/function, filters out elements which doesn't match the provided arguments (supports substrings and wildcards)
# returns all elements NOT matching the passed params
FILTER_OUT = $(foreach v,$(2),$(if $(findstring $(1),$(v)),,$(v)))

# macro/function, filters elements which matches the provided arguments (supports substrings and wildcards)
# returns all elements matching the passed params
FILTER = $(foreach v,$(2),$(if $(findstring $(1),$(v)),$(v),))

# exclude the configuration templates, provided by STM from build (we got our own implementation, duplicates would lead to linker errors!)
C_SRCS := $(filter-out src/hal/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c, $(C_SRCS))
C_SRCS := $(filter-out src/hal/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_msp_template.c, $(C_SRCS))

# create folder/subfolder structure for object/dependency files which gets generated during compilation 
C_SRCS_DIR := \
	$(patsubst %.c,%,$(C_SRCS))

# create list of object files which will be dirst generated from their corresponding source files and later included during the linking process
OBJS := \
	$(patsubst %.c,$(OBJDIR)/%.o,$(C_SRCS))

# create list of all HAL objects for later use	
OBJS_HAL := \
	$(call FILTER,/hal/, $(OBJS))

# create list of all FreeRTOS objects for later use
OBJS_FRTOS := \
	$(call FILTER,/FreeRTOS/, $(OBJS))

# remove HAL objects from common objects list, they will be linked into an extra library
OBJS := \
	$(call FILTER_OUT,/hal/, $(OBJS))

# remove FreeRTOS objects from common objects list, they will be linked into an extra library
OBJS := \
	$(call FILTER_OUT,/FreeRTOS/, $(OBJS))
	
# add linker script to common object list, needed during linking process
OBJS += \
	$(OBJDIR)/src/general/config/startup_stm32f429xx.o

# generated dependency files for link process
C_DEPS := \
	$(patsubst %.c,$(OBJDIR)/%.d,$(C_SRCS))

# generate object files from their corresponding source file and place them into $(OBJDIR)
$(OBJDIR)/%.o: %.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc $(CFLAGS) $(INCDIRS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# assemble linker script using the flags defined in $(ASMFLAGS) using a GCC passthrough
$(OBJDIR)/src/general/config/startup_stm32f429xx.o: src/general/config/startup_stm32f429xx.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc $(ASMFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# process HAL object files, link them into a library and place this library into $(LIBDIR) folder
$(LIBDIR)/libHAL.a: $(OBJS_HAL)
	arm-none-eabi-gcc-ar rcs $@ $^

# process FreeRTOS object files, link them into a library and place this library into $(LIBDIR) folder
$(LIBDIR)/libFRTOS.a: $(OBJS_FRTOS)
	arm-none-eabi-gcc-ar rcs $@ $^

# add inputs and outputs from these tool invocations to the build variables
SECONDARY_FLASH := \
	foxbms.hex \
	
SECONDARY_BIN := \
	foxbms.bin \

SECONDARY_LIST := \
	foxbms.lst \

SECONDARY_SIZE := \
	foxbms.siz \

# all Target is defined here
all: foxbms.elf secondary-outputs

# linker invocation starts here, using the provided flags, objects files and libraries
foxbms.elf: buildrepo buildrepo_lib $(OBJS) $(USER_OBJS) $(LIBDIR)/libHAL.a $(LIBDIR)/libFRTOS.a
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C++ Linker'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mlittle-endian -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -g3 -T "src/STM32F429ZIT6_FLASH.ld" -Xlinker --gc-sections -Wl,-Map,"foxbms.map" --specs=nano.specs -o "foxbms.elf" $(OBJS) $(USER_OBJS) $(LIBDIR)/libHAL.a $(LIBDIR)/libFRTOS.a
	@echo 'Finished building target: $@'
	@echo ' '

# hex file generation starts here, using objcopy
foxbms.hex: foxbms.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "foxbms.elf" "foxbms.hex"
	@echo 'Finished building: $@'
	@echo ' '

# generate listing of the corresponding assembler code for debugging purpose
foxbms.lst: foxbms.elf
	@echo 'Invoking: Cross ARM GNU Create Listing'
	arm-none-eabi-objdump --source --all-headers --demangle --line-numbers --wide "foxbms.elf" > "foxbms.lst"
	@echo 'Finished building: $@'
	@echo ' '

# print out infos regarding ram, flash, bss consumption, useful for debugging
foxbms.siz: foxbms.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley $(OBJS) "foxbms.elf"
	@echo 'Finished building: $@'
	@echo ' '

# bin file generation starts here, using objcopy (currently disabled due inconvenient memory layout in the linker script to prevent generation of 9xx MB file)
foxbms.bin: foxbms.elf
	@echo 'Create binary'
#	arm-none-eabi-objcopy -O binary "foxbms.elf" "foxbms.bin"
	@echo ' '
	
# helper function which generates need subfolder for storing the generated library files
buildrepo_lib:
	@$(call make-repo-lib)
	
define make-repo-lib
   for dir in $(LIBDIR); \
   do \
	mkdir -p $$dir; \
   done
endef

# helper function which generates needed subfolder structure for object/dependency files during compilation
buildrepo:
	@$(call make-repo)

define make-repo
   for dir in $(C_SRCS_DIR); \
   do \
	mkdir -p $(OBJDIR)/$$dir; \
   done
endef

# target clean defined here, makes use of the defined cleanup command to remove all generated files 
clean:
	-$(RM) \
		$(CC_DEPS)			\
		$(C++_DEPS)			\
		$(OBJS)				\
		$(C_UPPER_DEPS)		\
		$(CXX_DEPS)			\
		$(SECONDARY_FLASH)	\
		$(SECONDARY_BIN)	\
		$(SECONDARY_LIST)	\
		$(SECONDARY_SIZE)	\
		$(ASM_DEPS)			\
		$(S_UPPER_DEPS)		\
		$(C_DEPS)			\
		$(CPP_DEPS)			\
		foxbms.elf			\
		foxbms.map
	-@echo ' '

# secondary targets, responsible for tool invocations for additional outputs (see above)
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_BIN) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents x