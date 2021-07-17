#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QMessageBox>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    InitData();
    InitUI();
    connect_Singnal_Slot();
    setButton(libvlc_NothingSpecial);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::slots_MediaPlayerStopped()
{
    this->on_media_stop_clicked();

    ui->label_total->setText("00:00:00");
    ui->label_time->setText("00:00:00");
    ui->video_position->setValue(0);
}

void Widget::slots_media_player_time(int curtime)
{
    QString time;
    int duration;
    int tns, thh, tmm, tss;

    duration = m_player->media_get_length();
    tns = duration / 1000;
    thh = tns / 3600;
    tmm = (tns % 3600) / 60;
    tss = (tns % 60);
    time.sprintf("%02d:%02d:%02d", thh, tmm, tss);
    ui->label_total->setText(time);
    ui->video_position->setMaximum(duration);

    tns = curtime / 1000;
    thh = tns / 3600;
    tmm = (tns % 3600) / 60;
    tss = (tns % 60);
    time.sprintf("%02d:%02d:%02d", thh, tmm, tss);
    ui->label_time->setText(time);
    ui->video_position->setValue(curtime);
    ui->video_position->setPageStep(duration / 10);
}

void Widget::InitData()
{
    m_URL = "";
    m_player = new VlcPlayer;
    m_player->setURL(m_URL);
    m_player->set_media_player_hwnd(ui->label_video);
    ui->video_sound->setValue(50);
    systemModel = new QFileSystemModel(this);
}

void Widget::InitUI()
{
    this->setWindowTitle(QString("VLC多功能播放器"));
    QStringList filter;
    filter << "*.avi" << "*.mp4" << "*.flv" << "*.mp3" << "*.wav" << "*.264" << "*.mpeg";
    systemModel->setNameFilters(filter);
    systemModel->setNameFilterDisables(false);
    systemModel->setRootPath("");

    ui->treeView->setModel(systemModel);
    ui->treeView->setRootIndex(systemModel->setRootPath(""));

    ui->treeView->setColumnHidden(1, true);
    ui->treeView->setColumnHidden(2, true);
    ui->treeView->setColumnHidden(3, true);
}

void Widget::setButton(libvlc_state_t state)
{
    if (state == libvlc_NothingSpecial) {
        ui->media_play->setEnabled(true);
        //ui.media_pause->setEnabled(false);
        ui->media_stop->setEnabled(false);
    }
    else {
        ui->media_play->setEnabled(false);
        //ui->media_pause->setEnabled(true);
        ui->media_stop->setEnabled(true);
    }
}

void Widget::connect_Singnal_Slot()
{
    connect(this->m_player, SIGNAL(signal_MediaPlayerStopped()), this, SLOT(slots_MediaPlayerStopped()));
//    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(setRootIndex(QModelIndex)));
    connect(m_player, SIGNAL(signal_media_player_time(int)), this, SLOT(slots_media_player_time(int)));
}

void Widget::on_treeView_doubleClicked(const QModelIndex &index)
{
    QFileInfo fileinfo = systemModel->fileInfo(index);

    QString fileUrl = fileinfo.absoluteFilePath();
    //vlc播放格式要求
    if (fileUrl.contains("/"))
    {
        fileUrl.replace(QString("/"), QString("\\\\"));
    }
    m_URL = fileUrl;
    m_player->setURL(fileUrl);
    if (m_player->get_media_player_state() == libvlc_Playing)
    {
        m_player->media_stop();
    }
    m_player->media_play();
    setButton(libvlc_Playing);
}

void Widget::on_media_play_clicked()
{
    if (m_URL.isEmpty()) {
        QMessageBox::warning(this, QString("警告"),
                                       QString("URL为空"),
                                       QMessageBox::Ok,
                                       QMessageBox::Ok);
        return;
    }
    m_player->media_play();
    setButton(libvlc_Playing);
}

void Widget::on_media_pause_clicked()
{
    if (m_player->get_media_player_state() == libvlc_Playing) {
        m_player->media_pause();

        ui->media_pause->setIcon(QIcon(":/image/7.png"));
        ui->media_pause->setToolTip("播放");
    }
    else if (m_player->get_media_player_state() == libvlc_Paused) {
        m_player->media_pause();
        ui->media_pause->setIcon(QIcon(":/image/5.png"));
        ui->media_pause->setToolTip("暂停");
    }
}

void Widget::on_media_stop_clicked()
{
    m_player->media_stop();
    setButton(libvlc_NothingSpecial);
}

void Widget::on_video_position_sliderMoved(int position)
{
    m_player->setPosition(position);
}

void Widget::on_video_position_actionTriggered(int action)
{
    Q_UNUSED(action)
    m_player->setPosition(ui->video_position->sliderPosition());
}

void Widget::on_video_sound_actionTriggered(int action)
{
    Q_UNUSED(action)
//    qDebug()<<" videoSound action "<<action<<" value "<<ui->video_sound->sliderPosition()<<endl;
    m_player->media_set_voice(ui->video_sound->sliderPosition());
}

void Widget::on_video_sound_sliderMoved(int position)
{
    m_player->media_set_voice(position);
//    qDebug()<<"position"<<position;
}
