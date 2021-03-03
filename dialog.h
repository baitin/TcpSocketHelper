#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTcpServer>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "modbusdialog.h"
#include "dlt645dialog.h"
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_openPushButton_clicked();

    void on_conTypeComboBox_currentIndexChanged(int index);

    void on_sendPushButton_clicked();

    void on_clearLogPushButton_clicked();

    void on_clearRecPushButton_clicked();

    void on_recountPushButton_clicked();

    void on_modbusDialogPushButton_clicked();

    void on_dlt645DialogPushButton_clicked();

    void socketError(QAbstractSocket::SocketError);

//private signals:
  //  void windowClose();

   // void on_Dialog_destroyed();

    void on_pushButtonClear_clicked();

    void on_pushButtonCleanLog_clicked();

    void on_pushButtonOpen_clicked();

    void on_pushButtonSend_clicked();

    void on_pushButtonModbusDlg_clicked();

    void on_pushButtonDltDlg_clicked();

    void on_tabWidget_tabBarClicked(int index);

private:
    Ui::Dialog *ui;
    QTcpServer *tcpServer;
    QUdpSocket *udpSocket;
    QTcpSocket *tcpSocket;
    ModbusDialog *mdialog;
    Dlt645Dialog *ddialog;
    QTcpSocket *tcpClientSocket;
    QSerialPort *serial;
    int txcount = 0;
    int rxcount = 0;
    void openServer();
    void openClient();
};

#endif // DIALOG_H
