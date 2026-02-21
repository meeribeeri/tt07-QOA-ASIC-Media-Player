#include "PlaylistButtons.h"


PlaylistButtons::PlaylistButtons(QWidget* parent) : QWidget(parent) {
    //layout creation
    PlaylistButtons::layout = new QHBoxLayout(this);

    //button creation
    //next song button
    PlaylistButtons::nextSongButton = new QPushButton("&NextSong");
    connect(PlaylistButtons::nextSongButton, &QPushButton::clicked, playlistHandler, &PlaylistHandler::nextSong);

    //last song button
    PlaylistButtons::previousSongButton = new QPushButton("&Previous Song");
    connect(PlaylistButtons::previousSongButton, &QPushButton::clicked, playlistHandler, &PlaylistHandler::previousSong);



    //add buttons to layout
    PlaylistButtons::layout->addStretch(1);
    PlaylistButtons::layout->addWidget(PlaylistButtons::previousSongButton);
    PlaylistButtons::layout->addWidget(PlaylistButtons::nextSongButton);
    PlaylistButtons::layout->addStretch(1);
}