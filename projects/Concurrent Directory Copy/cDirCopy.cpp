/*
    Concurrent Programming
    Optative Project

    Implement a concurrent directory copy
    Tanase, Dragos
    21 October 2020

    Language: C++
*/

#define defaultThreadsCount 4
#include <iostream>
#include <pthread.h>
#include <chrono>
#include <dirent.h>
#include <vector>
#include <unistd.h>
#include <fstream>

const std::string correctLaunchExample = "Usage example:\n./cDirCopy [-c/-s] folder_src folder_dest\n";
const std::string flagC = "-c";
const std::string flagS = "-s";
const std::string actualDir = ".";
const std::string upperDir = "..";

std::vector<std::string> filesQueue;
std::vector<std::string> pasteQueue;
bool timerFlag = false;

struct threadArgs {
    int id;
    int index;
    int copyCount;
};

enum Error
{
    ERR_ARGS,
    ERR_NO_DIR,
    ERR_SAME_DIR,
    ERR_FAILED_DIR,
    ERR_QUEUES,
    ERR_COPY
};

// Procedure used for printing error messages
void error(const Error &n)
{
    switch(n)
    {
        case ERR_ARGS:
            std::cerr << "Error: Arguments not valid.\n" << correctLaunchExample;
            break;
        
        case ERR_NO_DIR:
            std::cerr << "Error: The source directory does not exist.\n";
            break;

        case ERR_SAME_DIR:
            std::cerr << "Error: The source and destination directory are the same.\n";
            break;

        case ERR_FAILED_DIR:
            std::cerr << "Error: Can't open destination directory.\n";
            break;

        case ERR_QUEUES:
            std::cerr << "Error: Unexpected error on queues creation.\n";
            break;
        
        case ERR_COPY:
            std::cerr << "Copy error.\n";

        default:
            std::cerr << "Unexpected error.\n";
    }
}

// Returns the thread count of the system
int getMachineThreads()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

// Checks whether the arguments are correct. The definition of valid call is defined on the original guidance for this project
bool checkArgs(const int &argc, char** &argv, std::string &from, std::string &to, int &threads)
{
    // Error case. There are few or too many arguments
    if(argc < 3 || argc > 4)    
    {
        error(ERR_ARGS);
        return false;
    }

    // Valid case 1. Only source and destination
    if(argc == 3)
    {
        from = argv[1];
        to = argv[2];

        if(from == to)
        {
            error(ERR_SAME_DIR);
            return false;
        }
        return true;
    }

    // Valid case 2. There is an argument related to the threadcount/concurrency
    //// Valid case 2.1. Threadcount/concurrency argument is found on first place
    if(argv[1] == flagC || argv[1] == flagS)
    {
        from = argv[2];
        to = argv[3];

        if(from == to)
        {
            error(ERR_SAME_DIR);
            return false;
        }

        if(argv[1] == flagC)
            threads = getMachineThreads();
            
        if(argv[1] == flagS)
            threads = 1;

        timerFlag = true;
        return true;
    }

    //// Valid case 2.2. Threadcount/concurrency argument is found on last place
    if(argv[3] == flagC || argv[3] == flagS)
    {
        from = argv[1];
        to = argv[2];
        
        if(from == to)
        {
            error(ERR_SAME_DIR);
            return false;
        }

        if(argv[3] == flagC)
            threads = getMachineThreads();
            
        if(argv[3] == flagS)
            threads = 1;

        timerFlag = true;
        return true;
    }

    // Error case. There are 3 arguments, but the threadcount/concurrency
    //             related one is not placed correctly
    error(ERR_ARGS);
    return false;
}

// Lists all the files and directories to copy
bool createQueues(const std::string &from, std::vector<std::string> &foldersQueue, std::vector<std::string> &filesQueue)
{
	// Open directory stream
	DIR *dir = opendir(from.c_str());
	if (!dir) {
		error(ERR_FAILED_DIR);
		return false;
	}

	// Print all files and directories within the directory
	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL) {
        switch (ent->d_type) {
            case DT_REG:    // It's a file
                filesQueue.push_back(from + ent->d_name);
                break;

            case DT_DIR:    // It's a directory
                // Ignore `./` and `../`
                if(ent->d_name == actualDir || ent->d_name == upperDir)
                    continue;

                // Enqueues the directory
                foldersQueue.push_back(from + ent->d_name + "/");

                // Launches recursively the queue creation on the detected directories.
                createQueues(from + ent->d_name + "/", foldersQueue, filesQueue);
                break;

            default:
                printf("@ Not supported: %s\n", ent->d_name);
		}
	}
	closedir(dir);
    return true;
}

// Procedure that checks the existance of the destination folder. The `create`
// flag makes it create the directory if it doesn't exists
bool folderExists(const std::string &destiny, const bool &create)
{
    // If you can't open the directory, that means it does not exists
    DIR *dir = opendir(destiny.c_str());
	if (!dir && create) 
    {   // If the directory does not exists, create it.
        std::string command = "mkdir " + destiny;
        system(command.c_str());
    }

    else if(!dir)
    {
        return false;
    }

	else // If you could open it, you should close it aswell. (RAII???)
        closedir(dir);
    
    return true;
}

// Procedure that creates the destination folders
void createFolders(const std::vector<std::string> &foldersQueue, const std::string &destino)
{
    std::string comando;

    // Nos aseguramos de que existe el directorio destino
    // Check whether the destination folder exists or not.
    folderExists(destino, true);

    // Create the nested folders structure in order they were found
    for(auto folder : foldersQueue)
    {
        comando = "mkdir " + destino + "/" + folder;
        system(comando.c_str());
    }
}

// The detected files and directories are saved with `src/file` structure. This removes the source
std::vector<std::string> removeFrom(const std::vector<std::string> &data, const std::string &from)
{
    std::vector<std::string> sol;

    for(unsigned int i = 0; i < data.size(); i++)
        sol.push_back(data[i].substr(from.length(), data[i].length()));

    return sol;
}

// [Thread function]
// Performs the copy using system build in `cp` command
void* systemCopy(void* voidArgs)
{
    threadArgs *args = (threadArgs *)voidArgs;
    std::string command;

    int i = (*args).index;
    int count = (*args).copyCount;
    while(count > 0)
    {
        command = "cp " + filesQueue[i] + " " + pasteQueue[i];
        system(command.c_str());

        i++;
        count--;
    }

    pthread_exit(voidArgs);
}

// [Thread function]
// Performs a binary copy
// src: https://stackoverflow.com/questions/5420317/reading-and-writing-binary-file
void* binaryCopy(void* voidArgs)
{
    threadArgs *args = (threadArgs *)voidArgs;

    int i = (*args).index;
    int count = (*args).copyCount;
    while(count > 0)
    {
        // Open files
        std::ifstream inFile(filesQueue[i].c_str(), std::ios::binary);
        std::ofstream outFile(pasteQueue[i].c_str(), std::ios::binary);

        // Read from origin, write to dest
        std::copy(
            std::istreambuf_iterator<char>(inFile),
            std::istreambuf_iterator<char>( ),
            std::ostreambuf_iterator<char>(outFile)
        );

        i++;
        count--;
    }

    pthread_exit(voidArgs);
}

// Concatenates a string to an entire string vector. (Should have used map probably)
void addDestination(std::vector<std::string> &data, const std::string &dest)
{
    unsigned int i;
    std::string destConcat = dest + "/";

    for(i = 0; i < data.size(); i++)
        data[i] = destConcat + data[i];
}


int main(int argc, char** argv)
{
    std::string from, to;
    std::vector<std::string> foldersQueue;
    int threads = defaultThreadsCount;
    int *output, state;
    typedef std::chrono::high_resolution_clock timer;
    auto timerStart = timer::now();
    auto timerStop = timer::now();

    // Check whether the args are valid or not
    if(!checkArgs(argc, argv, from, to, threads))
        return -1;

    if(!folderExists(from, false))
    {
        error(ERR_NO_DIR);
        return -1;
    }

    // Queues startup
    if(!createQueues(from, foldersQueue, filesQueue))
    {
        error(ERR_QUEUES);
        return -1;
    }  
    pasteQueue = removeFrom(filesQueue, from);
    addDestination(pasteQueue, to);
    

    // Remove source prefix
    foldersQueue = removeFrom(foldersQueue, from);
    createFolders(foldersQueue, to);


    // File copy
    if(threads == 1)    // Sequential copy
    {
        // Preparation
        pthread_t tId;
        threadArgs argumentos;
        argumentos.id = 0;
        argumentos.copyCount = filesQueue.size();
        argumentos.index = 0;


        // Execution
        timerStart = timer::now();
        
        #ifdef binCopy
            pthread_create(&tId, NULL, binaryCopy, &argumentos);
        #else
            pthread_create(&tId, NULL, systemCopy, &argumentos);
        #endif


        // Output readout
        state = pthread_join(tId, (void **) &output);
        timerStop = timer::now();
        if(state)
        {
            error(ERR_COPY);
            std::cerr << "Codigo error:\t" << state << std::endl;
        }

    }

    else                // Concurrent copy
    {
        // Preparation

El reparto de ficheros a copiar por cada hilo
No habra un master como tal, solo llamo asi al que tendra que encargarse
del resto de la division entera, unos ficheros mas normalmente

        // File count split through the threads
        //// There won't be any `master` orchestrating the operations
        //// What's called `master` is the thread that will have to copy
        //// the bigger section
        int nSlave = filesQueue.size() / threads;
        int nMaster = filesQueue.size() - (nSlave * (threads - 1));

        // File index startup for each thread. All filenames are stored on the same vector
        int copyIndex[threads];

        copyIndex[0] = 0;
        for(int i = 1; i < threads; i++)
            copyIndex[i] = nMaster + (i - 1)*nSlave;
        

        pthread_t tId[threads];
        threadArgs argumentos[threads];
        for(int i = 0; i < threads; i++)
        {
            argumentos[i].id = i;
            argumentos[i].index = copyIndex[i];
            argumentos[i].copyCount = (i != 0) ? nSlave : nMaster;
        }


        // Execution
        timerStart = timer::now();
        for(int i = 0; i < threads; i++)
        {
            #ifdef binCopy
                pthread_create(&tId[i], NULL, binaryCopy, &argumentos[i]);
            #else
                pthread_create(&tId[i], NULL, systemCopy, &argumentos[i]);
            #endif
        }

        
        // Output readout
        for(int i = 0; i < threads; i++)
        {
            state = pthread_join(tId[i], (void **) &output);
            if(state)
            {
                error(ERR_COPY);
                std::cerr << "Error code:\t" << state << std::endl;
            }
        }
        timerStop = timer::now();
    }

    // [INFO] Prints the elapsed time, only when `-s` or `-c` was specified
    if(timerFlag)
    {
        std::chrono::duration<double> timespan = std::chrono::duration_cast<std::chrono::duration<double>>(timerStop - timerStart);
        std::cout << "Execution time: " << timespan.count() << " seconds.\n";  
    }

    return 0;
}