# stm32-arm-gcc-example
这是一个用来说明ubuntu下stm32开发环境搭建的示例项目

# 系统，库，硬件要求

- ubuntu 22.04

- STM32F10x_StdPeriph_Lib_V3.x.x(FWLIB，不是HAL)

- stlink v2

- stm32f103c8t6 blue pill(我在实践时使用的是这款MPU，具体stm32f10x系列应该都可以参考)

# 移植目标

将windows上再MDK中正常编译调试的项目移植到ubuntu系统，使用arm tools chain的开发环境上。

# 需要解决的问题

- ubuntu上ARM基本开发环境的搭建

  - 软件的下载和安装

  - ubuntu22.04上，arm-none-eabi-gdb依赖python3.8的问题

- MDK项目移植

  - 将代码适配于arm-none-eabi-gcc编译器

  - 将链接脚本（.ld文件）适用于arm-none-eabi-gcc编译器

  - 编写makefile用于管理整个项目

# 具体实施步骤

## ubuntu上ARM的基本开发环境准备

依赖库下载

```
sudo apt update

sudo apt-get install libncursesw5 # arm-none-eabi-gdb的交互界面依赖
```

---

准备arm工具

[下载arm-gnu-toolchain-13.2](https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz)

将下载的arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz解压并移动到你想存放的文件夹中（这里我将他存放在/opt/gcc-arm文件夹下）。

```
tar -zvf ./arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz -C /opt/gcc-arm
```

给arm开发工具配置环境变量，在~/.bashrc中添加环境变量：

```
export GCC_ARM="/opt/gcc-arm/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi"
export PATH=$PATH:$GCC_ARM/bin
```

刷新环境变量或关闭终端再重新开启让环境变量生效。终端输入：

```
example@example:~$arm-none-eabi-gcc --version
arm-none-eabi-gcc (Arm GNU Toolchain 13.2.rel1 (Build arm-13.7)) 13.2.1 20231009
Copyright (C) 2023 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

如果显示以上信息则环境变量配置正确，arm工具配置完成。

---

安装调试stlink tools调试工具

```
sudo apt install stlink-tools
```

等待安装后，将stlink v2 链接上单片机(相见附录)，并将stlink v2插入USB上，在终端输入：

```
example@example:~$ st-info --probe
Found 1 stlink programmers
  version:    V2J41S7
  serial:     47006100060000364C41534E
  flash:      65536 (pagesize: 1024)
  sram:       20480
  chipid:     0x0410
  descr:      F1xx Medium-density
```

如果显示以上信息，则说明安装成功且单片机连接正常。更多关于stlink tools的使用方法详见附录。到此，stlink v2需要的软件安装完成。

### ubuntu22.04上，arm-none-eabi-gdb依赖python3.8的问题

arm-none-eabi-gdb默认使用python3.8，在实践过程中尝试更改为python3.10，但是失败，因此下面讲述如何安装python3.8，python3.8的安装不会影响系统默认的python环境为python3.10。参考[这里](https://stackoverflow.com/questions/72187185/arm-none-eabi-gdb-cannot-start-because-of-some-python-error)的第二个回答。

```
sudo add-apt-repository ppa:deadsnakes/ppa # 添加这个仓库用来下载python3.8
sudo apt update
sudo apt install python3.8 # 安装python3.8
```

安装完成后，在终端输入：

```
example@example:~$ arm-none-eabi-gdb
GNU gdb (Arm GNU Toolchain 13.2.rel1 (Build arm-13.7)) 13.2.90.20231008-git
Copyright (C) 2023 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "--host=x86_64-pc-linux-gnu --target=arm-none-eabi".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<https://bugs.linaro.org/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word".
(gdb) 
```

显示以下信息表示安装arm gdb可以正常运行。

至此，ubuntu上arm的基础开发环境准备完成。

## MDK项目移植

将windows系统下的MDK项目复制到ubuntu文件夹下，首先需要建立一个有完整源文件的项目系统，接下来成为适用于arm-toolchain的项目。其中需要包括：用户自定义的驱动程序和应用代码，stm32f10x标准库文件（cortex-m3 core 文件和外设驱动文件）。

如果直接复制的MDK项目下没有标准库文件，可以从这里[下载](https://www.st.com/en/embedded-software/stsw-stm32054.html)，这是官方的下载渠道，需要登陆或者提供一个电子邮件，让st官方向这个电子邮件中发送下载链接。下载后解压并将STM32F10x_StdPeriph_Lib_V3.x.x/Libraries下的CMSIS STM32F10x_StdPeriph_Driver拷贝到适用于arm-toolchain的项目目录下，以下为一个适用于arm-toolchain的项目的示例结构：

```
一个适用于ARM toolchain的项目的结构
├── CMSIS # 内和驱动，从标准库中直接拷贝
│   ├── CM3
│   │   ├── CoreSupport
│   │   │   ├── core_cm3.c # 之后会修改这个文件来适配arm gcc
│   │   │   └── core_cm3.h
│   │   └── DeviceSupport
│   │       └── ...
│   └── Startup
│       └── startup_stm32f10x_md.s # 适用于stm32f103c8t6
├── Libraries # 这个文件夹是STM32F10x_StdPeriph_Driver文件夹的重命名，存放外设驱动程序，从标准库中拷贝STM32F10x_StdPeriph_Driver的内容到这里
│   ├── inc
│   │   └── ... 
│   └── src
│       └── ...
├── linkscript # 链接脚本
│   └── stm32_flash.ld
├── makefile # makefile文件
├── newlib_interface # newlib的一些必要的接口
│   └── interface.c
├── SRC
│   └── ... # 我在这里存放用户驱动
└── USER
    ├── ... # 应用程序文件
    ├── includes
    │   ├── main.h 		# 这些文件是MDK创建新项目时自动创建的
    │   ├── stm32f10x_conf.h 	# 这些文件是MDK创建新项目时自动创建的
    │   └── stm32f10x_it.h 	# 这些文件是MDK创建新项目时自动创建的
    ├── main.c 			# 这些文件是MDK创建新项目时自动创建的
    └── stm32f10x_it.c 		# 这些文件是MDK创建新项目时自动创建的
```

一些MDK工程的配置文件文件搁置在一边即可，OUT文件夹也被去除，这是因为我选择将编译过后的文件直接和其对应源文件存放在同一个文件夹中，如果有需要不同的存放方式可以在makefile中更改。

下文所有设计到相关项目结构的，都以上述结构为准。根据具体需求更改文件夹的，需要注意使用自己的路径。

### 编译器选择

整个项目的编译和链接全部使用arm-none-eabi-gcc来完成。

### 将代码适配于arm-none-eabi-gcc编译器

这里可能是编译器的问题，需要修改core_cm3.c中的文件，来保证译正常进行。[官方论坛参考](https://community.st.com/t5/stm32-mcus-products/quick-strex-question/td-p/491429)，一个更加简洁的整理在[这里](https://blog.csdn.net/Chuancey_CC/article/details/78474001)的第四部分。具体操作为：

```
736    __ASM volatile ("strexb %0, %2, [%1]" : "=r" (result) : "r" (addr), "r      " (value) );

753    __ASM volatile ("strexh %0, %2, [%1]" : "=r" (result) : "r" (addr), "r      " (value) );

000    # 将第一个"=r"更改为"=&r"，下面是更改后的，上面时原始代码

736    __ASM volatile ("strexb %0, %2, [%1]" : "=&r" (result) : "r" (addr), "r      " (value) );

753    __ASM volatile ("strexh %0, %2, [%1]" : "=&r" (result) : "r" (addr), "r      " (value) );
```

二者所在函数为：uint32_t __STREXB(uint8_t value, uint8_t *addr)和uint32_t __STREXH(uint16_t value, uint16_t *addr)，若行号不对应可以按照函数名查找。

### 将启动文件适用于arm-none-eabi-gcc编译器

MDK使用的启动文件和arm-none-eabi-gcc使用的启动文件语法格式不同。在之前下载的标准库文件中
```
STM32F10x_StdPeriph_Lib_V3.6.0/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7
```

这个文件夹下查找，找到单片机对应的启动文件。stm32f103c8t6适用于startup_stm32f10x_md.s这个启动文件。将这个启动文件复制到适用于ARM toolchain的项目的CMSIS/StartUp下，将原始启动文件备份后复制到这个文件夹即可。

### 将链接脚本（.ld文件）适用于arm-none-eabi-gcc编译器

推荐直接用官方模板中的.ld文件，这个文件存放在：

```
STM32F10x_StdPeriph_Lib_V3.6.0/Project/STM32F10x_StdPeriph_Template/TrueSTUDIO
```
文件夹下，这个文件夹下有多个子文件夹，其中的ld文件是针对不同型号的单片机设计的。针对stm32f103c8t6，使用STM3210C-EVAL/stm32_flash.ld。若不确定可以参考对应文件夹中的说明文档。

以上方法应当适用于大多数情况，但是实践过程中并不奏效（在编译后运行时，调用SystemInit时会进入HardFault_Handle）。下面是针对stm32f103c8t6的另一个解决方案，连接文件参考[一个可行的ld文件](https://github.com/microdynamics-robot-quadcopter/breeze_firmware_none/blob/master/make/Libraries/LinkScript/stm32f10x_flash.lds)，下载后更改文件名为stm32_flash.ld，存放再项目的linkscript下即可。

### 将MicroLIB替换为NEWLIB

如果在MDK中使用了MicorLIB，则需要将NEWLIB替换为NEWLIB，因为在arm-none-eabi-gcc中使用的是NEWLIB。体现在编译选项中即使用--specs=nano.specs选项。NEWLIB需要提供一些基本的接口，否则在链接过程中会报错。在newlib_interface文件夹下新建interface.c文件，编写以下内容先搪塞过去，让项目可以正常运行。如果你使用到了其中的函数（例如printf等），则需要完善整个文件中的函数：

```
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "stm32f10x_conf.h"
#include "stm32f10x.h"

#undef errno
extern int errno;

int _write(int file, char *ptr, int len) {
    int i;

    if (file != 1 && file != 2) {
        errno = EBADF;
        return -1;
    }

    for (i = 0; i < len; i++) {
        while (!(USART1->SR & USART_SR_TXE)); // 等待发送数据寄存器为空
        USART1->DR = (*ptr++ & (uint16_t)0x01FF); // 写DR会清除TXE标志
    }
    return len;
}


int _read(int file, char *ptr, int len) {
    int n;
    int num = 0;
    char c;

    if (file != 0) {
        errno = EBADF;
        return -1;
    }

    for (n = 0; n < len; n++) {
        while (!(USART1->SR & USART_SR_RXNE)); // 等待接收到数据
        c = (char)(USART1->DR & (uint16_t)0x01FF); // 读DR会清除RXNE标志
        *ptr++ = c;
        num++;
    }
    return num;
}

int _close(int file) {
    return -1;
}

int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

// 这个函数用于文件系统，stm32中不需要
int _isatty(int file) {
    return 1;
}

// 这个函数用于文件系统，stm32中不需要
int _lseek(int file, int ptr, int dir) {
    return 0;
}

// 这个函数用于malloc，如果使用到需要重写
char *_sbrk(int incr) {
	return 0x00;
}

void _exit(int status) {
    while (1) {}
}

int _kill(int pid, int sig) {
    return -1;
}

int _getpid(void) {
    return 1;
}
```

### 编写makefile用于管理整个项目

编写makefile首先需要设计编译这些文件需要那些编译器选项。可以从MDK的Options for Target界面的C/C++标签下，找到Compiler control string。一些选项在arm-none-eabi-gcc下不奏效，但是提供可替代的选项，对应更改即可。makefile基础可以参考[这篇文章](https://www.cnblogs.com/24fahed/articles/18123687)。以下是针对当前项目结构的makefile：

```
Project := example-template
# 编译工具
ARM_TOOLPREFIX := arm-none-eabi-

CC := $(ARM_TOOLPREFIX)gcc
AS := $(CC) -x assembler-with-cpp
DB := $(ARM_TOOLPREFIX)gdb
LD := $(ARM_TOOLPREFIX)ld

OBJCOPY := $(ARM_TOOLPREFIX)objcopy

# 工作路径
WORK_DIR = $(shell pwd)

# cortex-m3 core 代码和外设驱动代码所在文件夹
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

DIR_MAIN := $(WORK_DIR)/USER

# 由MICORLIB切换到NEWLIB需要使用的文件
DIR_NEWLIBAPI := $(WORK_DIR)/newlib_interface

# 组装包含文件夹
DIR_INCLUDE := $(addprefix -I, $(DIR_NEWLIBAPI)) \
	       $(addprefix -I, $(DIR_STM32CORE)) \
	       $(addprefix -I, $(DIR_STM32FWLIB)/inc) \
	       $(addprefix -I, $(DIR_BSP)) \
	       $(addprefix -I, $(DIR_MAIN)/includes)

# 查找所有需要编译的C语言源文件
SRC_C += $(wildcard $(addsuffix /*.c, $(DIR_NEWLIBAPI)))
SRC_C += $(wildcard $(addsuffix /*.c, $(DIR_STM32CORE)))
SRC_C += $(wildcard $(addsuffix /src/*.c, $(DIR_STM32FWLIB)))
SRC_C += $(wildcard $(addsuffix /*.c, $(DIR_BSP)))
SRC_C += $(wildcard $(addsuffix /*.c, $(DIR_MAIN)))

# 需要编译的汇编源文件
SRC_ASM := ./CMSIS/Startup/startup_stm32f10x_md.s

# C源文件编译结果的输出位置
OUT_C := $(patsubst %.c,%.o, $(SRC_C))
# 汇编源文件编译结果输出位置
OUT_ASM := $(patsubst %.s,%.o, $(SRC_ASM))

# 宏定义，USER_BOARD_SUPPORT是针对我实践的代码设计的，不是必要，STM32F10X_MD是针对stm32f10xc这种中等容量的芯片提供的。具体使用那些，可以从MDK的Options for Target 的C/C++标签，Preprocessor Symbles的define中查看。
DEFS := STM32F10X_MD USE_STDPERIPH_DRIVER USER_BOARD_SUPPORT

DDEFS := $(addprefix -D, $(DEFS))

# 针对stm32f103c8t6的编译信息
LINK_SCRIPT := ./linkscript/stm32_flash.ld
MCPU := cortex-m3

# 编译标志
SPECS := --specs=nano.specs -u _printf_float 
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

# 编译目标
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
```

将makefile存放在适用于ARM toolchain的项目的根目录下，并使用命令：make all，将自动编译整个项目；使用make clean将清除所有编译结果。最终将生成.elf，.hex，.bin三种格式的文件，根据需要烧录即可，也可以使用.elf文件通过gdb载入。

项目烧录：

```
st-flash write ./FILENAME.bin 0x8000000 # 针对stm32f10x从0x8000000开始
```

至此，就完全将一个MDK项目移植到了Ubuntu上，使用ARM Toolchain开发了。

# 项目调试

本项目使用stlink v2配合stlink-tools调试单片机。思路为使用arm-none-eabi-gdb链接运行st-util的本地端口调试。具体操作如下：

- 开启st-util

```
example@example:~ $ st-util 
st-util
2024-04-09T16:06:50 WARN common.c: NRST is not connected
2024-04-09T16:06:50 INFO common.c: F1xx Medium-density: 20 KiB SRAM, 64 KiB flash in at least 1 KiB pages.
2024-04-09T16:06:50 INFO gdb-server.c: Listening at *:4242...
```

如果出现以下输出，则st-util开始运行，gdb-server并监听4242端口。

- 链接gdb-server

输入arm-none-eabi-gdb进入gdb命令行，首先需要指定调试文件，

```
(gdb) file FILENAME.elf
```

再链接到gdb-server：

```
(gdb) target extended-remote :4242
Remote debugging using :4242
millis ()
    at /home/fahed/Source/stm32/roboot/other-source/example2/SRC/timer/y_timer.c:42
42	}
(gdb) 
```
如果出现以下输出，则说明已经链接到gdb-server，millis是接入时正在执行的函数名称。之后的所有的调试操作和gdb相同。

- 重新运行项目

如果需要重新运行，可以直接输入start或者run，gdb将结束当前进程并等待单片机复位后，重头执行，这个过程可能需要手动复位单片机，等待一会按下reset按钮即可。如果想让调试器在进入项目程序后就停止，可以在Reset_Handler上打断点。

# 附录

- stlink v2链接单片机

![连接单片机](https://img2024.cnblogs.com/blog/3423403/202404/3423403-20240409161902923-874181281.png)

（图片来源：Beginning STM32_ Developing with FreeRTOS, libopencm3 and -- Warren Gay -- Technology in Action, 1st ed., 2018 -- Apress）

- 编译选项（AI输出的）

--specs=nano.specs：使用为嵌入式系统优化的nano版本标准库，减小程序大小。
-u _printf_float：包含支持浮点数打印的代码，即使源代码中未直接使用浮点打印。
-fsingle-precision-constant：将浮点常量当作单精度处理，减少空间占用。
-fno-common：禁止生成"common"段，有助于减少未初始化的全局变量引起的内存浪费。
-ffunction-sections：将每个函数放在独立的段中，便于链接时优化移除未使用的函数。
-fdata-sections：将每个数据项放在独立的段中，便于链接时优化移除未使用的数据。
-mcpu=$(MCPU)：指定目标CPU类型，优化生成的代码以适应特定的微处理器。
-c：仅编译不链接，生成目标文件。
-g：生成调试信息。
-O0：不进行优化，便于调试。
-gdwarf-2：生成DWARF版本2的调试信息。
-mthumb：生成Thumb指令集的代码，通常用于ARM架构，减小代码大小。
-fomit-frame-pointer：省略帧指针，节省寄存器资源，可能会影响调试。
-Wall：开启所有警告消息。
-fverbose-asm：生成详细的汇编代码，便于分析。
-fno-exceptions：禁用异常处理，减小程序大小，C++特有。
-fno-rtti：禁用运行时类型信息，减小程序大小，C++特有。
-fno-threadsafe-statics：不生成线程安全的静态局部变量初始化代码，减小程序大小，C++特有。
-fvisibility=hidden：默认隐藏符号，减小程序大小，C++特有。
-std=c++11：使用C++11标准编译程序。
-lm：链接数学库。
-nostartfiles：不使用标准系统启动文件，常见于裸机或操作系统内核编程。
-Xlinker：传递选项给链接器。
--gc-sections：在链接时移除未使用的段，减小程序大小。
-T$(LINK_SCRIPT)：指定链接器脚本，用于控制内存布局。
-Wl,-Map=$(PROJECT).map：生成map文件，列出程序的内存布局。
--cref：在map文件中生成交叉引用表。
--no-warn-mismatch：忽略链接时的目标文件和库的架构不匹配警告。

# 参考文章

[STM32 gcc编译环境搭建](https://blog.csdn.net/Chuancey_CC/article/details/78474001)

[makefile概述](https://www.cnblogs.com/24fahed/articles/18123687)

[链接文件](https://github.com/microdynamics-robot-quadcopter/breeze_firmware_none/blob/master/make/Libraries/LinkScript/stm32f10x_flash.lds)

[GCC linker script and STM32](https://www.stf12.org/developers/freerots_ec-linker_script.html)

[STM32开发环境搭建(ARM-GCC)](https://microdynamics.github.io/1.%20Breeze%20Mini%E5%9B%9B%E8%BD%B4%E9%A3%9E%E8%A1%8C%E5%99%A8/2.2%20STM32%E5%BC%80%E5%8F%91%E7%8E%AF%E5%A2%83%E6%90%AD%E5%BB%BA%28ARM-GCC%29/)

[How to debug a HardFault on an ARM Cortex-M MCU](https://interrupt.memfault.com/blog/cortex-m-hardfault-debug)

Gay, W. (2018). Beginning STM32: Developing with FreeRTOS, libopencm3 and GCC. Apress.



