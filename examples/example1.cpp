#include <iostream>
#include <fstream>

#include "actrie.h"

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << ":-(" << std::endl;
        return 1;
    }

    actrie<> t;

    std::ifstream ifs(argv[1]);
    std::string str;
    while(std::getline(ifs, str))
    {
        //std::cout << "[ " << str << " ]\n";
        t.insert(str);
    }
    t.build();

    // test move constructor
    actrie<> t2 = std::move(t);

    while(std::getline(std::cin, str))
    {
        int count = t2.process(str);
        std::cout << count << "\t" << str << std::endl;
    }

    return 0;
}
