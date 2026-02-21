#include "ViewPlaylistWindow.h"

ViewPlaylistWindow::ViewPlaylistWindow(QWidget* parent, bool autoEdit) : QMainWindow(parent) {
    this->setAttribute(Qt::WA_DeleteOnClose);
    QWidget* main = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(main);

    //title setup
    ViewPlaylistWindow::title = new QLabel();
    ViewPlaylistWindow::title->setFrameStyle(QFrame::Box | QFrame::Raised);
    ViewPlaylistWindow::title->setText("Playlist: " + playlistHandler->getFileName());
    ViewPlaylistWindow::title->setAlignment(Qt::AlignCenter);
    ViewPlaylistWindow::title->setTextInteractionFlags(Qt::NoTextInteraction);

    //text area to show songs
    ViewPlaylistWindow::textArea = new QLabel();
    ViewPlaylistWindow::textArea->setFrameStyle(QFrame::Box | QFrame::Raised);
    ViewPlaylistWindow::textArea->setText(playlistHandler->getSongs());
    ViewPlaylistWindow::textArea->setAlignment(Qt::AlignCenter);
    ViewPlaylistWindow::textArea->setTextInteractionFlags(Qt::NoTextInteraction);

    QScrollArea* textScroll = new QScrollArea();
    textScroll->setWidget(ViewPlaylistWindow::textArea);
    textScroll->setWidgetResizable(true);

    //buttons making
    ViewPlaylistWindow::editButton = new QPushButton("&Edit");
    connect(ViewPlaylistWindow::editButton, &QPushButton::clicked, this, &ViewPlaylistWindow::openEditWindow);

    ViewPlaylistWindow::closeButton = new QPushButton("&Close");
    connect(ViewPlaylistWindow::closeButton, &QPushButton::clicked, this, &QWidget::close); //makes close button close the window
    //button widget
    QWidget* buttonWidget = new QWidget();
    //button layout
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(ViewPlaylistWindow::editButton);
    buttonLayout->addWidget(ViewPlaylistWindow::closeButton);
    buttonLayout->addStretch(1);

    connect(playlistHandler, &PlaylistHandler::playlistUpdated, this, &ViewPlaylistWindow::update);

    //add widgets to layout
    layout->addWidget(ViewPlaylistWindow::title);
    layout->addWidget(textScroll);
    layout->addWidget(buttonWidget);

    ViewPlaylistWindow::setCentralWidget(main);

    //set min size & resize
    ViewPlaylistWindow::setMinimumSize(150, 150);
    ViewPlaylistWindow::resize(150, 150);
    qoaHandler->getAudioHandler()->force_pause();


    if (autoEdit) {
        this->openEditWindow();
    }
}

void ViewPlaylistWindow::openEditWindow() {

    EditPlaylistWindow* editPlaylistWindow = new EditPlaylistWindow(this);
    ViewPlaylistWindow::editWindows.push_back(editPlaylistWindow);
    editPlaylistWindow->show();
}

void ViewPlaylistWindow::update() {
    ViewPlaylistWindow::textArea->setText(playlistHandler->getSongs());
    ViewPlaylistWindow::title->setText("Playlist: " + playlistHandler->getFileName());
}

ViewPlaylistWindow::~ViewPlaylistWindow() {
    playlistHandler->playFirstSong();
    

    qoaHandler->getAudioHandler()->retoggle_pause_state();
}