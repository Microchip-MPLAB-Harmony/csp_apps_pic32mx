---
parent: Harmony 3 peripheral library application examples for PIC32MX family
title: CORETIMER periodic interrupt 
has_children: false
has_toc: false
---

[![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)](https://www.microchip.com)

# CORETIMER periodic interrupt

This example application shows how to use the CoreTimer to generate periodic interrupts.

## Description

This example application configures the CoreTimer Peripheral Library to generate periodic interrupts. The application registers a periodic timeout callback. It toggles an LED every time the callback is triggered.

## Downloading and building the application

To clone or download this application from Github, go to the [main page of this repository](https://github.com/Microchip-MPLAB-Harmony/csp_apps_pic32mx) and then click **Clone** button to clone this repository or download as zip file.
This content can also be downloaded using content manager by following these [instructions](https://github.com/Microchip-MPLAB-Harmony/contentmanager/wiki).

Path of the application within the repository is **apps/coretimer/coretimer_periodic_timeout/firmware** .

To build the application, refer to the following table and open the project using its IDE.

| Project Name      | Description                                    |
| ----------------- | ---------------------------------------------- |
| pic32mx470_curiosity.X | MPLABX project for [Curiosity PIC32MX470 Development Board](https://www.microchip.com/Developmenttools/ProductDetails/dm320103) |
| pic32mx_eth_sk2.X | MPLABX project for [PIC32 Ethernet Starter Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/PartNO/DM320004) |
| pic32mx_xlp_sk.X | MPLABX project for [PIC32MX274 XLP Starter Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/DM320105) |
| pic32mx_sk.X | MPLABX project for [PIC32MX Starter Kit](https://www.microchip.com/Developmenttools/ProductDetails/DM320001) |
|||

## Setting up the hardware

The following table shows the target hardware for the application projects.

| Project Name| Board|
|:---------|:---------:|
| pic32mx470_curiosity.X | [Curiosity PIC32MX470 Development Board](https://www.microchip.com/Developmenttools/ProductDetails/dm320103) |
| pic32mx_eth_sk2.X | [PIC32 Ethernet Starter Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/PartNO/DM320004) |
| pic32mx_xlp_sk.X | [PIC32MX274 XLP Starter Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/DM320105) |
| pic32mx_sk.X | [PIC32MX Starter Kit](https://www.microchip.com/Developmenttools/ProductDetails/DM320001) |
|||

### Setting up [Curiosity PIC32MX470 Development Board](https://www.microchip.com/Developmenttools/ProductDetails/dm320103)

- Connect mini USB cable to the 'Debug USB' connector(J3) on the board to the computer

### Setting up [PIC32 Ethernet Starter Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/PartNO/DM320004)

- Connect mini USB cable to the 'Debug USB' connector(J7) on the board to the computer

### Setting up [PIC32MX274 XLP Starter Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/DM320105)

- Connect micro USB cable to the 'Debug USB' connector(J9) on the board to the computer

### Setting up [PIC32MX Starter Kit](https://www.microchip.com/Developmenttools/ProductDetails/DM320001)

- Connect mini USB cable to the 'Debug USB' connector(J1) on the board to the computer

## Running the Application

1. Build and program the application project using its IDE
2. LED Blinks continuosly

The following table provides the details of LED:

| Board | LED Name|
| ---- | ---------|
| [Curiosity PIC32MX470 Development Board](https://www.microchip.com/Developmenttools/ProductDetails/dm320103) | LED1 |
| [PIC32 Ethernet Starter Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/PartNO/DM320004) | LED1 |
| [PIC32MX274 XLP Starter Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/DM320105) | LED1 (RED) |
| [PIC32MX Starter Kit](https://www.microchip.com/Developmenttools/ProductDetails/DM320001) | LED3 (GREEN) |
|||
