#include "Window.h"

Window::Window(QWidget* parent)
    : QMainWindow(parent)
{
    //setup actions for menubar menus
    //menu actions
    //fileMenu
    QAction* openFile = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Open File"), this);
    openFile->setShortcuts(QKeySequence::New);
    openFile->setStatusTip(tr("Open a QOA file to run"));
    connect(openFile, &QAction::triggered, fileHandler, &FileHandler::openSongFile);

    //playlistMenu
    QAction* openPlaylist = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Open Playlist"), this);
    openPlaylist->setShortcuts(QKeySequence::New);
    openPlaylist->setStatusTip(tr("Open a playlist file to use"));
    connect(openPlaylist, &QAction::triggered, fileHandler, &FileHandler::openPlaylist);

    QAction* newPlaylist = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew), tr("&New Playlist"), this);
    newPlaylist->setShortcuts(QKeySequence::New);
    newPlaylist->setStatusTip(tr("Create a new playlist"));
    connect(newPlaylist, &QAction::triggered, fileHandler, &FileHandler::newPlaylist);
    connect(fileHandler,&FileHandler::newPlaylistMade,this,&Window::openPlaylistWindow);

    QAction* deviceStatusOpen = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::Computer), tr("&COM Ports"), this);
    deviceStatusOpen->setShortcuts(QKeySequence::New);
    deviceStatusOpen->setStatusTip(tr("Change COM Ports"));
    connect(deviceStatusOpen, &QAction::triggered, this, &Window::openDeviceStatus);
    connect(qoaHandler, &QOAHandler::needComSelector, this, &Window::openDeviceStatus);

    QAction* debugWindowOpen = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::Computer), tr("&Open Debug Window"), this);
    debugWindowOpen->setShortcuts(QKeySequence::New);
    debugWindowOpen->setStatusTip(tr("Opens the debug window for well... debugging"));
    connect(debugWindowOpen, &QAction::triggered, this, &Window::openDebugWindow);

    //create menus in the menubar & add actions to them
    Window::fileMenu = Window::menuBar()->addMenu(tr("&File"));
    Window::fileMenu->addAction(openFile);
    
    Window::playlistMenu = Window::menuBar()->addMenu(tr("&Playlist"));
    Window::playlistMenu->addAction(openPlaylist);
    Window::playlistMenu->addAction(newPlaylist);

    Window::toolsMenu = Window::menuBar()->addMenu(tr("&Debug"));
    Window::toolsMenu->addAction(deviceStatusOpen);
    Window::toolsMenu->addAction(debugWindowOpen);

    //create mainArea widget
    Window::mainArea = new MainArea(this);

    //set central widget to mainArea
    setCentralWidget(Window::mainArea);

    //resize self and set minimum size
    Window::setMinimumSize(300,350);
    Window::resize(300,500);

    //other connects
    connect(qoaHandler, &QOAHandler::activeWindowMain, this, &Window::setActive);

    if (qoaHandler->getReadWrite() == -1 || qoaHandler->getDedicatedRead() == -1) {
        openDeviceStatus();
    }
}

void Window::openDeviceStatus() {
    //create new device status window if one does not already exist
    /*if (Window::deviceStatus != nullptr) {
        return;
    }
    Window::deviceStatus = new COMSelector();
    Window::deviceStatus->show();*/
    COMSelector* deviceStatus = new COMSelector(this);
    Window::COMSelectors.push_back(deviceStatus);
    deviceStatus->show();
}


void Window::openPlaylistWindow() {
    ViewPlaylistWindow* playlistWindow = new ViewPlaylistWindow(this);
    Window::playlistWindows.push_back(playlistWindow);
    playlistWindow->show();
}

void Window::openDebugWindow() {
    DebugWindow* debugWindow = new DebugWindow(this);
    Window::debugWindows.push_back(debugWindow);
    debugWindow->show();
}

void Window::setActive() {
    Window::activateWindow();
}