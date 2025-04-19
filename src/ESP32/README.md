# Firmware Overview

This project involves two main microcontrollers — the **dsPIC33AK128MC106** and the **ESP32**, working in synchrony to perform data acquisition and communication over WiFi.

---

## ESP32 WiFi Module (Arduino-based)

The **ESP32** microcontroller acts as the bridge between the **GUI** and the **dsPIC**, enabling **wireless communication** over WiFi. It is programmed using the **Arduino IDE**.

### `wifi_code.ino`

- Sets up the **WiFi connection** (SSID, password, and network config).
- Listens for incoming requests (from PC GUI or smartphone).
- Forwards received commands to the dsPIC via **UART interface**.
- Acts as a transparent relay to support wireless control of the dsPIC.
  
---

## System Overview Diagram

    [GUI] ← WiFi → [ESP32] ←UART→ [dsPIC33AK128MC106]
                                      |
                  ┌───────────────────┼────────────────────┐
                  ▼                   ▼                    ▼
              [Multiplexer]     [Electronic Switch]     [ADC/DAC]
