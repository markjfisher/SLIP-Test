# SLIP test application

A test application written to try out SLIP functionality over TCP.

## Building

In a MSYS2 MINGW64 shell, or Linux:

```shell
cd build
cmake ..
make
```

In a MSYS2 CLANG64 or UCRT64 shell

```shell
cd build
cmake ..
ninja
```

All of these will generate `SLIPTerminal.exe`

## Usage

### Start a listener on an address and port

```shell
start 0.0.0.0 8010
```

### Create "Device" connections

On other machines/terminals, run another copy of the application and connect to the listener

```shell
# terminal 1
connect 127.0.0.1 8010 1,NETWORK

# terminal 2
connect 127.0.0.1 8010 1,clock,2,printer
```

Note, you will need the correct IP address if the listener is running on a different machine to the one you are connecting the device from.

### List the connected devices

```text
# in listener terminal
> info

Listener: ip_address = 0.0.0.0, port = 8010, is_listening = true, connections = [Connection: is_connected = true, devices = [{device_index = 1, capability = {unit_id = 1, name = NETWORK}}], TCPConnection specific info: {socket = 300}, Connection: is_connected = true, devices = [{device_index = 2, capability = {unit_id = 1, name = clock}}, {device_index = 3, capability = {unit_id = 2, name = printer}}], TCPConnection specific info: {socket = 324}]
```

Here we can see NETWORK is from 1st connection with device_index 1, unit_id 1.
On the 2nd connection we see clock (device_index 2, unit id 1) and printer (device_index 3, unit id 2)

### Issue status command

On the listener terminal:

```text
# send a status request to NETWORK (device index 1)
> status 1
Found valid status response:1

# send a status request to clock (device index 2)
> status 2
Found valid status response:1

# send a status request to printer (device index 3)
> status 3
Found valid status response:1
```

Each command will be passed to the connection that hosts the given device index, and in our simple application simply responds back to the listener with a value of "1".

You will see on each terminal the appropriate output for the status request, e.g

```text
FakeSmartPortHandler::status called with request: StatusRequest: {status_code = 0, Request: Request: {seq_number = 10, command_number = 0, sp_unit = 1}}
```

In this simple application, the sequence number is hardcoded to 10.

## classes diagram

```shell
dot -Tpng -Eminlen=2 classes.dot > classes.png
```
