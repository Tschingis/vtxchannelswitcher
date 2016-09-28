# VTX Channel Switcher

This project is a nice little helper for people using video transmitters in quadrocopters or other remote controlled vehicles with an VTX. It allows one to switch the video channel of a unify vtx with the attached remote control utilising a rc channel. It consists of a hardware part which connects the rc channel to the unify vtx and a lua script extension for the wide spread taranis remote control to setup the vtx channel.

## Restrictions

* Works currently only with unify vtx with smartaudio support (V1 or V2) (tested Unify 5.8 Pro HV)
* Might not work with audio connected, since a cheap voltage divider (5V to 3.3V) is used to interface the unify. (Maybe someone tests this). The board could be easyly changed to run directly from 3.3V when available so no voltage divider is needed.
* LUA Script works only with OpenTX Remotes (Taranis tested), but the vtx channel switcher can also be controlled with a poti on the remote control.

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

## Building the MCU Software 

The software for the MCU is build with arduino since its easy to install and use. So install the following software

* [Arduino IDE 1.6](https://www.arduino.cc/)
* ATTiny13 Core for arduino 1.6 [Core13](https://sourceforge.net/projects/ard-core13/?source=navbar)  the version *core13_022_arduino_1_6.zip*. Installation is done by unzipping the downloaded file and then put the folder *attiny13* into *Your User/documents/Arduino/hardware*. If the directory should not exist, create it.

After installing Arduino IDE and the required ATTiny13 Core you should start the Arduino IDE and check if the ATTiny13 can be select as Board. ![Arduino Board Selection](/images/arduino_attiny13a_board.png)

If everything went ok, you should select the board *Attiny 13A standalone 1.2mhz*. Now open the arduino project found in the channelswitcher directory. The sketch should compile ok when you press the verify button and you are now ready to flash the mcu.

## Flashing the ATTiny

Flashing a standalone attiny is not as comfortable as most arduino boards since you have to use the Atmel ISP interface of the mcu to flash it. In order to use the ISP you have to connect the ISP to the attiny. The supplied pcb layout does not provide any solder points to connect the ISP lines to the attiny. You should just solder some cables directly to the mcu pins for programming (or if you are using a DIP part you could use a socket) which will be removed afterwards. I will not go into too much detail of how to connect the ISP to the attiny13 as there are many tutorials out there which show how to connect it. The attiny13 is pin compatible (in respect to ISP) with the attiny45/85 so you can also use a tutorial for these MCUs on how to wire up the MCU against the ISP. If you are using an ArduinoISP the wiring would be 

* Arduino +5V      --->  ATtiny Pin 8
* Arduino Ground --->  ATtiny Pin 4
* Arduino Pin 10   --->  ATtiny Pin 1
* Arduino Pin 11    --->  ATtiny Pin 5
* Arduino Pin 12    --->  ATtiny Pin 6
* Arduino Pin 13    --->  ATtiny Pin 7

When everything is connected and powered up you should be able to program the attiny by pressing the upload button in the arduino ide. If the programming fails check the connections. Pin 1 on the attiny is marked with a small triangle on top. There should also be no connection between the isp connection wires (solder blobs etc.).

## Connecting the VTX Switcher to the Unify VTX

The connection to the unify vtx is rather simple since you have only to connect the audio cable from the unify with the smart audio pad of the vtx channel switcher.

![Board Wiring](/images/board_wiring.jpg)

## Setting up the Flight Controller for Channel Forwarding

The most commonly used case for this project will be that it is installed inside a copter which is controlled by a flight controller. Nowadays the most users have serial receiver installed which will not output the required servo signal for the vtx channel switcher. I will show for some flight controllers how to setup an channel forwarding which puts the required servo signal on a output channel of the flight controller

### Setting up Betaflight/Cleanflight

Betaflight/Cleanflight has the possiblity to output a servo signal to the pwm pins on the used board. I tested it only with an RMDO Board but it should apply to other boards as well. Betaflight can enable channel forwarding in the feature section of the configurator but this just leads to frustration as it does not activate all required features so just open the CLI and type

    feature CHANNEL_FORWARDING 
    feature SERVO_TILT
    save

Now you can setup a servo mapping in the configurator gui. On most F3 targets ESC Pins 7-8 map to servo 0-1. The servo pane actually maps an input channel to a servo(ESC pin). Just set a checkbox on the input which should be used for the servo, i.e. check the box *A2* on *Servo 0*. Now you have mapped Aux2 input channel to ESC Pin 7 (servo0), the rest of the settings should be kept untouched.
![Servo Mapping](/images/servo_mapping.png)

### Setting up KISS FC

For the KISS FC its much easier (as supposed). You should just select in the AUX1-4 channel section one of the functions *PWM5: Servo Mid* or *PWM6: Servo Mid*. This maps the input from the selected AUX channel to the PWM5 or PWM6 output pin of the KISS FC.

## Setting up Taranis Channel Switcher LUA Script

The FrSky Taranis is a very nice remote control with excellent software support. This allows us to put a LUA Script on the Taranis (SD Card) and show a custom screen when executed which modifies the model memory in order to send the correct servo signal for a specific video channel. 
The script actually computes a global variable value for GV1 (can be changed to any other if already used) which is then used inside a mixer to generate an usuable servo signal for the vtx channel switcher. 
The first step is to copy over the *channelselector.lua* script from the repository *opentx* directory to the sd card of the taranis. You can put it anythere on the sd card, but you should use the root of the card since there will be no shortcut (switch assignment) to the script and you have to navigate to the script in the sd card browser of the taranis. 
The next step is to create a new mixer for the rc channel which has been mapped previously to vtx channel switcher hw. In the mixer change the values

    Source -> MAX
    Weight -> GV1  (long enter press on the weight allows to assign global variables)
    
This is everything which must be setup.

## Starting the LUA Script

You can now start the *channelselector.lua* script when you are on the main screen of the taranis by a long press on the menu button and then go to the sd card browser (page btn). In the sd card browser navigate to the script and execute it (select it with enter and choose *Execute*). **Have Fun**


