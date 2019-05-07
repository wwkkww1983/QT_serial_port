#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QTimer>
#include "list.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    QSerialPort *serialport;
    QTimer *send_timer;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void child_close();

private slots:

    void readdata();

    void timer_send();

    void on_scan_port_clicked();

    void on_about_info_clicked();

    void on_exit_clicked();

    void on_open_port_clicked();

    void on_clear_receive_clicked();

    void on_clear_counter_clicked();

    void on_clear_send_clicked();

    void on_send_triger_clicked();

    void on_loopcheck_clicked();

    void on_list_window_clicked();

private:
    Ui::MainWindow *ui;
    QString send_buf;
    int send_length;
    int single_send_counter=0;
    int send_times_counter=0;
    list * list_send=0;
    bool child_alive=0;
    bool gengrated=0;

    int charTOhex(char dat);
};

#endif // MAINWINDOW_H
