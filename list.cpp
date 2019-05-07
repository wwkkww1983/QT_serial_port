#include "list.h"
#include "ui_list.h"

list::list(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::list)
{
    ui->setupUi(this);
    ui->current_index->setText(QString::number(1, 10));//selection clear
    send_timer_list=new QTimer(this);
    setWindowTitle(QStringLiteral("队列发送"));
    setWindowIcon(QIcon(":/ico/icon"));
}

list::~list()
{
    if(send_timer_list!=NULL)
    {
        delete send_timer_list;
    }
    delete ui;
}

void list::on_return_main_clicked()
{
    send_timer_list->stop();//notify parent . change data flow direction
    emit child_done();
    this->close();
}

void list::on_insert_above_clicked()
{
    QItemSelectionModel* temp=ui->send_list->selectionModel();
    QString buf=ui->edit_item->text();
    if(buf.isEmpty())
    {
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("新数据为空"),QMessageBox::Ok);
    }
    else
    {
        char index;
        if(temp->selectedRows().length())//if select something
        {
            index=temp->selectedRows().at(0).row();
        }
        else
        {
            QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("无被选项"),QMessageBox::Ok);
            return;
        }
        if(ui->send_mode->currentIndex()==0)//text
        {
            buf.insert(0,"TXT: ");
            ui->send_list->insertItem(index,buf.toLatin1());
        }
        else
        {
            buf.simplified();//remove \n \t \r etc
            buf.remove('\n');//i don't know why,but simplified() failed to remove '\n'
            buf.remove(' ');
            buf.remove("0x");
            buf.remove("0X");//finally the form is like "112233AAFF"
            if(buf.length()%2==1)
            {
                buf.insert(0,'0');
            }
            buf.insert(0,"HEX: ");
            ui->send_list->insertItem(index,buf.toLatin1());
        }
        if(temp->selectedRows().length()==0)
        {
            ui->current_index->setText(QString::number(1,10));//first data
        }
        else
        {
            ui->current_index->setText(QString::number(index+2,10));//add data
        }
        ui->list_len->setText(QString::number(ui->send_list->count(), 10));//refresh length
    }
}

void list::on_insert_below_clicked()
{
    QItemSelectionModel* temp=ui->send_list->selectionModel();
    QString buf=ui->edit_item->text();
    if(buf.isEmpty())
    {
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("新数据为空"),QMessageBox::Ok);
    }
    else
    {
        char index;
        if(temp->selectedRows().length())//if select something
        {
            index=temp->selectedRows().at(0).row()+1;
        }
        else
        {
            QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("无被选项"),QMessageBox::Ok);
            return;
        }
        if(ui->send_mode->currentIndex()==0)//text
        {
            buf.insert(0,"TXT: ");
            ui->send_list->insertItem(index,buf.toLatin1());
        }
        else
        {
            buf.simplified();//remove \n \t \r etc
            buf.remove('\n');//i don't know why,but simplified() failed to remove '\n'
            buf.remove(' ');
            buf.remove("0x");
            buf.remove("0X");//finally the form is like "112233AAFF"
            if(buf.length()%2==1)
            {
                buf.insert(0,'0');
            }
            buf.insert(0,"HEX: ");
            ui->send_list->insertItem(index,buf.toLatin1());
        }
        ui->list_len->setText(QString::number(ui->send_list->count(), 10));//refresh length
    }
}

void list::on_delete_select_clicked()
{
    QItemSelectionModel* temp=ui->send_list->selectionModel();
    char index;
    if(temp->selectedRows().length())//if select something
    {
        index=temp->selectedRows().at(0).row();//get index
        ui->send_list->takeItem(index);
        if((index+1)<ui->send_list->count())
        {
            ui->current_index->setText(QString::number(index+1,10));//normal item
        }
        else
        {
            if(ui->send_list->count()==0)
            {
                ui->current_index->setText(QString::number(1,10));//first item
            }
            else
            {
                ui->current_index->setText(QString::number(ui->send_list->count(), 10));//last item
            }
        }
        ui->list_len->setText(QString::number(ui->send_list->count(), 10));//refresh length
    }
    else
    {
       QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("无被选项"),QMessageBox::Ok);
    }
}

void list::on_clear_list_clicked()
{
    ui->send_list->clear();
    ui->current_index->setText(QString::number(1,10));
    ui->list_len->setText(QStringLiteral("0"));
}


void list::on_receive_clear_clicked()
{
    ui->receive_window->clear();
}

void list::readdata_list()
{
    QByteArray buf;
    buf=serialport_list->readAll();//get register data
    if(!buf.isEmpty())
    {
        if(ui->receive_mode->currentIndex()==0)
        {
            ui->receive_window->append(tr(buf));//text display
        }
        else
        {
            ui->receive_window->append(tr(buf.toHex()));//hex display
        }
    }
}

 void list::get_serial_info(QSerialPort *upload_port)//pass port info, connect slot
 {
     serialport_list=upload_port;
     connect(serialport_list,&QSerialPort::readyRead,this,&list::readdata_list);
     connect(send_timer_list,&QTimer::timeout,this,&list::timer_send_list);

 }

void list::on_insert_here_clicked()
{
    QItemSelectionModel* temp=ui->send_list->selectionModel();
    QString buf=ui->edit_item->text();
    if(buf.isEmpty())
    {
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("新数据为空"),QMessageBox::Ok);
    }
    else
    {
        if(temp->selectedRows().length())
        {
            if(ui->send_mode->currentIndex()==0)//text
            {
                buf.insert(0,"TXT: ");
                ui->send_list->selectedItems().at(0)->setText(buf);
            }
            else
            {
                buf.simplified();//remove \n \t \r etc
                buf.remove('\n');//i don't know why,but simplified() failed to remove '\n'
                buf.remove(' ');
                buf.remove("0x");
                buf.remove("0X");//finally the form is like "112233AAFF"
                if(buf.length()%2==1)
                {
                    buf.insert(0,'0');
                }
                buf.insert(0,"HEX: ");
                ui->send_list->selectedItems().at(0)->setText(buf);
            }

        }
        else
        {
            QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("无被选项"),QMessageBox::Ok);
        }
    }

}

void list::on_send_list_itemSelectionChanged()
{
    QItemSelectionModel* temp=ui->send_list->selectionModel();
    if(temp->selectedRows().length())
    {
        ui->current_index->setText(QString::number(temp->selectedRows().at(0).row()+1, 10));//refresh selection
    }
}



void list::on_send_head_clicked()
{
    if(ui->send_list->count())
    {
        send_counter=0;
        send_buf_list=ui->send_list->item(send_counter)->text();//get send data
        send_counter++;
        send_timer_list->start(ui->send_duration->value());
        ui->send_once->setDisabled(1);//ui change
        ui->send_head->setDisabled(1);
        ui->send_select->setDisabled(1);
        ui->send_stop->setEnabled(1);
        ui->insert_above->setDisabled(1);
        ui->insert_below->setDisabled(1);
        ui->insert_here->setDisabled(1);
        ui->delete_select->setDisabled(1);
        ui->clear_list->setDisabled(1);
        timer_send_list();
    }
    else
    {
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("空队列"),QMessageBox::Ok);
    }
}

void list::on_send_select_clicked()
{
    if(ui->send_list->selectedItems().isEmpty()==0)
    {
        send_counter=ui->current_index->text().toInt()-1;
        send_buf_list=ui->send_list->item(send_counter)->text();//get send data
        send_counter++;
        send_timer_list->start(ui->send_duration->value());
        ui->send_once->setDisabled(1);//ui change
        ui->send_head->setDisabled(1);
        ui->send_select->setDisabled(1);
        ui->send_stop->setEnabled(1);
        ui->insert_above->setDisabled(1);
        ui->insert_below->setDisabled(1);
        ui->insert_here->setDisabled(1);
        ui->delete_select->setDisabled(1);
        ui->clear_list->setDisabled(1);
        timer_send_list();
    }
    else
    {
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("无被选项"),QMessageBox::Ok);
    }
}

void list::on_send_stop_clicked()
{
    send_timer_list->stop();
    ui->send_once->setEnabled(1);//ui change
    ui->send_head->setEnabled(1);
    ui->send_select->setEnabled(1);
    ui->send_stop->setDisabled(1);
    ui->insert_above->setEnabled(1);
    ui->insert_below->setEnabled(1);
    ui->insert_here->setEnabled(1);
    ui->delete_select->setEnabled(1);
    ui->clear_list->setEnabled(1);
}

void list::timer_send_list()
{
    if(send_buf_list.at(0)=='T')//text
    {
        send_buf_list.remove(0,5);//type mark remove
        serialport_list->write(send_buf_list.toLatin1());
    }
    else//hex
    {
        send_buf_list.remove(0,5);
        QByteArray hex_buf;
        for(int i=0;i<(send_buf_list.length()/2);i++)//convent
        {
            char mini_buf[3]={0};
            mini_buf[0]=charTOhex_list(send_buf_list.at(2*i).toLatin1());
            mini_buf[1]=charTOhex_list(send_buf_list.at(2*i+1).toLatin1());
            if(mini_buf[0]<0||mini_buf[1]<0)
            {
                qDebug("%i\t %i",mini_buf[0],mini_buf[1]);
                QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("16进制数出界\n请检查数据是否存在大于“0XFF”的部分\n"),QMessageBox::Ok);
                send_timer_list->stop();
                ui->send_once->setEnabled(1);//ui change
                ui->send_head->setEnabled(1);
                ui->send_select->setEnabled(1);
                ui->send_stop->setDisabled(1);
                ui->insert_above->setEnabled(1);
                ui->insert_below->setEnabled(1);
                ui->insert_here->setEnabled(1);
                ui->delete_select->setEnabled(1);
                ui->clear_list->setEnabled(1);
                return;
            }
            else
            {
                mini_buf[2]=mini_buf[0]*16+mini_buf[1];
                hex_buf.append(mini_buf[2]);
            }
        }
        serialport_list->write(hex_buf);
    }
    if(send_counter>=ui->send_list->count())//finish
    {
        send_timer_list->stop();
        ui->send_once->setEnabled(1);//ui change
        ui->send_head->setEnabled(1);
        ui->send_select->setEnabled(1);
        ui->send_stop->setDisabled(1);
        ui->insert_above->setEnabled(1);
        ui->insert_below->setEnabled(1);
        ui->insert_here->setEnabled(1);
        ui->delete_select->setEnabled(1);
        ui->clear_list->setEnabled(1);
    }
    else//next
    {
        send_buf_list=ui->send_list->item(send_counter)->text();//get send data
        send_counter++;
    }
}

int list::charTOhex_list(char dat)
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


void list::on_insert_last_clicked()
{
    QString buf=ui->edit_item->text();
    if(buf.isEmpty())
    {
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("新数据为空"),QMessageBox::Ok);
    }
    else
    {
        char index;
        index=ui->send_list->count();
        if(ui->send_mode->currentIndex()==0)//text
        {
            buf.insert(0,"TXT: ");
            ui->send_list->insertItem(index,buf.toLatin1());
        }
        else
        {
            buf.simplified();//remove \n \t \r etc
            buf.remove('\n');//i don't know why,but simplified() failed to remove '\n'
            buf.remove(' ');
            buf.remove("0x");
            buf.remove("0X");//finally the form is like "112233AAFF"
            if(buf.length()%2==1)
            {
                buf.insert(0,'0');
            }
            buf.insert(0,"HEX: ");
            ui->send_list->insertItem(index,buf.toLatin1());
        }
        ui->list_len->setText(QString::number(ui->send_list->count(), 10));
    }
}

void list::on_send_once_clicked()
{
    if(ui->send_list->selectedItems().isEmpty()==0)
    {
        send_counter=ui->current_index->text().toInt()-1;
        send_buf_list=ui->send_list->item(send_counter)->text();//get send data
        send_counter++;
        ui->send_once->setDisabled(1);//ui change
        ui->send_head->setDisabled(1);
        ui->send_select->setDisabled(1);
        ui->send_stop->setEnabled(1);
        ui->insert_above->setDisabled(1);
        ui->insert_below->setDisabled(1);
        ui->insert_here->setDisabled(1);
        ui->delete_select->setDisabled(1);
        ui->clear_list->setDisabled(1);
        timer_send_list();
        ui->send_once->setEnabled(1);//ui change
        ui->send_head->setEnabled(1);
        ui->send_select->setEnabled(1);
        ui->send_stop->setDisabled(1);
        ui->insert_above->setEnabled(1);
        ui->insert_below->setEnabled(1);
        ui->insert_here->setEnabled(1);
        ui->delete_select->setEnabled(1);
        ui->clear_list->setEnabled(1);

    }
    else
    {
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("无被选项"),QMessageBox::Ok);
    }

}

void list::on_save_list_clicked()
{
    QString file_addr = QFileDialog::getSaveFileName(this,QStringLiteral("保存列表"),".",QStringLiteral("列表文件(*lis);;所有文件(*)"));
    if(file_addr.isEmpty())
    {
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("未保存"),QMessageBox::Ok);
    }
    else
    {
        if(!file_addr.endsWith(".lis"))
        {
         file_addr.append(".lis");//fill type mark
        }
        QFile list_file(file_addr);
        if(!list_file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("文件保存失败"),QMessageBox::Ok);
        }
        else
        {
            for(int i=0;i<ui->send_list->count();i++)
            {
                    list_file.write(ui->send_list->item(i)->text().toLatin1());
                    list_file.write("\n");
            }
            list_file.close();
        }
    }
}

void list::on_load_list_clicked()
{
    QString file_addr = QFileDialog::getOpenFileName(this,QStringLiteral("加载列表"),".",QStringLiteral("列表文件(*lis);;所有文件(*)"));
    if(file_addr.isEmpty())
    {
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("未加载"),QMessageBox::Ok);
    }
    else
    {
        QFile list_file(file_addr);
        if(!list_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("文件打开失败"),QMessageBox::Ok);
        }
        else
        {
            QString buf;
            list_file.seek(0);
            while(!list_file.atEnd())
            {
                buf=list_file.readLine();
                ui->send_list->insertItem(ui->send_list->count(),buf.remove('\n'));
                buf.clear();
            }
            ui->list_len->setText(QString::number(ui->send_list->count(), 10));
            list_file.close();
        }
    }

}
