ENCE 361 Lab A03 Group 2


Authors of this project:

Jed McDermott
Thavy Thach
Gabriel Lake


Project files and layout:

Grading: on Git lab, ence361-2018/lab_A03_group_2/for-grading

The dependencies folder contains resources used by the source code,
including the Orbit OLED library folder (OrbitOLED) and a standard library (ustdlib.c). These
were not modified and are not intended for grading, but they are included because they are 
necessary resources. The source folder contains all code written by the authors to meet 
project requirements. All of the source folder code is meant for grading. 


Project layout:

Source code is separated into C files based on the task or related tasks that the code should perform. 
All included libraries, macros, and function prototypes  (except for buttons4.c and circBufT.c) may be found
in the header file Helicopter.h. Therefor all source files include Helicopter.h. Code is executed using round
robin architecture in main.c, in addition to interrupt driven tasks, found in Helicopter.c and heliYaw.c. 


Thank you to the ENCE 361 course coordinators, instructors, teaching assistants, and developers who
helped organize this project. 

Demo:
![Demo](https://github.com/thavythach/Embedded-Helicopter/blob/master/helicopter.gif?raw=true)
