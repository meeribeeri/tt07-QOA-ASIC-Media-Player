#pragma once
#include "QtCore\qobject.h"
#include <iostream>
#include "FileHandler.h"
#include "QOAHandler.h"

class Settings :
    public QObject
{
    Q_OBJECT
public:
    Settings();
    bool getCOMPortSave();
public slots:
    void openInitialSettings();
    void onClose();
    void setCOMPortSave(bool save_state);
signals:
    void saveSettings(QString volume, QString COM);
private:
    bool save_COM_ports;
};

extern Settings* settings;