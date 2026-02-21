#include "SongControls.h"


SongControls::SongControls(QWidget* parent) : QWidget(parent) {
    SongControls::layout = new QHBoxLayout(this);

    //button implementation
    SongControls::pauseButton = new QPushButton("");
    //pause button logic:
    SongControls::pauseButton->setCheckable(true);
    SongControls::pauseButton->setMaximumSize(40, 40);
    SongControls::pauseButton->resize(40, 40);

    QIcon pauseIcons;
    //pauseIcons.addFile("Pause Button.png",SongControls::pauseButton->size(), QIcon::Normal,QIcon::On);
    pauseIcons.addPixmap(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackPause).pixmap(SongControls::pauseButton->size(), QIcon::Normal, QIcon::Off), QIcon::Normal, QIcon::On);
    pauseIcons.addPixmap(QIcon::fromTheme(QIcon::ThemeIcon::MediaPlaybackStart).pixmap(SongControls::pauseButton->size(), QIcon::Normal, QIcon::Off), QIcon::Normal, QIcon::Off);
    SongControls::pauseButton->setIcon(pauseIcons);
    SongControls::pauseButton->setIconSize(QSize(40,40));
    
    connect(SongControls::pauseButton, &QPushButton::pressed, qoaHandler->getAudioHandler(), &AudioHandler::pause);

    //forward audio button
    SongControls::forwardButton = new QPushButton("");
    SongControls::forwardButton->setMaximumSize(40, 40);
    SongControls::forwardButton->resize(40, 40);
    SongControls::forwardButton->setIcon(QIcon("forward 5s.png"));
    SongControls::forwardButton->setIconSize(QSize(40, 40));
    connect(SongControls::forwardButton, &QPushButton::pressed, qoaHandler, &QOAHandler::forwardAudio);

    //rewind audio button
    SongControls::backButton = new QPushButton("");
    SongControls::backButton->setMaximumSize(40, 40);
    SongControls::backButton->resize(40, 40);
    SongControls::backButton->setIcon(QIcon("reverse 5s.png"));
    SongControls::backButton->setIconSize(QSize(40, 40));
    connect(SongControls::backButton, &QPushButton::pressed, qoaHandler, &QOAHandler::rewindAudio);

    //volume slider
    SongControls::volumeSlider = new QSlider();
    SongControls::volumeSlider->setMinimum(0);
    SongControls::volumeSlider->setMaximum(100);
    SongControls::volumeSlider->setOrientation(Qt::Vertical);
    SongControls::volumeSlider->setSliderPosition(qoaHandler->getAudioHandler()->getVolume());
    connect(SongControls::volumeSlider, &QSlider::valueChanged, qoaHandler->getAudioHandler(), &AudioHandler::setVolume);

    //volume display + icon
    SongControls::volumeIcon = new QLabel();
    SongControls::volumeIcon->setPixmap(QIcon::fromTheme(QIcon::ThemeIcon::AudioVolumeHigh).pixmap(QSize(VOLUME_ICON_SIZE, VOLUME_ICON_SIZE), QIcon::Normal, QIcon::Off));
    SongControls::volumeIcon->setTextInteractionFlags(Qt::NoTextInteraction);

    SongControls::volumeDisplay = new QLabel();
    SongControls::volumeDisplay->setFrameStyle(QFrame::Box | QFrame::Sunken);
    SongControls::volumeDisplay->setText(QString::number(qoaHandler->getAudioHandler()->getVolume()) + "%");
    SongControls::volumeDisplay->setAlignment(Qt::AlignCenter);
    SongControls::volumeDisplay->setTextInteractionFlags(Qt::NoTextInteraction);
    SongControls::volumeDisplay->setFixedSize(35,25);

    //add buttons to layout
    SongControls::layout->addStretch(1);
    SongControls::layout->addWidget(SongControls::backButton);
    SongControls::layout->addWidget(SongControls::pauseButton);
    SongControls::layout->addWidget(SongControls::forwardButton);
    SongControls::layout->addWidget(SongControls::volumeSlider);
    SongControls::layout->addWidget(SongControls::volumeIcon);
    SongControls::layout->addWidget(SongControls::volumeDisplay);
    SongControls::layout->addStretch(1);
    
    connect(qoaHandler->getAudioHandler(), &AudioHandler::volumeUpdated, this, &SongControls::updateVolumeDisplay);

    SongControls::updateVolumeDisplay();
}

void SongControls::updateVolumeDisplay() {
    int volume = qoaHandler->getAudioHandler()->getVolume();
    QPixmap volumeIcon;
    switch (volume) {
    case 100:
        volumeIcon = QIcon::fromTheme(QIcon::ThemeIcon::AudioVolumeHigh).pixmap(QSize(VOLUME_ICON_SIZE, VOLUME_ICON_SIZE), QIcon::Normal, QIcon::Off);
        break;
    case 0:
        volumeIcon = QIcon::fromTheme(QIcon::ThemeIcon::AudioVolumeMuted).pixmap(QSize(VOLUME_ICON_SIZE, VOLUME_ICON_SIZE), QIcon::Normal, QIcon::Off);
        break;
    default:
        if (volume > 50) {
            volumeIcon = QIcon::fromTheme(QIcon::ThemeIcon::AudioVolumeMedium).pixmap(QSize(VOLUME_ICON_SIZE, VOLUME_ICON_SIZE), QIcon::Normal, QIcon::Off);
        }
        else {
            volumeIcon = QIcon::fromTheme(QIcon::ThemeIcon::AudioVolumeLow).pixmap(QSize(VOLUME_ICON_SIZE, VOLUME_ICON_SIZE), QIcon::Normal, QIcon::Off);
        }
        break;
    }
    SongControls::volumeIcon->setPixmap(volumeIcon);
    SongControls::volumeDisplay->setText(QString::number(volume) + "%");
    SongControls::volumeDisplay->setAlignment(Qt::AlignCenter);
    SongControls::volumeDisplay->setTextInteractionFlags(Qt::NoTextInteraction);
}