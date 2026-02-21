#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qframe.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qlayoutitem.h>
#include <QtWidgets/qtextedit.h>
#include <qtextoption.h>
#include "PlaylistHandler.h"
#include "FileHandler.h"

class SongInformation :
    public QWidget
{
    Q_OBJECT
public:
    SongInformation(QWidget* parent = nullptr);
private slots:
    void update();
    void updateTextManual();
private:
    QVBoxLayout* layout;

    //title aka song
    QLabel* title;

    //text area
    QTextEdit* textArea;

    //scroll area
    QScrollArea* scrollArea;
};

