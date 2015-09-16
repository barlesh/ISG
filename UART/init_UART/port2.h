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

#define START_BYTE	0
#define END_BYTE		255
#define REG_BYTE		1

#define PACKET_SIZE	66

mraa_uart_context uart;
FILE* src;

int inoming_modem();
int incoming_erlang();
int get_modem_frame(unsigned char* buffer);
int get_erlang_frame(unsigned char* uart_buf);
void sendToErlang(unsigned char *buffer, int len);
void sendToModem(unsigned char* buffer, int len);
int init_modem();

void init_file(FILE* src);
void writeToFile2(FILE* src ,char* str, int len);
void writeToFile(FILE* src ,char* str);
