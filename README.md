# EmbeddedGraphicsRender

Author: Adam Alderton, Gonville & Caius College, aa2301@cam.ac.uk
Premise: Final coursework for 4B25.

## Introduction

This repository is a fork of the *Warp* firmware which can be found [here](https://github.com/physical-computation/Warp-firmware). This project is built upon the basis of the Warp firmware for a number of reasons:
- To borrow its compatibility with FRDM-KL03 development board.
- Its convenient and portable build process.
- Its KSDK integration.

Implemented is a framework to render simple 3D graphics on an OLED display. A microcontroller carries out the computations - hence memory considerations are paramount in the implementation.

This project was

## Repository Layout

Much of the repositary matches that of the original Warp firmware. Almost all of the changes implemented in this projection lie in the `src/boot/ksdk1.1.0/graphics`. The most central file to the implementation is `graphics.*` so it is advised to look there to begin to gain an intuition of the implementation.

## Demos

## Hardware

- FRDM-KL03 development board, available [here](https://www.mouser.co.uk/ProductDetail/NXP-Semiconductors/FRDM-KL03Z?qs=y0HsvvugQ45hZ%2FWfA4he7w%3D%3D) and elsewhere.
- Adafruit mini OLED display, available [here](https://www.adafruit.com/product/684) and elsewhere.
- Basic breadboarding equipment including wires.
- Mini USB to Mini USB cable.
  
## Dependencies

- KSDK1.1 (Included with Warp firmware)
- ARM GNU toolchain, available [here](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm).
- SEGGER JLINK tools, available [here](https://www.segger.com/downloads/jlink/?).
- (More information regarding these are given in the Warp firmware repository).

## Getting Started

### Wiring the Hardware

- *GND*: KL03 GND to OLED #1
- *VCC*: KL03 3.3V to OLED #2
- *MOSI*: KL03 PTA8 to OLED #8
- *SCK*: KL03 PTA9 to OLED #7
- *CS*: KL03 PTB11 to OLED #4
- *DC*: KL03 PTA12 to OLED #6
- *RST*: KL03 PTB0 to OLED #5

### Building the Firmware

The build process of this program is exactly that of the Warp firmware. In order to build a particular demo, edit the preprocessor definitions in `graphics.h`. Comments in the file indicate what should be edited and where.

To load the firmware onto the KL03 on the development board, the following commands should be execute in the SEGGER JLINK software:
```
unlock kinetis
loadfile <full-path-to-this-repository>/build/ksdk1.1/work/demos/Warp/armgcc/Warp/release/Warp.srec
r
go
```

## Implementation Notes

The KL03 only has 2KB of SRAM available to it. Hence, clearly, the limiting resource is memory as opposed to compute power.

## Limitations / Suggestions for Future Work

