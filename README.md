# 🔌 Monitor Ac Voltage, Current, Power, Energy_wh, threshold power alert and power ON/OFF in Thingspeak website

## 📘 **Project Overview**

This project monitors **AC Voltage**, **Current**, **Power**, and **Energy** using an **ESP32**,
and uploads the data to **ThingSpeak Cloud**.
It also allows **remote relay control** through the ThingSpeak dashboard.

Sensors used:

* **ZMPT101B Voltage Sensor** — for AC voltage measurement 
* **ACS712-30A Current Sensor** — for AC current measurement

---

## 🧰 **Components Required**

| Component                              | Description          | Quantity    |
| -------------------------------------- | -------------------- | ----------- |
| **ESP32 Dev Board**                    | Main microcontroller | 1           |
| **ZMPT101B Voltage Sensor**            | Measures AC voltage  | 1           |
| **ACS712-30A Current Sensor**          | Measures AC current  | 1           |
| **Relay Module (5V)**                  | Switches load ON/OFF | 1           |
| **AC Load (Bulb/Fan/Appliance ≤ 30A)** | Test load            | 1           |
| **Jumper Wires**                       | For connections      | As required |

---

## ⚙️ **Working Principle**

### 🔹 1. Voltage Measurement (ZMPT101B)

* Converts high AC voltage into a small isolated signal using an internal transformer and op-amp.
* The output waveform is centered around **1.65V** (DC offset when Vcc = 3.3V).
* The module includes a multi-turn potentiometer to adjust sensitivity.

**Formula:**
[
V_{AC} = \frac{V_{rms}}{\text{Sensitivity}}
]

#### **🧮 Measuring Sensitivity of ZMPT101B**

1. **Setup the Module**

   * Connect the ZMPT101B to the ESP32 (OUT → GPIO 34).
   * Apply a known AC voltage (e.g., 230V).

2. **Measure Output Voltage**

   * Read analog output using `analogRead(VOLTAGE_PIN)` on ESP32.

3. **Adjust the Potentiometer**

   * Turn slowly until the sine wave has **maximum amplitude** without clipping (0–3.3V).

4. **Calculate Sensitivity**

   * Sensitivity = Output Voltage (Vpp) / Input AC Voltage
   * Example:

     ```
     Vout = 1.8V peak-to-peak
     Vin = 230V
     Sensitivity = 1.8 / 230 = 0.007826 V/V
     ```
   * So, final **Sensitivity ≈ 0.0078 V/V**

---

### 🔹 2. Current Measurement (ACS712-30A)

* Uses **Hall Effect** to measure current.
* Output is centered at **1.65V** (zero current).
* Sensitivity = **66 mV/A** for 30A version.

**Formula:**
[
I_{inst} = \frac{(V_{out} - 1.65)}{0.066}
]

Where `Vout` is the measured voltage from the ACS712 output pin.

---

### 🔹 3. Power and Energy Calculation

* **Power (W):**
  [
  P = V_{rms} \times I_{rms}
  ]
* **Energy (Wh):**
  [
  E_{Wh} = P \times \text{Time (in hours)}
  ]

---

### 🔹 4. Relay Control via ThingSpeak

* **Field 5** in ThingSpeak is used for **remote relay control**:

  * `1` → Relay ON
  * `0` → Relay OFF

---

## 🌐 **ThingSpeak Setup**

1. Create a ThingSpeak account.
2. Create a new **channel** and add the following fields:

| Field   | Purpose       |
| ------- | ------------- |
| Field 1 | Voltage       |
| Field 2 | Current       |
| Field 3 | Power         |
| Field 4 | Energy (Wh)   |
| Field 5 | Relay Control |
| Field 6 | Power Alert   |

3. Copy your **Write API Key** and **Read API Key** and paste into your ESP32 code.

---

## 🔌 **Circuit Connections**

| ESP32 Pin | Sensor/Module    | Function      |
| --------- | ---------------- | ------------- |
| 34        | ZMPT101B OUT     | Voltage Input |
| 35        | ACS712 OUT       | Current Input |
| 2         | Relay IN         | Load Control  |
| 3.3V      | Both Sensors VCC | Power         |
| GND       | Common Ground    | —             |

---

## 📄 **Formulas Summary**

| Parameter       | Formula              | Notes                         |
| --------------- | -------------------- | ----------------------------- |
| **Vrms**        | √(Σ(Vinst²)/N)       | Remove offset before squaring |
| **Irms**        | √(Σ(Iinst²)/N)       | Hall effect sensor output     |
| **Power (W)**   | Vrms × Irms          | Instant power                 |
| **Energy (Wh)** | Power × time (hours) | Total energy consumption      |

---
ur README and include diagrams or calculation sections if you want.
