#include "port4.h"





void main(){
		char modem_buffer[100];
		char erlang_buffer[MODEM_PACKET_SIZE];
		
		int m_len=0, e_len=0;
		
		init_modem();
		init_file(src);
		set_nonblock_flag(0,1);
		writeToFile(src, "init\n");
		while(1){
			if( is_incoming_modem() > 0){
				writeToFile(src, "\ndata from modem\n");
				m_len = get_modem_frame(modem_buffer);
				if(m_len>0){
					writeToFile(src, "\ngot the data:\n");
					writeToFile2(src, modem_buffer, m_len);
					send_to_erlang(modem_buffer, m_len);
				
					}//if(m_len>0)
			
			}//if(modem)
			e_len = get_erlang_frame(erlang_buffer);
			if(e_len>0) {
					writeToFile2(src, "\ngot erl data:\n", 14);
					writeToFile2(src, erlang_buffer, e_len);
					writeToFile2(src, "\n", 1);
					sendToModem(erlang_buffer, e_len);
					memset(erlang_buffer,0,MODEM_PACKET_SIZE);
				}//if(e_len)
			e_len = 0;
			m_len = 0;
			
		}//while(1)
		
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////		Erlang related functions		///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int send_to_erlang(unsigned char* uart_buffer, int len){
		//int x;
	//for(i=0;i<len;i++){
		//if( (x=write_cmd(uart_buffer,len))<1 ) exit(-111);
		//else return x;
		writeToFile2(src, "sent it to erl\n", 15);
		return write_cmd(uart_buffer,len);
	}
	
int get_erlang_frame(unsigned char* erlang_buffer){
	int type, b, index=0, len=0, flag=1;
  byte ans[10], rec_buf[100];
  while(flag){
  	if(read_cmd(rec_buf) > 0 ){
  				type = rec_buf[0];
					b = rec_buf[1];
					//if(type==EOF || b == EOF) { exit(-1); }					
  				switch(type) {
  						case START_BYTE:
  									writeToFile(src, "\ngot start BYTE:\n");
  									if( memset(erlang_buffer, 0, MODEM_PACKET_SIZE) == NULL) {exit(MEMORY_ERROR);}
  									//if(index!=0 || len !=0) { return 0; break;} 
  									erlang_buffer[index]=b;
  									index++;	len++;
  									writeToFile2(src, erlang_buffer, len);
  							break;
  					
  						case REG_BYTE:
  									//if(index == 0 || len == 0) { return 0; break;}
  									erlang_buffer[index]=b;
  									index++;	len++;
  							break;
  							
  						case END_BYTE:
  									writeToFile(src, "\ngot last BYTE\n");
  									//if(index == 0 || len == 0) { return 0; break;}  
  									erlang_buffer[index]=b;	index++; len++; 
  									writeToFile2(src, erlang_buffer ,len);
  									return len;							
  							break;

  						default : return 0; break;
  				}//switch
  	
  	}//if(read)
  	else
  		flag =0;
  
  }//while(flag)
  		/*while (read_cmd(rec_buf) > 0) {
  		//while( read(0, rec_buf, MODEM_PACKET_SIZE) > 0 ){
  				writeToFile2(src, "4" ,1);
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

  						default : return 0; break;
  				}//switch
  		}//while(read_cmd>0)*/
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
	char msg[40];
	int x = mraa_uart_data_available(uart,0);
	return x;
	//return 0;
}


int get_modem_frame(unsigned char* uart_buffer){
	return mraa_uart_read(uart, uart_buffer, 100);
}


void sendToModem(unsigned char* buffer, int len){
	char temp[5];
	char msg[50];
	int ans=0;
	writeToFile2(src, buffer,len);
	ans = mraa_uart_write( uart , buffer, len);
	sprintf(msg, "\ntry to write to uart %d [byte].\twrote %d [byte]\n", len, ans);
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
	if ( (src = fopen("/home/ISG/uart/port4.txt", "a")  ) < 0) {exit(FILE_ERROR);}
	if( (fputs(str, src) ) <0 ) {exit(-11);}
	fclose(src);
}


void writeToFile2(FILE* src ,char* str, int len){
	int i=0;
	if ( (src = fopen("/home/ISG/uart/port4.txt", "a")  ) < 0) {exit(FILE_ERROR);}
	for(i;i<len;i++){
		if(fputc(str[i], src) == EOF) { exit(-11);}
	}
	fclose(src);
}

void init_file(FILE* src){
	if ( (src = fopen("/home/ISG/uart/port4.txt", "w+")  ) < 0) {exit(FILE_ERROR);}
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





 	

/* Set the O_NONBLOCK flag of desc if value is nonzero,
   or clear the flag if value is 0.
   Return 0 on success, or -1 on error with errno set. */

int
set_nonblock_flag(int desc, int value)
{
  int oldflags = fcntl (desc, F_GETFL, 0);
  /* If reading the flags failed, return error indication now. */
  if (oldflags == -1)
    return -1;
  /* Set just the flag we want to set. */
  if (value != 0)
    oldflags |= O_NONBLOCK;
  else
    oldflags &= ~O_NONBLOCK;
  /* Store modified flag word in the descriptor. */
  return fcntl (desc, F_SETFL, oldflags);
}

