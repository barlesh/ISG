
#include "port2.h"


int incoming_modem(){
	return mraa_uart_data_available(uart,0);

}

int incoming_erlang(){
	//if (feof(stdin)) return 0;
	//else return 1;
	return 1;

}

int get_modem_frame(unsigned char* buffer){
	return mraa_uart_read(uart, buffer, 66); 	

}

int get_erlang_frame(unsigned char* uart_buf){
  int type, b, index=0, len=0;
  byte ans[10], rec_buf[10];
	int flag = 1;
	writeToFile(src, "start getting frame from erlang\n"); 
	while(flag){
  		while (read_cmd(rec_buf) > 0) {
  		writeToFile(src, "got 2 bytes\n");
  				type = rec_buf[0];
					b = rec_buf[1];					
  				switch(type) {
  						case START_BYTE:
  									if( memset(uart_buf, 0, sizeof(uart_buf)) == NULL) {exit(MEMORY_ERROR);}
  									writeToFile(src, "got first BYTE\n");
  									if(index!=0 || len !=0) { return 0; break;} 
  									uart_buf[index]=b;
  									index++;	len++;
  							break;
  					
  						case REG_BYTE:
  									if(index == 0 || len == 0) { return 0; break;}
  									writeToFile(src, "got some BYTE\n");
  									uart_buf[index]=b;
  									index++;	len++;
  							break;
  							
  						case END_BYTE:
  									writeToFile(src, "got last BYTE\n");
  									if(index == 0 || len == 0) { return 0; break;}  
  									uart_buf[index]=b;	index++; flag =0;
  									//sendByUART(uart_buf,len+1);
  									//sendToErlang(OK); 
  									return len;							
  							break;
  				}//switch
  		}//while(read_cmd>0)  
  }//while(flag)
	return 1;
}

void sendToErlang(unsigned char *buffer, int len){
		int i=0;
		for(;i<len;i++){
			write_cmd(buffer,1);
		}

}


void sendToModem(unsigned char* buffer, int len){
	char temp[5];
	char msg[100];
	int ans=0;
	
	writeToFile(src, "at sendToModem::printing  ptr_UART:\n");
	writeToFile2(src, (char*)&uart, 5);
	
	writeToFile(src, "\ntry to send by UART\n");
	
	ans = mraa_uart_write( uart , buffer, len);
	sprintf(msg, "try to write to uart %d [byte].\twrote %d [byte]\n", len, ans);
	writeToFile(src, msg);
}

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



int main() {
	int len;
	unsigned char buffer[100];
	init_file(src);
	init_modem();

	while(1){
			if(incoming_modem()) {			//if msg is being sent from modem via uart
				writeToFile(src, "data available from uart\n"); 			
				len = get_modem_frame(buffer);		//get the msg from modem
				sendToErlang(buffer, len);				//send the frame to erlang app
				memset(buffer,0,sizeof(buffer)); len = 0;
			}//if(incoming_uart)
			
			if(incoming_erlang()){
				writeToFile(src, "got data from erlang\n");
				len = get_erlang_frame(buffer);
				sendToModem(buffer, len+1);
				memset(buffer,0,sizeof(buffer)); len =0;
			
			}
	}//while(1)

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
	if ( (src = fopen("/home/ISG/uart/port2.txt", "a")  ) < 0) {exit(FILE_ERROR);}
	if( (fputs(str, src) ) <0 ) {exit(-11);}
	fclose(src);
}


void writeToFile2(FILE* src ,char* str, int len){
	int i=0;
	if ( (src = fopen("/home/ISG/uart/port2.txt", "a")  ) < 0) {exit(FILE_ERROR);}
	for(;i<len;i++){
		if(fputc(str[i], src) == EOF) { exit(-11);}
	}
	fclose(src);
}

void init_file(FILE* src){
	if ( (src = fopen("/home/ISG/uart/port2.txt", "w+")  ) < 0) {exit(FILE_ERROR);}
	if( (fputs("starting c_transmitter log:\n", src) ) <0 ) {exit(FILE_ERROR);}
	fclose(src);
}

