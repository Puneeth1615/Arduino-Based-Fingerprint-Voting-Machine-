# Arduino-Based-Fingerprint-Voting-Machine

This is an Arduino-based fingerprint voting machine project that uses a fingerprint sensor, LCD display, EEPROM, and tactile switches to manage a simple electronic voting system.

> This project is written in **Arduino C++**, not Python. I worked with existing C++ code for this hardware-specific implementation. I plan to port or integrate parts of it into Python using Raspberry Pi in the future.

Features

- Fingerprint-based user authentication
- Voter verification (prevents double voting)
- Vote submission via buttons
- Real-time vote count display
- EEPROM-based persistent storage

How It Works

1. The fingerprint sensor enrolls or verifies users.
2. After verification, voters choose their candidate using physical buttons.
3. The system stores votes and prevents double voting.
4. Results can be displayed on the LCD.

Hardware Used

- Arduino Uno/Mega
- R305/GT511C3 fingerprint sensor
- I2C 16x2 LCD
- Buzzer
- Push buttons
- EEPROM (built-in)
- Jumper wires, breadboard, etc.

Future Plans

- Create a **Python version using Raspberry Pi and GPIO**.
- Possibly rewrite the voting logic and serial communication in Python.
- Add a web-based dashboard for live results using Flask or Streamlit.


