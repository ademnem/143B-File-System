#include "file_system.hpp"
#include <iostream>


int Disk::getFreeBlock() {
    for (int i = 8; i < 512; ++i)
        if (blocks[0][i] == byte{0})
            return i;

    return -1;
}

FileDescriptor* Disk::getDirFileDescriptor() {
    return (FileDescriptor*)&(blocks[1][0]);
}
FileDescriptor* Disk::getFileDescriptor(int index) {
    return (FileDescriptor*)&(blocks[(index / 512) + 1][index % 512]);
}
int Disk::getFreeFileDescriptor() {
    for (int b = 1; b <= 6; ++b) {
        for (int i = sizeof(FileDescriptor); i < 6 * 512; i += sizeof(FileDescriptor)) {
            FileDescriptor* fd = (FileDescriptor*)&(blocks[b][i % 512]);
            if (fd->length == 0 && fd->blocks[0] == 0)
                return i;
        }
    }

    return -1;
}

FileEntry* Disk::getFileEntry(const string &name) { 
    FileDescriptor* dir = getDirFileDescriptor();

    for (int i = 0; i < 3 * 512 && dir->blocks[i / 512]; i += sizeof(FileEntry)) {
        int block = dir->blocks[i / 512];
        int index = (i % 512);
        FileEntry* fe = (FileEntry*)&(blocks[block][index]);

        if (string(fe->name) == name)
            return fe;
    }  
    return nullptr;
}
FileEntry* Disk::getFreeFileEntry() {
    FileDescriptor* dir = getDirFileDescriptor();

    for (int i = 0; i < 3 * 512 && dir->blocks[i / 512]; i += sizeof(FileEntry)) {
        int block = dir->blocks[i / 512];
        int index = (i % 512);
        FileEntry* fe = (FileEntry*)&(blocks[block][index]);

        if (!fe->name[0])
            return fe;
    }  
    return nullptr;
}



void Disk::initialize() {
    // set all bytes to 0
    for (int b = 0; b <= 63; ++b)
        for (int i = 0; i < 512; ++i)
            blocks[b][i] = byte{0};

    // set bitmap
    for (int i = 0; i <= 7; ++i)
        blocks[0][i] = byte{255};

    // directory file descriptor
    FileDescriptor* dir = getDirFileDescriptor();
    dir->length = 0;
    dir->blocks[0] = 7;

    initialized = true;
}
bool Disk::createFile(const string &name) {
    // check if name length is valid
    if (name.length() <= 0 || name.length() >= 4)
        return false;

    // check if filename exists in directory
    FileEntry* fe = getFileEntry(name);
    if (fe)
        return false;

    // check for free blocks
    int freeBlock = getFreeBlock();
    if (freeBlock == -1)
        return false;

    // check for free FileDescriptor
    int freeFileDescriptorIndex = getFreeFileDescriptor(); 
    if (freeFileDescriptorIndex == -1)
        return false;
    
    // check for free FileEntry
    fe = getFreeFileEntry();
    if (!fe)
        return false;

    // initialize FileEntry
    strncpy(fe->name, name.c_str(), 3);
    fe->index = freeFileDescriptorIndex;

    // initialize FileDescriptor
    FileDescriptor* fd = getFileDescriptor(freeFileDescriptorIndex);
    fd->length = 0;
    fd->blocks[0] = freeBlock;
    blocks[0][freeBlock] = byte{255};

    return true;
}
bool Disk::deleteFile(const string &name) {
    // check if name length is valid
    if (name.length() <= 0 || name.length() >= 4)
        return false;

    // check if filename exists in directory
    FileEntry* fe = getFileEntry(name);
    if (!fe)
        return false;

    // get FileDescriptor
    FileDescriptor* fd = getFileDescriptor(fe->index);

    // bitmap 0 for all allocated blocks
    // zero out all values for FileDescriptor    
    for (int b = 0; b < 3 && fd->blocks[b]; ++b) {
        blocks[0][fd->blocks[b]] = byte{0};
        
        for (int i = 0; i < 512 && blocks[fd->blocks[b]][i] != byte{0}; ++i)
            blocks[fd->blocks[b]][i] = byte{0};                 

        fd->blocks[b] = 0;
    }
    fd->length = 0;

    // zero out all values for FileEntry
    for (int i = 0; i < 4 && fe->name[i]; ++i)
        fe->name[i] = '\0';
    fe->index = 0;
    
    return true;
}
string Disk::directoryFiles() {
    string files{};
    FileDescriptor* dir = getDirFileDescriptor();

    for (int i = 0; i < 3 * 512 && dir->blocks[i / 512]; i += sizeof(FileEntry)) {
        int block = dir->blocks[i / 512];
        int index = (i % 512);
        FileEntry* fe = (FileEntry*)&(blocks[block][index]);

        if (fe->name[0]) // if the FileEntry is valid it will have a name and the 0 index will be initialized
            files += fe->name + string(" ") + to_string(getFileDescriptor(fe->index)->length) + string(" ");
    }  

    return files;
}



void initialize() {
    cout << "system initialized" << endl;
}
void create(Disk &disk, const string &name) {
    if (!disk.initialized)
        cout << NOT_INIT_MSG << endl;

    if (disk.createFile(name))
        cout << name << " created" << endl;
    else
        cout << ERROR_MSG << endl;  
}
void destroy(Disk &disk, const string &name) {
    if (!disk.initialized)
        cout << NOT_INIT_MSG << endl;

    if (disk.deleteFile(name))
        cout << name << " created" << endl;
    else
        cout << ERROR_MSG << endl;  
}
void directory(Disk &disk) {
    if (!disk.initialized)
        cout << NOT_INIT_MSG << endl;

    string files = disk.directoryFiles();
    if (files == "")
        cout << "directory empty" << endl;
    else
        cout << files << endl;        
}
