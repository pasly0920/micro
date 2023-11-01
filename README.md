# 마이크로 프로세서 설계

## Install

1. Install [CP210x USB to UART Bridge VCP Drivers 설치](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads)
2. 정상 인식 확인 `ls -l /dev/tty.SLAB_USBtoUART`
3. AVR GNU Compiler 설치 `% xcode-select --install
   % brew tap osx-cross/avr
   % brew install avr-gcc`
4. avrdude 설치 `brew install avrdude`



## Use

```shell
make
./upload.sh
```
