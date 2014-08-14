#include <iostream>
#include <fstream>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/regex.hpp>

using namespace boost;

/*int main(int argc, char ** argv)
{
    std::ifstream file("/home/thiago/workspace/ler.txt");
    boost::iostreams::filtering_istream in;
    in.push(file);
    for(std::string str; std::getline(in, str); )
    {
        std::cout << str << '\n';
    }

    return 0;
}*/
