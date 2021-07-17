#include "vlcplayer.h"
#include <QDebug>
#include <QThread>
#include <windows.h>
VlcPlayer::VlcPlayer(QObject *parent) : QObject(parent)
{
    libvlc_eventManager = NULL;
    libvlc_inst = NULL;
    libvlc_mp = NULL;
    libvlc_m = NULL;
    libvlc_state = libvlc_NothingSpecial;						/*播放状态*/
    libvlc_mainWindow = NULL;
    libvlc_WId = 0;
}

bool VlcPlayer::setURL(QString &URL)
{
    if(URL.isEmpty())
    {
        qDebug()<<"URL is Empty"<<endl;
        return false;
    }
    libvlc_url = URL;
    return true;
}

void VlcPlayer::setPosition(float position)
{
    float length = libvlc_media_player_get_length(libvlc_mp);
    libvlc_media_player_set_position(libvlc_mp,position/length);
}

void VlcPlayer::media_play()
{
    if (libvlc_state != libvlc_NothingSpecial) {
        qDebug()<<"Media is playing now."<<endl;
        return;
    }

    const char* version;
    version = libvlc_get_version();
    qDebug()<<"version:"<<version<<endl;

    /* Load the VLC engine */
    const char* const vlc_args[] = {
        "--demux=h264",
        "--ipv4",
        "--no-prefer-system-codecs",
        "--rtsp-caching=300",
        "--network-caching=500",	//网络额外缓存值 (ms)
        "--rtsp-frame-buffer-size=10000000",
        "--rtsp-tcp",				//RTSP采用TCP传输方式
    };
    /* Create a new item */
    if (libvlc_url.contains("rtsp")) {
        //URL播放rtsp网络流
        /* Load the VLC engine */
        libvlc_inst = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);

        if (libvlc_inst == NULL)
        {
            qDebug()<<"libvlc_new error"<<endl;
            return;
        }
        libvlc_m = libvlc_media_new_location(libvlc_inst, libvlc_url.toStdString().c_str());
        if (libvlc_m == NULL)
        {
            qDebug()<<"error :libvlc_media_new_location error"<<endl;
            return;
        }
    }
    else
    {
        libvlc_inst = libvlc_new(0, NULL);
        if (libvlc_inst == NULL)
        {
            qDebug()<<"libvlc_new error"<<endl;
            return;
        }
        //本地播放视频流
        libvlc_m = libvlc_media_new_path(libvlc_inst, libvlc_url.toStdString().c_str());
        static int index = 0;
        qDebug()<<"URL["<<index++<<"]:"<<libvlc_url.toStdString().c_str();
        if (libvlc_m == NULL)
        {
            qDebug()<<"libvlc_media_new_path error";
            media_stop();
            return;
        }
    }

    /* Create a media player playing environement */
    libvlc_mp = libvlc_media_player_new_from_media(libvlc_m);

    /*添加事件*/
    attachEvents();

    /* No need to keep the media now */
    libvlc_media_release(libvlc_m);


    //on windows
    if (libvlc_mainWindow != NULL){
        Sleep(45);
        libvlc_WId = libvlc_mainWindow->winId();
        libvlc_media_player_set_hwnd(libvlc_mp, (void*)libvlc_WId);
    }
    else
    {
        Sleep(45);
    }

    /* play the media_player */
    libvlc_media_player_play(libvlc_mp);
    libvlc_state = libvlc_Playing;
}

void VlcPlayer::media_pause()
{
    libvlc_state = libvlc_media_player_get_state(libvlc_mp);
    if (libvlc_state == libvlc_Playing) {
        if (libvlc_media_player_can_pause(libvlc_mp))
        {
            libvlc_media_player_set_pause(libvlc_mp, 1);
            libvlc_state = libvlc_Paused;
        }
    }
    else if (libvlc_state == libvlc_Paused) {
        libvlc_media_player_set_pause(libvlc_mp, 0);
        libvlc_state = libvlc_Playing;
    }
}

void VlcPlayer::media_stop()
{
    if (libvlc_mp != NULL) {
        libvlc_media_player_stop(libvlc_mp);
        libvlc_media_player_release(libvlc_mp);
        libvlc_release(libvlc_inst);
        libvlc_mp = NULL;
        libvlc_m = NULL;
        libvlc_eventManager = NULL;
        libvlc_inst = NULL;		/*vlc播放器实例*/
    }
    libvlc_state = libvlc_NothingSpecial;

}

int  VlcPlayer::media_get_length()
{
    return m_timeLength;
}

void VlcPlayer::media_set_voice(int value)
{
    if(value < 0)
        return;
    if(libvlc_mp!=NULL){
        libvlc_audio_set_volume(libvlc_mp,value);
    }
}

void VlcPlayer::MediaPlayerEncounteredError()
{
    qDebug()<<"================播放错误=================="<<endl;
}

void VlcPlayer::set_media_player_hwnd(QWidget* widget)
{
    libvlc_mainWindow = widget;
}

libvlc_state_t VlcPlayer::get_media_player_state()
{
    return libvlc_state;
}

void VlcPlayer::attachEvents()
{
    // 事件列表
    QList<libvlc_event_e> events;
    events << libvlc_MediaPlayerOpening
           << libvlc_MediaPlayerBuffering
           << libvlc_MediaPlayerPlaying
           << libvlc_MediaPlayerPaused
           << libvlc_MediaPlayerStopped
           << libvlc_MediaPlayerEncounteredError
           << libvlc_MediaPlayerMuted
           << libvlc_MediaPlayerUnmuted
           << libvlc_MediaPlayerAudioVolume
           << libvlc_MediaPlayerLengthChanged
           << libvlc_MediaPlayerTimeChanged
           << libvlc_MediaPlayerPositionChanged
           << libvlc_MediaPlayerEndReached;
    // 订阅事件
    libvlc_eventManager = libvlc_media_player_event_manager(libvlc_mp);
    foreach (const libvlc_event_e &event, events) {
        libvlc_event_attach(libvlc_eventManager, event, handleEvents, this);
    }
}

void VlcPlayer::handleEvents(const libvlc_event_t *event, void *userData)
{
    VlcPlayer* player = static_cast<VlcPlayer*>(userData);
    switch (event->type) {
    // 播放状态改变
    case libvlc_MediaPlayerOpening:{
        qDebug()<<"libvlc_MediaPlayerOpening"<<endl;
        break;
    }
    case libvlc_MediaPlayerBuffering:{
        break;
    }
    case libvlc_MediaPlayerPlaying: {
        emit player->signal_MediaPlayerPlaying();
        break;
    }
    case libvlc_MediaPlayerPaused: {
        break;
    }
    case libvlc_MediaPlayerStopped: {
        //当 libvlc_media_player_stop(libvlc_mp); 被调用才会触发该信号
        qDebug()<< "播放结束" <<endl;
        break;
    }
    case libvlc_MediaPlayerEncounteredError: {
        player->MediaPlayerEncounteredError();
        break;
    }
    // 时长改变时获取一次总时长
    case libvlc_MediaPlayerLengthChanged: {
        player->m_timeLength = libvlc_media_player_get_length(player->libvlc_mp);
        break;
    }
    // 播放时间改变
    case libvlc_MediaPlayerTimeChanged: {
        //播放时间是一直在变的
        int curtime =libvlc_media_player_get_time(player->libvlc_mp);
        player->signal_media_player_time(curtime);
        break;
    }
    // 播放位置改变
    case libvlc_MediaPlayerPositionChanged: {
        //播放位置是一直在变的,个人认为等同 libvlc_MediaPlayerTimeChanged
        break;
    }
    case libvlc_MediaPlayerEndReached:
        //当视频播放结束时触发该信号
        emit player->signal_MediaPlayerStopped();
        break;
    default:
        break;
    }
}
