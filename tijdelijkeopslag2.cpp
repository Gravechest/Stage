#include "main.h"
#include "mainwindow.h"
#include "contolpanel.h"
#include "QPushButton"
#include "QTextBrowser"
#include "QInputDialog"


//Netwerk pakket onderdelen

Controlpanel *controlpanel;
QComboBox *combobox;
QInputDialog *dialog;
QLabel *label;

void MainWindow::OpenLowpadViewer(){
    label = new QLabel(this);
    dialog = new QInputDialog;
    controlpanel = new Controlpanel;
    controlpanel->layout = new QVBoxLayout(this);
    QWidget *browserWidget = new QWidget(this);
    controlpanel->button1 = new QPushButton(this);
    controlpanel->table = new QTableWidget(this);
    combobox = new QComboBox(this);
    combobox->setFixedSize(100,30);
    controlpanel->button1->setFixedSize(100,100);
    label->setText("battery");
    controlpanel->layout->addWidget(combobox);
    controlpanel->layout->addWidget(controlpanel->button1);
    controlpanel->layout->addWidget(dialog);
    controlpanel->layout->addWidget(label);
    m_pQMdiArea->addSubWindow(browserWidget);
    browserWidget->setLayout(controlpanel->layout);
    browserWidget->showMaximized();
    controlpanel->searchData("battery");
    controlpanel->insertItem("voltage");
    controlpanel->done();
}

//verzend een HTTP request naar de server en krijg de data terug


/*
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
*/
