#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qslider.h>
#include "QOAHandler.h"
#include <iostream>

class SongPlaybar :
    public QWidget
{
    Q_OBJECT
public:
    SongPlaybar(QWidget* parent = nullptr);
public slots:
    void updateSongLength(int songLengthNew, int samples);
    void updateTimePassed(int timePassedNew, int sampleNum);
private:
    QLabel* timePassed;
    QLabel* songLength;

    QHBoxLayout* layout;
    QSlider* audioSlider;

};

