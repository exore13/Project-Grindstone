#include <iostream>

int main(int argc, char** argv)
{
    std::string response;

    std::cout << "Can I test code on github?: ";
    std::getline(cin, response);

    if(response = string("yes"))
        std::cout "Correct!" std::endl;
    else
        std::cout "It's not there yet..." :: std::endl;


    return 0;
}