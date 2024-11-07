# AVR Setup

This project is to have a base project that explains the setup process to develop for AVR chips.

## Project Structure

- main.c The blink project
- install_deps.sh Run this to install all the tools needed to build and flash AVR projects.
- Makefile Contains commands to build and burn the AVR project.
- compile_flags.txt Contains setup flags to work with the C LSP (clangd)

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

