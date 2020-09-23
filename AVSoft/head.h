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

#define R_COUNT 10
#define W_COUNT 10
#define ITER_COUNT 10
#define WRITERS_SLEEP 3000U
#define READERS_SLEEP 3000U

#endif