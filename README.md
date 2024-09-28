# QTChat
This is a peer-to-peer chat service which will support messaging, file sharing and voice calls. All fully end-to-end encrypted.
# Dependencies
Portaudio - Used to do the handling of audio when doing VoIP.
You might have to edit cmakelists.txt to make sure the include paths are set correctly.
#Details
- Encryption
The sockets used in this project is the standard winsock2 sockets. A wrapper called Rawsocket is used for easier use, and the CWizSSLSocket class inherits from Rawsocket. CWizSSLSocket is responsible for encrypting all socket data to and from the application, and needs only the user to supply a chain certificate, private key and a ca-certificate to function.
# TODO
- Test VoIP
- File sharing
When this is implemented, there should be a way to display images/videos in the chat window. Perhaps even make it possible to play a video file as it is being received in the chat window.
-Autogenerate certificates
A separate server should be built that listens on a set port, and generates certificates and keys for clients that ask for them. These should be packed into a password protected .pfx file, and transmitted over an unencrypted socket to the client. 

