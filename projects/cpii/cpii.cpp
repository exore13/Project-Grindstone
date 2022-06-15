// Un script que ejecuta el comando copy indicado, pero que
// ademas va mostrando en formato de porcentaje el estado
// de la copia.


#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <fstream>
#include <chrono>
#include <stdio.h>
#include <string>
#include <sstream>


const int n_threads = 2;
const std::string K_NUMBERS = "0123456789";

pthread_mutex_t mutex;
int program_state = 0;  // 0 = worker dead
                        // 1 = worker bussy


// Credit cplusplus forum
// https://www.cplusplus.com/forum/beginner/117874/
// 
// Executes a given command and redirects it's output
// towards a string that's returned afterwards
std::string pipe_to_string( const char* command )
{
    FILE* file = popen( command, "r" ) ;

    if( file )
    {
        std::ostringstream stm ;

        constexpr std::size_t MAX_LINE_SZ = 1024 ;
        char line[MAX_LINE_SZ] ;

        while(fgets(line, MAX_LINE_SZ, file))
            stm << line << '\n';

        pclose(file) ;
        return stm.str() ;
    }

    return "" ;
}

// Given a target directory of filename, it returns the
// target size in KiloBytes
int get_target_size(const std::string &target, const std::string &du_args)
{
    std::string _command = "du " + du_args + " " + target;
    static std::string result = pipe_to_string(_command.c_str());

    // Cuts the rest of the info
    result = result.substr(0, result.find_first_not_of(K_NUMBERS));

    return stoi(result);
}

// Extracts the origin argument from the copy command
std::string get_origin(const std::string &command)
{
    static std::string dump;

    dump = command.substr(command.find_first_of(' ') + 1);

    // Check for recursive arg
    if(dump[1] == 'R' || dump[1] == 'r')
        dump = dump.substr(dump.find_first_of(' ') + 1);

    dump = dump.substr(0, dump.find_first_of(' '));

    return dump;
}

// Extracts the target argument from the copy command
std::string get_target(const std::string &command)
{
    static std::string dump;
    dump = command.substr(command.find_last_of(' ') + 1);

    return dump;
}

// Code to be executed by a thread
// It performs the system copy command
void worker(void** arg)
{
    // Updates the program_state
    pthread_mutex_lock(&mutex);
    program_state = 1;
    pthread_mutex_unlock(&mutex);

    // Casts the void** to string
    std::string command = (string *)arg;

    // Executes the order
    system(command.c_str());

    // Updates the program_state
    pthread_mutex_lock(&mutex);
    program_state = 0;
    pthread_mutex_unlock(&mutex);

    // Rest in peace beloved thread
    pthread_exit(arg);
}

// Code to be executed by a thread
// It draws an output exposing the copy state
void output_info(void** arg)
{
    auto timer_start = std::chrono::high_resolution_clock::now();

    // Casts the void** to string
    std::string command = (string *)arg;
    std::string origin = get_origin(command);
    std::string target = get_target(command);
    std::string buffer;
    

    while(1) // TODO - Loop while worker thread still alive
    {
        auto timer_checkpoint = std::chrono::high_resolution_clock::now();
        auto elapsed_time = timer_checkpoint - timer_start;
        
        // If worker has finished, exit the loop
        pthread_mutex_lock(&mutex);
        if(program_state == 0)
            break;
        pthread_mutex_unlock(&mutex);

        buffer = "@ Time ";

        

        sleep(0.1f);
    }



    pthread_exit(arg);
}

int main(int argc, char** argv)
{
    typedef std::chrono::high_resolution_clock timer; // High precision clock
    pthread_t t_id[n_threads] = {1, 2}; // Threads id
    pthread_mutex_init(&mutex, NULL);

    int *output; // Storage of thread exit code
    int state; // Thread join code

    // Prepare cp command
    std::string command = "cp ";
    for(int i = 1; i < argc; i++)
    {
        command += argv[i];
        command += " ";
    }


    // Start the timer
    auto timer_start = timer::now();


    // This thread launches the system copy command
    pthread_create(&t_id[0], NULL, worker, &command); 

    // Thread in charge of giving visual information about the copy state
    pthread_create(&t_id[1], NULL, output_info, &command)


    // Waits for the threads to end
    for(int i = 0; i < n_threads; i++)
    {
        state = pthread_join(t_id[i], (void **) &output);
        if(state)
            std::cerr << "|Thread error|\n" << state << std::endl;
        
    }

    // Stop the timer
    auto time_stop = timer::now();



}
