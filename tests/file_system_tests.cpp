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
    assert(strncmp((char*)&(M[0]), "abcdefghij", 10) == 0);
    cout << bytesWritten << endl; 

    cout << create("foo") << endl;
    feFoo = (FileEntry*)&(D[7][0]);
    assert(strcmp(feFoo->name, "foo") == 0);
    assert(feFoo->index == 16);
    fdFoo = (FileDescriptor*)&(C[1][16]);
    assert(fdFoo->length == 0);
    for (int i = 0; i < 3; ++i)
        assert(fdFoo->blocks[i] == 0);
 
    fooIndex = open("foo");
    cout << "foo opened " << fooIndex << endl;
    ofFoo = OFT[1];
    assert(ofFoo.index == 16);
    assert(ofFoo.pos == 0);
    assert(ofFoo.size == 0);
    for (int i = 0; i < BLOCK_SIZE; ++i)
        assert(ofFoo.buff[i] == empty[i]);

    cout << write(1, 0, 5) << endl;   
    assert(strncmp((char*)ofFoo.buff, "abcde", 5) == 0);
    assert(strncmp((char*)D[8], "abcde", 5) == 0);
    assert(ofFoo.size == 5);
    assert(ofFoo.pos == 5);
    assert(fdFoo->length == 5);

    cout << write(1, 5, 2) << endl;   
    assert(strncmp((char*)ofFoo.buff, "abcdefg", 7) == 0);
    assert(strncmp((char*)D[8], "abcdefg", 7) == 0);
    assert(ofFoo.size == 7);
    assert(ofFoo.pos == 7);
    assert(fdFoo->length == 7);

    cout << directory() << endl;

    cout << "position is " << seek(1, 0) << endl;
    assert(ofFoo.pos == 0);

    cout << read(1, 10, 3) << endl;
    assert(ofFoo.pos == 3);
    assert(strncmp((char*)M + 10, "abc", 3) == 0);

    cout << read_memory(0, 20) << endl;

    fooIndex = close(1);
    cout << fooIndex << " closed" << endl;
    assert(ofFoo.pos == 0);
    assert(ofFoo.index == 0);
    assert(ofFoo.size == 0);
    for (int i = 0; i < BLOCK_SIZE; ++i)
        assert(ofFoo.buff[i] == empty[i]);

    cout << directory() << endl;
}
void full_file_test(void) {
    cout << initialize() << endl; 

    cout << create("abc") << endl;
    FileEntry* feABC = (FileEntry*)&(D[7][0]);
    assert(strcmp(feABC->name, "abc") == 0);
    assert(feABC->index == 16);
    FileDescriptor* fdABC = (FileDescriptor*)&(C[1][16]);
    assert(fdABC->length == 0);
    for (int i = 0; i < 3; ++i)
        assert(fdABC->blocks[i] == 0);


    int abcIndex = open("abc");
    cout << "abc opened " << abcIndex << endl;
    OpenFile& ofABC = OFT[1];
    assert(ofABC.index == 16);
    assert(ofABC.pos == 0);
    assert(ofABC.size == 0);
    // check buffer 
    char empty[BLOCK_SIZE] = {0};
    for (int i = 0; i < BLOCK_SIZE; ++i)
        assert(ofABC.buff[i] == empty[i]);

    // setup for writing memory and onto disk
    string a{};
    string b{};
    string c{};
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        a += 'a';
        b += 'b';
        c += 'c';
    }

    cout << write_memory(0, a) << " bytes writen to M" << endl;
    for (int i = 0; i < BLOCK_SIZE; ++i) 
        assert(M[i] == a[i]);
    
    cout << write(1, 0, 512) << endl;
    int b0 = fdABC->blocks[0];
    assert(b0 == 8);
    assert(C[0][8] == 255); 
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        assert(ofABC.buff[i] == a[i]);
        assert(D[b0][i] == a[i]);
    }
    assert(fdABC->length == 512);
    assert(ofABC.pos == 512);
    assert(ofABC.size == 512);

    cout << write_memory(0, b) << " bytes writen to M" << endl;
    for (int i = 0; i < BLOCK_SIZE; ++i) 
        assert(M[i] == b[i]);

    cout << write(1, 0, 512) << endl;
    int b1 = fdABC->blocks[1];
    assert(b1 == 9);
    assert(C[0][9] == 255); 
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        assert(ofABC.buff[i] == b[i]);
        assert(D[b1][i] == b[i]);
    }
    assert(fdABC->length == 1024);
    assert(ofABC.pos == 1024);
    assert(ofABC.size == 1024);

    cout << write_memory(0, c) << " bytes writen to M" << endl;
    for (int i = 0; i < BLOCK_SIZE; ++i) 
        assert(M[i] == c[i]);

    cout << write(1, 0, 512) << endl;
    int b2 = fdABC->blocks[2];
    assert(b2 == 10);
    assert(C[0][10] == 255); 
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        assert(ofABC.buff[i] == c[i]);
        assert(D[b2][i] == c[i]);
    }
    assert(fdABC->length == 1536);
    assert(ofABC.pos == 1536);
    assert(ofABC.size == 1536);

    cout << directory() << endl;

    cout << "position is " << seek(1, 0) << endl; 
    assert(ofABC.pos == 0);
    assert(ofABC.size == 1536);
    for (int i = 0; i < BLOCK_SIZE; ++i)
        assert(ofABC.buff[i] == a[i]);

    cout << read(1, 0, 512) << endl;
    for (int i = 0; i < BLOCK_SIZE; ++i)
        assert(M[i] == ofABC.buff[i]);

    cout << read_memory(0, 512) << endl;
    
    cout << directory() << endl;
}
void actual_submission(void) {
    /*
    cout << initialize() << endl;
    cout << write_memory(0, "0123456789") << " bytes written to M" << endl;
    cout << create("abc") << endl;
    cout << "abc opened " << open("abc") << endl;
    cout << write(1, 3, 5) << " bytes written to 1" << endl; 
    cout << "position is " << seek(1, 0) << endl; 
    cout << read(1, 20, 9) << endl;
    cout << read_memory(20, 5) << endl;
    */

    cout << initialize() << endl;
    cout << write_memory(0, "abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234ABCDEFGHIJ") << " written to M" << endl;
    cout << create("abc") << endl;
    cout << open("abc") << endl;
    cout << write(1, 0, 400) << endl;
    cout << write(1, 0, 100) << endl;
    cout << write(1, 0, 100) << endl;
}


int main() {

    cout << "---=== TESTING ===---" << endl;
    // sample_test_case();
    // full_file_test();
    actual_submission();

    return 0;
}
