# Enclosure Design and Construction
We had a simple enclosure design for our project, which was primarily focused on housing the electronics and providing access to the necessary ports. 

## Design Software
- The enclosure was designed using **Autodesk Fusion 360**, which provided a 3D parametric design environment.
- The final laser-cut layout was exported as a **DXF (Drawing Exchange Format)** file, the industry-standard format for 2D cutting paths.

---

## Box Construction Details

- **Material**: The panels were cut from **5 mm thick clear acrylic sheets**, offering a good balance between strength and aesthetics.
- **Structure**: The enclosure consists of **six interlocking panels** — top, bottom, front, back, and two sides.
- **Joinery**:
  - Each panel has precisely aligned **notches and tabs**, allowing them to fit together like puzzle pieces without additional fixtures.
  - These interlocks reduce the need for mechanical fasteners and improve ease of assembly.
- **Assembly**:
  - The panels are joined using **hot glue**, which offers strong adhesion while being easy to apply.
  - The enclosure is lightweight, transparent for visual inspection, and sturdy enough to house electronics safely.

---

## Custom Cutouts and Access Ports

Each panel was carefully designed with cutouts for interfacing components and ease of access:

- **Back Panel**:
  - Includes a circular slot for the **power supply input**.
  - Ensures that the AC-DC SMPS module or USB-powered units can be neatly connected without internal clutter.

- **Side Panel (Control Interface Panel)**:
  - Features a precise cutout for the **dsPIC33AK128MC106 microcontroller board** so that its USB/debug port remain externally accessible.
  - **Below the microcontroller**, there are **eight circular holes** which allow the connections to the **transducer connectors**.
    - These connectors interface with external **piezoelectric sensors**, and the holes ensure that cables can be plugged in directly without disassembling the box.
