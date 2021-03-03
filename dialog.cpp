#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
//    QRegExp rx("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
//    QRegExpValidator v(rx, 0);
    ui->setupUi(this);
   // ui->targetIPLineEdit->setValidator(&v);
   // ui->targetIPLineEdit->setInputMask("000.000.000.000;0");
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            // qDebug() << address.toString();
            ui->localIpLineEdit->setText(address.toString());
    }
    //setStyle("macintosh");
    mdialog = NULL;
    ddialog = NULL;
    tcpClientSocket = NULL;
    tcpServer = NULL;
    udpSocket = NULL;
    tcpSocket = NULL;
    serial = NULL;
   // connect(parent, SIGNAL(QWidget::close()), [=](){
  //      qDebug()<<"close the windows ";
  //  });
    ui->comboBoxPort->clear();
    QStringList list;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        list<<info.portName();
    }
    qSort(list.begin(), list.end());
    ui->comboBoxPort->addItems(list);
    ui->comboBoxBuadRate->setCurrentIndex(2);
    ui->receiveTextEdit->document()->setMaximumBlockCount(100);
    ui->textEditReceive->document()->setMaximumBlockCount(100);
    ui->logTextEdit->document()->setMaximumBlockCount(20);
    ui->textEditLog->document()->setMaximumBlockCount(20);
    ui->openPushButton->setFocus();
    ui->lineEditRxCount->setText(QString::number(rxcount, 10));
    ui->lineEditTxCount->setText(QString::number(txcount, 10));
}

Dialog::~Dialog()
{

    delete ui;
}

//建立连接啰。
void Dialog::on_openPushButton_clicked()
{
    int selected = ui->conTypeComboBox->currentIndex();
 //   int port = ui->localSpinBox->value();
  //  qDebug()<<"选择项："<<selected<<"  端口："<<port;
    if(selected == 0){
        openServer();
    }else if (selected == 1){
        openClient();
    }else if(selected == 2){

    }else if(selected == 3){

    }
}

void Dialog::openClient(){
    int port = ui->targetSpinBox->value();
    QString tartgetIp = ui->targetIPLineEdit->text();
    if(tartgetIp.isEmpty()){
       return;
    }
    if(tcpClientSocket == NULL){
        tcpClientSocket = new QTcpSocket(this);
        tcpClientSocket->connectToHost(QHostAddress(tartgetIp), port);
    }else{
        if(tcpClientSocket != NULL && tcpClientSocket->isOpen()){
            tcpClientSocket->disconnectFromHost();
        }
        return;
    }
    connect(tcpClientSocket, &QTcpSocket::connected,
        [=](){
        ui->openPushButton->setText("关闭");
        ui->sendPushButton->setEnabled(true);
        QString msg = QString("地址：%1，端口：%2，连接成功！").arg(tartgetIp).arg(port);
        ui->targetIPLineEdit->setEnabled(false);
        ui->targetSpinBox->setEnabled(false);
        ui->localSpinBox->setEnabled(false);
        ui->conTypeComboBox->setEnabled(false);
        ui->logTextEdit->setText(msg.toUtf8().data());
    });
    connect(tcpClientSocket, &QTcpSocket::disconnected,
        [=](){
        if(tcpClientSocket != NULL){
            tcpClientSocket->close();
            tcpClientSocket = NULL;
        }
        ui->openPushButton->setText("打开");
        ui->sendPushButton->setEnabled(false);
        ui->targetIPLineEdit->setEnabled(true);
        ui->targetSpinBox->setEnabled(true);
        ui->localSpinBox->setEnabled(true);
        ui->conTypeComboBox->setEnabled(true);
        QString msg = QString("服务器已断开连接！");
        ui->logTextEdit->append(msg.toUtf8().data());
    });
    connect(tcpClientSocket, &QTcpSocket::readyRead, [=](){
        QByteArray ba = tcpClientSocket->readAll();
        if(ui->hexRecCheckBox->isChecked()){
            ui->receiveTextEdit->append(ba.toHex());
        }else{
            ui->receiveTextEdit->append(ba);
        }
        if(ui->echoCheckBox->isChecked()){
            on_sendPushButton_clicked();
        }
    });
    //connect(tcpClientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    int count = 0;
    while(count < 20){

        if(!tcpClientSocket->waitForConnected(1500)){
            ui->logTextEdit->append("连接不成功");
            tcpClientSocket->close();
            if(!ui->autoConnCheckBox->isChecked()){
                tcpClientSocket = NULL;
                break;
            }else{
                count++;
                tcpClientSocket->connectToHost(QHostAddress(tartgetIp), port);
            }
        }else{
            break;
        }
    }
}

void Dialog::socketError(QAbstractSocket::SocketError error){
    switch(error)
    {
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::critical(this, tr("Opening connection"), tr("Connection refused, server not found, check IP and Port "));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::critical(this, tr("Opening connection"), tr("Connection refused, server refused the connection, check IP and Port and that server is available"));
            break;
        case QAbstractSocket::RemoteHostClosedError:
            QMessageBox::warning(this, tr("Disconnected"), tr("Server closed the connection "));
            break;
        default:
            QMessageBox::critical(this, tr("Information"), tr("<em>ERROR : ") + tcpClientSocket->errorString() + tr("</em>"));
    }
   // qDebug()<<"连接异常";
    //ui->uiClientConnectBtn->setText(tr("Connect"));
}

void Dialog::openServer(){
    int port = ui->localSpinBox->value();
    if(tcpServer == NULL){
        tcpServer = new QTcpServer(this);

    }
    if(tcpServer->isListening()){
        if(tcpSocket != NULL){
            tcpSocket->disconnectFromHost();
        }
        tcpServer->close();
        tcpServer = NULL;
        ui->openPushButton->setText("打开");
        ui->sendPushButton->setEnabled(true);
        QString msg = QString("端口：%1，监听已关闭！").arg(port);
        ui->localSpinBox->setEnabled(true);
        ui->conTypeComboBox->setEnabled(true);
        ui->logTextEdit->setText(msg.toUtf8().data());
       return;
    }
    tcpServer->listen(QHostAddress::Any, port);
    ui->openPushButton->setText("关闭");
    ui->sendPushButton->setEnabled(true);
    QString msg = QString("端口：%1，监听成功！").arg(port);
    ui->localSpinBox->setEnabled(false);
    ui->conTypeComboBox->setEnabled(false);
    ui->logTextEdit->append(msg.toUtf8().data());
    connect(tcpServer, &QTcpServer::newConnection,
        [=](){
        tcpSocket = tcpServer->nextPendingConnection();
        QString ip = tcpSocket->peerAddress().toString();
        qint16 port = tcpSocket->peerPort();
        QString info = QString("[%1:%2]，成功连接！").arg(ip).arg(port);
        ui->logTextEdit->append(info.toUtf8().data());
        connect(tcpSocket, &QTcpSocket::readyRead, [=](){
            QByteArray ba = tcpSocket->readAll();
            if(ui->hexRecCheckBox->isChecked()){
                ui->receiveTextEdit->append(ba.toHex());
            }else{
                ui->receiveTextEdit->append(ba);
            }
            if(ui->echoCheckBox->isChecked()){
                on_sendPushButton_clicked();
            }
            rxcount += 1;
            ui->lineEditRxCount->setText(QString::number(rxcount));
        });
        connect(tcpSocket, &QTcpSocket::disconnected,
           [=](){
            if(tcpSocket != NULL){
                tcpSocket->close();
                tcpSocket = NULL;
            }
        });
    });

}

//根据不同的连接方式，启用和禁用一些无关的输入框。
void Dialog::on_conTypeComboBox_currentIndexChanged(int selected)
{
    //int selected = ui->conTypeComboBox->currentIndex();
    if(selected == 0){
        ui->targetIPLineEdit->setEnabled(false);
        ui->targetSpinBox->setEnabled(false);
        ui->autoConnCheckBox->setEnabled(false);
    }else{
        ui->targetIPLineEdit->setEnabled(true);
        ui->targetSpinBox->setEnabled(true);
        ui->autoConnCheckBox->setEnabled(true);
    }
}

void Dialog::on_sendPushButton_clicked()
{
   if(tcpSocket == NULL && tcpClientSocket == NULL && udpSocket == NULL){
       QMessageBox::warning(this, "warning", "没有可用连接！", QMessageBox::Ok, QMessageBox::Ok);
   }
   QString msg = ui->sendTextEdit->toPlainText();
   int index = ui->conTypeComboBox->currentIndex();
   if(index == 0){
       if(ui->hexSendCheckBox->isChecked()){
           tcpSocket->write(QByteArray::fromHex(msg.toLatin1()));
       }else{
            tcpSocket->write(msg.toUtf8().data());
       }
   }else if(index == 1){
       if(ui->hexSendCheckBox->isChecked()){
           tcpClientSocket->write(QByteArray::fromHex(msg.toLatin1()));
       }else{
            tcpClientSocket->write(msg.toUtf8().data());
       }
   }else{
       if(ui->hexSendCheckBox->isChecked()){
           udpSocket->write(QByteArray::fromHex(msg.toLatin1()));
       }else{
            udpSocket->write(msg.toUtf8().data());
       }
   }
   txcount += 1;
   ui->lineEditTxCount->setText(QString::number(txcount));
}

void Dialog::on_clearLogPushButton_clicked()
{
   ui->logTextEdit->setText(NULL);
}

void Dialog::on_clearRecPushButton_clicked()
{
   ui->receiveTextEdit->setText(NULL);
}

void Dialog::on_recountPushButton_clicked()
{
   ui->lineEditTxCount->setText(NULL);
   ui->lineEditRxCount->setText(NULL);
}

void Dialog::on_modbusDialogPushButton_clicked()
{
    if(mdialog == NULL){
        mdialog = new ModbusDialog(this);
        mdialog->setModal(true);
       // connect(mdialog->ui, SIGNAL(clicked()), mdialog, SLOT(accept()));
       // connect(mdialog->ui, SIGNAL(clicked()), mdialog, SLOT(reject()));


    }
    mdialog->show();
    // show modal window event loop and wait for button clicks
    int dialogCode = mdialog->exec();

    // act on dialog return code
    if(dialogCode == QDialog::Accepted) {
        // YesButton clicked
       // qDebug()<<"ok"<<*(mdialog->val);
        ui->sendTextEdit->setText(*(mdialog->val));
    }
    if(dialogCode == QDialog::Rejected) {
        // NoButton clicked
    }
}

void Dialog::on_dlt645DialogPushButton_clicked()
{
    if(ddialog == NULL){
        ddialog = new Dlt645Dialog(this);
        ddialog->setModal(true);
    }
    ddialog->show();
    int dialogCode = ddialog->exec();

    // act on dialog return code
    if(dialogCode == QDialog::Accepted) {
        // YesButton clicked
       // qDebug()<<"ok"<<*(mdialog->val);
        ui->sendTextEdit->setText(*(ddialog->val));
    }
}


void Dialog::closeEvent(QCloseEvent *event)
{
   //  qDebug()<<"dddd";
    if(tcpClientSocket != NULL){
        tcpClientSocket->disconnectFromHost();
        //tcpClientSocket->close();
    }
    if(tcpSocket != NULL){
        tcpSocket->disconnectFromHost();
       // tcpSocket->close();
        tcpSocket = NULL;
    }
    if(tcpServer != NULL){
        tcpServer->close();
    }
    // qDebug()<<"dddd";
    QDialog::closeEvent(event);

}

void Dialog::on_pushButtonClear_clicked()
{
    ui->textEditReceive->clear();
}

void Dialog::on_pushButtonCleanLog_clicked()
{
   ui->textEditLog->clear();
}

void Dialog::on_pushButtonOpen_clicked()
{
    if(serial == NULL){
        serial = new QSerialPort(this);
    }
    if(serial->isOpen()){
        serial->close();
        serial = NULL;
        ui->comboBoxPort->setEnabled(true);
        ui->comboBoxBuadRate->setEnabled(true);
        ui->comboBoxDataBit->setEnabled(true);
        ui->comboBoxCheck->setEnabled(true);
        ui->comboBoxStopBit->setEnabled(true);
        ui->comboBoxFlowControl->setEnabled(true);
        ui->pushButtonOpen->setText("打开");
        ui->pushButtonSend->setEnabled(false);
//enable settings.
        return;
    }

    serial->setPortName(ui->comboBoxPort->currentText());
    int index = ui->comboBoxBuadRate->currentIndex();
    if(index == 0){
        serial->setBaudRate(QSerialPort::Baud2400);
    }else if(index == 1){
        serial->setBaudRate(QSerialPort::Baud4800);
    }else if (index == 2){
        serial->setBaudRate(QSerialPort::Baud9600);
    }else if (index == 3){
        serial->setBaudRate(QSerialPort::Baud19200);
    }else if (index == 4){
        serial->setBaudRate(QSerialPort::Baud115200);
    }
    index = ui->comboBoxCheck->currentIndex();
    if(index == 0){
        serial->setParity(QSerialPort::NoParity);
    }else if(index == 1){
        serial->setParity(QSerialPort::OddParity);
    }else if(index == 2){
        serial->setParity(QSerialPort::EvenParity);
    }else if(index == 3){
        serial->setParity(QSerialPort::MarkParity);
    }else if(index == 4){
        serial->setParity(QSerialPort::SpaceParity);
    }
    index = ui->comboBoxDataBit->currentIndex();
    if(index == 0){
        serial->setDataBits(QSerialPort::Data8);
    }else if(index == 1){
        serial->setDataBits(QSerialPort::Data7);
    }else if(index == 2){
        serial->setDataBits(QSerialPort::Data6);
    }else if(index == 3){
        serial->setDataBits(QSerialPort::Data5);
    }
    index = ui->comboBoxStopBit->currentIndex();
    if(index == 0){
        serial->setStopBits(QSerialPort::OneStop);
    }else if(index == 1){
        serial->setStopBits(QSerialPort::OneAndHalfStop);
    }else if(index == 2){
        serial->setStopBits(QSerialPort::TwoStop);
    }
    index = ui->comboBoxFlowControl->currentIndex();
    if(index == 0){
        serial->setFlowControl(QSerialPort::NoFlowControl);
    }else if(index == 1){
        serial->setFlowControl(QSerialPort::SoftwareControl);
    }else if(index == 2){
        serial->setFlowControl(QSerialPort::HardwareControl);
    }
    if (serial->open(QIODevice::ReadWrite)) {
        ui->comboBoxPort->setEnabled(false);
        ui->comboBoxBuadRate->setEnabled(false);
        ui->comboBoxDataBit->setEnabled(false);
        ui->comboBoxCheck->setEnabled(false);
        ui->comboBoxStopBit->setEnabled(false);
        ui->comboBoxFlowControl->setEnabled(false);
        ui->pushButtonOpen->setText("关闭");
        ui->pushButtonSend->setEnabled(true);
        QString msg = QString(tr("Connected to %1 : %2, %3, %4, %5, %6"))
                              .arg(ui->comboBoxPort->currentText())
                              .arg(ui->comboBoxBuadRate->currentText())
                              .arg(ui->comboBoxDataBit->currentText())
                              .arg(ui->comboBoxCheck->currentText())
                              .arg(ui->comboBoxStopBit->currentText())
                              .arg(ui->comboBoxFlowControl->currentText());
        ui->textEditLog->setText(msg);
        connect(serial, &QSerialPort::readyRead, [=](){
            QByteArray data = serial->readAll();
            if(ui->checkBoxHexRec->isChecked()){
                ui->textEditReceive->append(data.toHex());
            }else{
                ui->textEditReceive->append(data);
            }
            if(ui->checkBoxEcho->isChecked()){
                on_pushButtonSend_clicked();
            }
            rxcount += 1;
            ui->lineEditRxCount->setText(QString::number(rxcount));
        });
     } else {
            QMessageBox::critical(this, tr("Error"), serial->errorString());
     }
}

void Dialog::on_pushButtonSend_clicked()
{
    if(serial == NULL && !serial->isOpen()){
        QMessageBox::warning(this, "warning", "没有可用连接！", QMessageBox::Ok, QMessageBox::Ok);
    }
    QString msg = ui->textEditSend->toPlainText();

    if(ui->checkBoxHexSend->isChecked()){
        serial->write(QByteArray::fromHex(msg.toLatin1()));
    }else{
         serial->write(msg.toUtf8().data());
    }
    txcount += 1;
    ui->lineEditTxCount->setText(QString::number(txcount));
}

void Dialog::on_pushButtonModbusDlg_clicked()
{
    if(mdialog == NULL){
        mdialog = new ModbusDialog(this);
        mdialog->setModal(true);
       // connect(mdialog->ui, SIGNAL(clicked()), mdialog, SLOT(accept()));
       // connect(mdialog->ui, SIGNAL(clicked()), mdialog, SLOT(reject()));


    }
    mdialog->show();
    // show modal window event loop and wait for button clicks
    int dialogCode = mdialog->exec();

    // act on dialog return code
    if(dialogCode == QDialog::Accepted) {
        // YesButton clicked
       // qDebug()<<"ok"<<*(mdialog->val);
        ui->textEditSend->setText(*(mdialog->val));
    }

}

void Dialog::on_pushButtonDltDlg_clicked()
{
    if(ddialog == NULL){
        ddialog = new Dlt645Dialog(this);
        ddialog->setModal(true);
    }
    ddialog->show();
    int dialogCode = ddialog->exec();

    // act on dialog return code
    if(dialogCode == QDialog::Accepted) {
        // YesButton clicked
       // qDebug()<<"ok"<<*(mdialog->val);
        ui->textEditSend->setText(*(ddialog->val));
    }
}

void Dialog::on_tabWidget_tabBarClicked(int index)
{
   if(index == 0){
       ui->openPushButton->setFocus();
   }else if(index == 1){
       ui->pushButtonOpen->setFocus();
   }
}
