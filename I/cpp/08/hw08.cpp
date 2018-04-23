#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
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
    std::vector<std::pair<int, std::string>> res;
    for(auto &elem : counter) {
        res.emplace_back(elem.second, elem.first);
    }
    std::sort(res.begin(), res.end(), std::greater<std::pair<int, std::string>>());
    for(auto &elem : res) {
        std::cout << elem.first << ": " << elem.second << std::endl;
    }
}