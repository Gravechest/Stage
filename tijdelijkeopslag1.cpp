
#ifndef CONTOLPANEL_H
#define CONTOLPANEL_H

#include <cstdlib>
#include <cstring>
#include <QSctpSocket>
#include <QPushButton>
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QBoxLayout>

const char HTTPconn[] = "Connection: keep-alive\r\n";
const char HTTPdefault1[] = "GET /";
const char HTTPdefault2[] = " HTTP/1.1\r\n";

class Controlpanel : public QWidget{
    Q_OBJECT
public:
    QPushButton *button1;
    void connect(char ip[],unsigned short port){
        TCPsock.connectToHost(ip,port);
    }
    void webSocketHeader(int payload){
        command[packetMemSz] = 0x81;
        command[packetMemSz+1] = 0x7e;
        command[packetMemSz+2] = payload >> 8;
        command[packetMemSz+3] = payload & 0xff;
        packetMemSz += 4;
    }
    //voeg met deze functie je HTTP GET command toe!
    void HTTPgetcommand(char *string){
        int stringSize = strlen(string);
        memcpy(command + packetMemSz,"GET /",5);
        packetMemSz += 5;
        memcpy(command + packetMemSz,string,stringSize);
        packetMemSz += stringSize;
        memcpy(command + packetMemSz,HTTPdefault2,sizeof(HTTPdefault2));
        packetMemSz += sizeof(HTTPdefault2) - 1;
    }
    void HTTPpostcommand(char *string){
        int stringSize = strlen(string);
        memcpy(command + packetMemSz,"POST /",6);
        packetMemSz += 6;
        memcpy(command + packetMemSz,string,stringSize);
        packetMemSz += stringSize;
        memcpy(command + packetMemSz,HTTPdefault2,sizeof(HTTPdefault2));
        packetMemSz += sizeof(HTTPdefault2) - 1;
    }
    //voeg hier custom onderdelen aan je pakket toe
    void addToPacket(const char string[]){
        int stringSize = strlen(string);
        memcpy(command + packetMemSz,string,stringSize);
        packetMemSz += stringSize;
        if(HTTP){
            command[packetMemSz] = 0x0d;
            command[packetMemSz + 1] = 0x0a;
            packetMemSz += 2;
        }
    }
    void addToPacketRaw(char *str){
        int stringSize = strlen(str);
        memcpy(command + packetMemSz,str,stringSize);
        packetMemSz += stringSize;
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
        TCPsock.waitForReadyRead();
        memset(serverDat,0,sizeof(serverDat));
        int mem = 0;
        int datasz = 0;
        for(;;){
            datasz = TCPsock.read(serverDat + mem,sizeof(serverDat));
            if(!datasz){
                break;
            }
            mem += datasz;
        }
        qDebug() << command;
        qDebug() << serverDat;
        free(command);
    }
    QString sortMessage(char *str){
        int len = strlen(str);
        for(int i = 0;i < 1023000;i++){
            if(!memcmp(serverDat + i,str,len)){
                int sz = i;
                for(;serverDat[sz] != '"';sz++){}
                for(;(serverDat[i] > 0x2f && serverDat[i] < 0x3a) || (serverDat[i] > 0x40 && serverDat[i] < 0x5b) || (serverDat[i] > 0x60 && serverDat[i] < 0x7b);i++){}
                for(;(serverDat[i] < 0x30 || serverDat[i] > 0x39) && (serverDat[i] < 0x41 || serverDat[i] > 0x5a) && (serverDat[i] < 0x61 || serverDat[i] > 0x7a);i++){}
                sz = i;
                for(;serverDat[sz] != ',';sz++){}
                sz -= i;
                char *nstr = new char[sz+1]();
                memcpy(nstr,serverDat+i,sz-2);
                QString rstring;
                rstring.append(nstr);
                delete[] nstr;
                return rstring;
            }
        }
        return 0;
    }
    void searchData(QString str){
        connect((char*)"172.16.164.194",15672);
        newPacket(true);
        QString nstr;
        nstr.append("api/queues/%2f/lowpad-04d3b04a33aa%2F");
        nstr.append(str.toLocal8Bit().data());
        nstr.append("/get");
        HTTPgetcommand((char*)"api/whoami");
        addToPacket((char*)"authorization: Basic ZXVyb3RlYzpldXJvdGVj");
        addToPacket((char*)"Host: 172.16.164.194:15672\r\n");
        sendPacket();
        tableC++;
        table = new QTableWidget[tableC+1];
        table[tableC].setColumnCount(2);
    }
    void insertItem(QString it){
        item = new QTableWidgetItem();
        item->setText(sortMessage(it.toLocal8Bit().data()));
        table[tableC].setRowCount(table[tableC].columnCount()+1);
        table[tableC].setItem(1,1,item);
        delete item;
    }
    void done(){
        layout->addWidget(&table[tableC]);
    }
    char serverDat[1024000];
    QString sortedData;
    QTableWidget *table;
    QBoxLayout *layout;
private slots:
private:
    char commandLength;
    char commandString[400];
    bool HTTP;
    QTcpSocket TCPsock;
    QTableWidgetItem *item;

    char *command;
    int packetMemSz = 0;
    int tableC = -1;
};
//gebruik deze functie als je websockets wilt gebruiken
#endif // CONTOLPANEL_H

