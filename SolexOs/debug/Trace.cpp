#include <SolexOs/Debug.h>
#include <iostream>
#include <cstring>

extern "C" {
  uint8_t convertToHex(uint32_t num);
  uint32_t strLength(const uint8_t *string, uint32_t maxLen);

  uint8_t convertToHex(uint32_t num) {
    num = num & 0xF;
    return static_cast<uint8_t>((num > 9) ? 'A' + num - 10 : '0' + num);
  }

  uint32_t strLength(const uint8_t *string, uint32_t maxLen) {
    uint32_t len = 0;
    while ((*string != 0) && (len < maxLen)) {
      ++string;
      len++;
    }
    return len;
  }

  void DBG_SendString(uint32_t, uint32_t, const uint8_t* funcName, const uint8_t* msg) {
    std::cout << reinterpret_cast<const char *>(funcName) << ":" << reinterpret_cast<const char *>(msg);
  }

  void DBG_SendNumber(uint32_t, uint32_t, uint8_t size, uint64_t num) {
    constexpr auto MAX_DIGITS = 16 + 1;
    uint32_t digits = size * 2;
    if( digits >= MAX_DIGITS ) {
      digits = MAX_DIGITS - 1;
    }
    uint8_t buffer[MAX_DIGITS];
    for( uint32_t i = 0; i < digits; ++i ) {
      buffer[i] = convertToHex((num >> (digits - 1 - i) * 4) & 0xF);
    }
    buffer[digits] = 0;
    std::cout << reinterpret_cast<const char *>(buffer);
  }

  void DBG_SendBuffer(uint32_t, uint32_t, uint32_t len, const uint8_t* buff) {
    uint8_t buffer[16 * 2 + 2];
    while (len > 0) {
      uint32_t segment = len > 16 ? 16 : len;
      uint8_t *dest = &buffer[0];
      for( uint32_t i = 0; i < segment; ++i ) {
        *dest = convertToHex(*buff >> 4);
        dest++;
        *dest = convertToHex(*buff);
        dest++;
        buff++;
      }
      *dest = ' ';
      dest++;
      *dest = 0;
      std::cout << reinterpret_cast<const char *>(buffer);
      len = len - segment;
    }
  }

  void DBG_Space(uint32_t, uint32_t) {
    std::cout << " ";
  }

  void DBG_SendRawString(uint32_t, uint32_t, const char * str) {
    std::cout << str;
  }

  /**
   * Don't allow setting from the library.
   */
  void DBG_ConfigDebug(uint32_t) {
  }
}
