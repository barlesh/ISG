#include "port6_trans.h"

void main(){
	int fd; 
	char buffer2[66];	
	char buffer3[]="massage number X\n";
	char buffer[]={1,0,0,0,0,6,7,8,9,0,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,10,10};

	unsigned short i=48,j=0, m_len=0, flag=0, len=0, len2=0;
	int payload = 64, header_size =2; 
	printf("starting.....(termios)\n");
	init_file(src);
	fd = init_modem();

	len = sizeof(buffer);
	
	while(1){
		sleep(5);
		buffer[len - 3] = i;
		printf("sending massage of length %d, msg is:", len);
		showMsg(buffer, len);
		printf("\n");
		
		sendToModem(fd, buffer, len);
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
	 int fd,c, res;
        struct termios oldtio,newtio;
        char buf[255];
        
        fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY ); 
        if (fd <0) {perror(MODEMDEVICE); exit(-1); }
        
        tcgetattr(fd,&oldtio); /* save current port settings */
        
        bzero(&newtio, sizeof(newtio));
        newtio.c_cflag = BAUDRATE  | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR;
        newtio.c_oflag = 0;
        
        /* set input mode (non-canonical, no echo,...) */
        newtio.c_lflag = 0;
         
        newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
        newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */
        
        tcflush(fd, TCIFLUSH);
        tcsetattr(fd,TCSANOW,&newtio);
        return fd;
}

int is_incoming_modem(){
	char msg[40];
	int x=0;
	//if ( (x = mraa_uart_data_available(uart,0) ) < 0   ) {
	  //		writeToFile(src, "error reading data from uart\n");
  		//	exit(UART_ERROR);
	//}	
	return x;
}


int get_modem_frame(unsigned char* uart_buffer){
	//return mraa_uart_read(uart, uart_buffer, 200);
}

void sendToModem(int fd, char* buffer, int len){
	int ans=0;
	ans = write(fd, buffer, len);
	//ans = mraa_uart_write( uart , buffer, len);
	printf("try to write to uart %d [byte].\twrote %d [byte]\n", len, ans);
}

/*void send_garbage(int len){
	char buffer[len];
	int i=0;
	for(i;i<len; i++) 
		buffer[i] = '\n';
	printf("sending %d bytes of garbage\n", len);
	sendToModem(buffer,len);

}*/


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


