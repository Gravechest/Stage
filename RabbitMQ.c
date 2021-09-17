#include <stdio.h>
#include <WinSock2.h>
WSADATA data;
SOCKET sock;
SOCKADDR_IN adres;
int itt;
char line;
char x[100240];
char commandString[40];
char serverDat[100240];
char HTTPdefault[] = "GET / HTTP/1.1\r\n"
"authorization: Basic ZXVyb3RlYzpldXJvdGVj\r\n"
"Connection: keep-alive\r\n"
"Host: 172.16.160.218:15672\r\n\r\n"
;

void add4Space(int i){
	serverDat[itt] = 10;
	itt++;
	for(int i = 0;i < line;i++){
		serverDat[itt] = ' ';
		itt++;
	}
	serverDat[itt] = x[i];
	itt++;
	serverDat[itt] = 10;
	itt++;
	for(int i = 0;i < line;i++){
		serverDat[itt] = ' ';
		itt++;
	}
}

void add3Space(int i){
	serverDat[itt] = 10;
	itt++;
	for(int i = 0;i < line;i++){
		serverDat[itt] = ' ';
		itt++;
	}
	serverDat[itt] = x[i];
	itt++;

}
void add2Space(int i){
	serverDat[itt] = 10;
	itt++;
	for(int i = 0;i < line - 4;i++){
		serverDat[itt] = ' ';
		itt++;
	}
	serverDat[itt] = x[i];
	itt++;
	serverDat[itt] = 10;
	itt++;
	for(int i = 0;i < line;i++){
		serverDat[itt] = ' ';
		itt++;
	}
}

void addSpace(int i){
	serverDat[itt] = x[i];
	itt++;
	serverDat[itt] = 10;
	itt++;
	for(int i = 0;i < line;i++){
		serverDat[itt] = ' ';
		itt++;
	}
}

void readInfo(char com[],char size){
	char *command = calloc(sizeof(HTTPdefault) + size,1);
	memcpy(command,HTTPdefault,5);
	command += 5;
	memcpy(command,com,size);
	command += size;
	for(int i = 5;i < sizeof(HTTPdefault);i++){
		command[0] = HTTPdefault[i];
		command++;
	}
	command -= size + sizeof(HTTPdefault);
	send(sock,command,sizeof(HTTPdefault) + size,0);
	recv(sock,x,100240,0);
	for(int i = 0;i < sizeof(x);i++){
		switch(x[i]){
		case '[':
		case '{':
			line+=4;
			if(x[i - 1] == ',' || x[i - 1] == '{' || x[i - 1] == '['){
				addSpace(i);
				break;
			}
			add2Space(i);
			break;
		case ']':
		case '}':
			if(x[i + 1] == ','){
				line-=4;
				add3Space(i);
				break;
			}
			line-=4;
			add4Space(i);
			break;
		case ',':
			addSpace(i);
			break;
		default:
			serverDat[itt] = x[i];
			itt++;
		}
	}	
	printf("%s",serverDat);
	
	memset(serverDat,0,sizeof(serverDat));
	memset(x,0,sizeof(x));
	memset(commandString,0,sizeof(commandString));
	itt = 0;
	free(command);
	printf("%i",WSAGetLastError());
}

int main(){
	WORD ver = MAKEWORD(2, 2);
	WSAStartup(ver, &data);
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	adres.sin_family = AF_INET;
	adres.sin_port = htons(15672);
	adres.sin_addr.S_un.S_addr = htonl(2886770906);
	connect(sock,(SOCKADDR*)&adres,sizeof(adres));
	for(;;){
		scanf("%s",commandString);
		char size = 0;
		for(int i = 0;i < sizeof(commandString);i++){
			if(!commandString[i]){
				size = i;
				break;
			}
		}
		readInfo(commandString,size);
	}
	
}
