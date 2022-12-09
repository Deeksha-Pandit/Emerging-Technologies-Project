# Emerging-Technologies-Project








EMERGING TECHNOLOGY
PROJECT REPORT 
BY:
DEEKSHA PANDIT
MEGHNA PM
SHIVANGI DUBEY










Motivation and Problem Definition:
As roommates, we frequently argue over little things like who will do the cooking, cleaning, and grocery shopping. But our main disagreement is about who will switch on and off the fan at home.
We therefore made the decision to take advantage of this chance and apply the lessons learned from this course to create a gadget employing our M5Sticks that could finally solve our dilemma.
Lately Austin’s temperature has been very unpredictable. So, at times after switching on the fan, the temperature goes down and the apartment becomes very cold. In order to overcome this challenge, we came-up with a solution using our M5Sticks.

State of the Art:

The above device is used to get the temperature data which is then used to automate the turning on/off the fan.
This device costs around $300 whereas our devices together cost around $90-$100 which is very cost effective.
Design:
Our project requires 3 M5Sticks. One of the M5Sticks is mounted to a temperature sensor which will measure the temperature of the room. We are using an ENV III Hat which is a multifunctional environmental sensor adapted to the M5StickC series and used to detect temperature, humidity, and atmospheric pressure data. For our project, if the temperature exceeds a certain threshold (say 72 degrees) then a signal will be sent (via BLE) to the second M5Stick. Our project requires rapid collection and detection of environmental data, ENV-III HAT is a balanced performance, good choice with cost performance  as it has high reliability and long-term stability.
The second M5Stick will have a servo motor with a fan attached to it and so when the signal is received from M5Stick 1 the motor will switch on/off the fan. For this, we are using the SERVO HAT which is a servo motor module that comes with a 145°±10° range of motion. The motor also has capabilities to regulate the temperature and set it according to the ambient temperature. When the temperature is too high i.e above the set threshold, the servo motor will move. When the temperature is high and the fan rotates,  there will be a notification sent to the third M5stick using BLE (Bluetooth Low Energy).
Lastly, all the data will be collected and stored in the third M5Stick which will then send the data to store and display it on a M5Stick so that the temperature readings can be monitored, analyzed and utilized for calculating the energy consumption at our apartment and calculating our utility bills. For this we are using SPIFFS (SPI Flash File Storage) which is used for storing larger data in the form of files. SPIFFS is a small SD Card onboard the M5Stick itself. By default, about 1.5 MB of the onboard flash is allocated to SPIFFS. 


Assessment:
Looking back at the project, we were able to achieve most of the things that we planned
We faced difficulty trying to figure out how to connect the three M5Sticks together but then eventually we worked it out by treating the first M5Stick as a client for the second M5Stick and then the second M5Stick acting as a client for the third M5Stick.
However we had to make a few changes to our project here and there for example, we initially decided to use EZ data but we got a few errors while implementing it so we decide to switch to SPIFFS which stores data in flash memory
If we were to continue working on this project, we would probably work on saving the data on cloud and displaying it on a dashboard for easy accessibility

Appendix B:

Deeksha Pandit
Worked on the code for the temperature sensor and forwarded the data to the storage M5Stick. The M5Stick with the temperature hat measures the temperature of the surrounding and displays the data. Deeksha was alo responsible for sending this data to the second M5Stick with the servo motor attached. 

Meghna PM
Worked on the code for servo motor. She was responsible for reading the data coming from the first M5Stick and turning the fan/off accordingly. We set a threshold temperature. If the temperature is mor than the threshold the fan will be switched on otherwise it would be switched off. 

Shivangi Dubey
Worked on the code for data storage and display using SPIFFS and taking the information from the temperature sensor M5Stick. Shivangi was responsible for reading and storing the data coming from the second M5Stick using SPIFFS which is SPI Flash file storage. She worked on the code to write the data to a file and then read it from that file and display on the third M5Stick.


