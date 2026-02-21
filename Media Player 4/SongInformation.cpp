#include "SongInformation.h"


SongInformation::SongInformation(QWidget* parent) : QWidget(parent) {
    //create layout
    SongInformation::layout = new QVBoxLayout(this);
    //add widget parts to widget

    //set title widget stuff
    SongInformation::title = new QLabel();
    SongInformation::title->setFrameStyle(QFrame::Box | QFrame::Raised);
    SongInformation::title->setText("No Song Playing");
    SongInformation::title->setAlignment(Qt::AlignCenter);
    SongInformation::title->setTextInteractionFlags(Qt::NoTextInteraction);

    

    //set main body scroll
    SongInformation::scrollArea = new QScrollArea();
    //create and set text area
    SongInformation::textArea = new QTextEdit();
    SongInformation::textArea->setReadOnly(true);
    SongInformation::textArea->setTextInteractionFlags(Qt::NoTextInteraction);
    SongInformation::textArea->setWordWrapMode(QTextOption::NoWrap);
    SongInformation::textArea->setText("No Song Available");

    //layout for text area
    QWidget* textAreaWidgetWhole = new QWidget();
    QHBoxLayout* textAreaLayout = new QHBoxLayout(textAreaWidgetWhole);
    textAreaLayout->addStretch(1);


    //add text area to scroll
    SongInformation::scrollArea->setWidget(SongInformation::textArea);
    SongInformation::scrollArea->setWidgetResizable(true);
    textAreaLayout->addWidget(SongInformation::scrollArea);
    textAreaLayout->addStretch(1);

    //add widgets to layout
    SongInformation::layout->addWidget(SongInformation::title);
    SongInformation::layout->addWidget(textAreaWidgetWhole);

    //connect update function to playlistUpdated signal of PlaylistHandler
    connect(playlistHandler, &PlaylistHandler::playlistUpdated, this, &SongInformation::update);

    //connect manually opening a file to updateTextmanual
    connect(fileHandler, &FileHandler::openedSongManually, this, &SongInformation::updateTextManual);
}


void SongInformation::update() {
    QString song = fileHandler->getCurrentSongFileName();
    if (song.isEmpty()) {
        SongInformation::textArea->setText("No Song Available");
    }
    else {
        SongInformation::title->setText("Playing: " + song);
    }
    SongInformation::textArea->setText(playlistHandler->getSongsFormatted());
}

void SongInformation::updateTextManual() {
    QString song = fileHandler->getCurrentSongFileName();
    if (song.isEmpty()) {
        SongInformation::textArea->setText("No Song Available");
    }
    else {
        SongInformation::title->setText("Playing: " + song);
    }
    SongInformation::textArea->setText(">>" + fileHandler->getCurrentSongFileName());
}