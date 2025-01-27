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
    OFT[0].size = 0;
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
    if (index != 0 && of.index == 0) // if file is open at index
        return -1;
    else if (pos >= of.size) // if position exceeds file size
        return -1;
 
    // if pos moves to different block
    if (pos / BLOCK_SIZE != of.pos / BLOCK_SIZE) {
        // open file descriptor
        int b = of.index / BLOCK_SIZE + 1;
        FileDescriptor* fd = (FileDescriptor*)&(C[b][of.index % BLOCK_SIZE]);

        // copy buffer into current block
        int currentBlock = fd->blocks[of.pos / BLOCK_SIZE];  
        copy_buffer(D[currentBlock], of.buff, BLOCK_SIZE);
        // copy size into file descriptor
        fd->length = of.size;
        
        // copy new block into buffer
        int newBlock = fd->blocks[pos / BLOCK_SIZE];
        copy_buffer(of.buff, D[newBlock], BLOCK_SIZE);
    } 

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
    for (int i = 0; i < BLOCK_SIZE * 3 && seek(0, i) != -1; i += sizeof(FileEntry)) {
        OpenFile& dir = OFT[0];
        FileEntry* feTemp = (FileEntry*)&(dir.buff[i % BLOCK_SIZE]);
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

    // open file descriptor
    int b = of.index / BLOCK_SIZE + 1;
    FileDescriptor* fd = (FileDescriptor*)&(C[b][of.index % BLOCK_SIZE]);

    // save buffer and update length of file descriptor
    b = fd->blocks[of.pos / BLOCK_SIZE];
    copy_buffer(D[b], of.buff, BLOCK_SIZE);   
    fd->length = of.size;

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

    // check if file exists NEEED TO DO WITH OFT
    FileDescriptor* dirFd = (FileDescriptor*)&(D[1][0]);
    for (int b = 0, i = 0; i < 3 * BLOCK_SIZE && (b = dirFd->blocks[i/BLOCK_SIZE]); i += sizeof(FileEntry)) {
        FileEntry* fe = (FileEntry*)&(D[b][i % BLOCK_SIZE]); 
        if (strncmp(fe->name, name.c_str(), 3) == 0)
            return "error";
    }

    // check for open block
    int openB = 0;
    for (int i = 8; i < BLOCK_SIZE; ++i) {
        if (C[0][i] == 0) {
            openB = i;
            break;
        }
    }
    if (openB < 8) 
        return "error";

    // check for open file descriptor
    int openFD = 0;
    for (int i = sizeof(FileDescriptor); i < 6 * BLOCK_SIZE; i += sizeof(FileDescriptor)) { 
        int b = i / BLOCK_SIZE + 1;
        FileDescriptor* fd = (FileDescriptor*)&(C[b][i % BLOCK_SIZE]);
        if (fd->length == 0 && fd->blocks[0] == 0) {
            openFD = i;
            break;
        }
    }
    if (openFD == 0)
        return "error";

    // NEED TO DO WITH OFT
    // check dir for open FileEntry 
    FileEntry* fe = nullptr;
    for (int i = 0; i < BLOCK_SIZE * 3 && seek(0, i) != -1; i += sizeof(FileEntry)) {
        OpenFile& dir = OFT[0];
        FileEntry* feTemp = (FileEntry*)&(dir.buff[i % BLOCK_SIZE]);
        if (strncmp(feTemp->name, name.c_str(), 3) == 0) {
            fe = feTemp;
            break;
        }
    }
    if (!fe) // check if FileEntry exists
        return "error";   
    
    // CREATE THE FILE NOW

    return name + " created";
}
string destroy(const string &name) {
    return name + " destroyed";
}

