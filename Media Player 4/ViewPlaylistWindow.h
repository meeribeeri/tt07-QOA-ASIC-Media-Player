#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qmainwindow.h>
#include "EditPlaylistWindow.h"
#include "PlaylistHandler.h"
#include <QOAHandler.h>

class ViewPlaylistWindow :
    public QMainWindow
{
    Q_OBJECT
public:
    ViewPlaylistWindow(QWidget* parent = nullptr, bool autoEdit = false);
    ~ViewPlaylistWindow();
public slots:
    void update();
private:
    void openEditWindow();
private:
    //title
    QLabel* title;

    //text area to show all songs
    QLabel* textArea;

    //buttons
    QPushButton* editButton;
    QPushButton* closeButton;

    //QList for edit windows
    QList<EditPlaylistWindow*> editWindows;

};

