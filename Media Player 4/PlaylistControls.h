#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>
#include "PlaylistButtons.h"
#include "PlaylistHandler.h"

class PlaylistControls :
    public QWidget
{
    Q_OBJECT
public:
    PlaylistControls(QWidget* parent = nullptr);
private slots:
    void updateTitle();

private:
    //layout
    QVBoxLayout *layout;

    //title
    QLabel *title;

    //buttons widget
    PlaylistButtons* buttons;
};

