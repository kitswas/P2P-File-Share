# P2P-File-Share

A simple peer-to-peer file sharing system using TCP sockets.
[View Source.](https://github.com/kitswas/P2P-File-Share)  
This was made for an assignment in a graduate course on Operating Systems.  
For educational purposes.

> [!CAUTION]
> Warranty NOT included. Use at your own risk.

## How it works

The system consists of two components:

1. **Tracker**:  
   A server that keeps track of the files available on the network.  
   It listens for incoming connections from clients and other trackers.  
   It also responds to queries from clients and other trackers.

2. **Client**:
   A client that connects to the tracker to get a list of files available on the network.  
   It can download files from other clients on the network.  
   It can also upload files to other clients on the network.

### Tracker

The tracker is a server that listens for incoming connections from clients and other trackers.  
Every request to the tracker is saved as a Transaction.  
Atleast one tracker must be online at any point in time.  
Whenever a new tracker joins the network, it receives a list of all the successful transactions from the others.  
The new tracker then executes these transactions to get up to date with the network.

[![Tracker Multithreading Architecture](./TrackerMultithreadingArchitecture.svg)](./TrackerMultithreadingArchitecture.svg)

### Client

The client connects to the tracker to get a list of files available on the network.
It can download files from other clients on the network.

[![Client Multithreading Architecture](./ClientMultithreadingArchitecture.svg)](./ClientMultithreadingArchitecture.svg)

### Other Important Components

There are 2 daemon-type components that run in the background.

#### The Download Manager

The DownloadManager is responsible for downloading files from other clients on the network.  
The user of the class can enqueue a download request, and the DownloadManager will download the file in the background.  
The user can either poll the status of the downloads or register a callback to be notified when the download is complete.

#### The TCP Server

This is a part of our custom networking library.

The TCP Server is a high-level abstraction over TCP socket, which in turn is a high-level abstraction over the POSIX socket API.  
The TCPServer class is loosely based on the [QTcpServer](https://doc.qt.io/qt-6/qtcpserver.html) class from the Qt framework.
TCPSocket is similarly modelled after [QTcpSocket](https://doc.qt.io/qt-6/qtcpsocket.html).

### Things to Note

- The default block size for files is 512 KB.  
  This can be changed by modifying the `block_size` constant in fileinfo.hpp.
  
- TCP Server uses the `select` system call to handle multiple connections.  
  As a result, the maximum number of connections it can handle is limited.  
  The system will not scale well, and has problems handling files with many pieces.  
  When I created it, I was not aware of the existence of the improved `poll` and `epoll` system calls.  
  Honestly, `select()` should be deprecated by now.
  See:
  - [StackOverflow - Difference between poll and select](https://stackoverflow.com/q/970979/8659747)
  - [StackOverflow - epoll vs select](https://stackoverflow.com/a/2033785/8659747)

## Setting up your workspace

This project has a [.editorconfig file](https://editorconfig.org/) to enforce project level coding standards.  
CLion has built-in support,
VSCode requires [a plugin](https://marketplace.visualstudio.com/items?itemName=EditorConfig.EditorConfig).

## How to run

> [!IMPORTANT]
> Requires a POSIX compliant system.

This project requires [CMake](https://cmake.org/) to build.
Your IDE (VSCode or CLion) should automatically detect the CMakeLists.txt file and build the project.
Install extensions for CMake support if prompted.  
If you are using the command line, you can run the following commands:

```bash
cmake -B build
cmake --build build --config Release
./build/tracker/tracker ./build/tracker_info.txt 1            # run the tracker first
./build/client/client 127.0.0.1:9999 ./build/tracker_info.txt # and then the client
```

### Usage

#### Tracker commands

1. `quit`

#### Client commands

1. `quit`
2. `create_user <user_id> <passwd>`
3. `login <user_id> <passwd>`
4. `logout`
5. `create_group <group_id>`
6. `join_group <group_id>`
7. `leave_group <group_id>`
8. `list_requests <group_id>`
9. `accept_request <group_id> <user_id>`
10. `list_groups`
11. `list_files <group_id>`
12. `upload_file <file_path> <group_id>`
13. `download_file <group_id> <file_name> <destination_path>`
14. `show_downloads`

## Generating and Viewing Documentation

This project uses [Doxygen](https://www.doxygen.nl/index.html) to generate documentation.  
If Doxygen is available on your system,  
You can generate the documentation by running the `doc` CMake target.

```bash
cmake -B build
cmake --build build --target doc
```

This repository also has an automated workflow to generate documentatation via Github Actions.  

The generated documentation can be viewed at [/docs](./docs/index.html).

```bash
pushd docs ; python3 -m http.server 9999; popd # if you have python installed and want to use a server
```

```bash
open docs/index.html # or open the file from the OS file manager
```

A good starting point to explore the codebase is the [file listing page](./docs/files.html).
([files.html](files.html) if you are viewing this in a browser)
