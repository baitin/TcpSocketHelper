#include "dlt645dialog.h"
#include "ui_dlt645dialog.h"
#include <QDebug>

Dlt645Dialog::Dlt645Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dlt645Dialog)
{
    ui->setupUi(this);
    QStringList list;
    for(int i = 0; i <=255; i++){
        list<<(QString("%1").arg(i, 2, 16, QLatin1Char('0'))).toUpper();
    }
    ui->comboBoxDi0->addItems(list);
    ui->comboBoxDi1->addItems(list);
    ui->comboBoxDi2->addItems(list);
    ui->comboBoxDi3->addItems(list);
    //[0-9a-fA-F]+
   // QRegExp rx("[0-9a-fA-f]+");
   // QRegExpValidator v(rx, 0);
   // ui->lineEditAddr->setValidator(&v);
  //  setStyle("macintosh");
}

Dlt645Dialog::~Dlt645Dialog()
{
    delete ui;
}

void Dlt645Dialog::on_buttonBox_clicked(QAbstractButton *button)
{
    //开头
    val = new QString("68");
    //地址
    if(ui->comboBox->currentIndex() == 1){
        val->append("AAAAAAAAAAAA");
    }else{
        val->append(ui->lineEditAddr->text());
    }
    val->append("68");
    //控制码//长度
    if((ui->comboBoxCmd->currentIndex() == 0 && ui->comboBox->currentIndex() != 1) || (ui->comboBox->currentIndex() > 1 && ui->comboBoxCmd->currentIndex() != 4)){
        val->append("11");
        val->append("04");
    }else if(ui->comboBoxCmd->currentIndex() == 1 && ui->comboBox->currentIndex() == 0){
        val->append("12");//读后续数据
    }else if(ui->comboBoxCmd->currentIndex() == 2 && ui->comboBox->currentIndex() == 0){
        val->append("15");//改通讯地址
        val->append("06");
    }else if(ui->comboBoxCmd->currentIndex() == 3 && ui->comboBox->currentIndex() == 0){
        val->append("17");//更新波特率
        val->append("01");
    }else if(ui->comboBox->currentIndex() == 1){
        val->append("13");
        val->append("00");
    }else if(ui->comboBoxCmd->currentIndex() == 4){
        val->append("1C");
    }
    if(ui->comboBox->currentIndex() > 1){//常用命令
        if(ui->comboBox->currentIndex() == 2){
            val->append("33333333");
        }else if(ui->comboBox->currentIndex() == 3){
            val->append("33323333");
        }else if(ui->comboBox->currentIndex() == 4){
            val->append("33343435"); //V A
        }else if(ui->comboBox->currentIndex() == 5){
            val->append("33353435");
        }else if(ui->comboBox->currentIndex() == 6){
            val->append("33363435");//V C
        }else if(ui->comboBox->currentIndex() == 7){
            val->append("33323435");
        }else if(ui->comboBox->currentIndex() == 8){
            val->append("33343535"); //I A
        }else if(ui->comboBox->currentIndex() == 9){
            val->append("33353535"); //I B
        }else if(ui->comboBox->currentIndex() == 10){
            val->append("33363535"); //I C
        }else if(ui->comboBox->currentIndex() == 11){
            val->append("33323535"); //I
        }else if(ui->comboBox->currentIndex() == 12){
            val->append("33323635"); //P
        }else if(ui->comboBox->currentIndex() == 13){
            val->append("33323735"); //Q
        }else if(ui->comboBox->currentIndex() == 14){
            val->append("33323935"); //PFS
        }else if(ui->comboBox->currentIndex() == 15){
            val->append("3533B335"); //FF
        }else if(ui->comboBox->currentIndex() == 16){
            val->append("040005FF"); //ST
        }else if(ui->comboBox->currentIndex() == 17){
            val->append("0290FF00"); //leakage current
        }else if(ui->comboBox->currentIndex() == 18){
            val->append("0E3434343933333333333333333335"); //分
        }else if(ui->comboBox->currentIndex() == 19){
            val->append("0E3435343933333333333333333335"); //合
        }

    }else if (ui->comboBoxCmd->currentIndex() == 2 && ui->comboBox->currentIndex() == 0){//修改地址
        val->append(ui->lineEditVal->text());
    }else if (ui->comboBoxCmd->currentIndex() == 3 && ui->comboBox->currentIndex() == 0){//改波特率
        if(ui->comboBoxRate->currentIndex() == 0){ // 2400
            val->append("3B");
        }else if(ui->comboBoxRate->currentIndex() == 1){ //4800
            val->append("43");
        }else if(ui->comboBoxRate->currentIndex() == 2){ //9600
            val->append("53");
        }else if(ui->comboBoxRate->currentIndex() == 3){ //19200
            val->append("73");
        }
    }else if (ui->comboBox->currentIndex() != 1){//任意命令
    //数据标识
        val->append((QString("%1").arg((ui->comboBoxDi0->currentIndex() + 51)&0xFF, 2, 16, QLatin1Char('0'))).toUpper());
        val->append((QString("%1").arg((ui->comboBoxDi1->currentIndex() + 51)&0xFF, 2, 16, QLatin1Char('0'))).toUpper());
        val->append((QString("%1").arg((ui->comboBoxDi2->currentIndex() + 51)&0xFF, 2, 16, QLatin1Char('0'))).toUpper());
        val->append((QString("%1").arg((ui->comboBoxDi3->currentIndex() + 51)&0xFF, 2, 16, QLatin1Char('0'))).toUpper());
    }
    //CS校验
    QByteArray ba = QByteArray::fromHex(val->toLatin1().data());
    int csSum = csChecksum(ba);
    val->insert(0, QString("FEFEFEFE"));
    val->append((QString("%1").arg(csSum&0xFF, 2, 16, QLatin1Char('0'))).toUpper());
    //结尾
    val->append("16");
}

quint8 Dlt645Dialog::csChecksum(const QByteArray &ba){
    quint8 cs = 0;
    for(int i = 0; i < ba.length(); i++){
        cs += ba[i];
    }
    return cs;
}

void Dlt645Dialog::on_comboBoxCmd_currentIndexChanged(int index)
{
   if(ui->comboBoxCmd->currentIndex() == 2){// 修改通讯地址
        ui->lineEditVal->setEnabled(true);
        ui->comboBoxRate->setEnabled(false);
   }else if(ui->comboBoxCmd->currentIndex() == 3){// 修改波特率
       ui->lineEditVal->setEnabled(false);
       ui->comboBoxRate->setEnabled(true);
   }else{
        ui->lineEditVal->setEnabled(false);
        ui->comboBoxRate->setEnabled(false);
   }
}
