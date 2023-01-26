# Server-and-Clients-communicate-with-TCP-UDP-socket
The executing environment is run on Ubuntu.
* python 3.8.2
* GCC 9.3.0

Build the executable file

```
bash build.sh
```

Furst, run Server

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

if you run client successfully, *****Welcome to Game 1A2B***** pop out.
You can play 1A2B game as client.
There are instructions:
* register <username> <email> <password>
* login <username> <password>
* logout
* game-rule
* start-game
* exit
* help
