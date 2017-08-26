# HITCON Badge

A (very) simple manual for HITCON 2017 Electronic Badge.

*Last Update: Aug 26, 2017 9:29 AM*

## Overview

**HITCON 2017 Electronic Badge** (HEB) is based on **MediaTek MT7697** SoC.

> You can use microUSB instead of battery!
> PLEASE DO NOT RE-FLASH YOUR HEB UNLESS YOU KNEW HOW TO RESTORE IT OR NO LONGER WANTED TO PARTICIPATE IN THE CTF.

## Use as a HITCON Badge

            ____________________________________________
           / {IR}             ========    MediaTek {IR} \
           |                                            |
           |     *********************************      |
           |     *********************************      |
           |     *********************************      |
           |     ******** WOW SCREEN~ ************      |
           |     *********************************      |
           |     ************* SUCH NICE~ ********      |
           |     *********************************      |
           |     *********************************      |
           |     *********************************      |
           |                                            |
           |                                            |> [BATTERY SWITCH]
           |            [UP]                   [X]      |
           |   [LEFT] [CENTER] [RIGHT]       [Y] [A]    |
           |           [DOWN]                  [B]      |
           |                                            |
           |    {USR_LED}          [USR] [RST]          |
           \____________________________________________/

### Name Display

- The badge can display user defined text as owner's name.
- The string should be **ASCII** characters in **UTF-8 encoding** and its length should be **<= 15** chars.
- During idling, by holding the `USR` button will reveal the badge BLE name and a write prefix.
  - Connect your BLE capable device to HEB with the displayed name.
  - Write the user defined text to the BLE characteristic (*HEB only have one BLE service and one characteristic within*).
  - Format: `PREFIX` + `TEXT`.
  - e.g. if the prefix is `0123` and the user defined text is `Tom`, write UTF-8 string `0123Tom` to the BLE characteristic.
- The user defined text will be stored on NVDM and will be restore on reboots.
- To reset the defined text, holding `X` + `Y` + `A` + `B` during boot animation then confirm the reset. (Or just overwrite the current text if failed.)
- Some tools for BLE writing:
  - iOS: **LightBlue** https://itunes.apple.com/us/app/lightblue-explorer-bluetooth-low-energy/id557428110?mt=8
  - Android: **BLE Scanner** https://play.google.com/store/apps/details?id=com.macdom.ble.blescanner&hl=zh_TW
  - macOS: **Bluetooth Explorer** of the Additional Tools for **Xcode** https://developer.apple.com/download/more/

> Note: Reseting your HEB may result in disqualification of some HEB related awards!!

### Game Play

- There's a small game that can be activated via holding `RIGHT + X + B`.
  - You might be rewarded once your score is higher than **249**.
  - You can use either the joy-stick or the `X`, `Y`, `A` and `B` to contorl the snake direction.
  - You can also pause the game via the `CENTER` button.
- You might be able to catch some Pikachu if you were lucky.
  - You might be rewarded if you happen to catch the most.
  - Sharing your capture results with others will be disqualified.
  - If you encountered any error, it's (usually) because the signal to the source is too weak.

### Key Submission

- Snake: https://mini.hitcon.org/challenges#Snake
- Pikachu: https://badge.hitcon.org/

> Note: Factory reseting your HEB may result in disqualification of related awards!!

### Hints

- Snake:
  - Beat the game with _250+_ points, which is (almost) impossible and not likely what a hacker would do.
  - By googling the keyword "_MT7697_" (given by the badge's description), you should be able to find some __REALLY__ useful tools from the [official site](https://docs.labs.mediatek.com/resource/mt7687-mt7697/en/).
  - That site also has a guide regarding those __REALLY__ useful tools that gives you the detail of flash structure.
  - __MT7697__ is an ARM _Cortex-M4_ chip.
- Pikachu:
  - No, there's no way to cheat this game besides saving some labor work. Stop wasting your time!
  - Also, there were no hidden keys regarding the Bluetooth. But I can't say there're no easter eggs. (No keys, still.)

### FAQ

- Q: The animation freezed?
- A: Your batteries' voltage is too low. Replace them or use microUSB as power source.

- Q: I can't find my device?
- A: There are lots of BLE devices around, find somewhere without BLE disruption and retry. The device name is the same as its Bluetooth MAC address, it would help to locate your device if the name of the device can't be resolved due to disruption.

## License

    Copyright (C) 2017 Zhi-Wei Cai.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
