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

int foo(int arg);
int bar(int arg);
int read_cmd(byte *buf);
int write_cmd(byte *buf, int len);
int read_exact(byte *buf, int len);
int write_exact(byte *buf, int len);

//typedef unsigned char byte;


int foo(int arg){
	byte buf[10];
	int i;
	for(i=arg; i>0; i--) {
			buf[0] = i;
    	write_cmd(buf, 1);
    	sleep(2);
	}
	
	return 255;

}

//reverse byte (0b0101 -> 0b1010)
int bar(int arg){
	arg = (arg & 0xF0) >> 4 | (arg & 0x0F) << 4;
	arg = (arg & 0xCC) >> 2 | (arg & 0x33) << 2;
	arg = (arg & 0xAA) >> 1 | (arg & 0x55) << 1;
	   return arg;

}





int main() {
  int fn, arg, res;
  byte buf[100];
  while (read_cmd(buf) > 0) {
    fn = buf[0];
    arg = buf[1];

    if (fn == 1) {
      res = foo(arg);
    } else if (fn == 2) {
      res = bar(arg);
    } else if (fn == 3) {
	res = 7;
    }

		buf[0] = 3;
    buf[1] = res;
    write_cmd(buf, 2);
  }
  	  return 1;
}




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
