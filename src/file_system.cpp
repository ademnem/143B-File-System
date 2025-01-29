#include <iostream> // for debugging

#include "file_system.hpp"

OpenFile OFT[4] = {0};
unsigned char M[BLOCK_SIZE] = {0};
unsigned char C[7][BLOCK_SIZE] = {0};
unsigned char D[BLOCKS][BLOCK_SIZE] = {0};
bool init = false;


void copy_buffer(unsigned char *dest, unsigned char *source, int n) {
    for (int i = 0; i < n; ++i) {
        dest[i] = source[i];
    }
}
int get_open_block() {
    for (int i = 8; i < BLOCK_SIZE; ++i) {
        if (C[0][i] == 0) {
            C[0][i] = BYTE_ONE;            
            return i;
        }
    }        
    return -1;
}
FileDescriptor* get_file_descriptor(int index) {
    return (FileDescriptor*)&(C[index / BLOCK_SIZE + 1][index % BLOCK_SIZE]);
}


string initialize() {

    // initialize disk
    for (int b = 0; b < BLOCKS; ++b) 
        for (int i = 0; i < BLOCK_SIZE; ++i)
            D[b][i] = 0;

    /// initialize bitmap
    for (int i = 0; i < 8; ++i)
        D[0][i] = BYTE_ONE;
 
    // initialize directory file descriptor
    FileDescriptor* dirFD = (FileDescriptor*)&(D[1][0]);
    dirFD->length = 0;
    dirFD->blocks[0] = 7;
  
    // uninitialized file descriptor size = -1
    for (int i = sizeof(FileDescriptor); i < 6 * BLOCK_SIZE; i += sizeof(FileDescriptor)) {
        FileDescriptor* fd = (FileDescriptor*)&(D[i / BLOCK_SIZE + 1][i % BLOCK_SIZE]);
        fd->length = -1;
    }
    
    // initialize cache
    for (int b = 0; b < 8; ++b)
        for (int i = 0; i < BLOCK_SIZE; ++i)
            C[b][i] = D[b][i];

    // initialize memory
    for (int i = 0; i < BLOCK_SIZE; ++i)
        M[i] = 0;

    // initialize open file table
    for (int f = 0; f < 4; ++f) {
        OFT[f].pos = 0; 
        OFT[f].size = 0; 
        OFT[f].index = 0; 
        for (int i = 0; i < BLOCK_SIZE; ++i)
            OFT[f].buff[i] = 0;
    }
    
    // open dir
    OFT[0].pos = 0;  
    OFT[0].size = BLOCK_SIZE;
    OFT[0].index = 0;
    for (int i = 0; i < BLOCK_SIZE; ++i)  
        OFT[0].buff[i] = D[dirFD->blocks[0]][i];

    init = true;

    return "system initilaized";
}
int write_memory(const int &mem, const string &str) {
    if (!init)
        return 0;

    if (mem < 0 || mem + str.length() - 1 >= BLOCK_SIZE) // read out of bounds
        return 0;
             
    strncpy((char*)(M + mem), str.c_str(), str.length());
    
    return str.length();
}
string read_memory(const int &mem, const int &count) {
    if (!init)
        return "error";

    if (mem < 0 || mem + count - 1 >= BLOCK_SIZE) // read out of bounds
        return "error";

    char *str = new char[count + 1]; 
    strncpy(str, (char*)(M + mem), count);
    str[count] = '\0';
    
    string result{str};
    delete[] str;

    return result; 
}
int seek(const int &index, const int &pos) {
    if (!init)
        return -1;

    if (index < 0 || index > 4)
        return -1;

    OpenFile& of = OFT[index];
    if (index != 0 && of.index == 0) // no open file at index
        return -1;
    else if (pos >= of.size) // if position exceeds file size
        return -1;
 
    // get file descriptor
    int b = of.index / BLOCK_SIZE + 1;
    FileDescriptor* fd = (FileDescriptor*)&(C[b][of.index % BLOCK_SIZE]);
 
    // copy new block into buffer
    int newBlock = fd->blocks[pos / BLOCK_SIZE];
    copy_buffer(of.buff, D[newBlock], BLOCK_SIZE);
    of.pos = pos; 

    return pos;
}
int open(const string &name) {
    if (!init)
        return -1;

    // check filename length
    if (name.length() < 0 || name.length() > 3)
        return -1;
    // get open OFT index
    int tableIndex = 0;
    for (int i = 1; i < 4; ++i) {
        if (OFT[i].index == 0) {
            tableIndex = i;
            break;
        }
    }
    if (tableIndex == 0) // check if open index exists
        return -1;

    // get filename FileEntry
    FileEntry* fe =  nullptr;
    // search through directory 
    for (int i = 0; seek(0, i) != -1; i += sizeof(FileEntry)) {
        OpenFile& dir = OFT[0];
        FileEntry* feTemp = (FileEntry*)&(dir.buff[dir.pos % BLOCK_SIZE]);
        if (strncmp(feTemp->name, name.c_str(), 3) == 0) {
            fe = feTemp;
            break;
        }
    }
    if (!fe) // check if FileEntry exists
        return -1;   

    // get FileDescriptor
    int fdIndex = fe->index;
    int b = fdIndex / BLOCK_SIZE + 1;
    FileDescriptor* fd = (FileDescriptor*)&(C[b][fdIndex % BLOCK_SIZE]);

    // copy first block of file to OFT entry
    b = fd->blocks[0];
    copy_buffer(OFT[tableIndex].buff, D[b], BLOCK_SIZE);
    OFT[tableIndex].pos = 0;
    OFT[tableIndex].size = fd->length;
    OFT[tableIndex].index = fe->index;

    return tableIndex;    
}
int close(const int &index) {
    if (!init)
        return -1;

    // check if index is in bounds
    if (index < 1 || index > 3)
        return -1;

    // check if index is open
    OpenFile &of = OFT[index];
    if (of.index == 0)
        return -1;

    // reset OFT entry
    unsigned char empty[BLOCK_SIZE] = {0};  
    copy_buffer(of.buff, empty, BLOCK_SIZE);
    OFT[index].pos = 0;
    OFT[index].size = 0;
    OFT[index].index = 0;

    return index;
}
string create(const string &name) {
    if (!init)
        return "error";
    
    if (name.length() < 0 || name.length() > 3)
        return "error";

    // check if file exists
    for (int i = 0; seek(0, i) != -1; i += sizeof(FileEntry)) {
        OpenFile& dir = OFT[0]; 
        FileEntry* fe = (FileEntry*)&(dir.buff[dir.pos % BLOCK_SIZE]);
        if (strncmp(fe->name, name.c_str(), 3) == 0)
            return "error: exists";
    }

    // check for open file descriptor
    int openFD = 0;
    for (int i = sizeof(FileDescriptor); i < 6 * BLOCK_SIZE; i += sizeof(FileDescriptor)) { 
        int b = i / BLOCK_SIZE + 1;
        FileDescriptor* fd = (FileDescriptor*)&(C[b][i % BLOCK_SIZE]);
        if (fd->length == -1) {
            openFD = i;
            break;
        }
    }
    if (openFD == 0)
        return "error";

    // check dir for open FileEntry 
    FileEntry* fe = nullptr;
    int feIndex = 0;
    for (int i = 0; seek(0, i) != -1; i += sizeof(FileEntry)) {
        OpenFile& dir = OFT[0]; 
        FileEntry* feTmp = (FileEntry*)&(dir.buff[dir.pos % BLOCK_SIZE]);
        if (*(feTmp->name) == '\0') {
            fe = feTmp;
            feIndex = i;
            break;
        }
    }
    if (!fe)
        return "error";
    // try to expand directory
    FileDescriptor* dir = (FileDescriptor*)&(C[1][0]);
    if (!fe) { // check if FileEntry space exists (need to expand directory)     
        int openB;
        if (dir->blocks[1] == 0) { 
            openB = get_open_block();
            dir->blocks[1] = openB;
            dir->length = 2 * BLOCK_SIZE;
            seek(0, BLOCK_SIZE);
            fe = (FileEntry*)&(OFT[0].buff[OFT[0].pos % BLOCK_SIZE]);
        } else if (dir->blocks[2] == 0) {
            openB = get_open_block();
            dir->blocks[2] = openB;
            dir->length = 3 * BLOCK_SIZE;
            seek(0, 2 * BLOCK_SIZE);
            fe = (FileEntry*)&(OFT[0].buff[OFT[0].pos % BLOCK_SIZE]);
        } else {
            return "error";
        }
    }
    
    // CREATE THE FILE NOW
    strncpy(fe->name, name.c_str(), 3);
    fe->index = openFD;
    FileDescriptor* fd = (FileDescriptor*)&(C[openFD / BLOCK_SIZE + 1][openFD % BLOCK_SIZE]);
    fd->length = 0;

    // copy dir buff to D[]
    copy_buffer(D[dir->blocks[feIndex / BLOCK_SIZE]], OFT[0].buff, BLOCK_SIZE);

    return string{name} + " created";
}
string destroy(const string &name) {
    if (!init)
        return "error";
    
    if (name.length() < 0 || name.length() > 3)
        return "error";

    // check if file exists
    FileEntry* fe = nullptr;
    int feIndex = 0;
    for (int i = 0; seek(0, i) != -1; i += sizeof(FileEntry)) {
        OpenFile& dir = OFT[0]; 
        FileEntry* feTmp = (FileEntry*)&(dir.buff[dir.pos % BLOCK_SIZE]);
        if (strncmp(feTmp->name, name.c_str(), 3) == 0) {
            fe = feTmp;
            feIndex = i;
            break;
        }
    }
    if (!fe)
        return "error";
    
    // check if file open (can't be opened to destroy)
    for (int i = 1; i < 4; ++i) {
        if (OFT[i].index == fe->index)
            return "error";
    }

    // destroy
    FileDescriptor* fd = (FileDescriptor*)&(C[fe->index / BLOCK_SIZE + 1][fe->index % BLOCK_SIZE]);
    unsigned char empty[BLOCK_SIZE] = {0}; 
    for (int i = 0; i < 3; ++i) {
        if (fd->blocks[i]) { // check if block allocated
            copy_buffer(D[fd->blocks[i]], empty, BLOCK_SIZE); // wipe blocks
            C[0][fd->blocks[i]] = BYTE_ZERO; // zero on bitmap
        }
        fd->blocks[i] = 0; // set allocated block to zero
    }
    fd->length = -1; // reset length of FileDescriptor
    for (int i = 0; i < 4; ++i) // reset name of FileEntry
        fe->name[i] = '\0';
    fe->index = 0; // reset index of FileEntry

    // copy dir buff to D[]
    FileDescriptor* dir = (FileDescriptor*)&(C[1][0]);
    copy_buffer(D[dir->blocks[feIndex / BLOCK_SIZE]], OFT[0].buff, BLOCK_SIZE);

    return name + " destroyed";
}
string directory() {
    if (!init)
        return "error";

    string output{};
    for (int i = 0; seek(0, i) != -1; i += sizeof(FileEntry)) {
        OpenFile& dir = OFT[0]; 
        FileEntry* fe = (FileEntry*)&(dir.buff[dir.pos % BLOCK_SIZE]);
        if (*(fe->name)) {
            FileDescriptor* fd = get_file_descriptor(fe->index);
            output += string{fe->name} + ' ' + to_string(fd->length) + ' ';
        }
    }
    return output;
}
string read(const int &index, const int &mem, const int &count) {
    if (!init)
        return "error";
    
    OpenFile& of = OFT[index];
    if (of.index == 0 || mem < 0 || mem >= BLOCK_SIZE)
        return "error";

    if (of.pos + count >= of.size || mem + count >= BLOCK_SIZE)
        return "error";

    int currPos = of.pos;
    for (int i = 0; i < count; ++i) {
        M[mem + i] = of.buff[currPos + i];
    }
    of.pos = currPos + count;

    return to_string(count) + " bytes read from " + to_string(index);
}
string write(const int &index, const int &mem, const int &count) { // if full, store blocks, allocate new block
    if (!init)
        return "error";

    // check if index is open
    OpenFile& of = OFT[index];
    if (of.index == 0)
        return "error";
    
    // check won't index out of memory
    if (mem + count >= BLOCK_SIZE)
        return "error";

    FileDescriptor* fd = get_file_descriptor(of.index);
    if (of.pos + count >= of.size) {
        if (of.size >= 3 * BLOCK_SIZE) {
            return "error";
        } else {
            for (int i = 0; i < 3; ++i) { // allocate more space
                if (fd->blocks[i] == 0) {
                    int openB = get_open_block();  
                    fd->blocks[i] = openB; 
                    break;
                }
            }
        }
    }

    // this can go onto another buffer which has to be saved
    // need to save as its writing
    int currPos = of.pos;
    for (int i = 0; i < count; ++i) { 
        of.buff[currPos + i] = M[mem + i]; // write from M to OFT
        int b = fd->blocks[of.pos / BLOCK_SIZE];
        D[b][currPos + i % BLOCK_SIZE] = M[mem + i]; // write from OFT to D
    }
    of.pos = currPos + count;

    if (of.pos > of.size) {
        of.size = of.pos;
        fd->length = of.pos;
    }
      
    return to_string(count) + " bytes written to " + to_string(index);
}
