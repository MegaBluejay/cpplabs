#include <iostream>

#include "cube.hpp"
#include <bit>
#include <bitset>

int main() {
    uint16_t a = 1;
    uint8_t& b = *(reinterpret_cast<uint8_t*>(&a)+1);
    b = 1;
    b = std::rotr(b, 1);
    std::cout << std::bitset<16>(a) << std::endl;
    std::cout << a << std::endl;
}
