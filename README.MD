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
