#include <iostream>
#include "core/vm.h"

static char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*) malloc(fileSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }
    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

static void repl(VM* vm) {
    char line[1024];
    for(;;) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        vm->interpret(line);
    }
}

static void runFile(const char* path, VM* vm) {
    char* source = readFile(path);
    InterpretResult result = vm->interpret(source);
    if (result == INTERPRET_COMPILE_ERROR) exit(65);
    if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int argc, const char* argv[]) {
    VM *vm = new VM();
    if (argc == 1) {
        repl(vm);
    } else if (argc == 2) {
        runFile(argv[1], vm);
    } else {
        fprintf(stderr, "Usage: clox [path]\n");
        exit(64);
    }

    vm->freeVM();
    return 0;
}
