# PCB Design Issues:

- Our SMD INA on the PCB was not working. We replaced it with another SMD INA, but it still didn't work. Since we didn’t have any more SMD INAs, we used a DIP INA on a breadboard and connected it to the PCB — and it started working.

- In the ADC, we grounded the CMLP pin (pin 11), as the datasheet mentioned that for DC coupling, it should be grounded. However, during breadboard testing, we found that it worked when connected to 5V instead.

- Even after all this, the ADC on the PCB is still not working. We tried to figure out the issue by checking if the clock is working, if we are receiving data at the ADC, and if the power supply to the ADC is correct. We found that everything appears to be in order, yet we are still not receiving any data from the ADC.

# Purpose of the PCB:

- In our project, almost everything relied on the PCB. We had to use components like the DAC, TL072, power amplifier, electronic switch, multiplexer, INA, and ADC — all of which required reliable and organized connections. Using a PCB was the best choice to ensure clean and consistent wiring.

- Moreover, we found that small mistakes on the breadboard made debugging very cumbersome. In contrast, the PCB allowed us to make all the connections properly and verify them once. After the PCB arrived and we tested it, we could trust the setup and focus on the actual performance of the circuit.

# Explanation of PCB connections 

- Our microcontroller was connected to the DAC to convert digital data into analog signals. The signal was then passed through a TL072 and a power amplifier to amplify it. Next, an electronic switch was used to select which transducer would transmit the signal.

- After transmission, a multiplexer (MUX) was used to select the transducer that would receive the signal. The received signal was then passed through an INA and a clipper — the INA amplified the signal, while the clipper limited high voltages to protect the circuit.

- Finally, the processed analog signal was fed into an ADC to convert it back into digital form, which was then sent to the PC for post-processing.
