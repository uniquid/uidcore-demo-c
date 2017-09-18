# uidcore-demo-c
Demo code for uidcore-c library

Here are the steps to setup and build a minimal demo project.<br>
The uidcore-c library is at the moment supported only under Linux
and it needs the following libraries available on the system:
- pthread
- curl

The demo implements a very simple Uniquid Node featuring Provider and User functionalities.
As a Provider it implements a simple echo function: user-defined RPC 33.
As a user it can request a RPC execution with the following syntax from command line:<br>
```
ProviderName method ParameterString
Es.
demo0123456789 33 {"hello":"world"}
```

In this demo we provide an implementation of a transport for the Uniquid messages based on MQTT

To set up the demo you can simply clone it from github
```
git clone --recurse-submodules git@github.com:uniquid/uidcore-demo-c.git
cd uidcore-C-demo
# build the libraries
make -C uidcore-c
make -C paho.mqtt.c
# build the demo
make
```
else you can follow this steps (the commits of this repository will guide you through these steps)
```
# make the demo directory
mkdir uidcore-demo-c
# cd into it
cd uidcore-demo-c
# and init the git repository
git init
```
clone the uidcore-c library @ commit 7e6ca7...87c0ca and add it as submodule
```
git clone git@github.com:uniquid/uidcore-c.git
cd uidcore-c/
git checkout 7e6ca79743f0a61649d3c54f06abab5d9787c0ca
git submodule init
git submodule update --recursive
cd ..
git submodule add git@github.com:uniquid/uidcore-c.git
```
copy the example code from uidcore-c
```
cp uidcore-c/example_init.c demo_init.c
cp uidcore-c/example_provider.c demo_provider.c
cp uidcore-c/example_user.c demo_user.c
```
