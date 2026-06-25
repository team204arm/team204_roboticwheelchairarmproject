# Team 204 Wheelchair Robotic Arm Project

Welcome to Team 204’s Wheelchair Robotic Arm Project, sponsored by Parent Project Muscular Dystrophy (PPMD).

This project was developed as part of the Virginia Tech Mechanical Engineering Senior Capstone program. The goal of the project is to create a more affordable, accessible, and open-source assistive robotic arm system for powered wheelchair users, especially individuals living with Duchenne Muscular Dystrophy (DMD).

The system is built around the Waveshare RoArm M2-S and includes a custom wheelchair mounting structure, user controller, interchangeable end-effectors, battery system, and Arduino/ESP32-based software. The arm is designed to support everyday tasks such as pressing elevator buttons, bringing a cup toward the user, lifting small objects, and interacting with service animals.

By making this project open source, Team 204 hopes future students, developers, users, and caregivers can reproduce the system, improve the design, and add new functions that further support independence and accessibility.

---

## Project Overview

The wheelchair robotic arm system includes several major subsystems:

* A Waveshare RoArm M2-S robotic arm
* A custom wheelchair mounting structure
* A user controller with joystick, screen, buttons, and USB input
* Interchangeable end-effectors for different daily tasks
* A battery system for portable use
* Arduino/ESP32-based control software
* Open-source documentation for assembly, testing, and future development

The system supports both manual and automatic control. Users can move the arm directly through the controller or select pre-programmed motions such as stowing the arm, bringing a cup closer, or pressing an elevator button. The project also supports custom saved motions, allowing users to record and replay personalized arm movements.

---

## Installation Instructions

> **Note:** An external device, such as a laptop, is required to transfer code to the RoArm-M2-S.

---

## 1. Download the Project Files

1. Navigate to the project GitHub repository on an external device:

   [Team 204 Wheelchair Robotic Arm GitHub Repository](https://github.com/team204arm/team204_roboticwheelchairarmproject)

2. Download the following file:

   `RoArm-M2_With_Gripper.zip`

3. Extract the ZIP file to a convenient location on your computer.

---

## 2. Install Arduino IDE

1. Download the latest version of Arduino IDE from the official Arduino website:

   [Arduino Software Downloads](https://www.arduino.cc/en/software)

2. Follow the installation prompts.

3. During installation, allow any driver installations if prompted.

These drivers help your computer communicate with the microcontrollers used in the project.

---

## 3. Install the USB-to-UART Driver

The RoArm-M2-S communicates with the computer through a USB-to-UART bridge. This driver is required so the computer can detect the device and upload code properly.

1. Navigate to the Silicon Labs CP210x driver download page:

   [Silicon Labs CP210x USB-to-UART Driver Downloads](https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads)

2. Download the driver compatible with your system.

3. For Windows, the recommended driver is:

   `CP210x Windows Drivers`

4. Install the driver.

5. Restart your computer if prompted.

---

## 4. Set Up ESP32 Board Support

The RoArm-M2-S uses an ESP32-based controller, so Arduino IDE must be configured to recognize ESP32 boards.

1. Open Arduino IDE.

2. Navigate to:

   `File > Preferences`

3. Find the field labeled:

   `Additional Boards Manager URLs`

4. Add the following ESP32 board URL:

   `https://dl.espressif.com/dl/package_esp32_index.json`

5. Click **OK**.

6. Download the ESP32 development package provided for this project:

   [ESP32 Development Package](https://drive.google.com/file/d/1x9CRaSx1YjcPY6Ak4zT6IPjs87vRPhne/view?usp=sharing)

7. Unzip the downloaded package into the Arduino IDE packages folder.

   On Windows, this folder is typically located at:

   `C:\Users\<User Name>\AppData\Local\Arduino15`

8. Ensure the version matches `2.0.11` for the RoArm-M2-S.

9. Restart Arduino IDE.

10. When Arduino IDE prompts you to update your boards, click **Yes**.

11. Verify that the version in the folder structure above starts with `3.XX.XX`.

---

## 5. Install the Required Libraries

The project depends on several Arduino libraries required for the RoArm-M2-S code.

1. Download all necessary libraries for the RoArm-M2-S from the General-Libraries folder:

   [General-Libraries Folder](https://github.com/anjali-vishwakarma-vt/PPMD-VT-Senior-Design/tree/main/General-Libraries)

2. Navigate to your Arduino folder:

   `C:\Users\<username>\Documents\Arduino`

3. If a `libraries` folder does not already exist, create one.

4. Add the libraries downloaded from the General-Libraries folder into the `libraries` folder.

5. Open Arduino IDE.

6. Navigate to the Libraries tab and search for:

   `ArduinoJson`

7. Install the ArduinoJson library by Benoit Blanchon.

8. Use version `7.X.X` or newer. If problems occur, install version `7.4.3`.

9. If Arduino IDE asks to update your libraries, ignore the message unless your team has confirmed the newer versions are compatible.

---

## 6. Connect the RoArm-M2-S

1. Connect the external device to the RoArm-M2-S using a USB cable.

---

## 7. Find the COM Port on the RoArm-M2-S

1. Search for:

   `Device Manager`

   in the Windows search bar.

2. Open Device Manager.

3. Expand:

   `Ports (COM & LPT)`

4. Find the port labeled:

   `Silicon Labs CP210x`

5. Take note of the COM port number.

---

## 8. Select the Board and Port in Arduino IDE

1. Open Arduino IDE.

2. Select the COM port found in Device Manager:

   `Tools > Port`

3. Select the board:

   `ESP32 Dev Module`

---

## 9. Verify and Upload the Code

1. Open the RoArm-M2-S project code in Arduino IDE.

2. Click **Verify** to compile the code.

3. If the code verifies successfully, click **Upload**.

4. Wait until the upload is complete.

5. Once the upload is complete, unplug the external device from the RoArm-M2-S.

---

## Basic Operation

After the code has been uploaded, the system can be operated through the custom controller.

The controller includes:

* A display screen for menu navigation and user feedback
* A joystick for manual movement and menu control
* A freeze button to stop arm movement
* A mode button to switch between control modes
* A USB-A input for alternative control devices

The arm supports both manual control and automatic functions. Manual control allows the user to directly move the arm and gripper. Automatic mode allows the user to select stored motions from the menu.

---

## Main Functions

The robotic arm includes several built-in functions.

### Manual Control

Manual control allows the user to move the robotic arm directly using the joystick or an alternative control device.

### Gripper Control

Gripper control allows the user to control the wrist and gripper for object handling tasks.

### Stow Function

The stow function moves the arm into a safe travel position. This should be used before transporting the wheelchair or turning off the system.

### Bring Cup Function

The bring cup function helps move a cup toward the user. The function uses shared control, where the arm performs part of the motion and the user completes alignment.

### Elevator Press Function

The elevator press function assists the user with pressing elevator buttons. The arm extends forward, the user aligns the end-effector, and the arm returns to a stowed position after the button press.

### Custom Functions

The system allows users to record, store, rename, and replay up to three custom functions. These can be used for repeated tasks chosen by the user.

---

## End-Effectors

The system includes interchangeable end-effectors that attach to the wrist of the robotic arm using a quick-swap mechanism.

### Off-the-Shelf Gripper

The gripper is used for general object handling tasks such as lifting small objects, pressing buttons, holding cups, and interacting with lightweight items.

### Bin-and-Brush Attachment

The bin-and-brush attachment was designed for service animal interaction. It can assist with ball placement and brushing.

### Umbrella Holder

The umbrella holder secures an umbrella handle so the user can remain protected from rain without manually holding the umbrella.

### Blank End-Effector

A blank end-effector template is included so future users or teams can design additional attachments while staying compatible with the quick-swap system.

---

## Troubleshooting Notes

If the board does not appear under `Tools > Port`, reinstall the CP210x USB-to-UART driver.

If the upload fails, check that the correct ESP32 board package is installed.

If Arduino shows missing library errors, make sure all required libraries from `General-Libraries` were copied into the Arduino libraries folder.

If ArduinoJson errors occur, confirm that ArduinoJson by Benoit Blanchon is installed with version `7.X.X` or newer. If problems continue, try version `7.4.3`.

If the arm does not respond after upload, check the USB connection, power connection, selected board, and selected port.

Restart Arduino IDE after installing drivers, board packages, or libraries.

Use the tested package versions whenever possible, since newer software versions may not always be compatible with older ESP32 code.

---

## Project Purpose

This repository is intended to make the wheelchair robotic arm project easier to reproduce, modify, and improve.

The project was created to provide a lower-cost alternative to many existing wheelchair robotic arm systems. By keeping the project open source, future teams and users can continue improving the design, adding new end-effectors, expanding control options, and developing new functions that support people with DMD and other mobility-related needs.

---

## Acknowledgements

Team 204 would like to thank Parent Project Muscular Dystrophy for sponsoring and supporting this project.

We would also like to thank the project advisors, sponsor representatives, and previous wheelchair robotic arm team members whose work helped guide the development of this system.
