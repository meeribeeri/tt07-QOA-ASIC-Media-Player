#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qpushbutton.h>
#include "PlaylistHandler.h"

class PlaylistButtons :
    public QWidget
{
    Q_OBJECT
public:
    PlaylistButtons(QWidget* parent = nullptr);
private:
    //layout
    QHBoxLayout* layout;

    //buttons for next & previous song
    QPushButton* nextSongButton;
    QPushButton* previousSongButton;
};

