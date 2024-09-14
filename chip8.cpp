#include<iostream>
#include<cstdint>
#include<fstream>
#include<chrono>
#include<random>



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
class chip8{
    public:
        uint8_t registers[16]{};
        uint8_t memory[4096]{}; //curly braces initializes all elements to 0
        uint16_t index{};
        uint16_t pc{}; //curly braces initializes variable to 0
        uint16_t stack[16]{};
        uint8_t sp{};
        uint8_t delayTimer{};
        uint8_t soundTimer{};
        uint8_t keypad[16]{};
        uint32_t video[64 * 32]{};
        uint16_t opcode;

        std::default_random_engine randGen;
        std::uniform_int_distribution<uint8_t> randByte;

        chip8();
        void LoadROM(char const *);
};

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

int main(){
  
}