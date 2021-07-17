### 简介

- VLC 是一款自由、开源的跨平台多媒体播放器及框架，可播放大多数多媒体文件，以及 DVD、音频 CD、VCD 及各类流媒体协议。
- 由于VLC是完全开源的，所以我们可以获得所有的项目源码，并在此基础上进行二次开发

> VLC的官网：http://www.videolan.org/
> 
> VLC源码下载地址：http://download.videolan.org/pub/videolan/vlc/2.2.4/
> 
> 测试代码参考雷神的博客：http://blog.csdn.net/leixiaohua1020/article/details/42363079

### qt配置方法

**下载源码**

选择`win64/win32`,我选的是`win64`，下载.7z结尾的，我下载的是`vlc-2.2.4-win64.7z`

解压后找到sdk文件夹，这个文件夹最重要

### simple_libvlc_qt_player

> 第一次提交代码，最简单的libvlc播放器

创建控制台项目

**配置`.pro`文件**

在底部添加如下代码

~~~c
win32{
    INCLUDEPATH += $$PWD/sdk/include
    LIBS += -L$$PWD/sdk/lib -llibvlc -llibvlccore
}else{
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib -lvlc -lvlccore
}
~~~

**配置sdk**

将解压后的sdk文件夹复制到main.cpp文件夹下即可

测试代码见`simple_libvlc_qt_player`

运行结果播放`cuc_ieschool.flv`这个视频

### vlc_qt_player

完成vlc的基本封装，实例程序运行如下

![vlc播放器](https://img-blog.csdnimg.cn/20210208162705341.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQ0NTE5NDg0,size_16,color_FFFFFF,t_70)



新增功能，进度可调，声音可调