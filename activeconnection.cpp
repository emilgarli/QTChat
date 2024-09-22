#include "activeconnection.h"
#include "thread"
#include "portaudio.h"
#include <condition_variable>
#include <mutex>

struct AudioData {
    std::vector<char>* buffer;
    std::mutex* bufferMutex;
    std::condition_variable* bufferCv;
    CWizReadWriteSocket* comSock;
};

ActiveConnection::ActiveConnection(QObject *parent, CWizReadWriteSocket* hSocket)
    : QObject{parent}
{
    socket = hSocket;
}

int ActiveConnection::readHandler(char* inBuf, int bufLen){
    int iRead = 0;
    iRead = socket->Read(inBuf, bufLen, 0);
    return iRead;
}
int ActiveConnection::writeHandler(const char* outBuf, int bufLen){
    int iWrite = 0;
    iWrite = socket->Write(outBuf, bufLen);
    return iWrite;
}

static int recordCallback(const void* inputBuffer, void* outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void* userData) {

    auto* audioData = (AudioData*)userData;

    if (inputBuffer == nullptr) {
        return paContinue;  // Continue even if input is silent
    }

    // Cast input buffer to the appropriate type (16-bit int, since we're using paInt16)
    const int16_t* in = (const int16_t*)inputBuffer;

    audioData->comSock->Write(in, framesPerBuffer);

    return paContinue;  // Continue the stream
}

int ActiveConnection::audioTransmitter(){
    // Initialize PortAudio
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        return 1;
    }

    // Create a struct to hold the microphone input data
    auto audioData = std::make_shared<AudioData>();
    audioData->comSock = socket;

    PaStream* stream;
    err = Pa_OpenDefaultStream(&stream,
                               1,  // Input channels (microphone)
                               0,  // No output channels
                               paInt16, // Sample format (16-bit integer)
                               44100,   // Sample rate (standard rate for audio)
                               2048,    // Frames per buffer
                               recordCallback, // Callback function to handle the microphone input
                               audioData.get()); // User data passed to the callback
    if (err != paNoError) {
        return 1;
    }

    // Start the audio stream
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        return 1;
    }

    while (Pa_IsStreamActive(stream) == 1) {
        Pa_Sleep(100); // Let the recording run
    }

    // Stop and close the stream
    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        return 1;
    }

    // Terminate PortAudio
    Pa_Terminate();
    return 0;
}


int ActiveConnection::voiceChatHandler(CWizReadWriteSocket* socket, std::string clientName){
    PaStream* stream = nullptr;
    AudioData audioData;
    audioData.buffer = &audioBuffer_;
    audioData.bufferMutex = &bufferMutex_;
    audioData.bufferCv = &bufferCv_;

    PaError err = Pa_Initialize();
    if (err != paNoError) {
        return 1;
    }

    err = Pa_OpenDefaultStream(&stream,
                               0,          // no input channels
                               1,          // stereo output
                               paInt16,    // 16-bit integer output
                               44100,      // sample rate
                               paFramesPerBufferUnspecified,       // frames per buffer
                               paCallback, // callback function
                               &audioData  // user data
                               );
    if (err != paNoError) {
        //emit updateUI("PortAudio error: " + QString::toStdString(Pa_GetErrorText(err)));
        Pa_Terminate();
        return 1;
    }
    //Here we start the voice stream

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    //And here we start the thread that will fill the stream with
    //music data.
    std::thread readerThread(&ActiveConnection::audioReader,
                             this,
                             std::ref(audioBuffer_),
                             std::ref(bufferMutex_),
                             std::ref(bufferCv_));
    readerThread.join();
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
    return 0;
}

int ActiveConnection::paCallback(const void* inputBuffer,
                                  void* outputBuffer,
                                  unsigned long framesPerBuffer,
                                  const PaStreamCallbackTimeInfo* timeInfo,
                                  PaStreamCallbackFlags statusFlags,
                                  void* userData)
{
    if (!userData) {
        return paAbort;
    }
    AudioData* audioData = static_cast<AudioData*>(userData);
    if (!audioData->buffer || !audioData->bufferMutex || !audioData->bufferCv) {
        return paAbort;
    }
    short* out = static_cast<short*>(outputBuffer);
    std::unique_lock<std::mutex> lock(*audioData->bufferMutex);
    if (audioData->buffer->empty()) {
        std::fill(out, out + framesPerBuffer, 0); // Fill with silence if no data
        audioData->bufferCv->wait(lock); // Wait for new data
    }
    if (!audioData->buffer->empty()) {
        size_t bytesToCopy = std::min(audioData->buffer->size(), framesPerBuffer * sizeof(short));
        std::copy(audioData->buffer->begin(), audioData->buffer->begin() + bytesToCopy, reinterpret_cast<char*>(out));
        audioData->buffer->erase(audioData->buffer->begin(), audioData->buffer->begin() + bytesToCopy);
        /*
        if (bytesToCopy < framesPerBuffer * sizeof(short)) {
            std::fill(out + bytesToCopy / sizeof(short), out + framesPerBuffer * 2, 0); // Fill remaining with silence
        }*/
    }
    return paContinue;
}
void ActiveConnection::audioReader(std::vector<char>& buffer,
                                    std::mutex& bufferMutex,
                                    std::condition_variable& bufferCv)
{
    try {
        char tempBuffer[2048] = {};
        int iRead = 0;
        while (true) {
            iRead = socket->Read(tempBuffer, sizeof(tempBuffer));
            if (iRead <= 0) {
                break;
            }
            //Lock the shared buffer
            std::lock_guard<std::mutex> lock(bufferMutex);
            //Write to the shared buffer
            buffer.insert(buffer.end(), tempBuffer, tempBuffer + iRead);
            //And finally, notify the waiting playback thread that the buffer is open for reading
            bufferCv.notify_one();
        }
    }
    catch (std::exception& e) {
    }

    catch (...) {
    }

}
