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

## Assembly and Installation

For full assembly, installation, wiring, software setup, and troubleshooting instructions, please refer to the project assembly guide.

Arm Assembly Guide.pdf

---

## CAD Files

All CAD files for the project are stored in the Google Drive folder linked below:

[Team 204 CAD Files Google Drive Folder](https://drive.google.com/drive/folders/1xANLljKQt4pyda8Gyb92bH3nKtEWIrun?usp=drive_link)

The CAD folder includes the design files for the custom mechanical components used in the wheelchair robotic arm system, including mounting parts, controller components, quick-swap parts, and end-effector designs.

---

## Repository Contents

The repository currently contains the main software files, material list, and project documentation.

| File                        | Description                                                                                                                                                                     |
| --------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Joystick_Control.ino`      | Arduino code for operating the robotic arm using the joystick-based controller. This supports manual control, menu interaction, and control of the arm and gripper.             |
| `Mouse_Control.ino`         | Arduino code for controlling the robotic arm using a computer mouse as an alternative input method. This demonstrates the system’s ability to support multiple control devices. |
| `RoArm-M2_With_Gripper.zip` | Project code and related files for operating the Waveshare RoArm M2-S with the modified gripper setup. This file is used when uploading code to the RoArm-M2-S.                 |
| `Kit Material List.xlsx`    | Bill of materials for the project. This spreadsheet lists the parts, materials, and components needed to reproduce the robotic arm system.                                      |
| `README.md`                 | Provides an overview of the project, explains the current repository contents, and directs users to the assembly guide and CAD file folder.                                     |

---

## Basic Operation

After the system has been assembled and the code has been uploaded, the robotic arm can be operated through the custom controller.

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

The system includes interchangeable end-effectors that attach to the wrist of the robotic arm using a quick-swap mechanism. The CAD files for these end-effectors are included in the linked Google Drive CAD folder.

### Off-the-Shelf Gripper

The gripper is used for general object handling tasks such as lifting small objects, pressing buttons, holding cups, and interacting with lightweight items.

### Bin-and-Brush Attachment

The bin-and-brush attachment was designed for service animal interaction. It can assist with ball placement and brushing.

### Umbrella Holder

The umbrella holder secures an umbrella handle so the user can remain protected from rain without manually holding the umbrella.

### Blank End-Effector

A blank end-effector template is included so future users or teams can design additional attachments while staying compatible with the quick-swap system.

---

## Notes for Future Development

Future teams and contributors can build on this project by:

* Improving weather resistance
* Adding new end-effectors
* Expanding control options
* Improving software usability
* Adding more pre-programmed functions
* Refining the mounting system for different wheelchair models
* Improving long-term maintenance and user support documentation

---

## Project Purpose

This repository is intended to make the wheelchair robotic arm project easier to reproduce, modify, and improve.

The project was created to provide a lower-cost alternative to many existing wheelchair robotic arm systems. By keeping the project open source, future teams and users can continue improving the design, adding new end-effectors, expanding control options, and developing new functions that support people with DMD and other mobility-related needs.

---

## Acknowledgements

Team 204 would like to thank Parent Project Muscular Dystrophy for sponsoring and supporting this project.

We would also like to thank the project advisors, sponsor representatives, and previous wheelchair robotic arm team members whose work helped guide the development of this system.
