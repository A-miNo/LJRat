# LJRat
LJRat is a Remote Access Tool (RAT) project that will be used as a learning tool for me to create a modular/extensible framework for performing Red Team related tasks.

## 1.0.0 Objectives
- Build a core client that is capable of performing Get, Put, Execute, Load
- Build a modular lp/client that is easily able to integrate new modules
- Logging framework capable of providing useful command and debugging output

## Future Objectives
- Unit Tests
- Encryption for communication
- Tunneling
- Modules to extend functionality (Screenshot, injection, netstat, arp, security product evastion, etc)


## Building
The following packages are required to build the RAT Client
- Cmake (https://cmake.org/download/)
- MSVC Build Tools (https://aka.ms/vs/17/release/vs_BuildTools.exe)
- Python 3.10 (https://www.python.org/downloads/)
- Run build.bat afer installing the requisite components to generate the client