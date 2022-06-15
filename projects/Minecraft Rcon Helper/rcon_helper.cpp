#include <vector>
#include <iostream>
using namespace std;


const string mcrcon_path = "/home/goodboi/minecraft_servers/tools/mcrcon";


struct server {
    string name;
    string version;
    string ip;
    string rcon_port;
    string rcon_pwd;
};


vector<server> servers;


// Helper function to add entries to the servers vector
void add_server(const string &s_name, const string &s_version, const string &s_ip, const string &s_rcon_port, const string &s_rcon_pwd)
{
    server aux_server;

    aux_server.name = s_name;
    aux_server.version = s_version;
    aux_server.ip = s_ip;
    aux_server.rcon_port = s_rcon_port;
    aux_server.rcon_pwd = s_rcon_pwd;

    servers.push_back(aux_server);
}


// Populate servers vector with hardcoded data
void populate_servers()
{
    add_server("Stock+Dynmap", "Fabric 1.17.1", "localhost", "55003", "s0merc0np4ssw0rd");
    add_server("Axolote", "Paper 1.17.1", "localhost", "55001", "hdfsnofunci0na111");
    add_server("Creative Sandbox", "Fabric 1.17.1", "localhost", "55005", "ojikplk4536sdfq12qwa");
}


// Populate servers vector with data read from servers.csv
void populate_from_file()
{
    // TODO
}


void print_menu()
{
    cout << "#\tRCON Helper for Minecraft Servers\n";
    cout << "#\t\t~GoodBoi~\n";
    cout << "#\n";

    for(int i = 0; i < servers.size(); i++)
        cout << "#\t" << i+1 << " - " << servers[i].name << " " << servers[i].version << endl;

    cout << "#\n";
}


bool isNumber(const string& str)
{
    for (char const &c : str) {
        if (std::isdigit(c) == 0) return false;
    }
    return true;
}


int ask_for_server()
{
    string _choice;
    int choice;

    while(1)
    {
        cout << "#\tChose server(index): ";
        getline(cin, _choice);

        if(!isNumber(_choice))
        {
            cout << "#\tChoice should be a number...\n#\n";
            continue;
        }

        choice = stoi(_choice);

        if(choice <= 0 || choice > servers.size())
        {
            cout << "# ERR\tThe index should be inside the range [1 " << servers.size() << "]\n#\n";
            continue;
        }

        break;
    }

    cout << "#\tYou have chosen " << servers[choice-1].name << "\n";

    choice -= 1;
    return choice;
}


void command_loop(const int &choice)
{
    cout << "#\tEach line will be a command sent to " << servers[choice].name << "\n";
    cout << "#\tYou can type {q} to exit\n";
    char _command[200];
    sprintf(_command, "%s -H %s -P %s -p %s ", mcrcon_path.c_str(), servers[choice].ip.c_str(), servers[choice].rcon_port.c_str(), servers[choice].rcon_pwd.c_str());

    while(1)
    {
        cout << "\n";
        string input, command;
        getline(cin, input);

        if(input == "{q}")
            break;

        command = _command;
        command += "\"" + input + "\"";

        system(command.c_str());
    }

    cout << "\t~Good bye~\n\n";
}



int main(int argc, char** argv)
{
    system("clear");

    populate_servers();
    // populate_from_file(); // TODO

    print_menu();
    int choice = ask_for_server();

    command_loop(choice);

    return 0;
}