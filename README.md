# Water Pen

This is a simple program for the ATtiny85 microcontroller to monitor
potted plants. The form factor is meant to be small, like a pen.

## How it works

The device is first calibrated by pressing the button for the device.
Position the soil sensor in the dry air, the calibration LED (green LED) will
display for 3 seconds, then it will take an average of the air samples. The
calibration light will display again for 3 seconds, move the soil sensor to
a cup of water. Once the calibration LED is off it will take the average of
the water samples. The calibration LED will do a quick flash at the end to let
you know it is done.

After the device has been calibrated turn the potentiometer to the desired
threshold percentage of when the Alert LED (red LED) should turn on to signify
the plant needs to be watered.

Place the device in the potted plant. From here, the device reads the soil
moisture once every hour.

The device puts itself into sleep mode during this hour to save on battery life.
Need to figure out projected battery life, but shooting for at least 6 months.

## Hardware

This section is still in TODO.

Plans so far:
- ATtiny85
- Capacitive Soil Moisture Sensor
- Potentiometer
- Button
- RGB LED
- Battery is CR2032
- Step up voltage converter
- Small PCB design
- 3D printed case

## Project Structure

- `src` The source code for the project.
- `install_deps.sh` Run this to install all the tools needed to build and flash AVR projects.
- `Makefile` Contains commands to build and burn the AVR project.
- `compile_flags.txt` Contains setup flags to work with the C LSP (clangd)

## USBTiny

If using USBTiny programmer on linux you may have to run the upload command with sudo because
of permission issues.


## Setup in WSL

If you are trying to develop in WSL you need to forward the USB device onto the WSL environment.

1. Start by downloading this utility program `winget install --interactive --exact dorssel.usbipd-win`

2. Look for the USB port the arduino is using.

```bash
$ usbipd list
```

3. Bind the USB port

```bash
$ usbipd bind --busid <port> # example would be 1-2
```

4. Attach to WSL environment

```bash
$ usbipd attach --wsl --busid <port>
```

5. Verify the USB port on WSL.

If you do not have lsusb find whatever your system uses.

This will list the USB ports you have access to and the arduino port should be in the list.

```bash
$ lsusb
```

Once done you can just detach the USB port.

