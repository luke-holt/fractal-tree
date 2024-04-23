#!/usr/bin/env sh

gcc -Og -g main.c util.c -o tree -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

