#ifndef MODBUS_DIALOG_H
#define MODBUS_DIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include "JQChecksum.h"

namespace Ui {
class ModbusDialog;
}

class ModbusDialog : public QDialog
{
    Q_OBJECT

    int ti = 0;

public:
    explicit ModbusDialog(QWidget *parent = 0);
    ~ModbusDialog();
    QString *val;
private slots:
    void on_comboBoxFun_currentIndexChanged(int index);

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::ModbusDialog *ui;
};

#endif // DIALOG_H
