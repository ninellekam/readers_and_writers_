#ifndef HEAD_H
#define HEAD_H

#include <iostream>
#include <mutex>
#include <thread>
#include <random>
#include <string>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <csignal>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define R_COUNT 3
#define W_COUNT 3
#define ITER_COUNT 5
#define WRITERS_SLEEP 3000U
#define READERS_SLEEP 3000U

#endif