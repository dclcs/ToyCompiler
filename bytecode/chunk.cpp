//
// Created by cl d on 2024/7/10.
//

#include "chunk.h"

Chunk::Chunk() {
    this->code = nullptr;
    this->lines = nullptr;
    this->count = 0;
    this->capacity = 0;
}

void Chunk::writeChunk(uint8_t byte, int line) {

}
