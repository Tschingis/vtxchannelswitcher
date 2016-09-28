# VTX Channel Switcher

This project is a nice little helper for people using video transmitters in quadrocopters or other remote controlled vehicles with an VTX. It allows one to switch the video channel of a unify vtx with the attached remote control utilising a rc channel. It consists of a hardware part which connects the rc channel to the unify vtx and a lua script extension for the wide spread taranis remote control to setup the vtx channel.

## Required Knowledge

If you want to build such a device yourself you should be familar with arduino and programming mcus utilising the ISP interface, although i will try to explain the most parts or link to tutorials needed. In addition you should know how to solder mcus or in general electronics.

## Required Tools

In order to build a VTX channel switcher you need the following tools

* Solder iron
* Any arduino supported Atmel ISP Programmer (i.e. USBAsp or ArduinoISP)
* Required parts for the circuit (found in circuit, ATTiny13A MCU,some resistors and a capacitor)
* Multimeter with frequency measurement (not always needed)

## Building the Hardware

The interface between the rc receiver (or flight controller) and the unify vtx is a rather simple circuit which consist mainly of a ATTiny13A. This mcu is very low cost and needs nearly no additional hw to run reliable. The pcb is found in the circuit folder. You can use the provided pcb which uses SMD parts or you can just build your own version with DIP parts. The required VCC is 5V.

## Programming the Hardware


