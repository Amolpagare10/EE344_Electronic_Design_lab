# Graphical User Interface (GUI) Overview

Our GUI allows users to configure the **transmitter** and **receiver** setup of the transducers.

- Once the desired configuration is selected, users can **start transmission and reception** of ultrasonic signals.
- The results are then visualized on the GUI using a **HeatMap**.
- Currently, the **HeatMap is based on a pipe algorithm**, as the **Damage Index algorithm for plates** is still under study.
- For demonstration purposes, the pipe-based visualization will be used.

---

# User Manual – Key Instructions

## 1. Sensor Placement
- Mount piezoelectric sensors firmly on the plate following a planned layout.
- Maintain consistent spacing and ensure solid mechanical coupling.
- Clearly label each sensor for easy identification when assigning Tx/Rx roles.

## 2. Power Connection
- Plug the system into a **220V, 15A grounded socket**.
- Avoid using damaged plugs or loose/multi-plug extensions.
- This power connection supports the signal generation and data acquisition modules.

## 3. Wi-Fi Connection
- Connect your computer to the **Wi-Fi network** broadcast by the control box.
- No internet is needed — this is a local wireless connection.
- Ensure that firewall or antivirus settings do not block the communication port.

## 4. Sensor Configuration in Application
- Launch the **SHM (Structural Health Monitoring)** software.
- Assign transducers as **Transmitters (Tx)** and **Receivers (Rx)** via the interface.
- Choose sensor pairs that offer optimal coverage for the structure being analyzed.

## 5. Data Reception & DI Map Generation
- After configuration, signal data will be transmitted and collected.
- Once data is received, click the **orange button** in the GUI to generate the **Damage Index (DI) Map**.
- The DI Map highlights potential damage zones based on signal response characteristics.

---
