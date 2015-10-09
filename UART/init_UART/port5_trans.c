#include "port5_trans.h"

void main(){
	char buffer2[66];	
	char buffer[]="massage number X";
	char buffer3[]={1,2,3,4,5,6,7,8,9,0,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47};

	unsigned short i=48,j=0, m_len=0, flag=0, len=0, len2=0;
	int payload = 64, header_size =2; 
	printf("starting.....\n");
	init_file(src);
	init_modem();
	sleep(2);
	
	//send_garbage(5);
	//len = createMsg(buffer,payload, header_size);
	//showMsg(buffer, len);
	
	//or
	
	len = sizeof(buffer);
	
	while(1){
		sleep(3);
		buffer[len - 3] = i;
		printf("sending massage of length %d, msg is:", len);
		showMsg(buffer, len);
		printf("\n");
		
		sendToModem(buffer, len);
		//mraa_uart_flush(uart);
		i++;
		if(i==58) i=48;
	}//while(1)

}//main



unsigned short createMsg(char *buffer, int payload , int header_size){
	int i;
	buffer[0] = 3; buffer[1] = 0; 
	for(i=header_size; i<payload+header_size; i++)
		buffer[i] = i;
	buffer[payload+header_size-1] = '\n';
	return payload+header_size;

}

unsigned short createMsg2(char *buffer, int payload , int header_size){
	int i=0; 
	buffer=(char*)malloc((payload+header_size)*sizeof(char));
	buffer[0] = 'a'; buffer[1] = 'b'; 
	for(i=header_size; i<payload+header_size; i++)
		buffer[i] = 'c';
	//buffer[payload+header_size] = '\0';
	return payload+header_size;

}

void showMsg(char *buffer, unsigned short len){
	int i=0;
	for(i;i<len; i++)
		printf("%d_", buffer[i]);
	//printf("\n");

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////	Modem related functions		//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int init_modem(){
	 if( (uart = mraa_uart_init(0)) == NULL   ){
	   		writeToFile(src, "error INITIATING UART\n");
  			exit(UART_ERROR);
	 }
	 if ( mraa_uart_set_baudrate(uart, UART_BOUD_RATE)!=MRAA_SUCCESS) {
  		writeToFile(src, "error seting baudrate\n");
  		exit(UART_ERROR);
  		}		//set uart boud rate [bps]
  if ( mraa_uart_set_mode(uart, 8,MRAA_UART_PARITY_NONE , 1)!=MRAA_SUCCESS) {
  writeToFile(src, "error seting mode\n");
	}
	mraa_uart_set_flowcontrol(uart, 0, 0);
	writeToFile(src, "init modem\n");	
}

int is_incoming_modem(){
	char msg[40];
	int x=0;
	if ( (x = mraa_uart_data_available(uart,0) ) < 0   ) {
	  		writeToFile(src, "error reading data from uart\n");
  			exit(UART_ERROR);
	}	
	/*else if(x>0)
		printf("at is_incoming_modem X (m_len outside the function) is:%d\n", x);*/
	return x;
}


int get_modem_frame(unsigned char* uart_buffer){
	return mraa_uart_read(uart, uart_buffer, 200);
}

void sendToModem(char* buffer, int len){
	//char temp[5];
	//char msg[50];
	int ans=0;
	//writeToFile(src, "sending via uart. msg:\n");
	//writeToFile2(src, buffer,len);
	ans = mraa_uart_write( uart , buffer, len);
	//sprintf(msg, "\ntry to write to uart %d [byte].\twrote %d [byte]\n", len, ans);
	//writeToFile(src, msg);
	printf("try to write to uart %d [byte].\twrote %d [byte]\n", len, ans);
}

void send_garbage(int len){
	char buffer[len];
	int i=0;
	for(i;i<len; i++) 
		buffer[i] = '\n';
	printf("sending %d bytes of garbage\n", len);
	sendToModem(buffer,len);

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////	File Handlers		///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**this function write to file (log) the last buffer sent.
**Parameter: FILE* src - a pointer for a file descriptor to handle file. int type - first, last or middle byte of info,
** unsigned char byte - the byte to write to file
**/
void writeToFile(FILE* src ,char* str){
	if ( (src = fopen("/ISG/uart/port4.txt", "a")  ) < 0) {exit(FILE_ERROR);}
	if( (fputs(str, src) ) <0 ) {exit(-11);}
	fclose(src);
}


void writeToFile2(FILE* src ,char* str, int len){
	int i=0;
	if ( (src = fopen("/ISG/uart/port4.txt", "a")  ) < 0) {exit(FILE_ERROR);}
	for(i;i<len;i++){
		if(fputc(str[i], src) == EOF) { exit(-11);}
	}
	fclose(src);
}

void init_file(FILE* src){
	if ( (src = fopen("/ISG/uart/port4.txt", "w+")  ) < 0) {exit(FILE_ERROR);}
	writeToFile(src, "init log file\n");
	fclose(src);
}


