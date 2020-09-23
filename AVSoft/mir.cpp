#include <iostream>
#include <mutex>
#include <thread>
#include <random>
#include <string>
#include <fstream>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <csignal>

#define READERS_COUNT 10U
#define WRITERS_COUNT 5U
#define ITER_COUNT 10U
#define WRITERS_SLEEP_TIME_IN_MILLISECONDS 200U
#define READERS_SLEEP_TIME_IN_MILLISECONDS 1000U
#define BUFFER_FILE "buffer.txt"

#define is_empty(pFile) (pFile.peek() == std::ifstream::traits_type::eof())

std::mutex no_writers, no_readers, counter_mutex;
unsigned nreaders = 0;

// Get current date/time, format is YYYY-MM-DD HH:mm:ss
std::string currentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "Date: %Y-%m-%d Time: %X", &tstruct);
    return buf;
}

int intRand(const int & min, const int & max) {
    static thread_local std::mt19937 generator;
    std::uniform_int_distribution<int> distribution(min,max);
    return distribution(generator);
}

void signalHandler( int signum ) {
    std::cout << "Interrupt signal (" + std::to_string(signum) + ") received.\n";

    std::remove (BUFFER_FILE);

    exit(signum);  
}

void writer (unsigned j) {
    for (unsigned i = 0; i < ITER_COUNT; ++i) {
        no_writers.lock();
            no_readers.lock();
        no_writers.unlock();
        
        // writing
        std::ofstream out_file(BUFFER_FILE);
        if (!(out_file)) {
            throw std::runtime_error("Unable to open file for writing");
        }
        std::cout << "Writer: " + std::to_string(j) + " Iter: " + std::to_string(i) + " is writing\n";
        out_file << currentDateTime() << std::flush;
        out_file.close();
        std::this_thread::sleep_for(std::chrono::milliseconds(WRITERS_SLEEP_TIME_IN_MILLISECONDS));

        no_readers.unlock();
    }
}

void reader (unsigned j) {
    unsigned prev, current;
    for (unsigned i = 0; i < ITER_COUNT; ++i) {
        no_writers.lock();
            counter_mutex.lock();
                prev = nreaders;
                ++nreaders;
                if (!prev) {
                    no_readers.lock();
                }
            counter_mutex.unlock();
        no_writers.unlock();

        // reading
        std::ifstream in_file(BUFFER_FILE);
        bool empty = false;
        if (!(in_file.is_open())) {
            if (!(is_empty(in_file))) {
                throw std::runtime_error("Unable to open file for reading");
            } else {
                empty = true;
            }
        }
        std::string temp_string;
        if (!empty) {
            getline(in_file, temp_string);
            in_file.close();
            if (temp_string.empty()) {
                temp_string = "File is empty";
            }
        } else {
            temp_string = "File is not created";
        }
        std::cout << "\t\tReader: " + std::to_string(j) + "\tIter " + std::to_string(i) + " Reading: \t" + temp_string + "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(READERS_SLEEP_TIME_IN_MILLISECONDS));

        counter_mutex.lock();
            --nreaders;
            current = nreaders;
            if (!current) {
                no_readers.unlock();
            }
        counter_mutex.unlock();
    }
}

int main () 
{
    signal(SIGINT, signalHandler); 
    std::vector <std::thread> threads;
    unsigned readers = READERS_COUNT, writers = WRITERS_COUNT;
    
    for (unsigned j = 0; j < READERS_COUNT + WRITERS_COUNT; ++j) {
        if (readers == 0) {
            --writers;
            threads.push_back(std::thread(writer, j));
        } else if (writers == 0) {
            --readers;
            threads.push_back(std::thread(reader, j));
        } else {
            if (intRand(0, 1) == 0) {
                --writers;
                threads.push_back(std::thread(writer, j));
            } else {
                --readers;
                threads.push_back(std::thread(reader, j));
            }
        }
    }
    for (auto &t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    readers = READERS_COUNT;
    writers = WRITERS_COUNT;

    std::remove (BUFFER_FILE);
    exit(0);
}