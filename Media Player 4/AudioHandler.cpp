#include "AudioHandler.h"

AudioHandler::AudioHandler() {
    if (!SDL_Init(SDL_INIT_AUDIO)) {
        debugHandler->addErrorMessage("Failed to initialize SDL. SDL Error: " + QString::fromLocal8Bit(SDL_GetError()), AUDIOHANDLER_SOURCE);
        return;
    }
    
    AudioHandler::audio_spec = new SDL_AudioSpec();
    AudioHandler::audio_spec->format = SDL_AUDIO_S16LE;
    AudioHandler::audio_spec->channels = 1;
    AudioHandler::audio_device_open = false;
    AudioHandler::audio_paused = false;
    AudioHandler::audio_awaiting_force_pause = false;
    AudioHandler::volume_percent = 100;

}

bool AudioHandler::openAudioDevice() {
    AudioHandler::audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, AudioHandler::audio_spec, AudioHandler::audioCallback, NULL);
    if (AudioHandler::audio_stream == NULL) {
        debugHandler->addErrorMessage("Failed to open audio stream/device. This message is from within the openAudioDevice method. SDL Error: " + QString::fromLocal8Bit(SDL_GetError()), AUDIOHANDLER_SOURCE);
        AudioHandler::audio_device_open = false;
        return false;
    }

    AudioHandler::audio_device_open = true;
    AudioHandler::updateAudioVolume();
    return true;
}

void AudioHandler::changeAudioSpec(int frequency) {
    if (frequency < 0) {
        if (AudioHandler::audio_device_open && AudioHandler::audio_stream != nullptr) {
            SDL_ClearAudioStream(AudioHandler::audio_stream);
            SDL_DestroyAudioStream(AudioHandler::audio_stream);
            AudioHandler::audio_device_open = false;
        }
        return;
    }
    AudioHandler::audio_spec->freq = frequency;

    AudioHandler::start();
}

void AudioHandler::pause() {
    AudioHandler::audio_paused = !AudioHandler::audio_paused;
    if (AudioHandler::audio_stream != nullptr && AudioHandler::audio_device_open) {
        if (AudioHandler::audio_paused) {
            SDL_PauseAudioStreamDevice(AudioHandler::audio_stream);
        }
        else {
            SDL_ResumeAudioStreamDevice(AudioHandler::audio_stream);
        }
    }
}

void SDLCALL AudioHandler::audioCallback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount) { //callback method used by the audio stream to request more samples
    bool samples_obtained = false; //for whether or not samples were actually received or not
    std::vector<int16_t> data; //the sample container


    while (!samples_obtained) { //keeps repeating until the QOAHandler gives back samples.
        if (QThread::currentThread()->isInterruptionRequested()) { //because its a loop, the thread may not terminate when requested, so this just checks that and terminates if necessary
            return;
        }
        data = qoaHandler->aquireSamples(samples_obtained, additional_amount); //get the samples from the QOAHandler, additional_amount is how many are needed to play at regular speed
        if (samples_obtained) { //exits the loop if samples are obtained
            break;
        }
        
        if (fileHandler->getCurrentSongFileName().isEmpty()) { //exits if there is no song file, which can occur if the song changes to nothing, just a backup
            debugHandler->addErrorMessage("No song file found.", AUDIOHANDLER_SOURCE);
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500)); //pause the loop for a bit to not completely lock the thread
    }
    for (int tries = 0; tries < 3; tries++) { //attempts up to three times to put audio data into the audio stream
        if (SDL_PutAudioStreamData(stream, data.data(), data.size() * sizeof(int16_t))) { //exit on success
            break;
        }
        debugHandler->addErrorMessage("Data failed to enter audio stream. Retrying. SDL Error: " + QString::fromLocal8Bit(SDL_GetError()), AUDIOHANDLER_SOURCE);
    }
    
    data.clear(); //empty the sample container
}

void AudioHandler::start() {

    if (fileHandler->getCurrentSongFileName().isEmpty()) {
        SDL_ClearAudioStream(AudioHandler::audio_stream);
        SDL_DestroyAudioStream(AudioHandler::audio_stream);
        return;
    }

    if (!AudioHandler::audio_device_open) {
        if (!AudioHandler::openAudioDevice()) {
            debugHandler->addErrorMessage("Audio device failed to open, was not initially open.", AUDIOHANDLER_SOURCE);
            return;
        }
    }
    else {
        SDL_ClearAudioStream(AudioHandler::audio_stream);
        SDL_DestroyAudioStream(AudioHandler::audio_stream);
        if (!AudioHandler::openAudioDevice()) {
            debugHandler->addErrorMessage("Audio device failed to open, was open initially.", AUDIOHANDLER_SOURCE);
            return;
        }
    }
    if (!AudioHandler::audio_paused && !AudioHandler::audio_awaiting_force_pause) {
        SDL_ClearAudioStream(AudioHandler::audio_stream);
        if (!SDL_ResumeAudioStreamDevice(AudioHandler::audio_stream)) {
            debugHandler->addErrorMessage("Failed to start audio stream.device. SDL Error: " + QString::fromLocal8Bit(SDL_GetError()), AUDIOHANDLER_SOURCE);
            return;
        }
    }
    else if (AudioHandler::audio_awaiting_force_pause) {
        AudioHandler::audio_awaiting_force_pause = false;
        AudioHandler::force_pause();
    }
}

void AudioHandler::force_pause() {
    AudioHandler::audio_awaiting_force_pause = true;
    if (AudioHandler::audio_stream != nullptr && AudioHandler::audio_device_open) {
        SDL_PauseAudioStreamDevice(AudioHandler::audio_stream); 
    }
}

void AudioHandler::retoggle_pause_state() {
    AudioHandler::audio_awaiting_force_pause = false;
    if (AudioHandler::audio_stream != nullptr && AudioHandler::audio_device_open) {
        if (AudioHandler::audio_paused) {
            SDL_PauseAudioStreamDevice(AudioHandler::audio_stream);
        }
        else {
            SDL_ResumeAudioStreamDevice(AudioHandler::audio_stream);
        }
    }
}

AudioHandler::~AudioHandler() {
    SDL_Quit();
    if (AudioHandler::audio_device_open) {
        SDL_ClearAudioStream(AudioHandler::audio_stream);
        SDL_DestroyAudioStream(AudioHandler::audio_stream);
    }
    emit AudioHandler::finish();
}

int AudioHandler::getVolume() {
    return AudioHandler::volume_percent;
}

void AudioHandler::setVolume(int percent) {
    AudioHandler::volume_percent = percent;

    AudioHandler::updateAudioVolume();

    emit AudioHandler::volumeUpdated();
}

void AudioHandler::updateAudioVolume() {
    if (AudioHandler::audio_stream != nullptr && AudioHandler::audio_device_open) {
        if (!SDL_SetAudioStreamGain(AudioHandler::audio_stream, AudioHandler::volume_percent / 100.f)) {
            debugHandler->addErrorMessage("Failed to set audio volume. SDL Error: " + QString::fromLocal8Bit(SDL_GetError()), AUDIOHANDLER_SOURCE);
        }
    }
}
