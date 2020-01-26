# Brain
Read all sensor data from RS485, encode and send via RF. Also handle apogee logic and commands from ground station.

## Setup:
We're likely to be using the Teensy 4.0 board for the ground station. Setup the environment according to the instructions here:
https://www.pjrc.com/teensy/td_download.html

### Windows users:
1. Make sure git is installed with symlink support

During the install of git on Windows:

<img src="https://i.stack.imgur.com/Am9L1.png" width="300"/>

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

NOTE: Unless you have enabled developer mode in the latest version of Windows 10, you need to run bash as **administrator** to create symlinks

5. Reset all Symlinks (optional) If you have an existing repo, or are using submodules you may find that the symlinks are not being created correctly so to refresh all the symlinks in the repo you can run these commands.

```bash
find -type l -delete
git reset --hard
```
NOTE: This will reset any changes since last commit so make sure you have committed first

## Query:
The brain first initiates a query by sending a packet of format `"(ID)"` via RS485. ID is guaranteed to be two digits long, so an ID of 0 would be 00 and an ID of 23 would be 23. The `'('` and `')'` denotes a packet start and packet end respectively.

## Response:
The brain then waits up to a specified timeout time to receive a response of `"(IDxxxxxx)"`, ID being the same guaranteed two digit ID and `"xxxxxx"` being a variable length amount of data. `"xxxxxx"` cannot exceed the character denoted by a variable `characterLimit`.

## Wireless transmission:
Upon receiving a valid, non-timed out response from the sensor/sub-module (clients) of the appropriate ID, the brain then resends the full packet verbatim via hardware serial to the RF wireless transmission module, including the `'('` and `')'` chars. The packet without `'('` and `')'` chars are also logged to an onboard SD card in plaintext. Each new sensor reading will be on a new line.

## Interrupts:
The brain will cease querying its clients when it receives a full packet command from the ground station, of format `"(IDxxxxxx)"`, where ID has the same restrictions mentioned above, and `"xxxxxx"` is a variable length amount of data subject to the same restrictions mentioned in *Response*. This packet is then sent verbatim with the start and terminating chars via RS485 for client side handling.

## TODO: Apogee detection and parachute fire
## TODO: Active control
