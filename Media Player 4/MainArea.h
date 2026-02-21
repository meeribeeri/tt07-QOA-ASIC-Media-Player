#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include "SongInformation.h"
#include "SongPlaybar.h"
#include "SongControls.h"
#include "PlaylistControls.h"

class MainArea :
    public QWidget
{
    Q_OBJECT
public:
    MainArea(QWidget* parent = nullptr);
    
private:
    QVBoxLayout* layout;
    //ui widgets
    SongInformation* songInformation;
    SongPlaybar* songPlaybar;
    SongControls* songControls;
    PlaylistControls* playlistControls;
};

