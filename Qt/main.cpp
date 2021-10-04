#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <ctime>
#include <cstdlib>
#include <QThread>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <QMainWindow>
#include <QSctpSocket>
#include <QUdpSocket>
#include <QNetworkAccessManager>

char commandLength;
char serverDat[1024000];
char commandString[400];
char decodedJson[1024000];

//Netwerk pakket onderdelen
const char HTTPconn[] = "Connection: keep-alive\r\n";
const char HTTPdefault1[] = "GET /";
const char HTTPdefault2[] = " HTTP/1.1\r\n";

//De lowpad data
struct ROBOTDATA{
public:
    char hostname[40];
    char ipaddr[60];
    char type[5];
    char customer[30];
    char status[20];
};
//40 moet genoeg zijn
ROBOTDATA robots[40];
int robotCount;
int selectedRobot;

class UDPNETWORK {
public:
    UDPNETWORK(){
        UDPsock.bind(QHostAddress::Any,10394);
    }
    //het zoeken van een string naar keuze in het opgevangen pakketje
    void sortMessage(char str[],int size){
        memset(sortOutput,0,sizeof(sortOutput));
        for(int i = 0;i < 1000;i++){
            if(!memcmp(str,packet + i,size)){
                int sz = 0;
                i += size + 4;
                while(packet[i] != '"'){
                    sortOutput[sz] = packet[i];
                    sz++;
                    i++;
                }
                sortOutputLng = sz;
                break;
            }
        }
        //als gezocht word naar ip adress worden de spaties vervangen door newline caracters
        if(!memcmp(str,"ip_address",10)){
            for(int i = 0;i < sortOutputLng;i++){
                if(sortOutput[i] == ' '){
                    sortOutput[i] = 10;
                }
            }
        }
    }
    void sendMessage(char str[]){
        UDPsock.writeDatagram(str,QHostAddress::Broadcast,10394);
    }
    void recvMessage(){
        UDPsock.waitForReadyRead();
        UDPsock.readDatagram(packet,1000);
    }
    void close(){
        UDPsock.close();
    }
    char packet[1000];
    char sortOutput[100];
    int sortOutputLng;
private:
    QHostAddress Broadcast = QHostAddress::Broadcast;
    QUdpSocket UDPsock;
};

class TCPNETWORK {
public:
    TCPNETWORK(char ip[],unsigned short port){
        TCPsock.connectToHost(ip,port);
    }
    //gebruik deze functie als je websockets wilt gebruiken
    void webSocketHeader(int payload){
        command[packetMemSz] = 0x81;
        command[packetMemSz+1] = 0x7e;
        command[packetMemSz+2] = payload >> 8;
        command[packetMemSz+3] = payload & 0xff;
        packetMemSz += 4;
    }
    //voeg met deze functie je HTTP GET command toe!
    void HTTPcommand(char string[],int stringSize){
        memcpy(command + packetMemSz,HTTPdefault1,sizeof(HTTPdefault1));
        packetMemSz += sizeof(HTTPdefault1) - 1;
        memcpy(command + packetMemSz,string,stringSize);
        packetMemSz += stringSize;
        memcpy(command + packetMemSz,HTTPdefault2,sizeof(HTTPdefault2));
        packetMemSz += sizeof(HTTPdefault2) - 1;
    }
    //voeg hier custom onderdelen aan je pakket toe
    void addToPacket(const char string[],int stringSize){
        memcpy(command + packetMemSz,string,stringSize);
        packetMemSz += stringSize;
        if(HTTP){
            command[packetMemSz] = 0x0d;
            command[packetMemSz + 1] = 0x0a;
            packetMemSz += 2;
        }
    }
    void newPacket(bool http){
        HTTP = http;
        command = (char *)calloc(1000,1);
        packetMemSz = 0;
    }
    void sendPacket(){
        if(HTTP){
            command[packetMemSz] = 0x0d;
            command[packetMemSz + 1] = 0x0a;
            packetMemSz+=2;
        }
        TCPsock.write(command,packetMemSz);
        memset(serverDat,0,sizeof(serverDat));
        TCPsock.waitForReadyRead();
        int mem = 0;
        int datasz = 0;
        do{
            datasz = TCPsock.read(serverDat + mem,sizeof(serverDat));
            mem += datasz;
            TCPsock.waitForReadyRead();
        }
        while(datasz > 0);
        free(command);
    }
private:
    bool HTTP;
    QTcpSocket TCPsock;
    char *command;
    int packetMemSz = 0;
};
//custom json formatter
void decodeJson(char * input){
    int size = 0;
    for(int i = 0;;i++){
        if(!input[i]){
            size = i;
            qDebug() << size;
            break;
        }
    }
    int itt = 0;
    char line = 0;
    for(int i = 0;i < sizeof(serverDat);i++){
        switch(input[i]){
        case '{':
        case '[':
            line+=4;
            decodedJson[itt] = 10;
            for(int i2 = 0;i2 < line;i2++){
                itt++;
                decodedJson[itt] = ' ';
            }
            decodedJson[itt] = input[i];
            decodedJson[itt+1] = 10;
            for(int i2 = 0;i2 < line;i2++){
                itt++;
                decodedJson[itt+1] = ' ';
            }
            break;
        case '}':
        case ']':
            line-=4;
            decodedJson[itt] = 10;
            for(int i2 = 0;i2 < line;i2++){
                itt++;
                decodedJson[itt] = ' ';
            }
            itt++;
            decodedJson[itt] = input[i];
            decodedJson[itt+1] = 10;
            for(int i2 = 0;i2 < line;i2++){
                itt++;
                decodedJson[itt+1] = ' ';
            }
            break;
        case ',':
            decodedJson[itt] = input[i];
            decodedJson[itt+1] = 10;
            for(int i2 = 0;i2 < line;i2++){
                itt++;
                decodedJson[itt+1] = ' ';
            }
            break;
        default:
            decodedJson[itt] = input[i];

        }
        itt++;
    }
}
//verzend een HTTP request naar de server en krijg de data terug
void readInfo(char com[],char size){
    TCPNETWORK packet((char*)"172.16.160.218",15672);
    packet.newPacket(true);
    packet.HTTPcommand(com,size);
    packet.addToPacket((char*)"authorization: Basic ZXVyb3RlYzpldXJvdGVj",41);
    packet.addToPacket((char*)"Host: 172.16.160.218:15672",26);
    packet.sendPacket();
    decodeJson(serverDat);
    return;
}
// zoek op het broadcast netwerk naar robots op het LAN netwerk en sla deze op in het RAM, de gebruiker kan deze opvragen door op de refreshknop te drukken.
void robotLookup(){
    UDPNETWORK network;
    for(;;){
        network.recvMessage();
        network.sortMessage((char *)"hostname",8);
        for(int i = 0;;i++){
            if(!robots[i].hostname[0]){
                //zoeken op keywords
                memcpy(robots[i].hostname,network.sortOutput,network.sortOutputLng);
                network.sortMessage((char *)"ip_address",10);
                memcpy(robots[i].ipaddr,network.sortOutput,network.sortOutputLng);
                network.sortMessage((char *)"status",6);
                memcpy(robots[i].status,network.sortOutput,network.sortOutputLng);
                network.sortMessage((char *)"type",4);
                memcpy(robots[i].type,network.sortOutput,network.sortOutputLng);
                network.sortMessage((char *)"customer",8);
                memcpy(robots[i].customer,network.sortOutput,network.sortOutputLng);
                break;
            }
            if(!memcmp(network.sortOutput,robots[i].hostname,network.sortOutputLng)){
                break;
            }
        }
        QThread::msleep(2500);
    }
} 

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString string(serverDat);
    ui->robotOutput->append(string);
    std::thread thread1 (robotLookup);
    thread1.detach();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    ui->textBrowser->clear();
    readInfo(commandString,commandLength);
    QString string(decodedJson);
    ui->textBrowser->append(string);
    memset(decodedJson,0,sizeof(decodedJson));
}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    int length = arg1.length() + 4;
    memset(commandString,0,sizeof(commandString));
    char * str = 0;
    memcpy(&str,&arg1,length);
    str += 16;
    memcpy(commandString,"api/",4);
    for(int i = 4;i < length;i++){
        commandString[i] = str[0];
        str+=2;
    }
    commandLength = length;
}

void MainWindow::on_pushButton_3_clicked()
{
    for(;;robotCount++){
        if(!robots[robotCount].hostname[0]){
            break;
        }
        QString string(robots[robotCount].hostname);
        ui->allRobots->addItem(string);
    }
}

void MainWindow::on_allRobots_currentTextChanged(const QString &arg1)
{
    ui->robotOutput->clear();
    char * str = 0;
    char str2[40];
    memcpy(&str,&arg1,arg1.length());
    str+=16;
    for(int i = 0;i < arg1.length();i++){
        str2[i] = str[0];
        str+=2;
    }
    for(int i = 0;;i++){
        if(!memcmp(str2,robots[i].hostname,arg1.length())){
            selectedRobot = i;
            ui->robotOutput->setFontWeight(QFont::Bold);
            ui->robotOutput->append("ip addresses");
            ui->robotOutput->setFontWeight(QFont::Normal);
            ui->robotOutput->append(robots[i].ipaddr);
            ui->robotOutput->setFontWeight(QFont::Bold);
            ui->robotOutput->append("status");
            ui->robotOutput->setFontWeight(QFont::Normal);
            ui->robotOutput->append(robots[i].status);
            ui->robotOutput->setFontWeight(QFont::Bold);
            ui->robotOutput->append("customer");
            ui->robotOutput->setFontWeight(QFont::Normal);
            ui->robotOutput->append(robots[i].customer);
            ui->robotOutput->setFontWeight(QFont::Bold);
            ui->robotOutput->append("type");
            ui->robotOutput->setFontWeight(QFont::Normal);
            ui->robotOutput->append(robots[i].type);
            break;
        }
    }
}

//mislukte poging om op deze manier met de lowpads te verbinden en data op te vragen, deze functie doet dus niks
void MainWindow::on_pushButton_2_clicked()
{
    char ipAddr[15];
    char ipLng;
    for(int i = 0;;i++){
        if(robots[selectedRobot].ipaddr[i] == 10){
            i++;
            int i2 = i;
            while(robots[selectedRobot].ipaddr[i] && robots[selectedRobot].ipaddr[i] != 10){
                ipAddr[i - i2] = robots[selectedRobot].ipaddr[i];
                i++;
            }
            ipLng = i - i2;
            break;
        }
    }
    TCPNETWORK robotCon(ipAddr,9090);
    robotCon.newPacket(true);
    robotCon.HTTPcommand(0,0);
    char hostString[ipLng + 11];
    memcpy(hostString,"Host: ",6);
    memcpy(hostString + 6,ipAddr,ipLng);
    memcpy(hostString + ipLng + 6,":9090",5);
    robotCon.addToPacket(hostString,11 + ipLng);
    char originString[ipLng + 8];
    memcpy(originString,"Origin: http://",15);
    memcpy(originString + 15,ipAddr,ipLng);
    robotCon.addToPacket(originString,ipLng + 15);
    robotCon.addToPacket((char*)"Connection: upgrade",19);
    robotCon.addToPacket((char*)"Upgrade: websocket",18);
    robotCon.addToPacket((char*)"Sec-Websocket-Version: 13",25);
    robotCon.addToPacket((char*)"Sec-WebSocket-Key: ffffffffffffffffffffff==",43);
    robotCon.sendPacket();
    ui->robotOutput->append(serverDat);
    robotCon.newPacket(false);
    robotCon.webSocketHeader(386);
    robotCon.addToPacket("{\"op\":\"auth\",\"mac\":\"4a8b0482adcfa089fa77b89bcce655573d66b4122a55b9fb2a991b025991c823e2e33d718a935b9e5f143b3cbe684d970be3631d9e1e2d96aeb9ff76c4575ee4\",\"client\":\"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/93.0.4577.82 Safari/537.36\",\"dest\":\"ws://172.16.164.114:9090\",\"rand\":\"z3Xu7BheeV\",\"t\":1632724185.425,\"level\":\"admin\",\"end\":1632725185.425}",386);
    robotCon.sendPacket();
}

//custom HTTP request
void MainWindow::on_lineEdit_textEdited(const QString &arg1)
{
    int length = arg1.length() + 4;
    memset(commandString,0,sizeof(commandString));
    char * str = new char[400];
    memcpy(&str,&arg1,length);
    str += 16;
    memcpy(commandString,"api/",4);
    for(int i = 4;i < length;i++){
        commandString[i] = str[0];
        str+=2;
    }
    commandLength = length;
}

