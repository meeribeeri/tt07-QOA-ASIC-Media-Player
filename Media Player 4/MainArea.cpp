#include "MainArea.h"


MainArea::MainArea(QWidget* parent) : QWidget(parent) {
    //create layout
    MainArea::layout = new QVBoxLayout(this);
    //add widget parts to widget
    MainArea::songInformation = new SongInformation();
    MainArea::songPlaybar = new SongPlaybar();
    MainArea::songControls = new SongControls();
    MainArea::playlistControls = new PlaylistControls();

    MainArea::layout->addWidget(MainArea::songInformation);
    MainArea::layout->addWidget(MainArea::songPlaybar);
    MainArea::layout->addWidget(MainArea::songControls);
    MainArea::layout->addWidget(MainArea::playlistControls);
}