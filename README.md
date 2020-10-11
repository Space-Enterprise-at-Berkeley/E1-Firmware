# Brain
Read all sensor data via I2C, encode and send via RF. Also handle apogee logic and commands from ground station.

## Setup:
We're using the Teensy 4.0 board. Setup the environment according to the instructions here:
https://www.pjrc.com/teensy/td_download.html

### Windows users:
1. Make sure git is installed with symlink support

During the install of git on Windows, enable symbolic links:

<img src="https://i.stack.imgur.com/rQF1w.png" width="300"/>

2. Tell Bash to create hardlinks instead of symlinks

Edit: `(git folder)/etc/bash.bashrc` and add to bottom: `MSYS=winsymlinks:nativestrict`

3. Set git config to use symlinks
```bash
git config core.symlinks true
```
or

```bash
git clone -c core.symlinks=true <URL>
```
NOTE: I have tried adding this to the global git config and at the moment it is not working for me so I recommend adding this to each repo...

4. Pull the repo

NOTE: Unless you have enabled developer mode in the latest version of Windows 10, you need to run bash as **administrator** to create symlinks.

5. Reset all Symlinks (optional) If you have an existing repo, or are using submodules you may find that the symlinks are not being created correctly so to refresh all the symlinks in the repo you can run these commands.

```bash
find -type l -delete
git reset --hard
```
NOTE: This will reset any changes since last commit so make sure you have committed first.

## Query:
The brain first initiates a query by sending a two digit sensor ID and requesting 24 bytes of data from the board which the sensor is located on. The boards have addresses 0-127.

## Response:
The brain then reads all 24 bytes transmitted as an array, regardless of which bytes data was actually written to. Current implementation will not allow for more than 24 bytes (6 floats) to be received. The 24 bytes are converted back into the original 6 floats.

## Wireless transmission:
Upon receiving the data (6 floats), the packet is constructed in the format `{<sensor_ID>, <data1>, <data2>, ... <dataN>|checksum}`. The checksum is calculated using the fletcher 16 method (consists of 6 hexadecimal numbers). The brain then sends the whole packet via software serial to the RF wireless transmission module. Each new sensor reading will be sent in separate packets. 

## Interrupts:
The brain will cease querying its clients when it receives a full packet command from the ground station, of format `"(IDxxxxxx)"`, where ID has the same restrictions mentioned above, and `"xxxxxx"` is a variable length amount of data subject to the same restrictions mentioned in *Response*. This packet is then sent verbatim with the start and terminating chars via RS485 for client side handling.

## TODO: Apogee detection and parachute fire
## TODO: Active control
