#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "list.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serialport=new QSerialPort(this);
    send_timer=new QTimer(this);
    QObject::connect(serialport,&QSerialPort::readyRead,this,&MainWindow::readdata);
    QObject::connect(send_timer,&QTimer::timeout,this,&MainWindow::timer_send);
    setWindowTitle(QStringLiteral("串口调试工具"));
    setWindowIcon(QIcon(":/ico/icon"));

}

MainWindow::~MainWindow()
{
    if(send_timer!=NULL)
    {
        delete send_timer;
    }
    if(serialport!=NULL)
    {
        delete serialport;
    }
    delete ui;
}

void MainWindow::child_close()
{
    child_alive=0;//change data flow direction
}


void MainWindow::on_scan_port_clicked()
{
    ui->port_number->clear();
    if(serialport!=NULL)//check if port is available
    {
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())//find available port name
        {
            QSerialPort temp_port;
            temp_port.setPort(info);
            if(temp_port.open(QIODevice::ReadWrite))
            {
                bool isinlist=0;
                for(int i=0;i<(ui->port_number->count());i++)//check port name list ,avoid same item
                {
                    if(temp_port.portName()==ui->port_number->itemText(i))
                    {
                        isinlist=1;
                    }
                }
                if(isinlist==0)
                {
                    ui->port_number->addItem(temp_port.portName());
                }
                temp_port.close();
            }
        }
    }
    else
    {
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("串口初始化失败"),QMessageBox::Ok);
    }
}



void MainWindow::on_about_info_clicked()
{
    QMessageBox::about(this,QStringLiteral("关于程序"),QStringLiteral("由LXL编写\n仅用于教育用途\n东南大学,2018"));
}

void MainWindow::on_exit_clicked()
{
    this->close();
}

void MainWindow::on_open_port_clicked()
{
    if(serialport==NULL)
    {
        serialport=new QSerialPort();
    }
    if(serialport->isOpen())//close port
    {
        serialport->close();
        ui->port_number->setEnabled(1);
        ui->baud_rate->setEnabled(1);
        ui->data_bit->setEnabled(1);
        ui->data_check->setEnabled(1);
        ui->stop_bit->setEnabled(1);
        ui->flow_control->setEnabled(1);
        ui->scan_port->setEnabled(1);
        ui->send_triger->setDisabled(1);
        ui->list_window->setDisabled(1);
        ui->open_port->setText(QStringLiteral("打开串口"));
    }
    else
    {
        if(ui->port_number->count()!=0)//exist available port,following port config
        {
            serialport->setPortName(ui->port_number->currentText());
            if(serialport->open(QIODevice::ReadWrite))
            {
                ui->port_number->setDisabled(1);
                ui->baud_rate->setDisabled(1);
                ui->data_bit->setDisabled(1);
                ui->data_check->setDisabled(1);
                ui->stop_bit->setDisabled(1);
                ui->flow_control->setDisabled(1);
                ui->scan_port->setDisabled(1);
                ui->send_triger->setEnabled(1);
                ui->list_window->setEnabled(1);
                ui->open_port->setText(QStringLiteral("关闭串口"));
            }
            else
            {
                QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("串口打开失败"),QMessageBox::Ok);
                return;
            }
            serialport->setBaudRate(ui->baud_rate->value());
            switch(ui->data_bit->currentIndex())
            {
            case 0:serialport->setDataBits(QSerialPort::Data5);break;
            case 1:serialport->setDataBits(QSerialPort::Data6);break;
            case 2:serialport->setDataBits(QSerialPort::Data7);break;
            case 3:serialport->setDataBits(QSerialPort::Data8);break;
            default:serialport->setDataBits(QSerialPort::Data8);break;
            }
            switch(ui->stop_bit->currentIndex())
            {
            case 0:serialport->setStopBits(QSerialPort::OneStop);break;
            case 1:serialport->setStopBits(QSerialPort::OneAndHalfStop);break;
            case 2:serialport->setStopBits(QSerialPort::TwoStop);break;
            default:serialport->setStopBits(QSerialPort::OneStop);break;
            }
            switch(ui->data_check->currentIndex())
            {
            case 0:serialport->setParity(QSerialPort::NoParity);break;
            case 1:serialport->setParity(QSerialPort::OddParity);break;
            case 2:serialport->setParity(QSerialPort::EvenParity);break;
            case 3:serialport->setParity(QSerialPort::MarkParity);break;
            case 4:serialport->setParity(QSerialPort::SpaceParity);break;
            default:serialport->setParity(QSerialPort::NoParity);break;
            }
            switch(ui->flow_control->currentIndex())
            {
            case 0:serialport->setFlowControl(QSerialPort::NoFlowControl);break;
            case 1:serialport->setFlowControl(QSerialPort::HardwareControl);break;
            case 2:serialport->setFlowControl(QSerialPort::SoftwareControl);break;
            default:serialport->setFlowControl(QSerialPort::NoFlowControl);break;
            }
        }
        else
        {
            QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("无可用串口,请尝试刷新列表"),QMessageBox::Ok);
        }
    }
}

void MainWindow::readdata()
{
    if(child_alive==false)
    {
        QByteArray buf;
        buf=serialport->readAll();//get register data
        if(!buf.isEmpty())
        {
            if(ui->receive_type->currentIndex()==0)
            {
                ui->receive_display->append(tr(buf));//text display
            }
            else
            {
                ui->receive_display->append(tr(buf.toHex()));//hex display
            }
            ui->receive_counter->display(ui->receive_counter->intValue()+buf.length());//counter count
        }
    }
}

void MainWindow::on_clear_receive_clicked()
{
    ui->receive_display->clear();
}

void MainWindow::on_clear_counter_clicked()
{
    ui->receive_counter->display(0);
    ui->send_counter->display(0);
}

void MainWindow::on_clear_send_clicked()
{
    ui->send_input->clear();
}

void MainWindow::on_send_triger_clicked()
{
    if(ui->send_input->toPlainText().count()==0)
    {
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("发送区为空"),QMessageBox::Ok);
    }
    else
    {
        send_buf=ui->send_input->toPlainText();//get send data
        if(ui->send_type->currentIndex()==0)//text
        {
            send_length=send_buf.length();
        }
        else//hex
        {
            send_buf.simplified();//remove \n \t \r etc
            send_buf.remove('\n');//i don't know why,but simplified() failed to remove '\n'
            send_buf.remove(' ');
            send_buf.remove("0x");
            send_buf.remove("0X");//finally the form is like "112233AAFF"
            if(send_buf.length()%2==1)
            {
                send_buf.insert(0,'0');
            }
            send_length=send_buf.length()/2;
        }
        if(ui->send_triger->text()==QStringLiteral("发送"))//send action,for
        {
                single_send_counter=0;
                send_times_counter=0;
                send_timer->start(ui->period->value());
                ui->single_byte_check->setDisabled(1);
                ui->loopcheck->setDisabled(1);
                ui->send_triger->setText(QStringLiteral("中止发送"));
                timer_send();
        }
        else//kill send process
        {
            send_timer->stop();
            ui->single_byte_check->setEnabled(1);
            ui->loopcheck->setEnabled(1);
            ui->send_triger->setText(QStringLiteral("发送"));
        }
    }
}

void MainWindow::timer_send()
{
    if(ui->single_byte_check->isChecked())//send byte by byte
    {
        if(ui->send_type->currentIndex()==0)//text
        {
            char mini_buf[2]={send_buf.at(single_send_counter).toLatin1(),0};
            serialport->write(mini_buf);
        }
        else//hex
        {
            char mini_buf[4]={0,0,0,0};//string to hex
            mini_buf[0]=charTOhex(send_buf.at(2*single_send_counter).toLatin1());
            mini_buf[1]=charTOhex(send_buf.at(2*single_send_counter+1).toLatin1());
            if(mini_buf[0]<0||mini_buf[1]<0)
            {
                QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("16进制数出界\n请检查数据是否存在大于“0XFF”的部分"),QMessageBox::Ok);
                send_timer->stop();//kill send process
                ui->single_byte_check->setEnabled(1);
                ui->loopcheck->setEnabled(1);
                ui->send_triger->setText(QStringLiteral("发送"));
                return;
            }
            else
            {
                mini_buf[2]=mini_buf[0]*16+mini_buf[1];
                serialport->write(mini_buf+2);//send one byte
            }
        }
        ui->send_counter->display(ui->send_counter->intValue()+1);
        single_send_counter++;
        if(single_send_counter>=send_length)//one loop finish
        {
           single_send_counter=0;
           send_times_counter++;
           if((send_times_counter>=ui->send_times->value())&&(ui->loopcheck->isChecked()==0))//if not continues send,send finish
           {
               send_timer->stop();
               ui->single_byte_check->setEnabled(1);
               ui->loopcheck->setEnabled(1);
               ui->send_triger->setText(QStringLiteral("发送"));
           }
        }
    }
    else//send alltogether
    {
        if(ui->send_type->currentIndex()==0)//text
        {
            serialport->write(send_buf.toLatin1());
        }
        else//hex
        {
            QByteArray hex_buf;
            for(int i=0;i<send_length;i++)//convent
            {
                char mini_buf[3]={0};
                mini_buf[0]=charTOhex(send_buf.at(2*i).toLatin1());
                mini_buf[1]=charTOhex(send_buf.at(2*i+1).toLatin1());
                if(mini_buf[0]<0||mini_buf[1]<0)
                {
                    QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("16进制数出界\n请检查数据是否存在大于“0XFF”的部分"),QMessageBox::Ok);
                    send_timer->stop();
                    ui->single_byte_check->setEnabled(1);
                    ui->loopcheck->setEnabled(1);
                    ui->send_triger->setText(QStringLiteral("发送"));
                    return;
                }
                else
                {
                    mini_buf[2]=mini_buf[0]*16+mini_buf[1];
                    hex_buf.append(mini_buf[2]);
                }
            }
            serialport->write(hex_buf);
        }
        ui->send_counter->display(ui->send_counter->intValue()+send_length);
        send_times_counter++;
        if((send_times_counter>=ui->send_times->value())&&(ui->loopcheck->isChecked()==0))//if not continues send,send finish
        {
            send_timer->stop();
            ui->single_byte_check->setEnabled(1);
            ui->loopcheck->setEnabled(1);
            ui->send_triger->setText(QStringLiteral("发送"));
        }
    }
}

int MainWindow::charTOhex(char dat)
{
    if(dat>='0'&&dat<='9')
    {
        return dat-'0';
    }
    else
    {
        if(dat>='a'&&dat<='f')
        {
            return dat-'a'+10;
        }
        else
        {
            if(dat>='A'&&dat<='F')
            {
                return dat-'A'+10;
            }
            else
            {
                return -1;//not hex data
            }
        }
    }
}

void MainWindow::on_loopcheck_clicked()
{
    if(ui->loopcheck->isChecked())
    {
        ui->send_times->setDisabled(1);
    }
    else
    {
         ui->send_times->setEnabled(1);
    }
}

void MainWindow::on_list_window_clicked()
{
    child_alive=true;
    if(gengrated==0)//child not exist
    {
        list_send=new list(this);
        list_send->setModal(true);
        list_send->get_serial_info(serialport);//pass port info
        connect(list_send,SIGNAL(child_done()),this,SLOT(child_close()));//signal connection
        list_send->show();
        gengrated=1;
    }
    else//child is hidden
    {
        list_send->show();
    }
}
