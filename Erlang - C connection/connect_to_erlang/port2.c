/*
 * port2.c
 *
 *  Created on: Aug 23, 2015
 *      Author: barlesh
 */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define START_BYTE	0
#define END_BYTE	255
#define REG_BYTE	1
#define OK				1

int main(){
	int flag;
	FILE* src;
	char end = '\n', b;
	unsigned char buf[100];
	
	while (read_cmd(buf) > 0) {
		flag = buf[0];
		b = buf[1];
		switch(flag){
			case START_BYTE:
				//TODO: DELETE FILE IF EXIST
				src=fopen("/home/barlesh/ISG/Erlang - C connection/connect_to_erlang/filename.txt", "w");
				fputc(b, src);
				fclose(src);
				buf[0] = OK;
    		write_cmd(buf, 1);
				break;
			case REG_BYTE:
				src=fopen("/home/barlesh/ISG/Erlang - C connection/connect_to_erlang/filename.txt", "a");
				fputc(b, src);
				fclose(src);
				buf[0] = OK;
    		write_cmd(buf, 1);
				break;
			case END_BYTE:
				src=fopen("/home/barlesh/ISG/Erlang - C connection/connect_to_erlang/filename.txt", "a");
				fputc(b, src);
				fputc(end, src);
				fclose(src);
				buf[0] = OK;
    		write_cmd(buf, 1);
				break;
		
		}//switch
	
	}//while


}//MAIN



int read_cmd(unsigned char *buf)
{
  int len;

  if (read_exact(buf, 2) != 2)
    return(-1);
  len = (buf[0] << 8) | buf[1];
  return read_exact(buf, len);
}

int write_cmd(unsigned char *buf, int len)
{
  unsigned char li;

  li = (len >> 8) & 0xff;
  write_exact(&li, 1);

  li = len & 0xff;
  write_exact(&li, 1);

  return write_exact(buf, len);
}

int read_exact(unsigned char *buf, int len)
{
  int i, got=0;

  do {
    if ((i = read(0, buf+got, len-got)) <= 0)
      return(i);
    got += i;
  } while (got<len);

  return(len);
}

int write_exact(unsigned char *buf, int len)
{
  int i, wrote = 0;

  do {
    if ((i = write(1, buf+wrote, len-wrote)) <= 0)
      return (i);
    wrote += i;
  } while (wrote<len);

  return (len);
}
