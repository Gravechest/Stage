#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include <string.h>
#include <QMainWindow>
#include <QSctpSocket>
#include <winsock2.h>
WSADATA data;
SOCKET sock;
SOCKADDR_IN adres;
const unsigned char contentlng[] = {'c','o','n','t','e','n','t','-','l','e','n','g','t','h'};
const unsigned char endHeader[] = {'\r','\n','\r','\n'};
char commandLength;
int packetSize;
int headerSize;
int itt;
char line;
char serverDat[1024000];
char commandString[40];
char serverFormatedDat[1024000];
char HTTPdefault[] = "GET /api/ HTTP/1.1\r\n";
char HTTPauth[] = "authorization: Basic ZXVyb3RlYzpldXJvdGVj\r\n";
char HTTPhost[] = "Host: 172.16.160.218:15672\r\n";
char HTTPconn[] = "Connection: keep-alive\r\n";

bool init;

int exp10(int val1,int exp){
    for(int i = 0;i < exp;i++){
        val1 *= 10;
    }
    return val1;
}

void add4Space(int i){
    serverFormatedDat[itt] = 10;
    itt++;
    for(int i = 0;i < line;i++){
        serverFormatedDat[itt] = ' ';
        itt++;
    }
    serverFormatedDat[itt] = serverDat[i];
    itt++;
    serverFormatedDat[itt] = 10;
    itt++;
    for(int i = 0;i < line;i++){
        serverFormatedDat[itt] = ' ';
        itt++;
    }
}

void add3Space(int i){
    serverFormatedDat[itt] = 10;
    itt++;
    for(int i = 0;i < line;i++){
        serverFormatedDat[itt] = ' ';
        itt++;
    }
    serverFormatedDat[itt] = serverDat[i];
    itt++;

}
void add2Space(int i){
    serverFormatedDat[itt] = 10;
    itt++;
    for(int i = 0;i < line - 4;i++){
        serverFormatedDat[itt] = ' ';
        itt++;
    }
    serverFormatedDat[itt] = serverDat[i];
    itt++;
    serverFormatedDat[itt] = 10;
    itt++;
    for(int i = 0;i < line;i++){
        serverFormatedDat[itt] = ' ';
        itt++;
    }
}

void addSpace(int i){
    serverFormatedDat[itt] = serverDat[i];
    itt++;
    serverFormatedDat[itt] = 10;
    itt++;
    for(int i = 0;i < line;i++){
        serverFormatedDat[itt] = ' ';
        itt++;
    }
}

void readInfo(char com[],char size){
    int memSize = sizeof(HTTPdefault) + sizeof(HTTPhost) + sizeof(HTTPconn) + size - 1;
    char *command = (char *)calloc(memSize + size,1);
    char *commandMemBuf = command;
    memcpy(command,HTTPdefault,9);
    command += 9;
    memcpy(command,com,size);
    command += size;
    for(int i = 9;i < sizeof(HTTPdefault);i++){
        command[0] = HTTPdefault[i];
        command++;
    }
    command--;
    if(!init){
        memcpy(command,HTTPauth,sizeof(HTTPauth));
        command += sizeof(HTTPauth) - 1;
        memSize += sizeof(HTTPauth) - 1;
        init = true;
    }
    memcpy(command,HTTPhost,sizeof(HTTPhost));
    command += sizeof(HTTPhost) - 1;
    memcpy(command,HTTPconn,sizeof(HTTPconn));
    command += sizeof(HTTPconn) - 1;
    command[0] = 0x0d;
    command[1] = 0x0a;
    command = commandMemBuf;
    qDebug() << command;
    send(sock,command,memSize,0);
    recv(sock,serverDat,500,0);
    for(int i = 0;i < 500;i++){
        if(memcmp(serverDat + i,contentlng,sizeof(contentlng)) == 0){
            i+=16;
            int i2 = 0;
            while(serverDat[i + i2] > 0x30 && serverDat[i + i2] < 0x39){
                i2++;
            }
            i2--;
            for(int i3 = 0;i3 <= i2;i3++){
                packetSize += exp10((serverDat[i + i3] - 0x30),i2 - i3);
            }
        }
        if(memcmp(serverDat + i,endHeader,sizeof(endHeader)) == 0){
            headerSize = i;
        }
    }
    qDebug() << packetSize;
    qDebug() << headerSize;
    recv(sock,serverDat + 500,packetSize + headerSize,0);
    for(int i = 0;i < packetSize + headerSize + 7;i++){
        switch(serverDat[i]){
        case '[':
        case '{':
            line+=4;
            if(serverDat[i - 1] == ',' || serverDat[i - 1] == '{' || serverDat[i - 1] == '['){
                addSpace(i);
                break;
            }
            add2Space(i);
            break;
        case ']':
        case '}':
            if(serverDat[i + 1] == ','){
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
            serverFormatedDat[itt] = serverDat[i];
            itt++;
        }
    }
    memset(commandString,0,sizeof(commandString));
    packetSize = 0;
    headerSize = 0;
    itt = 0;
    free(command);
}
void startApp(){
    WORD ver = MAKEWORD(2, 2);
    WSAStartup(ver, &data);
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    adres.sin_family = AF_INET;
    adres.sin_port = htons(15672);
    adres.sin_addr.S_un.S_addr = htonl(2886770906);
    connect(sock,(SOCKADDR*)&adres,sizeof(adres));
}

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    startApp();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    readInfo(commandString,commandLength);
    QString string(serverFormatedDat);
    ui->textBrowser->append(string);
    memset(serverFormatedDat,0,sizeof(serverFormatedDat));
}


void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    memset(commandString,0,sizeof(commandString));
    char * str = 0;
    memcpy(&str,&arg1,arg1.length());
    str+= 16;
    for(int i = 0;i < arg1.length();i++){
        commandString[i] = str[0];
        str+=2;
    }
    qDebug() << commandString;
    commandLength = arg1.length();
    ui->textBrowser->clear();

}
