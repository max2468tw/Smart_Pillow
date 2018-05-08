# embARC Open Software Platform
## Introduction
[<b>embARC</b>](https://embarc.org/) is an open software platform to facilitate the development of embedded systems based on ARCv2 processors.

It is designed to provide a unified platform for ARCv2 users by defining consistent and simple software interfaces to the processor and peripherals, together with ports of several well known FOSS embedded software stacks to ARCv2 Processors.

embARC provides a solid starting foundation for embedded developers, simplifying development and improving time to market through software reuse.
## embARC Architecture
The general embARC Open Software Platform Architecture is shown in the block diagram below.
![embARC Open Software Platform Architecture](doc/documents/pic/embARC.jpg)
### ARC Hardware Abstraction Layer
This layer provides standard interfaces and definitions for ARCv2 processors.
### Device Hardware Abstraction Layer
This layer defines the interfaces and definitions for peripherals such as UART, I2C, GPIO, SPI, and WNIC. This layer simply defines the interfaces of device drivers, but does not contain detailed device driver implementations. Driver implementation is left to the application. A group of driver implementations for supported DesignWare IP peripherals are provided as a reference.
### Operating System Layer
This layer contains the support for embedded operating systems.
### Common Library Layer
This layer provides stubs for various support library implementations such as C library, math library, DSP, Floating Point or other libraries. These libraries can be bare-metal or OS based implementations.
### Middleware Layer
This layer integrates different embedded middleware, such as Shell/Command Line Interface(CLI), file systems, networking and IoT communication protocol stacks.
## How to Use embARC
The embARC software includes several examples to help the user get up and running quickly with embARC. Please refer to [embARC Documentation](doc/embARC_Document.html) for more information.
