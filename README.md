## Introduction

This repo aims to add some basic linux char drivers. I've tested these on Ubuntu Jammy (22.04, arm64).

1. simple_char_driver : A simple character driver that can take in text and prints it to dmesg. 
```
cd simple_char_driver
sudo ./test_and_run.sh
```

2. packet_sniffer : A simple packet sniffer that shows what TCP/UDP packets are being transferred. Uses a combination of a kernel module and a user space program.
```
cd packet_sniffer
sudo ./run_sniffer.sh # Starts the sniffer
sudo ./run_sniffer.sh clean # cleanup
```

3. packet_dropper : Drops some specified percent of packets randomly.
```
cd packet_dropper
sudo ./run_and_test.sh
```

4. loopback_netdev : Creates a loopback device that can be pinged to.
```
cd loopback_netdev
sudo ./run_and_test.sh
```

