# FPGA_Billiards
The original IP for FPGA Billiards, to run on the Nexys-4-DDR board. This project was for the course ECE532 at the University of Toronto, April 2015.



##How to use
To run the game, you will need:

- A Digilent Nexys-4-DDR board
- An ov7670 camera
- A 640x480 VGA computer module
- A red LED on the end of a pool cue.

To run the game, compile the Vivado project "project_1" and download it onto the board. Create a SDK project using the hardware files from this project. Add the C++ code in the Software directory (except for the test.cpp file) and run the program.

Enjoy playing your game of billiards.


##Repository Structure
###docs:
Contain the proposal and Final report for this group
###src:
####Hardware:
#####PositionLocatorTest_2014_1:
This hardware system is used to test and confiure the camera and position locator. The seven segment displays on the Nexys-4-DDR board can be used to test the configuration of the position locator.
#####project_1:
The descriptively named main project. Contains all the hardware needed to run the billiards game.
#####VivadoIP:
Contains the IP modules, imported by the other projects
######axi_to_7segDisplay_1.0:
Used in the test hardware to output values to the seven segment display
######pmod_input{2}_1.0:
Configures the camera module and reads the pixel values. Version 1 reads 320x240 pixel frames, version 2 uses 640x480.
######poisition_locator_1.0:
Custom IP for locating the pool cue on screen.
####PhysicsSoftware:
All of the source files for the program to run on the processor.


##Authors
Peter Lin

David McTavish

Peng Xue

##Acknowledgements
We would like to thank Professor Paul Chow and the rest of the teaching staff of ECE532 for running this course and providing all the resource we needed to create this project. 