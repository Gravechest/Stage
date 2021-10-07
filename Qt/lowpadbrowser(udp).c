#include "main.h"
#include "mainwindow.h"
#include <QUdpSocket>
struct ROBOTDATA{
public:
    char hostname[40];
    char ipaddr[3][20];
    char type[5];
    char customer[30];
    char status[20];
    char app_name[40];
    char uuid[40];
    char simulated[40];
};
ROBOTDATA robots[40];
int robotCount;
int selectedRobot;

QTableWidget *table;
QTableWidgetItem *item;

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
QFont font;
void insertItem(char * str,int x,int y,bool bold = 0){
    item = new QTableWidgetItem();
    if(bold){
        font.setBold(1);
        item->setFont(font);
    }
    item->setText(str);
    table->setItem(x,y,item);
}

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
                int itt1 = 0;
                for(int i2 = 0;i2 < 3;i2++){
                    int itt2 = 0;
                    while(network.sortOutput[itt1] != 10 && network.sortOutput[itt1]){
                        robots[i].ipaddr[i2][itt2] = network.sortOutput[itt1];
                        itt1++;
                        itt2++;
                    }
                    itt1++;
                }
                network.sortMessage((char *)"status",6);
                memcpy(robots[i].status,network.sortOutput,network.sortOutputLng);
                network.sortMessage((char *)"type",4);
                memcpy(robots[i].type,network.sortOutput,network.sortOutputLng);
                network.sortMessage((char *)"customer",8);
                memcpy(robots[i].customer,network.sortOutput,network.sortOutputLng);
                network.sortMessage((char *)"simulated_hardware",18);
                memcpy(robots[i].simulated,network.sortOutput,network.sortOutputLng);
                network.sortMessage((char *)"uuid",4);
                memcpy(robots[i].uuid,network.sortOutput,network.sortOutputLng);
                network.sortMessage((char *)"app_name",8);
                memcpy(robots[i].app_name,network.sortOutput,network.sortOutputLng);
                int row = i + 1;
                table->setRowCount(row+1);
                insertItem(robots[i].hostname,row,0);
                insertItem(robots[i].status,row,1);
                insertItem(robots[i].type,row,2);
                insertItem(robots[i].customer,row,3);
                insertItem(robots[i].ipaddr[0],row,4);
                insertItem(robots[i].ipaddr[1],row,5);
                insertItem(robots[i].ipaddr[2],row,6);
                insertItem(robots[i].simulated,row,7);
                insertItem(robots[i].uuid,row,8);
                insertItem(robots[i].app_name,row,9);
                break;
            }
            if(!memcmp(network.sortOutput,robots[i].hostname,network.sortOutputLng)){
                break;
            }
        }
        QThread::msleep(2500);
    }
}

void MainWindow::OpenLowpadBrowser()
{
    QWidget *browserWidget = new QWidget(this);
    browserWidget->resize(300,300);
    m_pQMdiArea->addSubWindow(browserWidget);
    browserWidget->showMaximized();
    QVBoxLayout *layout = new QVBoxLayout(this);
    table = new QTableWidget(this);
    browserWidget->setLayout(layout);
    layout->addWidget(table);
    table->setColumnCount(10);
    table->showMaximized();
    table->setRowCount(1);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    insertItem((char*)"naam",0,0,1);
    insertItem((char*)"status",0,1,1);
    insertItem((char*)"type",0,2,1);
    insertItem((char*)"klant",0,3,1);
    insertItem((char*)"ipadres 1",0,4,1);
    insertItem((char*)"ipadres 2",0,5,1);
    insertItem((char*)"ipadres 3",0,6,1);
    insertItem((char*)"gesimuleerd",0,7,1);
    insertItem((char*)"uuid",0,8,1);
    insertItem((char*)"app_name",0,9,1);
    std::thread thread1 (robotLookup);
    thread1.detach();
}
