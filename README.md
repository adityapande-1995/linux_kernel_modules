## Introduction

This repo aims to add some basic linux char drivers

1. simple_char_driver : A simple character driver that can take in text and prints it to dmesg. 
```
cd simple_char_driver
sudo ./test_and_run.sh
```

2. packet_sniffer : A simple packet sniffer that shows what TCP/UDP packets are being transferred.
```
cd packet_sniffer
sudo ./run_sniffer.sh # Starts the sniffer
sudo ./run_sniffer.sh clean # cleanup
```
