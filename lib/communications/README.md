# Communications Library

The Packet format that we use includes a checksum based on data, this is all well documented wherever our docs live. (They should move here, but someone else should do that.)

This checksum is not something you can calculate in your head, so a python script exists that takes in a string command and gives you a fully formed packet w/ the checksum. See [here](https://github.com/Space-Enterprise-at-Berkeley/Tests/blob/master/packet_gen.py).
