#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

#include <cstring>
#include <string>

#define BLOCKS 64
#define BLOCK_SIZE 512

// for bitmap
#define BYTE_ZERO 0;
#define BYTE_ONE 255;

using namespace std;


struct OpenFile {
    unsigned char buff[BLOCK_SIZE];
    int pos; // current position
    int size; // length of file
    int index; // descriptor index 0 <= index < 1536
};
struct FileDescriptor {
    int length; // in bytes
    int blocks[3]; // 0 if not allocated
};
struct FileEntry {
    char name[4];
    int index; // in bytes
};

extern OpenFile OFT[4];
extern unsigned char M[BLOCK_SIZE];
extern unsigned char C[7][BLOCK_SIZE]; 
extern unsigned char D[BLOCKS][BLOCK_SIZE];
extern bool init;

// helpers
void copy_buffer(unsigned char *dest, unsigned char *source, int n);

// file system
string initialize();

int write_memory(const int &mem, const string &str);
string read_memory(const int &mem, const int &count);

int seek(const int &index, const int &pos);

int open(const string &name);
int close(const int &index);

string create(const string &name);
string destroy(const string &name);

void directory();

void read(const int &index, const int &mem, const int &count); 
void write(const int &index, const int &mem, const int &count); // if full, store blocks, allocate new block

void save(ofstream *file);
void restore(ifstream *file);

#endif
