#include "SongPlaybar.h"

SongPlaybar::SongPlaybar(QWidget* parent) : QWidget(parent) { 
    SongPlaybar::layout = new QHBoxLayout(this); //layout

    //time passed label (left side of playbar)
    SongPlaybar::timePassed = new QLabel();
    SongPlaybar::timePassed->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    SongPlaybar::timePassed->setTextInteractionFlags(Qt::NoTextInteraction);
    SongPlaybar::timePassed->setText("--:--");
    SongPlaybar::timePassed->setMaximumHeight(25);

    //song length label (right side of playbar)
    SongPlaybar::songLength = new QLabel();
    SongPlaybar::songLength->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    SongPlaybar::songLength->setTextInteractionFlags(Qt::NoTextInteraction);
    SongPlaybar::songLength->setText("--:--");
    SongPlaybar::songLength->setMaximumHeight(25);

    //actual playbar
    SongPlaybar::audioSlider = new QSlider();
    SongPlaybar::audioSlider->setMinimum(0);
    SongPlaybar::audioSlider->setMaximum(1000000);
    SongPlaybar::audioSlider->setOrientation(Qt::Horizontal);
    connect(SongPlaybar::audioSlider, &QSlider::valueChanged, qoaHandler, &QOAHandler::jumpAudioToPoint);
    connect(SongPlaybar::audioSlider, &QSlider::sliderPressed, qoaHandler->getAudioHandler(), &AudioHandler::force_pause);
    connect(SongPlaybar::audioSlider, &QSlider::sliderReleased, qoaHandler->getAudioHandler(), &AudioHandler::retoggle_pause_state);

    //add all to layout
    SongPlaybar::layout->addWidget(SongPlaybar::timePassed);
    SongPlaybar::layout->addWidget(SongPlaybar::audioSlider);
    SongPlaybar::layout->addWidget(SongPlaybar::songLength);

    connect(qoaHandler, &QOAHandler::song_playbar_set_transfer, this, &SongPlaybar::updateSongLength);
    connect(qoaHandler, &QOAHandler::timePassed, this, &SongPlaybar::updateTimePassed);
}

void SongPlaybar::updateSongLength(int songLengthNew, int samples) {

    //get mins & secs, then get as str
    int minutes = songLengthNew / 60;
    int seconds = songLengthNew % 60;
    QString minString = QString::number(minutes);
    QString secString = QString::number(seconds);

    //set time strings appropriately
    SongPlaybar::timePassed->setText("0:00");
    SongPlaybar::songLength->setText(minString + ":" + QString("%1%2").arg(seconds < 10 ? "0" : "").arg(secString));

    //change audio max length & reset audio pos
    SongPlaybar::audioSlider->setMaximum(samples);
    SongPlaybar::audioSlider->setValue(0);

}

void SongPlaybar::updateTimePassed(int timePassedNew, int sampleNum) {
    
    //get mins & secs, then get as str
    int minutes = timePassedNew / 60;
    int seconds = timePassedNew % 60;
    QString minString = QString::number(minutes);
    QString secString = QString::number(seconds);

    //set time string appropriately
    SongPlaybar::timePassed->setText(minString + ":" + QString("%1%2").arg(seconds < 10 ? "0" : "").arg(secString));

    SongPlaybar::audioSlider->setValue(sampleNum);
}