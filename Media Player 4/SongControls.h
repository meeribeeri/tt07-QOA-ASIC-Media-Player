#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qslider.h>
#include <QtWidgets/qlabel.h>
#include "QOAHandler.h"
#include "AudioHandler.h"

#define VOLUME_ICON_SIZE 30

class SongControls :
    public QWidget
{
    Q_OBJECT
public:
    SongControls(QWidget* parent = nullptr);
public slots:
    void updateVolumeDisplay();
private:
    QHBoxLayout* layout;

    //buttons
    QPushButton* pauseButton;
    QPushButton* forwardButton;
    QPushButton* backButton;

    //volume slider & display
    QSlider* volumeSlider;
    QLabel* volumeDisplay;
    QLabel* volumeIcon;
};

