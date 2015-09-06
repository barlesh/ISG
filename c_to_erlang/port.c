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
#include <time.h>

typedef unsigned char byte;

#define START_BYTE	0
#define END_BYTE		255
#define REG_BYTE		1
#define OK					1
#define ERROR				-1


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

/**TODO
**/
int main() {
  int flag, b, index=0, len=0;
  byte i=1;
  byte buff[16] = {'m','a','s','s','a','g','e',' ','n','u','m','b','e','r',' ','X'};
  //printf("starting\n");
  //printf("%s\n", buff);
	//srand(time(NULL));
	int r = 3;// rand() % 10;
	while(i>0){
		sleep(r);
		buff[15] = i + '0';
		sendBufferToErlang(buff, 16);
		i++;	
		//r = rand() % 10;
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
