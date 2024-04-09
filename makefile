Project := example-template
# 编译工具
ARM_TOOLPREFIX := arm-none-eabi-

CC := $(ARM_TOOLPREFIX)gcc
AS := $(CC) -x assembler-with-cpp
DB := $(ARM_TOOLPREFIX)gdb
LD := $(ARM_TOOLPREFIX)ld

OBJCOPY := $(ARM_TOOLPREFIX)objcopy

# 文件路径
OUT_DIR := ./OUT
WORK_DIR = $(shell pwd)

DIR_STM32CORE += $(WORK_DIR)/CMSIS/CM3/CoreSupport
DIR_STM32CORE += $(WORK_DIR)/CMSIS/CM3/DeviceSupport/ST/STM32F10x
DIR_STM32FWLIB := $(WORK_DIR)/Libraries

# 开发板驱动
DIR_BSP += $(WORK_DIR)/SRC/beep
DIR_BSP += $(WORK_DIR)/SRC/delay
DIR_BSP += $(WORK_DIR)/SRC/led
DIR_BSP += $(WORK_DIR)/SRC/key
DIR_BSP += $(WORK_DIR)/SRC/rcc
DIR_BSP += $(WORK_DIR)/SRC/usart
DIR_BSP += $(WORK_DIR)/SRC/timer

# DIR_OTHER := $(WORK_DIR)/THIRD

DIR_MAIN := $(WORK_DIR)/USER

DIR_NEWLIBAPI := $(WORK_DIR)/newlib_interface

DIR_INCLUDE := $(addprefix -I, $(DIR_NEWLIBAPI)) \
	       $(addprefix -I, $(DIR_STM32CORE)) \
	       $(addprefix -I, $(DIR_STM32FWLIB)/inc) \
	       $(addprefix -I, $(DIR_BSP)) \
	       $(addprefix -I, $(DIR_MAIN)/includes)

SRC_C += $(wildcard $(addsuffix /*.c, $(DIR_NEWLIBAPI)))
SRC_C += $(wildcard $(addsuffix /*.c, $(DIR_STM32CORE)))
SRC_C += $(wildcard $(addsuffix /src/*.c, $(DIR_STM32FWLIB)))
SRC_C += $(wildcard $(addsuffix /*.c, $(DIR_BSP)))
SRC_C += $(wildcard $(addsuffix /*.c, $(DIR_MAIN)))

SRC_ASM := ./CMSIS/Startup/startup_stm32f10x_md.s

OUT_C := $(patsubst %.c,%.o, $(SRC_C))
OUT_ASM := $(patsubst %.s,%.o, $(SRC_ASM))

# define
DEFS := STM32F10X_MD USE_STDPERIPH_DRIVER USER_BOARD_SUPPORT

DDEFS := $(addprefix -D, $(DEFS))

# 针对stm32f103c8t6的编译信息
LINK_SCRIPT := ./linkscript/stm32_flash.ld
MCPU := cortex-m3

# 编译标志--specs=rdimon.specs 
SPECS := --specs=nano.specs -u _printf_float 
OPT   += -Os
OPT   += -fsingle-precision-constant
OPT   += -fno-common
OPT   += -ffunction-sections
OPT   += -fdata-sections
FLAGS_MCU := -mcpu=$(MCPU)

FLAGS_AS  := $(SPECS) $(FLAGS_MCU) $(OPT) -c -g -O0 -gdwarf-2 -mthumb
FLAGS_C   := $(SPECS) $(FLAGS_MCU) $(OPT) -c -g -O0 -gdwarf-2 -mthumb \
	     -fomit-frame-pointer -Wall -fverbose-asm $(DDEFS)
FLAGS_CXX := $(SPECS) $(FLAGS_MCU) $(OPT) -c -g -gdwarf-2 -mthumb \
	     -fomit-frame-pointer -Wall -fverbose-asm -fno-exceptions \
	     -fno-rtti -fno-threadsafe-statics -fvisibility=hidden -std=c++11 \
	     $(DDEFS)
FLAGS_LD  := $(SPECS) $(FLAGS_MCU) $(OPT) -lm -g -gdwarf-2 -mthumb \
	     -nostartfiles -Xlinker --gc-sections -T$(LINK_SCRIPT) \
	     -Wl,-Map=$(PROJECT).map,--cref,--no-warn-mismatch

# 编译目标，编译前确定启动文件更换为了gcc版本的https://blog.csdn.net/Chuancey_CC/article/details/78474001
.PHONY: all clean 

all: $(Project).hex

$(Project).hex: $(OUT_C) $(OUT_ASM)
	$(CC) $(FLAGS_LD) $(OUT_C) $(OUT_ASM) -o $(Project).elf
	$(OBJCOPY) -O ihex $(Project).elf $(Project).hex
	$(OBJCOPY) -O binary -S $(Project).elf $(Project).bin

$(OUT_C):%.o:%.c
	$(CC) $(FLAGS_C) $(DIR_INCLUDE) $< -o $@

$(OUT_ASM):%.o:%.s
	$(AS) $(FLAGS_AS)  $< -o $@

clean:
	rm $(OUT_ASM) $(OUT_C)
	rm $(Project).*
