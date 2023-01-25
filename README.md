# Server-and-Clients-communicate-with-TCP-UDP-socket
The executing environment is run on Ubuntu.
* python 3.8.2
* GCC 9.3.0

Build the executable file

```
bash build.sh
```

Run Server

```
./build/server <port>
```

Run Client

```
./build/client <IP adddress> <port>
```

or test default cases automatically

```
python testscript.py
```

arguments
```
-p : port
-t : testcase path
```

