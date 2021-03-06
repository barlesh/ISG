/*
 * port.c
 *
 *  Created on: Aug 23, 2015
 *      Author: barlesh
 */

#include <time.h>
#include "port.h"

typedef unsigned char byte;

#define START_BYTE	0
#define END_BYTE		255
#define REG_BYTE		1
#define OK					1
#define ERROR				-1


void init_file(FILE* src);
void writeToFile(FILE* src ,char* str);
int read_cmd(byte *buf);
int write_cmd(byte *buf, int len);
int read_exact(byte *buf, int len);
int write_exact(byte *buf, int len);
int sendByUART(unsigned char* buf,int len);
int sendBufferToErlang(char * buff, int len);





/**this function send a byte to erlang process via stdout (using write_cmd)
**Parameter: int result - byte to send
**/
int sendToErlang(int result){
	unsigned char buf[10];
	buf[0] = result;
	return write_cmd(buf,1);
}


int sendBufferToErlang(char * buff, int len){
	char temp_buff[10];
	int i=1;
	temp_buff[0] = START_BYTE;
	temp_buff[1] = buff[0];
	write_cmd(temp_buff, 2);
	for(i; i<len-1; i++) {
			temp_buff[0] = REG_BYTE;
			temp_buff[1] = buff[i];
			write_cmd(temp_buff, 2);
	}//for
	temp_buff[0] = END_BYTE;
	temp_buff[1] = buff[i];
	write_cmd(temp_buff, 2);
}



/**this function write to file (log) the last buffer sent.
**Parameter: FILE* src - a pointer for a file descriptor to handle file. int type - first, last or middle byte of info,
** unsigned char byte - the byte to write to file
**/
void writeToFile(FILE* src ,char* str){
	if ( (src = fopen("/home/ISG/uart/rec_log.txt", "a")  ) < 0) {exit(-10);}
	if( (fputs(str, src) ) <0 ) {exit(-11);}
	fclose(src);
}

void init_file(FILE* src){
	if ( (src = fopen("/home/ISG/uart/rec_log.txt", "w+")  ) < 0) {exit(-10);}
	if( (fputs("starting c_transmitter log:\n", src) ) <0 ) {exit(-11);}
	fclose(src);
}


/**TODO
**/
int main() {

  int flag, b, index=0, len=0;
  byte i=1;
	FILE* src;
	init_file(src);
	while(1){}
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
