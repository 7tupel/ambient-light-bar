# Readme

## Resources / Links
https://fipsok.de/udp


## UDP Multicast Connection Test

Start UDP Multicast server:
```sh
iperf -s -u -B 230.120.10.1 -p 8266
```

Send UDP multicast bytes:
```sh
echo -n -e "\x00" | nc -c -w 1 -v -u 230.120.10.1 8266
```

Send UDP unicast bytes:
```sh
echo -n -e "\x00" | nc -c -w 1 -v -u 192.158.5.XXX 8266
```

## Protocol Specs

### General

### Version 1

Version 1 Protocol:

```
Version (1 byte) | OP (1 byte) | Data (m bytes)
```

#### OP Codes

+ **0x01** - `RGBWI8`


##### RGBI8 - 0x01

5 data bytes:
```
Red (1 byte) | Green (1 byte) | Blue (1 byte) | Intensity (1 byte)
```

Example UDP message:
```sh
echo -n -e "\x01\x01\xff\xff\xbb\xaa" | nc -c -w 1 -v -u 192.168.5.111 8266
```
