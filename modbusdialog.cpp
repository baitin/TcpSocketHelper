#include "modbusdialog.h"
#include "ui_modbusdialog.h"
#include <QDebug>

ModbusDialog::ModbusDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModbusDialog)
{
    ui->setupUi(this);
    //setStyle("macintosh");
}


ModbusDialog::~ModbusDialog()
{
    delete ui;
}

void ModbusDialog::on_comboBoxFun_currentIndexChanged(int index)
{
    if(index == 2 || index == 4){
        ui->spinBoxLength->setEnabled(true);
        ui->lineEditValue->setEnabled(true);
        ui->spinBoxCount->setEnabled(false);
    }else if(index == 0 || index == 1 || index == 6 || index == 7){
        ui->spinBoxCount->setEnabled(true);
        ui->spinBoxLength->setEnabled(false);
        ui->lineEditValue->setEnabled(false);
    }else if(index == 3 || index == 5){
        ui->spinBoxCount->setEnabled(false);
        ui->spinBoxLength->setEnabled(false);
        ui->lineEditValue->setEnabled(true);
    }else{
        ui->spinBoxLength->setEnabled(false);
        ui->spinBoxCount->setEnabled(true);
        ui->lineEditValue->setEnabled(false);
    }
}


void ModbusDialog::on_buttonBox_clicked(QAbstractButton *button)
{
   val = new QString();
   val->append((QString("%1").arg(ui->spinBoxAddr->text().toInt(), 2, 16, QLatin1Char('0'))).toUpper());
   if(ui->comboBoxFun->currentIndex() == 0){
       val->append("03");
   }else if(ui->comboBoxFun->currentIndex() == 1){
       val->append("04");
   }else if(ui->comboBoxFun->currentIndex() == 2){
       val->append("10");
   }else if(ui->comboBoxFun->currentIndex() == 3){
       val->append("06");
   }else if(ui->comboBoxFun->currentIndex() == 4){
       val->append("0f");
   }else if(ui->comboBoxFun->currentIndex() == 5){
       val->append("05");
   }else if(ui->comboBoxFun->currentIndex() == 6){
       val->append("01");
   }else if(ui->comboBoxFun->currentIndex() == 7){
       val->append("02");
   }

   val->append((QString("%1").arg(ui->spinBoxStart->text().toInt(), 4, 16, QLatin1Char('0'))).toUpper()); //起始地址
   if(ui->comboBoxFun->currentIndex() != 3 && ui->comboBoxFun->currentIndex() != 5){
    val->append((QString("%1").arg(ui->spinBoxCount->text().toInt(), 4, 16, QLatin1Char('0'))).toUpper()); //数量
   }
   if(ui->comboBoxFun->currentIndex() == 2 || ui->comboBoxFun->currentIndex() == 4){
       val->append((QString("%1").arg(ui->spinBoxLength->text().toInt(), 2, 16, QLatin1Char('0'))).toUpper()); //写入值长度
   }
   if(ui->comboBoxFun->currentIndex() == 2 || ui->comboBoxFun->currentIndex() == 4 || ui->comboBoxFun->currentIndex() == 3 || ui->comboBoxFun->currentIndex() == 5){
       val->append(ui->lineEditValue->text()); // 写入址
   }
   QByteArray ba = QByteArray::fromHex(val->toLatin1().data());
   if(ui->comboBoxPro->currentIndex()==0){
       quint16 a =  JQChecksum::crc16ForModbus(ba);
       val->append((QString("%1").arg(a&0xFF, 2, 16, QLatin1Char('0'))).toUpper());
       val->append((QString("%1").arg(a>>8, 2, 16, QLatin1Char('0'))).toUpper());
   }else if(ui->comboBoxPro->currentIndex()==1){
       int len = val->length();
       ti += 1;
       val->insert(0,  (QString("%1").arg(len/2, 4, 16, QLatin1Char('0'))).toUpper()); // Length
       val->insert(0, "0000"); // Protocol Identifier
       val->insert(0, (QString("%1").arg(ti, 4, 16, QLatin1Char('0'))).toUpper()); // Transaction Identifier
   }
}
