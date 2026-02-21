#pragma once
#include <QtCore\qobject.h>
#include <qdatetime.h>
#include <qthread.h>
#include <chrono>
#include "QOAHandler.h"
#include "FileHandler.h"


#define QOAHANDLER_SOURCE 0
#define AUDIOHANDLER_SOURCE 1
#define FILEHANDLER_SOURCE 2
#define PLAYLISTHANDLER_SOURCE 3
#define QOA_DATA 4

#define ERROR_MESSAGES 0
#define SAMPLE_MESSAGES 1
#define getTime std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()


class DebugHandler :
    public QObject
{
    Q_OBJECT
public:
    DebugHandler();
    QString getSaveData();
    QString getSaveFileName();
public slots:
    void addSampleMessage(int16_t sample, int sampleNum, int source);
    void sampleWrite(int16_t sample, int sampleNum, int source);
    void addErrorMessage(QString message, int source);
    void getMessages(int messageType);
    void emptySamples();
    void saveLogFile();
signals:
    void sampleReceive(int16_t sample, int sampleNum, int source);
    void sendLog(QString message);
    void saveLog(QString fileName, QString toAppend);
private:
    QList<QString> errorMessages;
    QList<uint64_t> errorTimes;
    QList<int16_t> samplesDecoded;
    QList<uint64_t> sampleTimes;
    QString logFileName;

    int logType;
    int errorMessageSavePointer;

    uint64_t programStartTime;

};

extern DebugHandler* debugHandler;
