#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qcheckbox.h>
#include <QRegularExpressionValidator>
#include "QOAHandler.h"
#include "Settings.h"
#include <qthread.h>

//this is now a com port selector
class COMSelector :
    public QMainWindow
{
    Q_OBJECT
public:
    COMSelector(QWidget* parent = nullptr);
    ~COMSelector();
public slots:
    void COMPortChanged();
private:
    //text area
    QLabel* textArea;

    //title
    QLabel* title;

    //scroll for text
    QScrollArea* scroll;

    //buttons (widget & layout not stored as attribute
    QPushButton* closeButton;

    //com selector line edits
    QLineEdit* readWriteCOM;
    QLineEdit* dedicatedReadCOM;

    //com save checkbox
    QCheckBox* saveCheckBox;
};

