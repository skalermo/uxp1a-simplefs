#!/usr/bin/bash

./build/bin/set_up_fs.out
./build/bin/writer.out 1 "Short message" &
./build/bin/writer.out 1 "Serious whale" &
./build/bin/writer.out 1 "Deadly things" &
./build/bin/writer.out 1 "Another knife" &


./build/bin/reader.out 1 &
./build/bin/reader.out 2 &
