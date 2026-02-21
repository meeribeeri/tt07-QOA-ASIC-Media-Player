#pragma once
#include <qobject.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <iostream>
#include "QOAHandler.h"
#include <array>
#include <vector>
#include "DebugHandler.h"

class AudioHandler : public QObject
{
    Q_OBJECT
public:
    AudioHandler();
    ~AudioHandler();
    int getVolume();
public slots:
    void changeAudioSpec(int frequency);
    void pause();
    void start();
    void force_pause();
    void retoggle_pause_state();
    void setVolume(int percent);
signals:
    void finish();
    void volumeUpdated();
private:
    SDL_AudioSpec* audio_spec;
    SDL_AudioStream* audio_stream;

    static void audioCallback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount);

    bool audio_device_open;
    bool audio_paused;
    bool audio_awaiting_force_pause;

    int volume_percent;


    bool openAudioDevice();

    void updateAudioVolume();
};

