#include <stdio.h>
#include <WinSock2.h>
WSADATA data;
SOCKET sock;
SOCKADDR_IN adres;
int itt;
char line;
char x[100240];
char serverDat[100240];
char overview[] = "GET /api/overview HTTP/1.1\r\n"
"authorization: Basic ZXVyb3RlYzpldXJvdGVj\r\n"
"Host: 172.16.160.218:15672\r\n\r\n"
;
void addSpace(){
	return;
	for(int i = 0;i < line;i++){
		serverDat[itt] = 'x';
		itt++;
	}
}
int main(){
	WORD ver = MAKEWORD(2, 2);
	WSAStartup(ver, &data);
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	adres.sin_family = AF_INET;
	adres.sin_port = htons(15672);
	adres.sin_addr.S_un.S_addr = htonl(2886770906);
	connect(sock,(SOCKADDR*)&adres,sizeof(adres));
	send(sock,overview,sizeof(overview) - 1	,0);
	recv(sock,x,100240,0);

	for(int i = 0;i < sizeof(x);i++){
		serverDat[itt] = x[i];
		switch(x[i]){
		case ',':
			itt++;
			serverDat[itt] = '\n';
			addSpace();
			break;
		case ']':
		case '}':
			itt++;
			serverDat[itt] = '\n';
			line-=4;
			addSpace();
			break;
		case '[':
		case '{':
			itt++;
			serverDat[itt] = '\n';
			line+=4;
			addSpace();
			break;
		}
		itt++;
	}	
	printf("%s",serverDat);
}
