#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "vlcplayer.h"
#include <QPaintEvent>
#include <QTimer>
#include <QFileSystemModel>
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void slots_MediaPlayerStopped();            /*  播放停止响应槽函数 */
    void slots_media_player_time(int curtime);  /*  播放时间更新槽函数 */

private slots:

    void on_treeView_doubleClicked(const QModelIndex &index);   /*  tree双击事件  */

    void on_media_play_clicked();                               /*  开始播放按钮  */

    void on_media_pause_clicked();                              /*  暂停按钮  */

    void on_media_stop_clicked();                               /*  停止播放按钮  */

    void on_video_position_sliderMoved(int position);           /*  播放进度条移动事件  */

    void on_video_position_actionTriggered(int action);         /*  播放进度条点击事件  */

    void on_video_sound_actionTriggered(int action);            /*  声音进度条移动事件  */

    void on_video_sound_sliderMoved(int position);              /*  声音进度条点击事件  */

private:
    Ui::Widget *ui;
private:
    void InitData();                             /*  初始化数据    */
    void InitUI();                               /*  初始化UI界面  */
    void setButton(libvlc_state_t state);        /*  根据播放状态 设置按钮状态，是否可选 */
    void connect_Singnal_Slot();                 /*  关联信号与槽  */
private:
    VlcPlayer* m_player;                /*  播放器对象  */
    QString m_URL;                      /*  播放的URL  */
    QFileSystemModel* systemModel;      /*  文件系统模式  */
};
#endif // WIDGET_H
