# Package Guy - UEFI_Game
Low-level implementation of an arcade game written in C that runs directly as a native UEFI application (no OS required).

![Game Screenshot](/media/ss.png)

## About the project
The application demonstrates the practical use of the UEFI specification for creating system software. The game runs as a native boot application (`.efi`), taking full control of the hardware before the operating system loads.

The player controls a blue square, collecting coins while avoiding moving enemies.

There are two game modes available:
- Standard - positions of enemies and coins are predetermined
- Random - positions of enemies and coins are random

### Key System Mechanics:
* **Direct Framebuffer Access:** Pixel-by-pixel graphics rendering via direct writes to video memory (`EFI_GRAPHICS_OUTPUT_PROTOCOL`).
* **Hardware RNG:** Random generation of the board layout and enemies using the platform's hardware random number generator (`EFI_RNG_PROTOCOL`).
* **Asynchronous Events:** Smooth enemy movement independent of player actions, implemented using cyclic timer events (`EVT_TIMER` from UEFI Boot Services).
* **Low-level Input:** Hybrid keyboard handling – a blocking mode in the menu and active polling during gameplay (`EFI_SIMPLE_TEXT_INPUT_PROTOCOL`).

## Technologies and Tools

* **Language:** C
* **Library:** `gnu-efi`
* **Environment:** Ubuntu / WSL (Cross-compilation for x86_64)
* **Testing:** QEMU + UEFI firmware (OVMF)

### Requirements
Make sure you have the necessary packages installed:
```bash
sudo apt install gcc binutils gnu-efi qemu-system-x86 ovmf
