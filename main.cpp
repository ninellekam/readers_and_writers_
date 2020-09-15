//
//  main.cpp
//  head.h
//
//  Created by Нина Камкия on 08.09.2020.
//  Copyright © 2020 Нина Камкия. All rights reserved.
//


#include "head.h"
using namespace std;
mutex rd , wr, cmutex;
unsigned int readers = 0;

void    sigHandler( int sig) {
    cout << "SIGNAL SIGINT  with number " << sig << "Have";
    exit(sig);
}

int randomizer() {
    random_device rd;   // non-deterministic generator
    mt19937 gen(rd());  // to seed mersenne twister.
    uniform_int_distribution<> dist(0,1);
    return dist(gen);
}

std::string datecreate() {
    // current date/time based on current system
    time_t now = time(0);
    
    // convert now to string form
    char* dt = ctime(&now);
    // convert now to tm struct for UTC
    tm *gmtm = gmtime(&now);
    dt = asctime(gmtm);
    return dt;
}

void    writer(unsigned int i) {
    for (unsigned int j = 0 ; j < ITER_COUNT ; j++) {
        wr.lock();
        rd.lock();
        
        // writing start
        ofstream dat_file(FILE);
        if (!(dat_file))
            std::runtime_error("Can't open file\n");
        std::cout << "Writer: " << j << " iter №:" << i << " \n" ;
        dat_file << datecreate() ;
        dat_file.close();
        this_thread::sleep_for(std::chrono::milliseconds(3000U));
        // writing end
        rd.unlock();
        wr.unlock();
    }
}

void    reader(unsigned int i){
    for (unsigned int j = 0; j < ITER_COUNT ; j++) {
        wr.lock();
        
        cmutex.lock();
        readers++;
        if (readers == 1)
            rd.lock();
        cmutex.unlock();
       // wr.unlock();
        
         //reading start
        ifstream ffile(FILE);
        if (!(ffile.is_open()))
            if (!(ffile.peek() == EOF))
                cout << "ERROR open file\n" ;
        
        string str_temp;
        getline(ffile, str_temp);
        cout <<"Readers: " << i << " iter  №: " << j << "\n";
        if (!(str_temp.empty()))
            cout <<"Now readers reading: " << str_temp << "\n";
        else
            cout <<"File empty \n";
        this_thread::sleep_for(chrono::milliseconds(3000U));
        
        // reading end
        cmutex.lock();
        readers--;
           if (readers == 0)
               rd.unlock();
        wr.unlock();
        cmutex.unlock();
    }
}



int main()
{
    signal(SIGINT , sigHandler);
    vector <thread> threads;
    UI r = R_COUNT ;
    UI w = W_COUNT ;
    UI rw = R_COUNT + W_COUNT ;
    
    for (unsigned int i = 0; i < rw ; i++) {
        if (r == 0) {
            --w;
            threads.push_back(thread (writer,i));
        }
        else if (w == 0) {
            --r;
            threads.push_back(thread (reader,i));
        }
        else if (&randomizer == (0)){
            --w;
          threads.push_back(thread (writer,i));
        }
        else {
            --r;
         threads.push_back(thread (reader,i));
        }
    }
    
   // thr(threads);

   for (auto &iterator : threads)
   if (iterator.joinable())
       iterator.join();
    
    exit(0);
}
