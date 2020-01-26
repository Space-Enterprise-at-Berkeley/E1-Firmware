# Brain
Read all sensor data from RS485, encode and send via RF. Also handle apogee logic and commands from ground station.

## Setup:
We're likely to be using the Teensy 4.0 board for the ground station. Setup the environment according to the instructions here:
https://www.pjrc.com/teensy/td_download.html

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
