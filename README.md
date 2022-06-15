# Esp32-Serial-Test

On each device add the following definitions. Remember that you can change the pins for the one you want.

### Device 1

```c++
#define SENDER_TXD_PIN 17
#define RECEIVER_RXD_PIN 16
#define DEVICE 1
```

### Device 2

```c++
#define SENDER_TXD_PIN 17
#define RECEIVER_RXD_PIN 16
#define DEVICE 2
```

You can also modify the speed of each message 🔥

```c++
const int intervalSerial = 500;
```
