/*
 * port.c
 *
 *  Created on: Aug 23, 2015
 *      Author: barlesh
 */

#include "port.h"

typedef unsigned char byte;

#define START_BYTE	0
#define END_BYTE		255
#define REG_BYTE		1
#define OK					1
#define ERROR				-1
#define PACKET_SIZE	66

#define FILE_ERROR	-10
#define MEMORY_ERROR	-11
#define UART_ERROR	-12

int sendToErlang(int result);
void writeToFile2(FILE* src ,char* str, int len);
void writeToFile(FILE* src ,char* str);
void init_uart();
void init_file(FILE* src);
int read_cmd(byte *buf);
int write_cmd(byte *buf, int len);
int read_exact(byte *buf, int len);
int write_exact(byte *buf, int len);
int sendByUART(unsigned char* buf,int len);


/**		GLOBALS		**/
FILE* src;
mraa_uart_context uart;


/**this function send a byte to erlang process via stdout (using write_cmd)
**Parameter: int result - byte to send
**/
int sendToErlang(int result){
	unsigned char buf[10];
	buf[0] = result;
	return write_cmd(buf,1);
}

/**this function send a series of bytes (buf) of lengh len through UART using mraa lib 
**Parameter: unsigned char buf - location of buffer, int len - number of bytes to send via UART
**/
int sendByUART(unsigned char* buf,int len){
	char temp[5];
	char msg[100];
	int ans;
	if((ans = mraa_uart_write(uart, buf, len))!=len) {
		sprintf(msg, "try to write to uart %d bytes but wrote %d\n", len, ans);
		writeToFile(src, msg);
	}
	return 1;
}

/**this function write to file (log) the last buffer sent.
**Parameter: FILE* src - a pointer for a file descriptor to handle file. int type - first, last or middle byte of info,
** unsigned char byte - the byte to write to file
**/
void writeToFile(FILE* src ,char* str){
	if ( (src = fopen("/home/ISG/uart/trans_log.txt", "a")  ) < 0) {exit(FILE_ERROR);}
	if( (fputs(str, src) ) <0 ) {exit(-11);}
	fclose(src);
}


void writeToFile2(FILE* src ,char* str, int len){
	int i=0;
	if ( (src = fopen("/home/ISG/uart/trans_log.txt", "a")  ) < 0) {exit(FILE_ERROR);}
	for(;i<len;i++){
		if(fputc(str[i], src) == EOF) { exit(-11);}
	}
	fclose(src);
}

void init_file(FILE* src){
	if ( (src = fopen("/home/ISG/uart/trans_log.txt", "w+")  ) < 0) {exit(FILE_ERROR);}
	if( (fputs("starting c_transmitter log:\n", src) ) <0 ) {exit(FILE_ERROR);}
	fclose(src);
}

void init_uart(){
	if( (uart = mraa_uart_init(0)) == NULL) {exit(UART_ERROR);}
  if ( mraa_uart_set_baudrate(uart, UART_BOUD_RATE)!=MRAA_SUCCESS) {exit(UART_ERROR);}		//set uart boud rate [bps]
  if ( mraa_uart_set_mode(uart, 8,MRAA_UART_PARITY_NONE , 1)!=MRAA_SUCCESS) {exit(UART_ERROR);}
}

/**main function of this module. constently polling stdin for receiving paired of bytes - header & informatio from erlang process.
** by receiving, it decides if byte is first byte of series, last or "middle" (using "flag" - header - byte).
**after receiving the byte it collect it to buffer, waiting for closing byte (flag = 255), and then sending via UART.
**in addition, write the series to file
**/
int main() {
  int flag, b, index=0, len=0;
  byte uart_buf[PACKET_SIZE] ,ans[10], rec_buf[10];
	
	init_file(src);
	init_uart();
  
  while(1){
  		while (read_cmd(rec_buf) > 0) {
  				flag = rec_buf[0];
					b = rec_buf[1];					
  				switch(flag) {
  						case START_BYTE:
  									if( memset(uart_buf, 0, sizeof(uart_buf)) == NULL) {exit(MEMORY_ERROR);}
  									if(index!=0 || len !=0) { sendToErlang(ERROR); break;} 
  									uart_buf[index]=b;
  									index++;	len++;
  							break;
  					
  						case REG_BYTE:
  									if(index == 0 || len == 0) { sendToErlang(ERROR); break;}
  									uart_buf[index]=b;
  									index++;	len++;
  							break;
  							
  						case END_BYTE:
  									if(index == 0 || len == 0) { sendToErlang(ERROR); break;}  
  									uart_buf[index]=b;	index++;
  									sendByUART(uart_buf,len+1);
  									sendToErlang(OK); 
  									len = 0; index =0;								
  							break;
  				}//switch
  		}//while(read_cmd>0)  
  }//while1
  	  return 1;
}



/**FUNCTION FOR PIPLINING WITH ERLANG THROUGH STDIN, STDOUT
** Source - erlang officel documents example
**/
int read_cmd(byte *buf)
{
  int len;

  if (read_exact(buf, 2) != 2)
    return(-1);
  len = (buf[0] << 8) | buf[1];
  return read_exact(buf, len);
}

int write_cmd(byte *buf, int len)
{
  byte li;

  li = (len >> 8) & 0xff;
  write_exact(&li, 1);

  li = len & 0xff;
  write_exact(&li, 1);

  return write_exact(buf, len);
}

int read_exact(byte *buf, int len)
{
  int i, got=0;

  do {
    if ((i = read(0, buf+got, len-got)) <= 0)
      return(i);
    got += i;
  } while (got<len);

  return(len);
}

int write_exact(byte *buf, int len)
{
  int i, wrote = 0;

  do {
    if ((i = write(1, buf+wrote, len-wrote)) <= 0)
      return (i);
    wrote += i;
  } while (wrote<len);

  return (len);
}
