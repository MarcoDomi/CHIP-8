#include "chip8.h"
#include<string.h>

const unsigned int FONTSET_SIZE = 80;
uint8_t fontset[FONTSET_SIZE] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

const unsigned int START_ADDRESS = 0x200;

const unsigned int FONTSET_START_ADDRESS = 0x50;
chip8::chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()){
    pc = START_ADDRESS;

    // Load fonts into memory
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i){
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
}

void chip8::LoadROM(char const* filename){
    // Open the file as a stream of binary and move the file pointer to the end
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        // Get size of file and allocate a buffer to hold the contents
        std::streampos size = file.tellg();
        char *buffer = new char[size];

        // Go back to the beginning of the file and fill the buffer
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        // Load the ROM contents into the Chip8's memory, starting at 0x200
        for (long i = 0; i < size; ++i){
            memory[START_ADDRESS + i] = buffer[i];
        }

        // Free the buffer
        delete[] buffer;
    }
}

//clear the display
void chip8::OP_00E0(){
    memset(video, 0, sizeof(video));
}

//return from subroutine //overwrites the premptive pc += 2
void chip8::OP_00EE(){
    --sp;
    pc = stack[sp];
}

//jump to location nnn 
//interpreter sets PC to nnn
//jump doesnt remember its origin -> no stack interaction needed
void chip8::OP_1nnn()
{
    uint16_t address = opcode & 0x0FFFu; //extracts bottom 12 bits from opcode which is the desired address

    pc = address;
}

//call subroutine at nnn
void chip8::OP_2nnn(){
    uint16_t address = opcode & 0x0FFFu;

    stack[sp] = pc;
    ++sp;
    pc = address;
}

//skip next instruction if Vx == kk
//bc pc is +2 by cycle() we +2 again to skip next instruction
void chip8::OP_3xkk(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] == byte){
        pc += 2;
    }
}

//skip next instruction if Vx != kk
void chip8::OP_4xkk(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] != byte){
        pc += 2;
    }
}

//skip next instruction if Vx == Vy
void chip8::OP_5xy0(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u; //shift bits to right by 8 -> this places the 4 bits in least significant position which makes it easier to reference register
    uint8_t Vy = (opcode & 0x00F0u) >> 4u; //shift bits to right by 4 -> this places the 4 bits in least significatn position

    if (registers[Vx] == registers[Vy]){
        pc += 2;
    }
}


//assign kk to Vx
void chip8::OP_6xkk(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = byte;
}

//add byte to Vx
void chip8::OP_7xkk(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] += byte;
}

//assign Vy to Vx
void chip8::OP_8xy0(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vy];
}