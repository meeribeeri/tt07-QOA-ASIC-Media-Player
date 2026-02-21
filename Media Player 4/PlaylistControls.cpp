#include "PlaylistControls.h"


PlaylistControls::PlaylistControls(QWidget* parent) : QWidget(parent) {
    PlaylistControls::layout = new QVBoxLayout(this);

    //label setup
    PlaylistControls::title = new QLabel();
    PlaylistControls::title->setFrameStyle(QFrame::Panel | QFrame::Raised);
    PlaylistControls::title->setText("No Playlist Selected");
    PlaylistControls::title->setAlignment(Qt::AlignCenter);
    PlaylistControls::title->setTextInteractionFlags(Qt::NoTextInteraction);

    //button setup
    PlaylistControls::buttons = new PlaylistButtons();

    //add widgets to label
    PlaylistControls::layout->addWidget(PlaylistControls::title);
    PlaylistControls::layout->addWidget(PlaylistControls::buttons);

    //connect the playlist name being updated to the text being updated
    connect(playlistHandler,&PlaylistHandler::nameChanged,this,&PlaylistControls::updateTitle);
}

void PlaylistControls::updateTitle() {
    if (playlistHandler->getFileName() == "") {
        PlaylistControls::title->setText("No Playlist Selected");
        return;
    }
    PlaylistControls::title->setText("Playlist: " + playlistHandler->getFileName());
}