#include "port6_rec.h"





void main(){
	int fd;
	unsigned char buffer[100];
	unsigned char temp_buffer[100];
	char msg[100];
	int m_len;
	unsigned long error_counter=0;
	init_file(src);
	fd = init_modem();
	printf("starting receiver\n");
	while(1){
		m_len = 0;
		if( is_incoming_modem(fd) > 0){
			m_len = get_modem_frame(fd, buffer);
			if(m_len < 3 ) {
				showMsg(buffer, m_len);
				error_counter++;
			}//if(m_len<3)
			if(m_len>3){
				showMsg(buffer, m_len);
				//printf("error till now:%ld\n", error_counter);
			}//if(m_len>3)
		
		
		}//if(get_frame>0)
		if(m_len<0) {
			
			printf("m_len is:%d, bad!!", m_len);
		}
	}//while(1)

}//main


void showMsg(char *buffer, unsigned short len){
	int i=0;
	for(i;i<len; i++)
		printf("%d_", buffer[i]);
	printf("\n");

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
         
        newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
        newtio.c_cc[VMIN]     = 66;   /* blocking read until 5 chars received */
        
        tcflush(fd, TCIFLUSH);
        tcsetattr(fd,TCSANOW,&newtio);
        return fd;
}

/*return 1 if data available at terminal, 0 else*/
int is_incoming_modem(int fd){
	fd_set readset;
	struct timeval tv;
	
	FD_ZERO(&readset);
  FD_SET(fd, &readset);
  
  // Initialize time out struct
   tv.tv_sec = 0;
   tv.tv_usec = 0;
  return select(fd + 1, &readset, NULL, NULL, &tv);
}


int get_modem_frame(int fd, unsigned char* buffer){
	return read(fd, buffer, 66);
}

void sendToModem(unsigned char* buffer, int len){
	//char temp[5];
	//char msg[50];
	int ans=0;
	//writeToFile(src, "sending via uart. msg:\n");
	//writeToFile2(src, buffer,len);
	///ans = mraa_uart_write( uart , buffer, len);
	//sprintf(msg, "\ntry to write to uart %d [byte].\twrote %d [byte]\n", len, ans);
	//writeToFile(src, msg);
	printf("try to write to uart %d [byte].\twrote %d [byte]\n", len, ans);
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


