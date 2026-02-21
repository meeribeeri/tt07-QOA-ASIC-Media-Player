#pragma once
#include <vector>
#include <iterator>
#include <iostream>
#include <qstring.h>
#include "FileHandler.h"
#include <fstream>
#include <mutex>
#include "QOADataSender.h"
#include <qobject.h>
#include "qthread.h"
#include <chrono>
#include <array>
#include "AudioHandler.h"
#include "DebugHandler.h"

#define VID 0xcafe
#define PID 0x4002
#define SLICES_PER_FRAME 256
#define AUDIO_REWIND_FORWARD_TIME_SECONDS 5
#define SAMPLE_SEND_COUNT 40
#define SAMPLES_TO_DISCARD 300

uint64_t convert_to_uint64(std::vector<unsigned char>::iterator beginning_byte);

uint32_t convert_to_uint32(std::vector<unsigned char>::iterator beginning_byte);

uint32_t convert_to_uint24(std::vector<unsigned char>::iterator beginning_byte);

uint16_t convert_to_uint16(std::vector<unsigned char>::iterator beginning_byte);

class QOAHandler : public QObject
{
    Q_OBJECT
public:
    void splitAndSend(QString fileName, bool manual = true);
    QOAHandler();
    ~QOAHandler();
    int getReadWrite();
    int getDedicatedRead();
    std::vector<int16_t> aquireSamples(bool& successful, int bytes_requested);
    class AudioHandler* getAudioHandler();
    std::vector<uint16_t> getSamples();
    void resetCOMFallback();
public slots:
    void threadFinished();
    void changeCOMPorts(int readWritePort, int dedicatedReadPort);
    void add_samples(std::array<uint8_t, 40> samples);
    void sample_count_obtained(int samplerate, int samples);
    void forwardAudio();
    void rewindAudio();
    void jumpAudioToPoint(int sampleNum);
    void redoCOM();
signals:
    void start(QString fileName, int readWritePort, int dedicatedReadPort);
    void needComSelector();
    void song_playbar_set_transfer(int songLength, int sample_count);
    void setFrequency(int samplerate);
    void timePassed(int seconds_passed_total, int samples_passed);
    void activeWindowMain();
    void nextSong();
private:
    QThread dataSendThread;
    QThread audioThread;


    class AudioHandler *audioHandler;

    int readWritePort;
    int dedicatedReadPort;
    
    int song_playing_or_played; //literally just a check to ensure that a song has been attempted to play and that moving the song pos slider will not cause an error
    
    std::vector<uint16_t> decoded_samples; //vector for storing the received samples
    //std::array<int16_t, SAMPLE_SEND_COUNT> samples_to_send;
    std::vector<int16_t> samples_to_send;
    int max_samples;
    int samplerate;
    int current_write_location;
    int read_location;
    int samplesSavedIntoDebug;
    bool finishedDecode;

    std::vector<uint32_t> frame_jump_locations;
    bool stop_decoding_normal;
    bool discarded_first_samples;

    bool fallbackCOM;

    std::mutex sample_mutex;
    std::mutex sample_read_location_mutex;

};

extern QOAHandler* qoaHandler;