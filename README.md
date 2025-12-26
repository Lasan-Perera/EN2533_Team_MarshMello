# Team MarshMello 
## Module: EN2533 Robot Design and Competition

## 📖 Project Overview
Our robot was developed to complete multiple challenges announced two months before the competition. These challenges included line following, maze solving, box handling, ramp climbing, dashed-line navigation, barcode detection, ball pickup, wall following, arrow-based navigation, and ball shooting. The project required significant hardware and software development, and the final robot was able to autonomously navigate the arena and successfully complete the assigned tasks.

## 🤖 Robot

## 🔧 Robot Hardware Specifications
- **Max Dimensions**: 250mm × 250mm  
- **Power Supply**: 4 18650 Li-ion Battries
- **Components**:
  - Arduino Mega mini Development board x1
  - Raykha S8 – TCRT5000 based 8-channel reflective sensor array x1
  - Nema 17 Stepper Motors x2
  - DRV8825 Stepper Driver x2
  - LM2596 DC-DC Buck Converter x2
  - XL4015 DC-DC Buck Converter x1
  - MG90 metal gear servo motors x3
  - MG995 Tower Pro servo motor x1
  - SHARP GP2Y0A51SK0F Infrared IR Sensor x5
  - TCS34725 RGB Colour Recognition Sensor x1
  - Obstacle Avoidance Sensor IR Infrared Module x3
  - DC Motor 380 Motor x2
  - Rotary Encoder Module with Push Button
  - 0.96″ OLED display (debugging)

## 💻 Robot Software Specifications
- **Programming Language**: C++
- **IDE**: Arduino IDE
- **Libraries**:
  - Arduino.h
  - Servo.h
  - Adafruit_TCS34725.h
  - MultiStepper.h
  - AccelStepper.h
  - Wire.h
  - Adafruit_GFX
  - Adafruit_SSD1306
- **3D Modelling**: AutoDesk Fusion
- **Schemetic Design**: Altium Designer

![VS Code](https://img.shields.io/badge/VS_Code-0078d4?logo=visualstudiocode&logoColor=white)
![Arduino](https://img.shields.io/badge/Arduino-00979D?style=flat&logo=arduino&logoColor=white)
![Arduino CLI](https://img.shields.io/badge/Arduino_CLI-00979D?style=flat&logo=arduino&logoColor=white)  
![PlatformIO](https://img.shields.io/badge/PlatformIO-1E2D3B?style=flat&logo=platformio&logoColor=white)
![Altium Designer](https://img.shields.io/badge/Altium_Designer-FF0000?style=flat&logo=altium&logoColor=white)
![Autodesk Fusion 360](https://img.shields.io/badge/Autodesk_Fusion_360-1795FC?style=flat&logo=autodesk&logoColor=white)


## ⚙️ Mechanisms

### Gripper Mechanism
The gripper mechanism is designed for efficient box manipulation. It uses two servo motors—one for vertical lifting and one for gripping. The lifting action prevents box sliding and collisions on uneven terrain.  

A color sensor detects line color, while a Time-of-Flight sensor tower (three sensors) measures box height, distance to boxes, and portal status. This sensor integration ensures accurate handling and navigation.



### Coin Dropper Mechanism
The coin dropper uses a servo-driven rotating holder that releases a coin when aligned with a hole in the base.  

In the final task, the robot detects a black cross on uneven terrain and drops the coin precisely at the marked location, ensuring reliable placement.


## 🚩 Competition Tasks
- **Task 1**: The Labyrinth of Shadows (Grid Solving) 
- **Task 2**: The Ramp of Trials (Creation, Ascending, and Descending Ramp)
- **Task 3**: The Oracle of Runes (Barcode Reading)
- **Task 4**: The Circular Fortress (Wall Following and Ball Collection)
- **Task 5**: The Wall of Whispers & Pendulum of Doom 
- **Task 6**: The Line of Destiny & The Ring of Triumph (Shooting Task)

[![PDF](https://img.shields.io/badge/PDF-Download-red?style=flat&logo=adobe&logoColor=white)]([path/to/your-file.pdf](https://github.com/user-attachments/files/24347659/Robot_Design_Task_2025_251226_191908.pdf)) **Project Documentation**


## 🏟️ Arena
<img width="792" height="816" alt="image" src="https://github.com/user-attachments/assets/0292d8f1-3821-4599-a5c1-895216d76922" />


## 🏆 Achievements
- Successfully completed **6 out of 8 tasks** within the time limit  


## 💡 Testing Issues & Innovative Fixes

### Reverse Line Following Challenge
**Problem:**  
Encoder-based reverse navigation was inaccurate due to motor mismatch. The forward-mounted IR array also caused delayed feedback during reverse motion.

**Solution:**  
Two IR arrays were used—one at the front and one at the rear—allowing accurate and immediate feedback in both directions while reducing software complexity.

### IR Sensor Isolation Challenge
**Problem:**  
IR sensor interference occurred when detecting black lines on white backgrounds due to reflected IR rays.

**Solution:**  
A custom SolidWorks-designed cover was used to isolate each IR receiver, ensuring accurate perpendicular reflection detection.

## 👥 Group Members
| Name | Index Number | Email |
|------|-------------|-------|
| Balasooriya B A P I | 220054N | balasooriyabapi.22@uom.lk |
| Liyanage D L B B | 220362G | banuka2002liyanage@gmail.com |
| Pathirana P D R O | 220448C | oshadha1619@gmail.com |
| Fernando A R D | 220161N | rusirufernando513@gmail.com |
| Dineshara M C | 220128V | chandupadineshara@gmail.com |

