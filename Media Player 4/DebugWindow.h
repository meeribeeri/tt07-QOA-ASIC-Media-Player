#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qtextedit.h>
#include <QtWidgets/qcombobox.h>
#include "DebugHandler.h"

class DebugWindow :
    public QMainWindow
{
    Q_OBJECT
public:
    DebugWindow(QWidget* parent = nullptr);
public slots:
    void changeDisplay(int index);
    void actuallyChangeDisplay(QString message);
signals:
    void requestLog(int logType);
private:
    uint8_t displayChoice;

    QLabel* title;
    QTextEdit* debugTextArea;
    QScrollArea* textScroll;
};

