#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

#include "mraa.h"

typedef unsigned char byte;
#define OK					1
#define ERROR				-1
#define	UART_BOUD_RATE	38400
#define MODEM_PACKET_SIZE	66
#define FILE_ERROR	-10
#define MEMORY_ERROR	-11
#define UART_ERROR	-12

#define START_BYTE	0
#define END_BYTE		255
#define REG_BYTE		1
#define ASK_FOR_INCOMING_BYTE 254

#define PACKET_SIZE	66

mraa_uart_context uart;
FILE* src;

int is_incoming_modem();
int is_incoming_erlang();
int get_modem_frame(unsigned char* uart_buffer);
int get_erlang_frame(unsigned char* erlang_buffer);
int send_to_erlang(unsigned char* uart_buffer, int len);
void sendToModem(unsigned char* buffer, int len);
int init_modem();

void flush_stdin();

void init_file(FILE* src);
void writeToFile2(FILE* src ,char* str, int len);
void writeToFile(FILE* src ,char* str);
