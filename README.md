# HITCON Badge

A (very) simple manual for HITCON 2017 Electronic Badge.

## Overview

**HITCON 2017 Electronic Badge** (HEB) is based on **MediaTek MT7697** SoC.

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
- To reset the defined text, holding `X` + `Y` + `A` + `B` during boot animation then confirm the reset.

> Note: Reseting your HEB may result in disqualification of some HEB related awards!!

### Game Play

- There's a small game that can be activated via holding `RIGHT + X + B`.
  - You might be rewarded once your score is higher than **249**.
- You might be able to catch some Pikachu if you were lucky.
  - You might be rewarded if you happen to catch the most.
  - Sharing your capture results with others will be disqualified.

> Note: Factory reseting your HEB may result in disqualification of related awards!!

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
