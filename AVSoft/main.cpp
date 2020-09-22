#include "head.h"

using namespace std;
mutex rd , wr, cmutex;
unsigned int readers = 0;

void    sigHandler( int sig) {
    cout << "SIGNAL SIGINT  with number " << sig << endl;
    exit(sig);
}
void sys_err (string msg)
{
  std::cout << msg << std::endl;
  exit (1);
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

void    writer(void *addr) {
    for (unsigned int j = 0 ; j < ITER_COUNT ; j++) {
        wr.lock();
        rd.lock();
        
        // writing start
        string s = datecreate();
        memcpy(addr, s.c_str(), s.length());
        if ( s.empty() )
            sys_err("Can't use shared memory\n");
        std::cout << "Writer "  << " iter №:" << j << " \n" ;
        this_thread::sleep_for(std::chrono::milliseconds(3000U));
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
        cout <<"Readers: " << " iter  №: " << j << "\n";
        if ( buf[0] != '\0' )
            cout <<"Now readers reading: " << buf << "\n";
        else
            cout <<"Untill File empty \n";
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
    unsigned int r = R_COUNT ; unsigned int w = W_COUNT ;
    unsigned int rw = R_COUNT + W_COUNT ;
    pid_t pid;
    off_t offset  = sysconf(_SC_PAGE_SIZE);

	pid = getpid();

    int fd = open("text.txt", O_RDWR , 0666);
    //int fd = shm_open("t.txt", O_RDWR | O_CREAT, 0666);
    void *addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                     MAP_SHARED, fd, 0);

   
    for (unsigned int i = 0; i < rw ; i++) {
        if (r == 0) {
            --w;
            threads.push_back(thread(writer, addr));
        }
        else if (w == 0) {
            --r;
            threads.push_back(thread(reader, addr));
        }
        else if (&randomizer == (0)){
            --w;
          threads.push_back(thread(writer, addr));
        }
        else {
            --r;
         threads.push_back(thread(reader, addr));
        }
    }
    
   // thr(threads);

   for (auto &iterator : threads)
   if (iterator.joinable())
       iterator.join();
    
    exit(0);
}

// int shmid; 
   // message *msg_id;

    /* получение доступа к сегменту разделяемой памяти */
    // if ((shmid = shmget (SHM_ID, sizeof (message), 0)) < 0)
    // sys_err("client: can not get shared memory segment");
    
    // /* получение адреса сегмента */
    // if ( (msg_id = (message *) shmat(shmid, 0, 0)) == NULL)
    // sys_err ("client: shared memory attach error");


    	// get shared memory file descriptor (NOT a file)
	// fd = shm_open("text.txt",  O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR);
	// if (fd == -1)
	// {
	// 	perror("open");
	// 	return 10;
	// }
	// // map shared memory to process address space
	// addr = mmap(NULL, 4096,   PROT_READ | PROT_EXEC, MAP_SHARED , fd, 0);
	// if (addr == MAP_FAILED)
	// {
    //     cout <<"ok";
	// 	perror("mmap");
	// 	return 30;
	// }