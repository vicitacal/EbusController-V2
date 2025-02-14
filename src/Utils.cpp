#include <Utils.h>

String to_binary(uint16_t value) {
    char buffer[17];
    for (int i = 15; i >= 0; --i) {
        buffer[15 - i] = (value & (1 << i)) ? '1' : '0';
    }
    buffer[16] = '\0';
    return String(buffer);
}

String to_hex(uint16_t value) {
    char buffer[7];
    std::snprintf(buffer, sizeof(buffer), "0x%04X", value);
    return String(buffer);
}
