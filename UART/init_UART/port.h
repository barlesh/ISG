#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mraa.h"

typedef unsigned char byte;
#define OK					1
#define ERROR				-1
#define	UART_BOUD_RATE	38400
#define FILE_ERROR	-10
#define MEMORY_ERROR	-11
#define UART_ERROR	-12

static mraa_uart_context uart;

void set_uart();
mraa_uart_context* get_ptr_uart();
void init_uart();

void init_file(FILE* src);
void writeToFile2(FILE* src ,char* str, int len);
void writeToFile(FILE* src ,char* str);

//void set_uart(mraa_uart_context new_uart);

mraa_uart_context get_uart(){
	return uart;
}

void set_uart(){
	uart = mraa_uart_init(0);
} 


