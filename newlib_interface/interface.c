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
