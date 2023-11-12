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

Start a listener on an address and port

```shell
start 0.0.0.0 8010
```

On other machines/terminals, run another copy of the application and connect to the listener

```
```

## classes diagram

```shell
dot -Tpng -Eminlen=2 classes.dot > classes.png
```
