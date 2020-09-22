#include "head.h"

std::mutex rd , wr, cmutex;
unsigned int readers = 0;

void    sigHandler( int sig) {
    std::cout << "SIGNAL SIGINT  with number " << sig << std::endl;
    exit(sig);
}
void sys_err (std::string msg){
  std::cout << msg << std::endl;
  exit (1);
}

int randomizer() {
    std::random_device rd;          // non-deterministic generator
    std::mt19937 gen(rd());         // to seed mersenne twister.
    std::uniform_int_distribution<> dist(0,1);
    return dist(gen);
}


std::string datecreate() {
    time_t now = time(0);       // current date/time based on current system
    char* dt = ctime(&now);     // convert now to string form
    tm *gmtm = gmtime(&now);    // convert now to tm struct for UTC
    dt = asctime(gmtm);
    return dt;
}

void    writer(void *addr) {
    for (unsigned int j = 0 ; j < ITER_COUNT ; j++) {
        wr.lock();
        rd.lock();
        
        // writing start
        std::string s = datecreate();
        memcpy(addr, s.c_str(), s.length());
        if ( s.empty() )
            sys_err("Can't use shared memory\n");
        std::cout << "Writer "  << " iter №:" << j << " \n" ;
        std::this_thread::sleep_for(std::chrono::milliseconds(3000U));
        // writing end
        
        rd.unlock();
        wr.unlock();
    }
}

void    reader(void *addr){
    for (unsigned int j = 0; j < ITER_COUNT ; j++) {
        wr.lock();
        cmutex.lock();
        readers++;
        if (readers == 1)
            rd.lock();
        cmutex.unlock();
        
         //reading start
        char  buf[32];
        memcpy(buf, addr, 32);
        std::cout <<"Readers: " << " iter  №: " << j << "\n";
        if ( buf[0] != '\0' )
            std::cout <<"Now readers reading: " << buf << "\n";
        else
            std::cout <<"Untill File empty \n";
        std::this_thread::sleep_for(std::chrono::milliseconds(3000U));
        
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
    std::vector <std::thread> threads;
    unsigned int r = R_COUNT ;
    unsigned int w = W_COUNT ;
    unsigned int rw = R_COUNT + W_COUNT ;
    pid_t pid;
    off_t offset  = sysconf(_SC_PAGE_SIZE);
	pid = getpid();

    int fd = open("text.txt", O_RDWR , 0666);
    void *addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                     MAP_SHARED, fd, 0);

   
    for (unsigned int i = 0; i < rw ; i++) {
        if (r == 0) {
            --w;
            threads.push_back(std::thread(writer, addr));
        }
        else if (w == 0) {
            --r;
            threads.push_back(std::thread(reader, addr));
        }
        else if (&randomizer == (0)){
            --w;
          threads.push_back(std::thread(writer, addr));
        }
        else {
            --r;
         threads.push_back(std::thread(reader, addr));
        }
    }
    
   for (auto &iterator : threads)
   if (iterator.joinable())
       iterator.join();
    
    exit(0);
}