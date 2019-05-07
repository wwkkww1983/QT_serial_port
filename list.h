#ifndef LIST_H
#define LIST_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QTimer>
#include <QModelIndexList>
#include <QDebug>
#include <QFileDialog>

namespace Ui {
class list;
}

class list : public QDialog
{
    Q_OBJECT

public:
    explicit list(QWidget *parent = 0);
    ~list();

    void get_serial_info(QSerialPort *upload_port);

private slots:

    void readdata_list();

    void timer_send_list();

    void on_return_main_clicked();

    void on_insert_above_clicked();

    void on_insert_below_clicked();

    void on_delete_select_clicked();

    void on_clear_list_clicked();

    void on_receive_clear_clicked();

    void on_insert_here_clicked();

    void on_send_list_itemSelectionChanged();

    void on_send_head_clicked();

    void on_send_select_clicked();

    void on_send_stop_clicked();

    void on_insert_last_clicked();

    void on_send_once_clicked();

    void on_save_list_clicked();

    void on_load_list_clicked();

signals:
    void child_done();

private:
    Ui::list *ui;
    QSerialPort *serialport_list;
    QTimer *send_timer_list;
    int send_counter=0;
    QString send_buf_list;

    int charTOhex_list(char dat);
};

#endif // LIST_H
