#pragma once
#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qmenubar.h>
#include <iostream>
#include "MainArea.h"
#include "COMSelector.h"
#include "ViewPlaylistWindow.h"
#include "FileHandler.h"
#include "DebugWindow.h"

class Window :
    public QMainWindow
{
    Q_OBJECT
public:
    Window(QWidget* parent = nullptr);

//slots
public slots:
    void setActive();
private slots:
    void openDeviceStatus();
    void openPlaylistWindow();
    void openDebugWindow();
private:
    //layout
    QVBoxLayout *layout;

    //menubar menus
    QMenu* fileMenu;
    QMenu* playlistMenu;
    QMenu* toolsMenu;

    //bool for if the debug window is open
    bool debugWindowIsOpen;

    //central widget
    MainArea* mainArea;

    //windows list, as QLists so they all close when this window is closed, and so I can reopen them if they are manually closed, since finding a different way would require me to deal with a circle import, and just no
    //device status windows
    QList<COMSelector*> COMSelectors;
    //playlist windows
    QList<ViewPlaylistWindow*> playlistWindows;
    //debug windows
    QList<DebugWindow*> debugWindows;
};

