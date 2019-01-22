#include <SolexOs/utils/Crc32.hpp>
#include <SolexOs/utils/MagicCrc16.hpp>

namespace SolexOs {
  constexpr Crc32Lookup Crc32::crcTable;
  constexpr uint16_t MagicCrc16::crcTable[16];
}
