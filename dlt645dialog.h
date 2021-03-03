#ifndef DLT645_DIALOG_H
#define DLT645_DIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QByteArray>

namespace Ui {
class Dlt645Dialog;
}

class Dlt645Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dlt645Dialog(QWidget *parent = 0);
    ~Dlt645Dialog();
    QString *val;
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_comboBoxCmd_currentIndexChanged(int index);

private:
    Ui::Dlt645Dialog *ui;
    quint8 csChecksum(const QByteArray &ba);
};

#endif // DIALOG_H
