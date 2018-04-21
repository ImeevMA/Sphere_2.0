#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>

void word_counter(const std::string &fname);

constexpr size_t ARGNUM = 2;

int main(int argc, char **argv)
{
    try {
        if(argc != ARGNUM) {
            throw 1;
        }
        word_counter(argv[1]);
    } catch(...) {
        std::cerr << "error" << std::endl;
        return -1;
    }
    return 0;
}

void word_counter(const std::string &fname)
{
    std::ifstream fin(fname);
    if (!fin) {
        throw 2;
    }
    std::map<std::string, int> counter;
    while (fin.good()) {
        std::string word;
        fin >> word;
        if(counter.find(word) != counter.end()) {
            counter[word]++;
        } else {
            counter[word] = 1;
        }
    }
    auto comparator =
        [](std::pair<std::string, int> x, std::pair<std::string, int> x)
        {
            return x.second < y.second;
        }

    std::set<std::pair<std::string, int>, 
    std::sort(counter.begin(), counter.end(), counter.value_comp());
}