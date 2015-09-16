/*
 * port.c
 *
 *  Created on: Aug 23, 2015
 *      Author: barlesh
 */

#include "port.h"
//#include <mraa.h>

/**		GLOBALS		**/
FILE* src;





void init_uart(){
	writeToFile(src, "started initiating UART\n");	
	set_uart(); 
	writeToFile(src, "finished initiating UART\n");
	writeToFile(src, "at init aurt::printing  UART:\n");
	writeToFile2(src,(char*)&uart, 5);
  if ( mraa_uart_set_baudrate(get_uart(), UART_BOUD_RATE)!=MRAA_SUCCESS) {
  		writeToFile(src, "error seting baudrate\n");
  		exit(UART_ERROR);
  		}		//set uart boud rate [bps]
  if ( mraa_uart_set_mode(get_uart(), 8,MRAA_UART_PARITY_NONE , 1)!=MRAA_SUCCESS) {
  writeToFile(src, "error seting mode\n");
  exit(UART_ERROR);
  }
}


int main() {
	init_file(src);
	init_uart();
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




/**this function write to file (log) the last buffer sent.
**Parameter: FILE* src - a pointer for a file descriptor to handle file. int type - first, last or middle byte of info,
** unsigned char byte - the byte to write to file
**/
void writeToFile(FILE* src ,char* str){
	if ( (src = fopen("/home/ISG/uart/init_log.txt", "a")  ) < 0) {exit(FILE_ERROR);}
	if( (fputs(str, src) ) <0 ) {exit(-11);}
	fclose(src);
}


void writeToFile2(FILE* src ,char* str, int len){
	int i=0;
	if ( (src = fopen("/home/ISG/uart/init_log.txt", "a")  ) < 0) {exit(FILE_ERROR);}
	for(;i<len;i++){
		if(fputc(str[i], src) == EOF) { exit(-11);}
	}
	fclose(src);
}

void init_file(FILE* src){
	if ( (src = fopen("/home/ISG/uart/init_log.txt", "w+")  ) < 0) {exit(FILE_ERROR);}
	if( (fputs("starting c_transmitter log:\n", src) ) <0 ) {exit(FILE_ERROR);}
	fclose(src);
}
