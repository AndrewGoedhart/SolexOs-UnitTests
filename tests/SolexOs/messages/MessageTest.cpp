#include <SolexOs/messaging/Message.hpp>
#include <SolexOs/datastructures/ByteArray.hpp>
#include <SolexOs/utils/Crc32.hpp>
#include <drivers/CrcDriver.hpp>
#include <tasks/position/MsgInterfacePosition.hpp>
#include <utils/elements.hpp>
#include <testFramework/memoryLeaks.hpp>
#include <gtest/gtest.h>

namespace SolexOs {

  static constexpr Address SOURCE = Address(NODE_ID::FIRST_ALLOCATED_ID, TaskId::fromTaskOffset(0));
  static constexpr Address DESTINATION = Address(NodeId::NETWORK_BROADCAST(), TaskId::BROADCAST());
  static constexpr uint32_t HEADER = static_cast<uint16_t>(MessageId::POSITION_MEASUREMENT) << 22
      | DESTINATION.getBinary() << 11 | SOURCE.getBinary();

  // constants used for packet delinearation
  static constexpr uint8_t STX = 0x1Eu;
  static constexpr uint8_t ETX = 0x1Fu;



  static ByteArray createPayload() {
    auto data = ByteArray(10);
    auto stream = data.getWriteStream();
    for (uint8_t i = 0; i < 10; i++) {
      stream.write(static_cast<uint8_t>(i + 1));
    }
    return data;
  }

  static Message createTestMessage() {
    auto data = createPayload();
    Message msg = MSG_POSITION_MEASUREMENT::createMessage(0x12345678u,static_cast<uint8_t>(1u),PositionType::INVALID, 2u, 3u, data );
    msg.setSourceAddress(SOURCE);
    msg.setDestinationAddress(DESTINATION);
    return msg;
  }



  /**
   * Test that the message expands to the following format
   *    STX|LEN|ROUTE|PAYLOAD|CRC|ETX
   *
   *   Route is a packed field with
   * 00-10 SourceAddress
   *    0-4  TaskID
   *    5-10 NodeId
   * 11-21 DestinationAddress
   *    11-15  TaskID
   *    16-21  NodeId
   * 22-31 MessageId
   */
  TEST(MessageTest, testNetworkBytes) {
    checkForLeaks();
    Message msg = createTestMessage();
    auto data = createPayload();
    auto result = msg.getNetworkBytes();
    ReadStream stream = result.getReadStream();
    uint32_t expectedLength = msg.getLength()+11-4;

    EXPECT_EQ(result.getLength(), expectedLength);
    EXPECT_EQ(read<uint8_t>(stream), STX);
    EXPECT_EQ(read<uint8_t>(stream), expectedLength - 3);
    EXPECT_EQ(read<uint32_t>(stream), HEADER);
    EXPECT_EQ(read<Seconds>(stream), 0x12345678u);
    EXPECT_EQ(read<uint8_t>(stream), 1);
    EXPECT_EQ(read<PositionType>(stream), PositionType::INVALID);
    EXPECT_EQ(read<Reading>(stream), 2);
    EXPECT_EQ(read<Reading>(stream), 3);
    ByteArray readData(10);
    stream.read(readData);
    EXPECT_EQ(readData, data);
    auto crc = Crc32::computeCrc(result.getReadStream(1), expectedLength - 6);
    EXPECT_EQ(read < uint32_t > (stream), crc);
    EXPECT_EQ(read < uint8_t > (stream), ETX);
  }


  /**
   * Test that we can decode a network encoded packet
   *    STX|LEN|ROUTE|PAYLOAD|CRC|ETX
   */
  TEST(MessageTest, testConvertNetworkBytes) {
    checkForLeaks();
    Message msg = createTestMessage();
    auto raw = msg.getNetworkBytes();
    auto result = Message::convertNetworkBytes(raw);

    EXPECT_TRUE(result.isValid());
    EXPECT_EQ(result, msg);
  }


  /**
   * Test that we can decode a network encoded packet with garbage before start
   *    STX|LEN|ROUTE|PAYLOAD|CRC|ETX
   */
  TEST(MessageTest,testResync) {
    checkForLeaks();
    Message msg = createTestMessage();

    auto raw = msg.getNetworkBytes();
    auto addedGarbage = ByteArray(31);
    auto stream = addedGarbage.getWriteStream();
    stream.write(STX);
    stream.write(static_cast<uint8_t>(0x12));
    stream.writeBytes(raw, 29);
    auto result = Message::convertNetworkBytes(raw);

    EXPECT_TRUE(result.isValid());
    EXPECT_EQ(result, msg);

  }

  /**
   * Test that the system can detect errors in message
   */
  TEST(MessageTest,testInvalidCrc){
    checkForLeaks();
    Message msg = createTestMessage();
    auto raw = msg.getNetworkBytes();
    raw[12] = 10;
    auto result = Message::convertNetworkBytes(raw);
    EXPECT_TRUE(!result.isValid());

  }

  /**
   * Test that the system can deal with a completely garbage packet
   * bytes at start.
   */
  TEST(MessageTest,testGarbage){
    checkForLeaks();
    Message msg = createTestMessage();
    auto raw = msg.getNetworkBytes();
    raw[0] = 0xFF;
    auto result = Message::convertNetworkBytes(raw);
    EXPECT_TRUE(!result.isValid());

  }

  /**
   * Test that the system can deal with not enough data
   */
  TEST(MessageTest,testNotEnoughData){
    checkForLeaks();
    Message msg = createTestMessage();
    auto raw = msg.getNetworkBytes();
    raw.setLength(28);
    auto result = Message::convertNetworkBytes(raw);
    EXPECT_TRUE(!result.isValid());
  }

  /**
   * Test the encoding of a set of variables into a packet payload
   */
  TEST(MessageTest,testEncode){
    checkForLeaks();
    uint8_t a = 1;
    uint16_t b = 0x0302;
    uint32_t c = 0x07060504;
    int16_t d = -2;
    int32_t e = -3;
    uint8_t raw[] = {0x01, 0x02, 0x03, 0x04, 0x05,0x06,0x07, 0xFE, 0xFF, 0xFD, 0xFF, 0xFF,0xFF};
    ByteArray expected(raw, elements(raw));

    auto msg = Message::encode(MessageId::TEST_MESSAGE_2, a,b,c,d,e);
    auto payload = msg.getPayload();

    EXPECT_EQ(expected, payload);

  }

  /**
   * Test the decoding of a payload into a set of variables.
   */
  TEST(MessageTest,testDecode){
    checkForLeaks();
    uint8_t a;
    uint16_t b;
    uint32_t c;
    int16_t d;
    int32_t e;
    uint8_t raw[] = {0x01, 0x02, 0x03, 0x04, 0x05,0x06,0x07,0xFE, 0xFF, 0xFD, 0xFF, 0xFF,0xFF};
    ByteArray payload(raw, elements(raw));
    auto msg = Message(MessageId::TEST_MESSAGE_2, elements(raw));
    msg.getPayload() = payload;
    msg.decode(a,b,c,d,e);

    EXPECT_EQ(a, 0x01);
    EXPECT_EQ(b, 0x0302);
    EXPECT_EQ(c, 0x07060504u);
    EXPECT_EQ(d, -2);
    EXPECT_EQ(e, -3);

  }

}
