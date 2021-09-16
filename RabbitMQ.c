#include <stdio.h>
#include <WinSock2.h>
WSADATA data;
SOCKET sock;
SOCKADDR_IN adres;
char x[50240];
char serverDat[20240];
char overview[] = "GET /api/consumers HTTP/1.1\r\n"
"authorization: Basic ZXVyb3RlYzpldXJvdGVj\r\n"
"Host: 172.16.160.218:15672\r\n\r\n"
;
int main(){
	WORD ver = MAKEWORD(2, 2);
	WSAStartup(ver, &data);
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	adres.sin_family = AF_INET;
	adres.sin_port = htons(15672);
	adres.sin_addr.S_un.S_addr = htonl(2886770906);
	connect(sock,(SOCKADDR*)&adres,sizeof(adres));
	send(sock,overview,sizeof(overview) - 1	,0);
	recv(sock,x,20240,0);
	int itt = 0;
	printf("%s",x);
	for(int i = 0;i < sizeof(x);i++){
		if(x[i] == '"'){
			i++;
			while(x[i] != '"'){
				serverDat[itt] = x[i];
				i++;
				itt++;
			}
			serverDat[itt] = '\n';
			itt++;
		}
	}	
	printf("%s",serverDat);
}
