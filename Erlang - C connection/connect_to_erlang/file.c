#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main() {

	unsigned char b = 'v';
	FILE* src=fopen("/home/barlesh/ISG/Erlang - C connection/connect_to_erlang/filename.txt", "w");
	fputc(b,src);
	fclose(src);


return 0;
}
