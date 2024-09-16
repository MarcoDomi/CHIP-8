#pragma once

#include <iostream>
#include <cstdint>
#include <fstream>
#include <chrono>
#include <random>


class chip8
{
public:
    uint8_t registers[16]{};
    uint8_t memory[4096]{}; // curly braces initializes all elements to 0
    uint16_t index{};
    uint16_t pc{}; // curly braces initializes variable to 0
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