#include "DebugHandler.h"

DebugHandler::DebugHandler() {
    DebugHandler::programStartTime = getTime;
    DebugHandler::errorMessageSavePointer = 0;
    DebugHandler::logFileName = "QOA Media Player Log - " + QTime::currentTime().toString().replace(":", "-") + ".txt";


    connect(this, &DebugHandler::sampleReceive, this, &DebugHandler::sampleWrite);
}

void DebugHandler::addSampleMessage(int16_t sample, int sampleNum, int source) {
    emit DebugHandler::sampleReceive(sample, sampleNum, source);
}

void DebugHandler::sampleWrite(int16_t sample, int sampleNum, int source) {
    DebugHandler::samplesDecoded.push_back(sample);
    DebugHandler::sampleTimes.push_back(getTime - DebugHandler::programStartTime);
    if (DebugHandler::logType == SAMPLE_MESSAGES) {
        DebugHandler::getMessages(SAMPLE_MESSAGES);
    }
}

void DebugHandler::addErrorMessage(QString message, int source) {
    QString append;
    switch (source) {
    case QOAHANDLER_SOURCE:
        append = "QOA Handler: ";
        break;
    case AUDIOHANDLER_SOURCE:
        append = "Audio Handler: ";
        break;
    case FILEHANDLER_SOURCE:
        append = "File Handler: ";
        break;
    case PLAYLISTHANDLER_SOURCE:
        append = "Playlist Handler: ";
        break;
    case QOA_DATA:
        append = "QOA Data Handler Thread: ";
        break;
    default:
        //this should never run, but if it does:
        append = "Unknown Error Location: ";
        break;
    }

    DebugHandler::errorMessages.push_back(QString::number(getTime - DebugHandler::programStartTime) + "μs - " + append + message);
    DebugHandler::errorTimes.push_back(getTime - DebugHandler::programStartTime);

    if (DebugHandler::logType == ERROR_MESSAGES) {
        DebugHandler::getMessages(ERROR_MESSAGES);
    }
}

void DebugHandler::getMessages(int messageType) {
    QString message;
    DebugHandler::logType = messageType;
    switch (messageType) {
    case ERROR_MESSAGES:
        message =  DebugHandler::errorMessages.join("\n");
        break;
    case SAMPLE_MESSAGES:
        for (int i = 0; i < DebugHandler::samplesDecoded.count(); i++) {
            if (QThread::currentThread()->isInterruptionRequested()) {
                return;
            }
            message = message + QString::number(DebugHandler::sampleTimes[i]) + "μs - " + "Sample decoded: " + QString::number(DebugHandler::samplesDecoded[i]) + "\n";
        }
        break;
    }
    emit DebugHandler::sendLog(message);
}

void DebugHandler::emptySamples() {
    //save the log data before saving
    DebugHandler::saveLogFile();

    DebugHandler::samplesDecoded.clear();
    DebugHandler::sampleTimes.clear();
    if (DebugHandler::logType == SAMPLE_MESSAGES) {
        DebugHandler::getMessages(SAMPLE_MESSAGES);
    }
}

void DebugHandler::saveLogFile() {

    emit DebugHandler::saveLog(DebugHandler::logFileName, DebugHandler::getSaveData());
    

}

QString DebugHandler::getSaveData() {
    QString toSave;
    uint64_t samplePointer = 0;
    while (DebugHandler::errorMessageSavePointer < DebugHandler::errorMessages.count() && samplePointer < DebugHandler::sampleTimes.count()) {
        if (DebugHandler::errorTimes[DebugHandler::errorMessageSavePointer] < DebugHandler::sampleTimes[samplePointer]) {
            toSave = toSave + "\n" + DebugHandler::errorMessages[DebugHandler::errorMessageSavePointer];
            DebugHandler::errorMessageSavePointer++;
        }
        else {
            if (samplePointer == 0) {
                toSave = toSave + "\n------------------------ New Song Opened : " + fileHandler->getCurrentSongFileName() + " ------------------------";
            }

            toSave = toSave + "\n" + QString::number(DebugHandler::sampleTimes[samplePointer]) + "μs - " + "Sample decoded: " + QString::number(DebugHandler::samplesDecoded[samplePointer]);
            samplePointer++;
        }
    }

    while (DebugHandler::errorMessageSavePointer < DebugHandler::errorMessages.count()) {
        toSave = toSave + "\n" + DebugHandler::errorMessages[DebugHandler::errorMessageSavePointer];
        DebugHandler::errorMessageSavePointer++;
    }

    while (samplePointer < DebugHandler::sampleTimes.count()) {
        toSave = toSave + "\n" + QString::number(DebugHandler::sampleTimes[samplePointer]) + "μs - " + "Sample decoded: " + QString::number(DebugHandler::samplesDecoded[samplePointer]);
        samplePointer++;
    }

    return toSave;
}

QString DebugHandler::getSaveFileName() {
    return DebugHandler::logFileName;
}


DebugHandler* debugHandler = new DebugHandler();


