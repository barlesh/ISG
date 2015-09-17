#include "port3.h"

int main() {
	int x=0, len=0;
	unsigned char uart_buffer[100], erlang_buffer[100];
	init_file(src);
	init_modem();
	
	while(1){
		if(is_incoming_erlang()){
			
		
		
		}//if(incoming_erlang)
	
		if(is_incoming_modem() > 0 ){
			writeToFile(src, "Data from UART available. trying to get data\n");
			len = get_modem_frame(uart_buffer);
			writeToFile(src, "got data:\n");
			writeToFile2(src, uart_buffer, len);
			send_to_erlang(uart_buffer, len);
			
			
		}//if(incoming from moden)
	
	}//while(1)
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////		Erlang related functions		///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int is_incoming_erlang(){
	return 0;
}

int send_to_erlang(unsigned char* uart_buffer, int len){
	int i;
	for(i=0;i<len;i++){
		write_cmd(uart_buffer+i,1);
	}
	
	return len;

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////	Modem related functions		///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int init_modem(){
	uart = mraa_uart_init(0);
	 if ( mraa_uart_set_baudrate(uart, UART_BOUD_RATE)!=MRAA_SUCCESS) {
  		writeToFile(src, "error seting baudrate\n");
  		exit(UART_ERROR);
  		}		//set uart boud rate [bps]
  if ( mraa_uart_set_mode(uart, 8,MRAA_UART_PARITY_NONE , 1)!=MRAA_SUCCESS) {
  writeToFile(src, "error seting mode\n");
	}
}


int is_incoming_modem(){
	return mraa_uart_data_available(uart,0);
}

int get_modem_frame(unsigned char* uart_buffer){
	return mraa_uart_read(uart, uart_buffer, 20);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////	File Handlers		/////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**this function write to file (log) the last buffer sent.
**Parameter: FILE* src - a pointer for a file descriptor to handle file. int type - first, last or middle byte of info,
** unsigned char byte - the byte to write to file
**/
void writeToFile(FILE* src ,char* str){
	if ( (src = fopen("/home/ISG/uart/port3.txt", "a")  ) < 0) {exit(FILE_ERROR);}
	if( (fputs(str, src) ) <0 ) {exit(-11);}
	fclose(src);
}


void writeToFile2(FILE* src ,char* str, int len){
	int i=0;
	if ( (src = fopen("/home/ISG/uart/port3.txt", "a")  ) < 0) {exit(FILE_ERROR);}
	for(;i<len;i++){
		if(fputc(str[i], src) == EOF) { exit(-11);}
	}
	fclose(src);
}

void init_file(FILE* src){
	if ( (src = fopen("/home/ISG/uart/port3.txt", "w+")  ) < 0) {exit(FILE_ERROR);}
	if( (fputs("starting  log:\n", src) ) <0 ) {exit(FILE_ERROR);}
	fclose(src);
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////	Erlang <--> C Pipeliling		///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


