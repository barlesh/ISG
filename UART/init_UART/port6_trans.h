#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
//#include <sys/time.h>
#include <sys/resource.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
        
#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

typedef unsigned char byte;
#define OK					1
#define ERROR				-1
//#define	UART_BOUD_RATE	9600
#define	UART_BOUD_RATE	38400
#define MODEM_PACKET_SIZE	66
#define FILE_ERROR	-10
#define MEMORY_ERROR	-11
#define UART_ERROR	-12

#define START_BYTE	0
#define END_BYTE		255
#define REG_BYTE		1

#define PACKET_SIZE	66

//mraa_uart_context uart;
FILE* src;

unsigned short createMsg(char *buffer, int payload , int header_size);
void showMsg(char *buffer, unsigned short len);


int init_modem();
void sendToModem(int fd, char* buffer, int len);
void send_garbage(int len);

void init_file(FILE* src);
void writeToFile2(FILE* src ,char* str, int len);
void writeToFile(FILE* src ,char* str);

//unsigned long get_process_time();
