/*
 * port.c
 *
 *  Created on: Aug 23, 2015
 *      Author: barlesh
 */

#include "port.h"
//#include <mraa.h>

int main() {
    mraa_uart_context uart;
    uart = mraa_uart_init(0);
    if (uart == NULL) {
        exit(-1);
    }
    
    mraa_uart_set_baudrate(uart, UART_BOUD_RATE);		//set uart boud rate [bps]
    
    mraa_uart_set_mode(uart, 8,MRAA_UART_PARITY_NONE , 1);
//settings (8 bit par frame, no parity bit, single stop bit
  
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
