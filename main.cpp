#include <iostream>
#include "VM.h"
#include <fstream>
#include "VM_binaries.h"

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
    runBinFromStream(*fileStream, vm);
    delete fileStream;
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    return 0;
}