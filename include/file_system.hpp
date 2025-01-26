#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

#include <cstring>
#include <string>

#define ERROR_MSG "error"
#define NOT_INIT_MSG "system not intiailized"

using namespace std;


struct OpenFile {
    byte buffer[512];
    int pos; // current position
    int size; // size of file
    int index; // descriptor index
};

struct OpenFileTable {
    OpenFile OFT[4];
    // 0 is always set to directory
};

struct FileDescriptor {
    int length; // in bytes
    int blocks[3]; // 0 if not allocated
};

struct FileEntry {
    char name[4];
    int index; // in bytes
};

struct Disk {
    bool initialized = false;
    byte blocks[64][512];

    int getFreeBlock();

    FileDescriptor* getDirFileDescriptor();
    FileDescriptor* getFileDescriptor(int index);
    int getFreeFileDescriptor();

    FileEntry* getFileEntry(const string &name);
    FileEntry* getFreeFileEntry();

    void initialize();
    bool createFile(const string &name);
    bool deleteFile(const string &name);

    string directoryFiles();
};



void initialize(Disk &disk);
void create(Disk &disk, const string &name);
void destroy(Disk &disk, const string &name);

void open(Disk &disk, const string &name);
void close(Disk &disk, const string &name);

void directory(Disk &disk);

void read(int index, int mem, int count);
void write(Disk &disk, int index, int mem, int count);
void seek(Disk &dist, int index, int pos);

void read_memory();
void write_memory();


#endif
