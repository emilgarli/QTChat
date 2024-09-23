# QTChat
This is a peer-to-peer chat service which will support messaging, file sharing and voice calls. All fully end-to-end encrypted.
# Dependencies
Portaudio - Used to do the handling of audio when doing VoIP.
You might have to edit cmakelists.txt to make sure the include paths are set correctly.
# TODO
- Test VoIP
- File sharing
When this is implemented, there should be a way to display images/videos in the chat window. Perhaps even make it possible to play a video file as it is being received in the chat window.
- Encryption
Encryption is to be done using openssl. The plan is to use a polymorphism on the current socket class CWizReadWriteSocket to create CWizSSLSocket.
This socket class should feature self signed certificates and maybe even a way to automatically genereta new certificates when they expire.
