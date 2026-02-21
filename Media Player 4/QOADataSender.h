#pragma once
#include "qobject.h"
#include <vector>
#include <iterator>
#include <iostream>
#include <qstring.h>
#include "FileHandler.h"
#include <fstream>
#include <windows.h>
#include <string.h>
#include <sstream>
#include <array>
#include "DebugHandler.h"

#include <bitset>

#define HISTORY 0b1000
#define WEIGHT 0b0111
#define SF_QUANT_INDICATOR 0b0100
#define QR_INDICATOR 0b0001
#define TIMEOUT 500
#define BAUDRATE CBR_115200

class QOADataSender : public QObject
{
    Q_OBJECT
public:
    QOADataSender();
    ~QOADataSender();
public slots:
    void splitAndSend(QString fileName, int readWriteCOM, int readOnlyCOM);
signals:
    void finished();
    void send_decoded_samples(std::array<uint8_t, 40> samples);
    void send_sample_count(int samplerate, int sample_count);
    void finished_frame(uint32_t frame);
    void badCOMPorts();
    void sendError(QString message, int source);
private:
    std::vector<unsigned char> data;
    uint32_t sample_count;
    int samples_read;
    double frame_count;

    size_t data_read_offset;

    bool openFile(QString fileName);

    HANDLE read_write_COM_handle;
    HANDLE dedicated_read_COM_handle;
};

