**How to compile GBARunner3:**

- Install the latest version of **DevKitPRO** from their official repo: https://github.com/devkitPro/installer/releases
- Clone** GBARunner3** repo with submodules in your workspace:
`git clone --recursive https://github.com/Gericom/GBARunner3.git`
- Run `make` inside the `/code` folder, you should get `GBARunner3.nds` inside `/code/bootstrap`.
**NOTE:**Currently only Windows is able to compile, Linux/macOS users may get a fix in the future.

**To run it, you need:**
- `TwilightMenu++ 26.1.1` or higher already installed in your SD card/flashcard.
- Move your previously compiled `GBARunner3.nds` inside `/_nds/TWiLightMenu/emulators` folder in your SD card.
- Get (on your own) retail GBA bios, rename it as `bios.bin` and put it inside the `/_gba` folder in your SD card.
- Get the configs folder from the repository you cloned, copy it inside of the `/_gba` folder, it should be `/_gba/configs`.
- Write `GBARUNNER3_TEST = 1` text line under the `[SRLOADER]` section in `/_nds/TWiLightMenu/settings.ini`
- If you're using a flashcard, compatibility has been fixed, so will be able to run this as well.

You can check the current status on the compatibility list here (contributors are welcome):
https://docs.google.com/spreadsheets/d/1PTf9kW7L3MTIUU5WXvOnvSLTmgNG4K-CzDNe2U8Rd6Y/edit?usp=sharing
