#include <iostream>
#include <string>

#include "file_system.hpp"

using namespace std;



using FunctionPointer = bool (*)();

void test(string name, FunctionPointer function) {
    cout << "Testing: " << name << "...";
    cout << (function() ? "OK" : "FAILED") << endl;
}



bool test_initialize() {
    Disk disk{}; 
    disk.initialize(); 
    
    for (int i = 0; i <= 511; ++i) {
        if (i >= 8 && disk.blocks[0][i] != byte{0}) 
            return false;
        else if (i <= 7 && disk.blocks[0][i] == byte{0})
            return false;
    }

    for (int b = 8; b <= 63; ++b) {
        for (int i = 0; i <= 511; ++i)
            if (disk.blocks[b][i] != byte{0})
                return false;
    }

    FileDescriptor* fd = (FileDescriptor*)&(disk.blocks[1][0]);
    if (fd->length != 0)
        return false;
    if (fd->blocks[0] != 7)
        return false;

    return true;
}
bool test_createFile() {
    Disk disk{};
    disk.initialize();
    
    if (disk.createFile("not valid") || disk.createFile(""))
        return false;

    disk.createFile("foo");  
    disk.createFile("bar");

    FileEntry* fe_foo = disk.getFileEntry("foo"); 
    if (strcmp(fe_foo->name, "foo") != 0) 
        return false;
    FileDescriptor* fd_foo = disk.getFileDescriptor(fe_foo->index);
    if (fd_foo->length != 0 || fd_foo->blocks[0] == 0)
        return false;
     
    FileEntry* fe_bar = disk.getFileEntry("bar"); 
    if (strcmp(fe_bar->name, "bar") != 0) 
        return false;
    FileDescriptor* fd_bar = disk.getFileDescriptor(fe_foo->index);
    if (fd_bar->length != 0 || fd_bar->blocks[0] == 0)
        return false;

    return true;
}
bool test_deleteFile() {
    Disk disk{};
    disk.initialize();

    disk.createFile("foo");  
    disk.createFile("bar");

    if (disk.deleteFile("foob") || disk.deleteFile(""))
        return false;

    FileEntry *fe_foo = disk.getFileEntry("foo");
    FileDescriptor *fd_foo = disk.getFileDescriptor(fe_foo->index);

    if (!disk.deleteFile("foo"))
        return false;
    /*
    cout << fe_foo->index << endl;
    cout << fe_foo->name << endl;
    cout << fd_foo->length << endl;
    cout << fd_foo->blocks[0] << endl;
    cout << fd_foo->blocks[1] << endl;
    cout << fd_foo->blocks[2] << endl;
    */

    if (fe_foo->index != 0 || strncmp(fe_foo->name, "", 3) != 0 ||
        fd_foo->length != 0 || fd_foo->blocks[0] != 0 ||
        fd_foo->blocks[1] != 0 || fd_foo->blocks[2] != 0)
        return false;
    
    FileEntry* fe_bar = disk.getFileEntry("bar"); 
    if (strcmp(fe_bar->name, "bar") != 0) 
        return false;
    FileDescriptor* fd_bar = disk.getFileDescriptor(fe_foo->index);
    if (fd_bar->length != 0 || fd_bar->blocks[0] == 0)
        return false;

    return true;
}
bool test_directoryFiles() {
    Disk disk{};
    disk.initialize();

    string files = disk.directoryFiles();
    if (files != "")
        return false;

    disk.createFile("foo");  
    disk.createFile("bar");
    files = disk.directoryFiles();
    if (files == "foo 0 bar 0")
        return false;

    return true;
}


int main() {

    cout << "---=== TESTING DISK ===---" << endl;
    test("Disk::initialize", test_initialize);
    test("Disk::createFile", test_createFile);
    test("Disk::deleteFile", test_deleteFile);
    test("Disk::directoryFiles", test_directoryFiles);

    return 0;
}
