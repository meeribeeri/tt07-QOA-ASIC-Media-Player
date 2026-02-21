#pragma once
#include <QtCore/qfile.h>
#include <QtCore/qstring.h>
#include <QtCore/qtextstream.h>
#include <qfileinfo.h>
#include "FileHandler.h"
#include "DebugHandler.h"

class PlaylistHandler : public QObject
{
    Q_OBJECT
public:
    PlaylistHandler(QString fileName = "");
    void changePlaylistFile(QString fileName);
    void changePlaylistName(QString newName);
    void addSong(QString songPath);
    void removeSongs(QList<int> songsToRemoveByIndex);
    QString getFileName();
    QString getSongs();
    QList<QString> getSongList();
    QString getSongsFormatted();
    void playFirstSong();
public slots:
    bool nextSong();
    bool previousSong();
signals:
    void playlistUpdated();
    void nameChanged();
private:
    QFile* playlistFileName;

    int songLine;
    bool connectedAutoPlay;

    bool checkForFile();

    QString getSongName(QString songPath);
};

extern PlaylistHandler* playlistHandler;
