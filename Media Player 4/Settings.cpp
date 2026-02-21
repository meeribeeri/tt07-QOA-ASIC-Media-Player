#include "Settings.h"

Settings::Settings() {
    Settings::save_COM_ports = false;
    connect(this, &Settings::saveSettings, fileHandler, &FileHandler::saveSettings);
}

void Settings::openInitialSettings() {
    QString allSettings = fileHandler->getSettings();
    int volume_percent = allSettings.split("\n")[0].split(": ")[1].toInt();
    int COM1 = allSettings.split("\n")[1].split(" | ")[0].split(": ")[1].toInt();
    int COM2 = allSettings.split("\n")[1].split(" | ")[1].split(": ")[1].toInt();
    if (COM1 != -1 || COM2 != -1) {
        Settings::save_COM_ports = true;;
    }
    qoaHandler->getAudioHandler()->setVolume(volume_percent);
    qoaHandler->changeCOMPorts(COM1, COM2);
}

void Settings::onClose() {
    QString volume_line = "Volume Percentage: " + QString::number(qoaHandler->getAudioHandler()->getVolume());
    QString COM_line = Settings::save_COM_ports ? "COM1: " + QString::number(qoaHandler->getReadWrite()) + " | " + "COM2: " + QString::number(qoaHandler->getDedicatedRead()) : "COM1: -1 | COM2: -1";

    emit Settings::saveSettings(volume_line, COM_line);
}

bool Settings::getCOMPortSave() {
    return Settings::save_COM_ports;
}

void Settings::setCOMPortSave(bool save_state) {
    Settings::save_COM_ports = save_state;
}

Settings* settings = new Settings();