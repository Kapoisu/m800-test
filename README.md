# M800 Test

There are two primary modules within this project.
* UDP echo server: receive messages from clients and reply with the same messages.
* UDP client: send a message to a specific server and print the echo message.

The client can retry the sending procedure upon a failure if the ```--max-retry``` option is speficied.

## Tools
* CMake: building
* GCC: compiler

## Dependencies
* cargs: a third party library for parsing command-line arguments.

## Getting Start

Get the source code using the following command or download the zip file directly.
```
$ git clone https://github.com/Kapoisu/m800-test.git
```

To build the project:
```
$ cd m800-test
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build . --config Release --target all
```

If you're building the project on Windows with MinGW, you might have to specify the generator explicitly:
```
$ cmake -G "MinGW Makefiles" ..
```

## Usage
### Server
```
$ <server_executable> [-h | --help] [--ip=<address>] [--port=<number>]
```
### Client
```
$ <client_executable> [-h | --help] [--ip=<address>] [--port=<number>] [--max-retry=<number>] <message>
```

## Demo
https://user-images.githubusercontent.com/38966207/219833647-6af9c54a-1be3-4de7-a8ba-adc72dc602f5.mp4
