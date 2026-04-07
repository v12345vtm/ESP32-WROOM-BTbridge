# ESP32 Bluetooth-to-UART Full Duplex Bridge

This project creates a transparent, wireless **Bluetooth Classic (SPP)** bridge between two ESP32 modules. It allows you to replace a physical UART cable with a high-speed wireless link.

## 🚀 Features
* **Full Duplex:** Send and receive data simultaneously.
* **115200 Baud:** High-speed communication by default.
* **Hardcoded MAC Addressing:** Instant, reliable connection (no slow scanning).
* **Transparent Bridge:** Whatever goes into the USB/UART of ESP A comes out of the USB/UART of ESP B.

---

## 🛠 Setup Guide

### Step 1: Find the Slave's MAC Address
Each ESP32 has a unique hardware ID. You must identify the Slave's ID so the Master knows who to "call."
1. Upload `btontvangerESP/btontvangerESP.ino` to your **Slave** ESP32.
2. Open the Serial Monitor (**115200 baud**).
3. **Copy the generated line** that looks like in the master ino :  
   `uint8_t address[6] = {0xE8, 0x9F, 0x6D, 0xA8, 0x4E, 0xFE};`

### Step 2: Program the Slave
1. Upload `btontvangerESP/btontvangerESP.ino` to the same ESP32.
2. This device will now sit and wait for a connection from the Master.

### Step 3: Program the Master
1. Open `btzenderESP/btzenderESP.ino`.
2. **Paste your MAC address line** (from Step 1) into the top of the sketch.
3. Upload to your **second** ESP32.
4. Once powered, the Master will automatically find and lock onto the Slave.

---

 
---
# ESP32 Bluetooth-to-UART Full Duplex Bridge

This project creates a transparent, wireless **Bluetooth Classic (SPP)** bridge between two ESP32 WROOM modules. It allows you to replace a physical UART/USB cable with a high-speed wireless link. 
Tested to send and recieve OBIS P1 meter protokol , at 115200 speed and fine
Distance is very close stay within 4 meters 



## ⚠️ Troubleshooting & Notes
* **Reset both esp32 's:** when starting , maybe clear all old bits and get a fresh new boot start.
* **Serial Monitor:** Ensure your Serial Monitor is set to **115200 baud** and **"Both NL & CR"** to send data correctly.
* **Distance:** Bluetooth Classic on the ESP32 works best within 4 meters. For initial testing, keep the boards roughly 1 meter apart.
* **Power:** If the connection drops frequently, ensure your USB ports provide enough current (Bluetooth can spike power consumption).

## 📡 System Architecture

This bridge operates in **Full Duplex**, meaning both sides can send and receive data at the same time at **115200 8N1**.

```text
      COMPUTER A                                               COMPUTER B
   [Serial Monitor]                                         [Serial Monitor]
          |                                                        |
          | (USB Cable)                                            | (USB Cable)
          v                                                        v
+-----------------------+                        +-----------------------+
|  ESP32 MASTER (WROOM) |                        |  ESP32 SLAVE (WROOM)  |
|                       |      WIRELESS LINK     |                       |
|  [ USB-to-UART ]      |       (BT Classic)     |  [ USB-to-UART ]      |
|         |             |          115200        |         |             |
|         v             |          8N1           |         v             |
|  [ Hardware Serial ]  | <~~~~~~~~~~~~~~~~~~~~> |  [ Hardware Serial ]  |
|         |             |           |            |         |             |
|         v             |           |            |         v             |
|  [ BT Stack/Radio  ]  |           |            |  [ BT Stack/Radio  ]  |
+-----------------------+           |            +-----------------------+
                                    |
                          (Bidirectional / Full Duplex)
---
 
