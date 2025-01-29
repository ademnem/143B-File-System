#include <cassert>
#include <iostream>
#include <string>

#include "file_system.hpp"

using namespace std;



using FunctionPointer = bool (*)();

void test(string name, FunctionPointer function) {
    cout << "Testing: " << name << "...";
    cout << (function() ? "OK" : "FAILED") << endl;
}


void sample_test_case(void) {
    cout << initialize() << endl;

    cout << create("foo") << endl;
    FileEntry* feFoo = (FileEntry*)&(D[7][0]);
    assert(strcmp(feFoo->name, "foo") == 0);
    assert(feFoo->index == 16);
    FileDescriptor* fdFoo = (FileDescriptor*)&(C[1][16]);
    assert(fdFoo->length == 0);
    for (int i = 0; i < 3; ++i)
        assert(fdFoo->blocks[i] == 0);

    cout << create("bar") << endl;  
    FileEntry* feBar = (FileEntry*)&(D[7][8]);
    assert(strcmp(feBar->name, "bar") == 0);
    assert(feBar->index == 32);
    FileDescriptor* fdBar = (FileDescriptor*)&(C[1][32]);
    assert(fdBar->length == 0);
    for (int i = 0; i < 3; ++i)
        assert(fdBar->blocks[i] == 0);

    char empty[BLOCK_SIZE] = {0};

    int fooIndex = open("foo");
    cout << "foo opened " << fooIndex << endl;
    OpenFile& ofFoo = OFT[1];
    assert(ofFoo.index == 16);
    assert(ofFoo.pos == 0);
    assert(ofFoo.size == 0);
    for (int i = 0; i < BLOCK_SIZE; ++i)
        assert(ofFoo.buff[i] == empty[i]);

    string drOutput = directory();
    assert(drOutput == "foo 0 bar 0 ");
    cout << drOutput << endl;

    int seekFoo = seek(1, 10); 
    assert(seekFoo == -1); 
    if (seekFoo == -1)
        cout << "error" << endl;

    assert(close(fooIndex) == fooIndex); 
    assert(ofFoo.index == 0);
    assert(ofFoo.pos == 0);
    assert(ofFoo.size == 0);
    for (int i = 0; i < BLOCK_SIZE; ++i)
        assert(ofFoo.buff[i] == empty[i]);
    cout << fooIndex << " closed" << endl;

    cout << destroy("foo") << endl; 
    assert(strcmp(feFoo->name, "foo") != 0);
    assert(feFoo->index == 0);
    assert(fdFoo->length == -1);

    drOutput = directory();
    assert(drOutput == "bar 0 ");
    cout << drOutput << endl;

    
    cout << endl;
    cout << initialize() << endl;

    drOutput = directory();
    assert(drOutput == "");
    cout << drOutput << endl;

    int bytesWritten = write_memory(0, "abcdefghij");
    cout << bytesWritten << endl; 
}


int main() {

    cout << "---=== TESTING ===---" << endl;
    sample_test_case();

    return 0;
}
