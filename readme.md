README FILE

PROJECT NAME:
	Embedded Challenge Spring 2022: The pressure is on

AUTHORS:
	Douglas de Jesus (drd8913) and Mateo Bonilla (mjb9333)

SOFTWARE CONFIGURATION:
To use this project, use PlatformIO in Visual Studio Code. Please follow these steps to open the project:

	1. Download the zipped file.
	2. Unzip the project into the PlatformIO project directory.
	3. Open PlatformIO and click on the "Open Project".
	4. Navigate to the PlatformIO folder and select the project.
	5. Click on "Open".

HARDWARE CONFIGURATION:
To use this project, you will need to have the following hardware:

	1. STM32F429 Discovery Board.
	2. USB type B data cable.
	3. Pressure sensor: Honeywell MPRLS0300YG00001BB.
	4. Blood Pressure Cuff.
	5. Transition air tubbing.
	6. F-F wire jumpers.

The project works using SPI connection from the board to the sensor. Therefore, use the following pins for
communication with the sensor:
	- MOSI: PE_6
	- MISO: PE_5
	- SCLK: PE_2
	- SSEL: PE_4

For connection:
	- Connect MOSI -> PE_6, MISO -> PE_5, SCLK -> PE_2 and SSEL -> PE_4
	- POWER pin (sensor) to 3V (board) and GND pin (sensor) to GND (board).
	- Connect the tubing to the pressure port in the sensor.
	- Connect the other end of the tubing to the pressure cuff by unpluging the default miter.
	- Plug the board to a computer with the project files to build.


OPERATING INSTRUCTIONS:
After the hardware and software configurations are done follow the next steps:

	1. Plug the board to the computer.
	2. With the project already open on PlatformIO click Upload and Monitor.
	3. Wait for the project to build.
	4. Begin using the product.

SECTIONS OF THE CODE:
Inside the code, there following files are available (do not replace/erase any file):

	Constants.h
	/*
	This file contains all the constants that are used along the program. All of this constants
	contain important information that support the correct functionality of the program. 
	*/

	analysis.h
	/*
	This file is created to analyze the pressure data that it is read by the sensor. Inside this 
	file the are two functions, findMinIndex and analyze_data. The first function is used to find 
	the index of the local minimum inside the array of measured data. The second function is used
	to determine the systolic and diastolic pressure by finding the local maximum of the data in 
	the graph.  
	*/

	graphics.h
	/*
	This file contains all the information and functions that are used to create the graphs and 
	display information and data on the LCD depending on the which state of the exeuction process
	is the user. In addition, all the data and constants for setting up the LCD are described here
	as well as the all the data and information that it is displayed on the screen when a new scene
	is being executed.
	*/

	sensor.cpp
	/*
	This file contains the configuration to establish communication with the sensor. This file contains
	to functions, the first one (setupSensor) is used for general configurations of the communication
	protocol (SPI) such as the frequency and mode of operation. The second function (readPressure) is 
	used to perform the communication transaction using SPI to retrieve the data read from the sensor.
	In addition, some basic transformations using the transfer function of the sensor are performed to 
	determine the actual value of the pressure in mmHg
	*/

	sensor.h
	/*
	This file constains the pin declaration for the SPI connection of the sensor with the board. In addition
	it gives the constructor for the functions that are later declared in the sensor.cpp file.
	*/
	
	main.cpp
	/*
	This file contains the main function declaration for all the program to work. It contains the main state 
	machine that conrols the overall execution of the program. Also, it has the code to provide all the error
	handling for the anormal cases in the execution. 
	*/

COPYRIGHT:
This project belongs to Douglas de Jesus (drd8913@nyu.edu/douglasdejesus92@gmail.com) and Mateo Bonilla
(mjb9333@nyu.edu/mateo.bonilla4@gmail.com). Please consult the authors before doing any modifications.



	