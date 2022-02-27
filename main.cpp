#include <iostream>
#include "VM.h"
#include <fstream>
#include "VM_binaries.h"
// #include <chrono>

using namespace std;
using namespace VM_BINARIES;

class VMFileStream: public VMBinStream {
    public:
    ifstream* f;
    bool get(char& d) {
        return (bool) f->get(d);
    }
};

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        cerr << "please enter a binary file name\n";
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
        return 1;
    }
    VM vm;
    ifstream f(argv[1], ios::binary);
    if (!f) {
        cerr << "can't open file" << '\n';
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
        return 1;
    }
    VMFileStream* fileStream = new VMFileStream();
    fileStream->f = &f;
    // auto start = std::chrono::high_resolution_clock::now();
    runBinFromStream(*fileStream, vm);
    // auto elapsed = std::chrono::high_resolution_clock::now() - start;
    // long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    // std::cout << std::endl << microseconds / 1000000.0000 << std::endl;
    delete fileStream;
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    return 0;
}