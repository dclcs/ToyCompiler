//
// Created by cl d on 2024/7/10.
//

#ifndef TOYCOMPILER_CHUNK_H
#define TOYCOMPILER_CHUNK_H

#include "../common.h"

class Chunk {
    int count;
    int capacity;
    uint8_t* code;
    int* lines;

    Chunk();
    void writeChunk(uint8_t byte, int line);

};


#endif //TOYCOMPILER_CHUNK_H
