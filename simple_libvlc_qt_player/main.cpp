#include <QCoreApplication>
#include <Windows.h>
#include "vlc/vlc.h"


#pragma comment(lib,"libvlc.lib")
#pragma comment(lib,"libvlccore.lib")
using namespace std;

int main(int argc, char* argv[])
{
    libvlc_instance_t* inst;
    libvlc_media_player_t* mp;
    libvlc_media_t* m;

    libvlc_time_t length;
    int width;
    int height;
    int wait_time = 5000;

    const char* version;
    version = libvlc_get_version();
    printf("version: %s\n", version);
    inst = libvlc_new(NULL, NULL);

    char filename[1024] = "../simple_libvlc_qt_player/cuc_ieschool.flv";
    //m = libvlc_media_new_location (inst,"rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov" );
    m = libvlc_media_new_path (inst,filename);
    mp = libvlc_media_player_new_from_media(m);
    libvlc_media_release(m);
    libvlc_media_player_play(mp);

    //wait until the tracks are created
    _sleep(wait_time);
    length = libvlc_media_player_get_length(mp);
    width = libvlc_video_get_width(mp);
    height = libvlc_video_get_height(mp);
    printf("Stream Duration: %ds\n", length / 1000);
    printf("Resolution: %d x %d\n", width, height);
    //Let it play
    _sleep(length - wait_time);

    // Stop playing
    libvlc_media_player_stop(mp);
    libvlc_media_player_release(mp);
    libvlc_release(inst);

    return 0;
}
