# P2P-File-Share

A simple peer-to-peer file sharing system using TCP sockets.  
For educational purposes.

> [!CAUTION]
> Warranty NOT included. Use at your own risk.

## How it works

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
