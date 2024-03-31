#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OP_ADD 0x0
#define OP_DEC 0x1
#define OP_AND 0x2
#define OP_XOR 0x3
#define OP_LOAD 0x4
#define OP_HALT 0x5

#define OP_CODE_SHIFT 12
#define DESTINATION_REGISTER_SHIFT 9
#define REGISTER_1_SHIFT 6
#define REGISTER_2_SHIFT 0

#define ISOLATE_FIRST_FIVE_BITS(instruction) (instruction & 0x1F)
#define OP_CODE(instruction) (instruction << OP_CODE_SHIFT)

enum variables {
    v,
    a,
    b,
};

typedef uint16_t word_size;

word_size get_shifted_value(const char *line, const word_size shift) {
    word_size value = 0;

    if (strcmp(line, "v") == 0) {
        value |= v << shift;
    } else if (strcmp(line, "a") == 0) {
        value |= a << shift;
    } else if (strcmp(line, "b") == 0) {
        value |= b << shift;
    }

    return value;
}

word_size parse_and_compile(const char *line) {
    word_size instruction = 0;
    char destination[5];
    char source1[5];
    char source2[5];
    int value;

    if (sscanf(line, "let %s = %d", destination, &value) == 2) {
        instruction |= OP_CODE(OP_LOAD);
        instruction |= get_shifted_value(destination, DESTINATION_REGISTER_SHIFT);
        instruction |= ISOLATE_FIRST_FIVE_BITS(value);
    } else if (sscanf(line, "%s = %s + %s", destination, source1, source2) == 3) {
        instruction |= OP_CODE(OP_ADD);
        instruction |= get_shifted_value(destination, DESTINATION_REGISTER_SHIFT);
        instruction |= get_shifted_value(source1, REGISTER_1_SHIFT);
        instruction |= get_shifted_value(source2, REGISTER_2_SHIFT);
    } else if (sscanf(line, "%s -= %d", destination, &value) == 2) {
        instruction |= OP_CODE(OP_DEC);
        instruction |= get_shifted_value(destination, DESTINATION_REGISTER_SHIFT);
        instruction |= get_shifted_value(destination, REGISTER_1_SHIFT);
        instruction |= ISOLATE_FIRST_FIVE_BITS(value);
    } else if (sscanf(line, "%s = %s & %s", destination, source1, source2) == 3) {
        instruction |= OP_CODE(OP_AND);
        instruction |= get_shifted_value(destination, DESTINATION_REGISTER_SHIFT);
        instruction |= get_shifted_value(source1, REGISTER_1_SHIFT);
        instruction |= get_shifted_value(source2, REGISTER_2_SHIFT);
    } else if (sscanf(line, "%s = %s ^ %s", destination, source1, source2) == 3) {
        instruction |= OP_CODE(OP_XOR);
        instruction |= get_shifted_value(destination, DESTINATION_REGISTER_SHIFT);
        instruction |= get_shifted_value(source1, REGISTER_1_SHIFT);
        instruction |= get_shifted_value(source2, REGISTER_2_SHIFT);
    } else if (strcmp(line, "halt") == 0) {
        instruction |= OP_CODE(OP_HALT);
    } else {
        fprintf(stderr, "Error: Unsupported operation\n");
        exit(EXIT_FAILURE);
    }

    return instruction;
}

void write_instruction_to_file(word_size instruction, FILE *file) {
    fwrite(&instruction, sizeof(instruction), 1, file);
}

void start() {
    const char *source_files[] = {
            "/home/mykyta/uni/crossplatform-programming/compiler/source_files/sc_add.txt",
            "/home/mykyta/uni/crossplatform-programming/compiler/source_files/sc_and.txt",
            "/home/mykyta/uni/crossplatform-programming/compiler/source_files/sc_dec.txt",
            "/home/mykyta/uni/crossplatform-programming/compiler/source_files/sc_xor.txt",
            "/home/mykyta/uni/crossplatform-programming/compiler/source_files/complex.txt",
    };
    const char *binary_files[] = {
            "/home/mykyta/uni/crossplatform-programming/compiler/binary_files/sc_add.bin",
            "/home/mykyta/uni/crossplatform-programming/compiler/binary_files/sc_and.bin",
            "/home/mykyta/uni/crossplatform-programming/compiler/binary_files/sc_dec.bin",
            "/home/mykyta/uni/crossplatform-programming/compiler/binary_files/sc_xor.bin",
            "/home/mykyta/uni/crossplatform-programming/compiler/binary_files/complex.bin",
    };

    for (int i = 0; i < 5; i++) {
        printf("***************\n");
        printf("Source file: %s\n", source_files[i]);
        printf("Binary file: %s\n", binary_files[i]);

        FILE *source_file = fopen(
                source_files[i],
                "r"
        );

        if (!source_file) {
            perror("Error opening source file");

            return;
        }

        FILE *binary_file = fopen(
                binary_files[i],
                "wb"
        );

        if (!binary_file) {
            perror("Error opening binary file");
            fclose(source_file);

            return;
        }

        char line[256];

        printf("Compilation started.\n");
        printf("----------------\n");

        while (fgets(line, sizeof(line), source_file)) {
            printf("Compiling: %s", line);

            word_size instruction = parse_and_compile(line);

            write_instruction_to_file(instruction, binary_file);

            memset(line, 0, sizeof(line));

            printf("Compiled instruction: %04X\n", instruction);
            printf("----------------\n");
        }

        fclose(source_file);
        fclose(binary_file);

        printf("Compilation finished.\n");
    }
}

int main() {
    start();

    return 0;
}
