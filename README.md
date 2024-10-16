# QTChat
This is a peer-to-peer chat service which will support messaging, file sharing and voice calls. All fully end-to-end encrypted.
# Setup
You will need to create cryptographic certficates for both you and the peer, using the same certificate authority (CA). These are the openssl commands to do so:
- Create CA key:
openssl genpkey -algorithm RSA -out ca.pem -aes256 -pass pass:yourpassword -pkeyopt rsa_keygen_bits:4096
- Create CA self signed certificate:
openssl req -x509 -new -key ca.pem -sha256 -days 3650 -out ca.pem -passin pass:yourpassword
- Server private key:
openssl genpkey -algorithm RSA -out server.pem -pkeyopt rsa_keygen_bits:2048
- Certificate Sign Request:
openssl req -new -key server.key -out server.csr
- Now, create san.cnf and put it in you working directory. Put this in it:
[ req ]
distinguished_name = req_distinguished_name
x509_extensions = v3_req
prompt = no

[ req_distinguished_name ]
C = US
ST = New York
L = New York
O = Example Corp
CN = example.com

[ v3_req ]
subjectAltName = @alt_names

[ alt_names ]
DNS.1 = example.com
DNS.2 = www.example.com
IP.1 = 192.168.1.100 //Change this to the IP of the computer you are creating the certificate for

- Finally, we sign the certificate using the CA private key:
openssl x509 -req -in server.csr -CA ca.pem -CAkey ca.pem -CAcreateserial -out server.pem -days 365 -sha256 -extfile san.cnf -extensions v3_req -passin pass:yourpassword

My other project, Auto CA, is currently under construction and upon completion will automate this process.

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

