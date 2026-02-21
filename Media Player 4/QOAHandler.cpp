#include "QOAHandler.h"
uint64_t convert_to_uint64(std::vector<unsigned char>::iterator beginning_byte) {
    uint64_t converted_num = 0;
    for (std::vector<unsigned char>::iterator i = beginning_byte; i != beginning_byte + 8; i++) {
        converted_num = (converted_num << 8) | *i;
    }
    return converted_num;
}

uint32_t convert_to_uint32(std::vector<unsigned char>::iterator beginning_byte) {
    uint64_t converted_num = 0;
    for (std::vector<unsigned char>::iterator i = beginning_byte; i != beginning_byte + 4; i++) {
        converted_num = (converted_num << 8) | *i;
    }
    return converted_num;
}

uint32_t convert_to_uint24(std::vector<unsigned char>::iterator beginning_byte) {
    uint64_t converted_num = (uint32_t)0;
    for (std::vector<unsigned char>::iterator i = beginning_byte; i != beginning_byte + 3; i++) {
        converted_num = (converted_num << 8) | *i;
    }
    return converted_num;
}

uint16_t convert_to_uint16(std::vector<unsigned char>::iterator beginning_byte) {
    uint64_t converted_num = 0;
    for (std::vector<unsigned char>::iterator i = beginning_byte; i != beginning_byte + 2; i++) {
        converted_num = (converted_num << 8) | *i;
    }
    return converted_num;
}

/// QOA Handler

QOAHandler::QOAHandler() {
    QOAHandler::readWritePort = -1;
    QOAHandler::dedicatedReadPort = -1;
    QOAHandler::samplerate = 0;
    QOAHandler::fallbackCOM = false;
    QOAHandler::samplesSavedIntoDebug = 0;
    QOAHandler::finishedDecode = false;

    QOAHandler::audioHandler = new AudioHandler();
    QOAHandler::audioHandler->moveToThread(&audioThread);
    connect(&audioThread, &QThread::finished, QOAHandler::audioHandler, &QObject::deleteLater);
    connect(this, &QOAHandler::setFrequency, QOAHandler::audioHandler, &AudioHandler::changeAudioSpec);
    QOAHandler::audioThread.start();

    QOAHandler::samples_to_send = {};

    QOAHandler::song_playing_or_played = false;

    connect(this, &QOAHandler::start, debugHandler, &DebugHandler::emptySamples);
}


void QOAHandler::splitAndSend(QString fileName, bool manual) {
    if ((fileName == "" || fileName.isEmpty()) && manual) {
        return;
    }
    else if (fileName == "" || fileName.isEmpty()) {
        emit QOAHandler::setFrequency(-1);
    }

    if (!QOAHandler::dataSendThread.isFinished() && QOAHandler::dataSendThread.isRunning()) {
        //QOAHandler::dataSendThread.terminate();
        QOAHandler::dataSendThread.quit();
        QOAHandler::dataSendThread.requestInterruption();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    if (!QOAHandler::song_playing_or_played) {
        QOAHandler::song_playing_or_played = true;
    }

    QOAHandler::discarded_first_samples = false;
    QOAHandler::current_write_location = 0;
    QOAHandler::read_location = 0;
    QOAHandler::samplesSavedIntoDebug = 0;
    QOAHandler::decoded_samples.clear();
    QOAHandler::frame_jump_locations.clear();
    QOAHandler::finishedDecode = false;

    if (QOAHandler::readWritePort == -1 || QOAHandler::dedicatedReadPort == -1) {
        emit needComSelector();
    }

    if (QOAHandler::readWritePort == -1 || QOAHandler::dedicatedReadPort == -1) {
        debugHandler->addErrorMessage("COM Ports not selected.", QOAHANDLER_SOURCE);
        return;
    }

    QOADataSender* dataSender = new QOADataSender; //creates a QOADataSender object for sending/receiving data
    dataSender->moveToThread(&dataSendThread); //moves it into the dataSendThread
    connect(&dataSendThread, &QThread::finished, dataSender, &QObject::deleteLater); //connects the thread finishing and deleting the QOADataSender object
    connect(this, &QOAHandler::start, dataSender, &QOADataSender::splitAndSend); //connects QOAHandler's start signal to sarting the QOADataSender doing its job
    connect(dataSender, &QOADataSender::finished, this, &QOAHandler::threadFinished); //If the QOADataSender object has finished getting back samples, the thread is also finished
    connect(dataSender, &QOADataSender::send_decoded_samples, this, &QOAHandler::add_samples); //when the QOADataSender sends samples via a Qt Signal, the QOAHandler receives and processes them
    connect(dataSender, &QOADataSender::send_sample_count, this, &QOAHandler::sample_count_obtained); //to get back the total samples in the file
    connect(dataSender, &QOADataSender::badCOMPorts, this, &QOAHandler::redoCOM); //To handle if the COM ports fail, usually due to being wrong
    connect(dataSender, &QOADataSender::sendError, debugHandler, &DebugHandler::addErrorMessage); //Allows the QOADataSender to send error messages to the DebugHandler

    QOAHandler::dataSendThread.start(); //start the thread
    emit QOAHandler::start(fileName, QOAHandler::readWritePort, QOAHandler::dedicatedReadPort); //sends the start signal to start the QOADataSender performing its task
}

void QOAHandler::threadFinished() {
    QOAHandler::dataSendThread.exit();
    QOAHandler::finishedDecode = true;
}

QOAHandler::~QOAHandler() {
    QOAHandler::dataSendThread.quit();
    QOAHandler::dataSendThread.wait();
}

void QOAHandler::changeCOMPorts(int readWritePort, int dedicatedReadPort) {
    QOAHandler::readWritePort = readWritePort;
    QOAHandler::dedicatedReadPort = dedicatedReadPort;
}

int QOAHandler::getReadWrite() {
    return QOAHandler::readWritePort;
}

int QOAHandler::getDedicatedRead() {
    return QOAHandler::dedicatedReadPort;
}

void QOAHandler::add_samples(std::array<uint8_t, 40> samples) {
    uint16_t data;
    if (!QOAHandler::discarded_first_samples) {
        QOAHandler::sample_mutex.lock();
        if (QOAHandler::current_write_location < SAMPLES_TO_DISCARD) {
            QOAHandler::current_write_location++;
            QOAHandler::sample_mutex.unlock();

            for (int i = 0; i < 20; i++) {

                data = (samples[(i * 2)]) | (samples[(i * 2 + 1)] << 8);
                if (QOAHandler::samplesSavedIntoDebug < 1000) {
                    debugHandler->addSampleMessage(data, QOAHandler::current_write_location, QOAHANDLER_SOURCE);
                    QOAHandler::samplesSavedIntoDebug++;
                }
            }

            return;
        }

        QOAHandler::sample_mutex.unlock();

        QOAHandler::current_write_location = 0;
        QOAHandler::discarded_first_samples = true;
        emit QOAHandler::activeWindowMain();
        return;
    }

    for (int i = 0; i < 20; i++) {

        data = (samples[(i * 2)]) | (samples[(i * 2 + 1)] << 8); //format received samples into uint16_t values (samples for audio playing are 16 bits)
        QOAHandler::decoded_samples.push_back(data); //pushes data to the end of the decoded sample vector
        if (QOAHandler::samplesSavedIntoDebug < 1000) {
            debugHandler->addSampleMessage(data, QOAHandler::current_write_location, QOAHANDLER_SOURCE);
            QOAHandler::samplesSavedIntoDebug++;
        }
        QOAHandler::sample_mutex.lock(); //locks the mutex, or if already locked, queues and waits until the mutex is unlocked and it is its turn to lock it
        QOAHandler::current_write_location++; //increase the current write location var by 1
        QOAHandler::sample_mutex.unlock(); //unlock the mutex so it can be used by another task
        if (QOAHandler::current_write_location >= QOAHandler::max_samples) { 
            break;
        }
    }

    
    
    
}

void QOAHandler::sample_count_obtained(int samplerate, int samples) {
    emit QOAHandler::song_playbar_set_transfer(samples/samplerate, samples);
    emit QOAHandler::setFrequency(samplerate);
    QOAHandler::max_samples = samples;
    QOAHandler::samplerate = samplerate;

}

std::vector<int16_t> QOAHandler::aquireSamples(bool &successful, int bytes_requested) {
   
    QOAHandler::samples_to_send.clear();

    if (QOAHandler::finishedDecode && QOAHandler::read_location >= QOAHandler::decoded_samples.size() && QOAHandler::read_location > 1000) {
        emit QOAHandler::nextSong();
    }

    QOAHandler::sample_mutex.lock();
    QOAHandler::sample_read_location_mutex.lock();
    if ((QOAHandler::current_write_location >= QOAHandler::read_location + (bytes_requested/2) || QOAHandler::current_write_location >= QOAHandler::max_samples) || QOAHandler::finishedDecode) {
        for (int i = 0; i < bytes_requested/2; i++) {
            if (QOAHandler::read_location < QOAHandler::max_samples) {
                QOAHandler::samples_to_send.push_back(QOAHandler::decoded_samples[QOAHandler::read_location]);
                //for the sake of not adding another loop to this program, the qoahandler will send out the audio received debug message
                QOAHandler::read_location++;
            }
            else {
                debugHandler->addErrorMessage("All samples read yet attempted to read past.", QOAHANDLER_SOURCE);
                break;
            }
        }
        successful = true;
        //printf(" % i % i % i\n", QOAHandler::read_location, QOAHandler::samplerate, QOAHandler::read_location / QOAHandler::samplerate);
        emit QOAHandler::timePassed(QOAHandler::read_location / QOAHandler::samplerate, QOAHandler::read_location);
    }
    else {
        successful = false;
    }

    QOAHandler::sample_mutex.unlock();
    QOAHandler::sample_read_location_mutex.unlock();

    return QOAHandler::samples_to_send;
}


void QOAHandler::forwardAudio() {
    QOAHandler::sample_read_location_mutex.lock();
    QOAHandler::read_location = QOAHandler::read_location + (QOAHandler::samplerate * AUDIO_REWIND_FORWARD_TIME_SECONDS);
    if (QOAHandler::read_location > QOAHandler::max_samples) {
        QOAHandler::read_location = QOAHandler::max_samples;
    }


    QOAHandler::sample_read_location_mutex.unlock();
    emit QOAHandler::timePassed(QOAHandler::read_location / QOAHandler::samplerate, QOAHandler::read_location);
}

void QOAHandler::rewindAudio() {
    QOAHandler::sample_read_location_mutex.lock();
    QOAHandler::read_location = QOAHandler::read_location - (QOAHandler::samplerate * AUDIO_REWIND_FORWARD_TIME_SECONDS);
    if (QOAHandler::read_location < 0) {
        QOAHandler::read_location = 0;
    }
    QOAHandler::sample_read_location_mutex.unlock();
    emit QOAHandler::timePassed(QOAHandler::read_location / QOAHandler::samplerate, QOAHandler::read_location);
}

void QOAHandler::jumpAudioToPoint(int sampleNum) {
    QOAHandler::sample_read_location_mutex.lock();
    QOAHandler::read_location = sampleNum;
    QOAHandler::sample_read_location_mutex.unlock();
    if (QOAHandler::song_playing_or_played) {
        emit QOAHandler::timePassed(QOAHandler::read_location / QOAHandler::samplerate, QOAHandler::read_location);
    }
}

AudioHandler* QOAHandler::getAudioHandler() {
    return QOAHandler::audioHandler;
}

std::vector<uint16_t> QOAHandler::getSamples() {
    return QOAHandler::decoded_samples;
}

void QOAHandler::redoCOM() {
    if (!QOAHandler::fallbackCOM) {
        emit needComSelector();
        QOAHandler::fallbackCOM = true;
    }
}

void QOAHandler::resetCOMFallback() {
    QOAHandler::fallbackCOM = false;
}

QOAHandler* qoaHandler = new QOAHandler();