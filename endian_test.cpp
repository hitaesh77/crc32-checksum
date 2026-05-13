#include <iostream>
#include <cstdint>

// function to test if memory stored on computer is big endian or little endian
// if little endian, will return 0x01
// if big endian, will return 0x00
bool isLittleEndian() {
    uint16_t value = 0x0001;
    const uint8_t* bytes = (const uint8_t*) &value;

    // for (size_t i = 0; i < sizeof(value); i++) {
    //     printf("%02X \n", bytes[i]);
    // }

    return (*(uint8_t*)&value == 0x01);
}

int main() {
    if (isLittleEndian()) {
        std::cout << "Little Endian" << std::endl;
    } else {
        std::cout << "Big Endian" << std::endl;
    }
    return 0;
}
