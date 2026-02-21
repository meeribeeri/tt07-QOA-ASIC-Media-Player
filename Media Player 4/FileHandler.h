#pragma once
#include <qobject.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qfiledevice.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qcoreapplication.h>
#include "PlaylistHandler.h"
#include "QOAHandler.h"
#include <chrono>
#include "DebugHandler.h"

class FileHandler : public QObject
{
    Q_OBJECT
public:
    QString getPlaylistFileName();
    QString getCurrentSongFileName();
    void changeCurrentSong(QString song);
    QString getSettings();
    FileHandler();
public slots:
    void newPlaylist();
    void openPlaylist();
    void addSongToPlaylist();
    void openSongFile();
    void saveSettings(QString volume, QString COM);
    void playCurrentSongFile();
    void saveLogFile(QString fileName, QString append);
    void applicationClosing();
signals:
    void newPlaylistMade();
    void openedSongManually();
private:
    QString playlistFileName;
    QString currentSong;
    QString settingsFull;

    bool closing;
    
};

extern FileHandler* fileHandler;