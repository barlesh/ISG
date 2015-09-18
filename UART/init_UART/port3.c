#include "port3.h"

int main() {
	int x=0, len=0;
	char msg[100];
	unsigned char uart_buffer[MODEM_PACKET_SIZE], erlang_buffer[MODEM_PACKET_SIZE*2];
	init_file(src);
	writeToFile2(src, "starting log\n", 13 );
	init_modem();
	//flush_stdin();
	
	
	while(1){
		//writeToFile2(src, "in while(1)\n", 12 );
			len = get_erlang_frame(erlang_buffer);
			if(len>0) {
					writeToFile2(src, "got erl data:\n", 14);
					writeToFile2(src, erlang_buffer, len);
					writeToFile2(src, "\n", 1);
					sendToModem(erlang_buffer, len+1);
					memset(erlang_buffer,0,MODEM_PACKET_SIZE*2);
					len = 0;
			}
		else
		if(is_incoming_modem() > 0 ){
			//writeToFile2(src, "UART data available\n", 20 );
			//usleep(10); // let data stream through uart
			len = get_modem_frame(uart_buffer);
			writeToFile2(src, "data:\n", 6);
			writeToFile2(src, uart_buffer, len);
			usleep(10);
			x = send_to_erlang(uart_buffer, len);
			memset(uart_buffer,0,MODEM_PACKET_SIZE);
			writeToFile2(src, "reset uart_buffer\n", 18);
			
			
		}//if(incoming from moden)
	
	}//while(1)
	mraa_uart_stop(uart);
}

void flush_stdin(){
		char buff[100];
		int x;
		while ( (x=read(0, buff, 10)) > 0 ) {writeToFile2(src, buff, x); x=0;}
		writeToFile2(src, "flushed\n", 8);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////		Erlang related functions		///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int is_incoming_erlang(){
	char buff[2];
	if(read(0, buff, 1) > 0) { fseek(0,0,SEEK_SET);}
	else {return 0;} 
		
}

int send_to_erlang(unsigned char* uart_buffer, int len){
		//int x;
	//for(i=0;i<len;i++){
		//if( (x=write_cmd(uart_buffer,len))<1 ) exit(-111);
		//else return x;
		writeToFile2(src, "sent it to erl\n", 15);
		return write(1, uart_buffer, len);
	}

int get_erlang_frame(unsigned char* erlang_buffer){
  int type, b, index=0, len=0;
  byte ans[10], rec_buf[10];
  		while (read_cmd(rec_buf) > 0) {
  				writeToFile2(src, "*" ,1);
  				type = rec_buf[0];
					b = rec_buf[1];
					if(type==EOF || b == EOF) { exit(-1); }					
  				switch(type) {
  						case START_BYTE:
  									if( memset(erlang_buffer, 0, MODEM_PACKET_SIZE*2) == NULL) {exit(MEMORY_ERROR);}
  									if(index!=0 || len !=0) { return 0; break;} 
  									erlang_buffer[index]=b;
  									index++;	len++;
  							break;
  					
  						case REG_BYTE:
  									if(index == 0 || len == 0) { return 0; break;}
  									erlang_buffer[index]=b;
  									index++;	len++;
  							break;
  							
  						case END_BYTE:
  									writeToFile(src, "got last BYTE\n");
  									if(index == 0 || len == 0) { return 0; break;}  
  									erlang_buffer[index]=b;	index++; 
  									return len;							
  							break;
  						case ASK_FOR_INCOMING_BYTE:
  									return 0;
  									break;
  						default : return 0; break;
  				}//switch
  		}//while(read_cmd>0)  
	return 0;
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
	//return 0;
}

int get_modem_frame(unsigned char* uart_buffer){
	return mraa_uart_read(uart, uart_buffer, MODEM_PACKET_SIZE);
}


void sendToModem(unsigned char* buffer, int len){
	char temp[5];
	char msg[50];
	int ans=0;
	writeToFile2(src, buffer,len);
	ans = mraa_uart_write( uart , buffer, len);
	sprintf(msg, "try to write to uart %d [byte].\twrote %d [byte]\n", len, ans);
	writeToFile2(src, msg,46);
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
	for(i;i<len;i++){
		if(fputc(str[i], src) == EOF) { exit(-11);}
	}
	fclose(src);
}

void init_file(FILE* src){
	if ( (src = fopen("/home/ISG/uart/port3.txt", "w+")  ) < 0) {exit(FILE_ERROR);}
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


