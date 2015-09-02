/*
 * port.c
 *
 *  Created on: Aug 23, 2015
 *      Author: barlesh
 */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;

#define START_BYTE	0
#define END_BYTE	255
#define REG_BYTE	1
#define OK				1
#define ERROR				-1

int sendToErlang(int result);
void writeToFile(FILE* src, int type, unsigned char byte);
int read_cmd(byte *buf);
int write_cmd(byte *buf, int len);
int read_exact(byte *buf, int len);
int write_exact(byte *buf, int len);
int sendByUART(unsigned char* buf,int len);

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

	return 1;
}

/**this function write to file (log) the last buffer sent.
**Parameter: FILE* src - a pointer for a file descriptor to handle file. int type - first, last or middle byte of info,
** unsigned char byte - the byte to write to file
**/
void writeToFile(FILE* src, int type, unsigned char byte){
	switch(type){
			case START_BYTE:
				src=fopen("/home/barlesh/ISG/Erlang - C connection/connect_to_erlang/filename.txt", "w");
				fputc(byte, src);
				fclose(src);
				break;
			case REG_BYTE:
				src=fopen("/home/barlesh/ISG/Erlang - C connection/connect_to_erlang/filename.txt", "a");
				fputc(byte, src);
				fclose(src);
				break;
			case END_BYTE:
				src=fopen("/home/barlesh/ISG/Erlang - C connection/connect_to_erlang/filename.txt", "a");
				fputc(byte, src);
				//fputc('\0', src);
				fclose(src);
				break;
		
		}//switch

}

/**man function of this module. constently polling stdin for receiving paired of bytes - header & informatio from erlang process.
** by receiving, it decides if byte is first byte of series, last or "middle" (using "flag" - header - byte).
**after receiving the byte if either collect it to buffer, waiting for closin byte (flag = 255), and then sending via UART.
**in addition, write the series to file
**/
int main() {
  int flag, b, index=0, len=0;
  byte buf[100] ,ans[10];
	FILE* src;
  while(1){
  		while (read_cmd(buf) > 0) {
  				flag = buf[0];
					b = buf[1];
  				switch(flag) {
  						case START_BYTE:
  									if(index!=0 || len !=0) { sendToErlang(ERROR); break;} 
  									buf[index]=b;
  									index++;
  									len++;
  									writeToFile(src,flag,b);
  							break;
  					
  						case REG_BYTE:
  									if(index == 0 || len == 0) { sendToErlang(ERROR); break;}
  									buf[index]=b;
  									index++; len++;
  									writeToFile(src,flag,b);
  							break;
  							
  						case END_BYTE:
  									if(index == 0 || len == 0) { sendToErlang(ERROR); break;} 
  									buf[index]=b;
  									sendByUART(buf,len);
  									len = 0; index =0;
  									sendToErlang(OK);
  									writeToFile(src,flag,b);								
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
