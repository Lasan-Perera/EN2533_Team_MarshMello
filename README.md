# Team MarshMello 
## Module: EN2533 Robot Design and Competition

## 📖 Project Overview
Our robot was developed to complete multiple challenges announced two months before the competition. These challenges included line following, maze solving, box handling, ramp climbing, dashed-line navigation, barcode detection, ball pickup, wall following, arrow-based navigation, and ball shooting. The project required significant hardware and software development, and the final robot was able to autonomously navigate the arena and successfully complete the assigned tasks.

## 🤖 Robot

## 🔧 Robot Hardware Specifications
- **Max Dimensions**: 200mm × 190mm  
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


## ⚙️ Mechanisms and Algorithms
### Custom Motor Brackets, Wheel Rims, SHARP IR Brackets
<img src="https://github.com/user-attachments/assets/89c14042-1c5d-4e6c-a0d6-823893c8174a" width="300" height="300" />
<img src="https://github.com/user-attachments/assets/22e6be7a-fbba-4adf-b9f5-4d7d07c4fee7" width="300" height="300" />

### Balls and Box Pickup Mechanism

From the beginning, we decided to use an arm mechanism to pick up balls and boxes from the side. This required precise control of the robot. We planned to store the collected balls in a container located at the top of the robot. The container had to be positioned at an appropriate height so gravity could be used to feed the balls into the ball-sending mechanism, which was a major design challenge.

To address this, we used an L-shaped arm. A high-torque servo was used for the main arm movement, while a smaller servo controlled the gripper. This combination significantly simplified the mechanism.

The gripper had to handle both balls (4 cm diameter) and boxes (5 cm wide). To achieve this, I designed a special gripper with an internal cutout. When gripping a ball, the arms move closer than usual, allowing the ball to travel upward through the rail-like cut. Since the color sensor was placed at the top, this design allowed us to take color readings effectively.

However, the initial grip was not strong enough to hold boxes securely. To improve grip, we added hot glue inside the gripper and covered it with a rubber balloon. While this solved the box-gripping issue, it prevented the ball from moving upward.

To overcome this, we implemented a control solution: after gripping the ball and raising the arm, the gripper briefly opens and closes again. This allows the ball to move closer to the color sensor. This approach worked successfully.

<img width="397" height="496" alt="image" src="https://github.com/user-attachments/assets/5aa93e9d-a1d1-4257-a950-2cb823abd457" />

### Balls Storing Mechanism
The container had to be positioned at a suitable height so gravity could feed the balls into the ball-sending mechanism. The main challenge was designing a container within limited space that could store four balls while ensuring they would not block each other during release.

Later, I discovered that the original container design was approximately 5 mm lower than the required height. To fix this, I added a 5 mm thick 3D-printed spacer block in the middle of the structure.

<img width="557" height="398" alt="image" src="https://github.com/user-attachments/assets/efb444ee-13a7-4946-92c8-4741c1c92155" />

### Balls Shooting Mechanism

The ball sender needed to launch the balls through a given hoop. We used DC motors rotating at high speed, with the ball placed between them. To achieve sufficient grip, we used two toy truck wheels attached to the motors.

Finding the correct distance between the motors required several design iterations. Another major issue was ensuring that balls were sent one at a time. If two balls entered the mechanism simultaneously, the second ball would not travel the required distance. To solve this, I designed a blocking mechanism that prevents the next ball from entering while one ball is being launched.

Initially, we attempted to control the motors using relays, but this failed due to the high startup current. When the motors started, the relay would switch off, causing an endless on-off loop without the motors running properly. To resolve this issue, we replaced the relays with a motor driver, which worked reliably.

### Grid Solving Algorithm
The robot operates on a **Grid Navigation System** driven by a finite state machine.

- **Mapping**: Maintains an internal 8x8 coordinate grid that updates in real-time as sensors detect walls or boxes.  
- **Pathfinding**: Uses **Breadth-First Search (BFS)** to calculate the shortest path to any target cell. The path is dynamically recalculated after every step to adapt to newly discovered obstacles.  
- **Mission Logic**: A high-level **State Machine** controls the mission lifecycle:  
  - **Search**: Executes a "Snake Pattern" sweep of the grid to locate boxes.  
  - **Pickup**: When a box is found, uses a **Multi-Angle Selector** to find a valid approach vector (strict X-axis west/east approach) respecting the robotic arm's geometry.  
  - **Transport**: Navigates back to the sanctuary zone using the same BFS algorithm to drop the payload.  
  - **Avoidance**: Automatically marks blocked paths and inaccessible boxes to prevent deadlocks or infinite loops.



## 🚩 Competition Tasks
- **Task 1**: The Labyrinth of Shadows (Grid Solving) 
- **Task 2**: The Ramp of Trials (Creation, Ascending, and Descending Ramp)
- **Task 3**: The Oracle of Runes (Barcode Reading)
- **Task 4**: The Circular Fortress (Wall Following and Ball Collection)
- **Task 5**: The Wall of Whispers & Pendulum of Doom 
- **Task 6**: The Line of Destiny & The Ring of Triumph (Shooting Task)

**Project Documentation**
[![PDF](https://img.shields.io/badge/PDF-Download-red?style=flat&logo=adobe&logoColor=white)]([path/to/your-file.pdf](https://github.com/user-attachments/files/24347659/Robot_Design_Task_2025_251226_191908.pdf)) 


## 🏟️ Arena
<img width="792" height="816" alt="image" src="https://github.com/user-attachments/assets/0292d8f1-3821-4599-a5c1-895216d76922" />

## 👥 Team Members
| Name | Index Number |
|------|-------------|
| Lasan Sanjula Perera | 230487D |
| Hasith Nettikumara | 230436X |
| Kavija Karunarathna | 230322U |
| Hiruna Kariyawasam | 230318M |
| Rusiru Anupama | 230536E |

## 🏆 Achievements
- Successfully completed **5 out of 6 tasks** within the time limit  

<img width="1280" height="960" alt="image" src="https://github.com/user-attachments/assets/961e3777-fb7b-4ed8-8dc9-ac8582a0b2fc" />

|

