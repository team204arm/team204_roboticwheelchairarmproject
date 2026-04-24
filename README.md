# Installation Instructions
Note: An external device such as a laptop is required to transfer code to RoArm-M2-S
1.	Navigate to https://github.com/anjali-vishwakarma-vt/PPMD-VT-Senior-Design on an external device.  
  a.	Download RoArm-M2_With_Gripper.zip  
2.	Download the latest version of Arduino IDE from the Arduino website and follow the installation prompts. During installation, allow any driver installations if prompted. 
3.	Navigate to https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads and download the driver compatible with your system (recommended driver for Windows: CP210x Windows Drivers)
4.	Set up ESP32 Board Support  
  a.	Open Arduino IDE, then navigate to File  Preferences  
  b.	Add the ESP32 board URL https://dl.espressif.com/dl/package_esp32_index.json to the Additional Board Manager URLS  
  c.	Download the ESP32 development package under https://drive.google.com/file/d/1x9CRaSx1YjcPY6Ak4zT6IPjs87vRPhne/view?usp=sharing and unzip it into the Arduino IDE packages folder, typically located under “C:\Users\(User Name)\AppData\Local\Arduino15”. Ensure the version matches 2.0.11 for RoArm-M2-S.  
  d.	Restart Arduino IDE and when it prompts you to update your boards hit yes and then verify that your version in the folder structure above starts with a 3.XX.XX  
5.	Download all necessary libraries for RoArm-M2-S. These are found on our GitHub, linked under https://github.com/anjali-vishwakarma-vt/PPMD-VT-Senior-Design/tree/main/General-Libraries  
  a.	Navigate to “C:\Users\username\Documents\Arduino” In your files and add (if you don’t have this file already) libraries folder  
  b.	Add the libraries that were downloaded to the folder linked above  
  c.	Open Arduino IDE navigate to the libraries tab and search for: “ArduinoJson” this library is by: Benoit Blanchon install this Library with the version: 7.X.X+ (If problems occur download version 7.4.3)  
  d.	When Arduino asks to update your libraries (If these are your only libraries) ignore that message and don’t update the libraries  
6.	Connect the external device to RoArm-M2-S with a USB wire.  
7.	Find The COM Port on the Ro-Arm-M2-S.  
  a.	Search for “device manger” in the windows search bar  
  b.	Expand the tab for “Ports (COM & LPT)”  
  c.	Take note of the COM port number that the “silicon Labs CP210x” is on  
8.	Within Arduino IDE, select the COM port from above and then select “ESP32 Dev Module” as your board  
9.	Verify code
10.	Upload code
11.	Once upload is complete, unplug the external device. 
