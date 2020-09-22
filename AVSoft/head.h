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
#include <fcntl.h>
#include <unistd.h>

#define R_COUNT 2
#define W_COUNT 2
#define ITER_COUNT 2
#define WRITERS_SLEEP 3000U
#define READERS_SLEEP 3000U

#endif