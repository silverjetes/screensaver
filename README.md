# screensaver.scr
My new GDI malware made in C - remake of [quantizer.exe](https://youtu.be/2oqcYwFcr9k) by [wipet](https://github.com/whypet)- displaying GDI effects on your screen using layered windows, and even supporting stereo sound generation!
## 🚨 DISCLAIMER 🚨
**This is malicious software made for educational and entertainment purposes only.**

It has full capacity to destroy all data on all connected storage (or even make it unreadable), including the machine's operating system, rendering it unbootable. This software should **NOT** be ran on real hardware, but instead a controlled environment (like a snapshotted virtual machine with shared folders disabled and storing no important data). **It's creator is not responsible for any damage caused by the misuse of this software, and the responsibility lies purely on the end-user.**
It also includes severe flashing lights and disturbing noises, so people with photosensitivity of any kind should **not run this software anywhere at all**.
## 📝 Information
- **Language:** C (+Assembly)
- **Development time:** ~1,5 months
- **Inspiration:** [quantizer.exe](https://youtu.be/2oqcYwFcr9k)
- **Finished:** 1st November 2026

This malware uses layered windows mixed together with normal GDI to create interesting screen effects. Another unique aspect of this malware is stereo bytebeat generation, which was surprisingly easy to implement. And, finally, instead of being an .exe file like basically any other GDI malware, this one uses the .scr file extension, which works absolutely identical to a normal .exe file, but is identified by Windows as a "Screen Saver", which fits well for the malware's name!
I also challenged myself to make my very own **bootloader payload** for this project, which I've never really done before due to my lack of Assembly knowledge. With the help of my good friend, I learned some Assembly, and made my own bootloader payload displaying rainbow visuals made using math, with 9 different formulas available to be randomly chosen on boot!
## 🔑 Registry
There are a few registry values that can be used for screensaver:
- **Key:** `HKEY_CURRENT_USER\Software\screensaver`
- **`Block`**: A DWORD value of 1 will prevent screensaver from executing completely, it will refuse to run and display a message box informing the user that it's blocked.
- **`SafeMode`**: A DWORD value of 1 will prevent screensaver from launching destructive payloads. (**⚠ IMPORTANT!!!** If the message box does NOT say screensaver is running in safe mode, then it is NOT running in safe mode!!! Additionally, the malicious code is still in the binary, so **executing screensaver on real hardware is NOT recommended!!!** ⚠)
- **`DisableLayeredWindows`**: A DWORD value of 1 will disable all layered window payloads in case your machine is not compatible with them. It is important to note that on Windows 10+ this value would disable ALL GDI payloads.
- **`Seed`**: Any non-zero DWORD value will set the randomizer's seed to that value, and therefore all runs with this seed will be the exact same.
## 🧩 Compatibility
- Both x86 and x64 work!
- Windows XP
- Windows Vista and Windows 7 *(DWM will be disabled if not already using a Basic or Classic theme)*
- Windows 8 - Windows 10 before version 2004 *(no layered window support)*
- Windows 10 version 2004+ *(has layered window support but the GDI payloads are limited due to issues with the layered window compatibility method)*
## 🐛 Known bugs
- Lack of layered windows on Windows 8 - Windows 10, as listed in **🧩 Compatibility** *(but there isn't much that can be done about it)*
- Black screen on Windows Vista if running with Aero *(enable a Basic theme manually before running)*
- Sound stopping on modern Windows *(this is caused by registry corruption)*
## 👥 Credits
- RealMoonrise *(SetWindowDisplayAffinity)*
- xdq_0 *(suggestion for the name back in September of 2025)*
## ⚖ License
This project uses the **GPL-3.0** license!

This basically means that if you use this project's code, you **must** credit me, open source your project, and also set the license to GPL-3.0!
