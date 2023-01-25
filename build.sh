#!/bin/bash
gcc Server.c -o ./build/server -pthread
gcc Client.c -o ./build/client -pthread