#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qlineedit.h>
#include "FileHandler.h"
#include "PlaylistHandler.h"
#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qscrollarea.h>


class EditPlaylistWindow :
    public QMainWindow
{
    Q_OBJECT
public:
    EditPlaylistWindow(QWidget* parent = nullptr);
private slots:
    void closeWindow();
    void removeSongs();
    void update();
private:
    //playlist title
    QLineEdit* name;

    //remove song stuff
    QList<QLabel*> songLabels;
    QList<QCheckBox*> songCheckBoxes;
    QList<QWidget*> songWidgets;
    QPushButton* removeSongButton;
    QWidget* mainScrollWidget;
    QVBoxLayout* removeLayout;

    //add song stuff
    QPushButton* addSongButton;

    //close button
    QPushButton* closeButton;

    //functions
    QWidget* addSongToRemoveWidgetDisplay(QString song);
};

