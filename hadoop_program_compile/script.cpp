// Tanase, Dragos
//
// Compiled with -std=c++11

#include <iostream>
#include <chrono>
#include <unistd.h>
#include <sys/wait.h>



const std::string err_message = "Error en los parametros de entrada.\n./programa <fichero.java> <nombre_deseado.jar>\n";
const std::string output_dir = "_compilation_output";
bool trataArgs(int argc, char **argv)
{
    if(argc != 3)
    {
        std::cerr << err_message;
        return false;
    }

    return true;
}

void make_output_dir()
{
    std::string com = "mkdir " + output_dir;
    system(com.c_str());

}

int main(int argc, char **argv)
{
    typedef std::chrono::high_resolution_clock temp;
    std::chrono::duration<double> total_comp, total_jar;
    std::string com, fn;

    if(!trataArgs(argc, argv))
        return 0;


    fn = argv[1];
    com = "javac -classpath $(hadoop classpath) " + fn;
    fn = fn.substr(0, fn.find_first_of('.'));

    std::cout << "\n@\tComienza el proceso de compilacion...\n\t" << com << "\n\n";
    //// Compilacion
    auto start = temp::now();  // Compienzo el temporizador

    pid_t pid = fork(); // Creo un proceso hijo para la ejecucion de la compilacion
    if(pid == 0)
    {
        system(com.c_str());
        exit(0);
    }
    waitpid(pid, NULL, 0);  // El proceso padre espera a que muera el hijo creado
    
    auto stop = temp::now();   // Paro el temporizador
    std::cout << "\n@\tCompilacion finalizada\n";
    total_comp = std::chrono::duration_cast<std::chrono::duration<double>>(stop - start);


    //// Creado del .jar
    com = argv[2];
    com = "jar cf " + com + " " + fn + "*.class";

    std::cout << "@\tComienza la formacion del .jar...\n\t" << com << "\n\n";

    start = temp::now();
    pid = fork(); // Creo un proceso hijo para la ejecucion del comando
    if(pid == 0)
    {
        system(com.c_str());
        exit(0);
    }
    waitpid(pid, NULL, 0);  // El proceso padre espera a que muera el hijo creado
    stop = temp::now();
    std::cout << "@\tFinalizado\n";
    total_jar = std::chrono::duration_cast<std::chrono::duration<double>>(stop - start);

    
    std::cout << "\tCompilacion\t" << total_comp.count() << " segundos\n";
    std::cout << "\tCreacion .jar\t" << total_jar.count() << " segundos\n";

    return 0;

}

