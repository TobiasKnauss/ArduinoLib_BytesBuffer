#include <AUnit.h>

#include <ByteRingBuffer.h>

//--------------------------------------------------------------------
// Test of the functions
// - Create ()
test (ByteRingBuffer_Create_Test1)
{
  //---------- Arrange ----------
  uint8_t value = 0;
  ByteRingBuffer* pRingBuffer = (ByteRingBuffer*)&value;

  //---------- Act ----------
  bool result = ByteRingBuffer::Create (20, 0xFF, pRingBuffer);

  //---------- Assert ----------
  assertFalse (result);
  assertEqual (pRingBuffer, &value);
}

//--------------------------------------------------------------------
// Test of the functions
// - Create ()
test (ByteRingBuffer_Create_Test2)
{
  //---------- Arrange ----------
  ByteRingBuffer* pRingBuffer = nullptr;

  //---------- Act ----------
  bool result = ByteRingBuffer::Create (0, 0xFF, pRingBuffer);

  //---------- Assert ----------
  assertFalse (result);
  assertEqual (pRingBuffer, nullptr);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - Create ()
// - get_Length ()
test (ByteRingBuffer_Create_Test3)
{
  //---------- Arrange ----------
  uint16_t length = 20;
  ByteRingBuffer* pRingBuffer = nullptr;

  //---------- Act ----------
  bool result = ByteRingBuffer::Create (length, 0xFF, pRingBuffer);

  //---------- Assert ----------
  assertTrue (result);
  assertNotEqual (pRingBuffer, nullptr);
  assertEqual (pRingBuffer->get_Length (), length);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - ReadByteAndMovePtr ()
// - MoveReadPointer ()
// - SetReadPointer ()
// - get_CurrentReadAddress ()
// - get_pData ()
test (ByteRingBuffer_ReadByteAndMovePtr_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint8_t newValue1 = 0x12;
  uint16_t length = 5;
  ByteRingBuffer* pRingBuffer = nullptr;
  bool result = ByteRingBuffer::Create (length, defaultValue, pRingBuffer);
  assertTrue (result);

  //---------- Act & Assert ----------
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  for (uint16_t index = 0; index < 2 * length; index++)
  {
    assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
    assertEqual (pRingBuffer->get_CurrentReadAddress (), (index + 1) % length);
    assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  }

  //---------- Arrange ----------
  pRingBuffer->get_pData ()[0] = newValue1;

  //---------- Act & Assert ----------
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);

  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue1);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)1);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)2);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)3);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)4);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);

  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue1);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)1);
  pRingBuffer->MoveReadPointer ();
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)2);
  pRingBuffer->MoveReadPointer ();
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)3);
  pRingBuffer->MoveReadPointer (2);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);

  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue1);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)1);
  pRingBuffer->SetReadPointer (0);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);

  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue1);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  pRingBuffer->MoveReadPointer (2);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);
  pRingBuffer->SetReadPointer (0);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue1);

  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// - ReadBytesAndMovePtr ()
test (ByteRingBuffer_ReadBytesAndMovePtr_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteRingBuffer* pRingBuffer = nullptr;
  bool result = ByteRingBuffer::Create (length, defaultValue, pRingBuffer);
  assertTrue (result);
  uint8_t cache[10];

  //---------- Act & Assert ----------
  assertFalse (pRingBuffer->ReadBytesAndMovePtr (0,  nullptr, false));
  assertFalse (pRingBuffer->ReadBytesAndMovePtr (1,  nullptr, false));
  assertFalse (pRingBuffer->ReadBytesAndMovePtr (10, nullptr, false));
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (0,  cache, false));
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (1,  cache, false));
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (2,  cache, false));
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (9,  cache, false));
  assertFalse (pRingBuffer->ReadBytesAndMovePtr (10, cache, false));
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (0,  cache, true));
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (1,  cache, true));
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (2,  cache, true));
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (9,  cache, true));
  assertFalse (pRingBuffer->ReadBytesAndMovePtr (10, cache, true));

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// - ReadBytesAndMovePtr ()
// - get_pData ()
test (ByteRingBuffer_ReadBytesAndMovePtr_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteRingBuffer* pRingBuffer = nullptr;
  bool result = ByteRingBuffer::Create (length, defaultValue, pRingBuffer);
  assertTrue (result);

  uint8_t cacheSize = 10;
  uint8_t cache[10];

  //---------- Act & Assert ----------
  memset (&cache, 0x00, sizeof (cache));
  assertTrue (pRingBuffer->ReadBytesAndMovePtr (3, cache, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < cacheSize; index++)
    assertEqual (cache[index], index < 3 ? defaultValue : 0x00);

  memset (&cache, 0x00, sizeof (cache));
  assertTrue (pRingBuffer->ReadBytesAndMovePtr (7, cache, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)1);
  for (uint16_t index = 0; index < cacheSize; index++)
    assertEqual (cache[index], index < 7 ? defaultValue : 0x00);

  //---------- Arrange ----------
  for (uint16_t index = 0; index < length; index++)
    pRingBuffer->get_pData()[index] = (((index + 7) % length) + 1) * 11;  // -> 88 99 11 22 33 44 55 66 77

  //---------- Act & Assert ----------
  memset (&cache, 0x00, sizeof (cache));
  pRingBuffer->SetReadPointer (0);  // -> reading starts at address 0
  assertTrue (pRingBuffer->ReadBytesAndMovePtr (9, cache + 1, false));  // cache: 00 88 99 11 22 33 44 55 66 77
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);
  assertEqual (cache[0], 0);
  assertEqual (cache[1], 88);
  assertEqual (cache[2], 99);
  assertEqual (cache[3], 11);
  assertEqual (cache[4], 22);
  assertEqual (cache[5], 33);
  assertEqual (cache[6], 44);
  assertEqual (cache[7], 55);
  assertEqual (cache[8], 66);
  assertEqual (cache[9], 77);

  memset (&cache, 0x00, sizeof (cache));
  pRingBuffer->SetReadPointer (5);  // -> reading starts at address 5
  assertTrue (pRingBuffer->ReadBytesAndMovePtr (4, cache + 2, true));
  assertTrue (pRingBuffer->ReadBytesAndMovePtr (2, cache    , true));
  assertTrue (pRingBuffer->ReadBytesAndMovePtr (3, cache + 6, true));
  for (uint16_t index = 0; index < cacheSize; index++)
    assertEqual (cache[index], (uint8_t)((9 - index) * 11));
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)5);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteByteAndMovePtr ()
// - MoveWritePointer ()
// - SetWritePointer ()
// - get_CurrentWriteAddress ()
// - get_pData ()
test (ByteRingBuffer_WriteByteAndMovePtr_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint8_t newValue1 = 0x12;
  uint8_t newValue2 = 0x34;
  uint8_t newValue3 = 0x56;
  uint16_t length = 9;
  ByteRingBuffer* pRingBuffer = nullptr;
  assertTrue (ByteRingBuffer::Create (length, defaultValue, pRingBuffer));

  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_pData ()[index], defaultValue);
    assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  }
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);

  //---------- Act ----------
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  pRingBuffer->MoveWritePointer (3);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  pRingBuffer->WriteByteAndMovePtr (newValue1);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)4);
  pRingBuffer->MoveWritePointer ();
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)5);
  pRingBuffer->WriteByteAndMovePtr (newValue2);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)6);
  pRingBuffer->SetWritePointer (1);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  pRingBuffer->WriteByteAndMovePtr (newValue3);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)2);

  //---------- Assert ----------
  for (uint16_t index = 0; index < length * 3; index++)
  {
    assertEqual (pRingBuffer->get_pData ()[index % length], index % length == 3 ? newValue1 : index % length == 5 ? newValue2 : index % length == 1 ? newValue3 : defaultValue);
    assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)2);
  }

  //---------- Act ----------
  pRingBuffer->WriteByteAndMovePtr (newValue3);
  pRingBuffer->WriteByteAndMovePtr (newValue3);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)4);
  pRingBuffer->SetWritePointer (7);
  pRingBuffer->WriteByteAndMovePtr (newValue1);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)8);

  //---------- Assert ----------
  assertEqual (pRingBuffer->get_pData ()[0], defaultValue);
  assertEqual (pRingBuffer->get_pData ()[1], newValue3);
  assertEqual (pRingBuffer->get_pData ()[2], newValue3);
  assertEqual (pRingBuffer->get_pData ()[3], newValue3);
  assertEqual (pRingBuffer->get_pData ()[4], defaultValue);
  assertEqual (pRingBuffer->get_pData ()[5], newValue2);
  assertEqual (pRingBuffer->get_pData ()[6], defaultValue);
  assertEqual (pRingBuffer->get_pData ()[7], newValue1);
  assertEqual (pRingBuffer->get_pData ()[8], defaultValue);

  //---------- Act ----------
  pRingBuffer->SetWritePointer (3);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < length; index++)
    pRingBuffer->WriteByteAndMovePtr (index * 10);

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->get_pData ()[index], (uint8_t)(((index + 6) % length) * 10));

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// - WriteBytesAndMovePtr ()
test (ByteRingBuffer_WriteBytesAndMovePtr_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteRingBuffer* pRingBuffer = nullptr;
  bool result = ByteRingBuffer::Create (length, defaultValue, pRingBuffer);
  assertTrue (result);
  uint8_t cache[10];

  //---------- Act & Assert ----------
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (0,  nullptr, false));
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (1,  nullptr, false));
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (10, nullptr, false));
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (0,  cache, false));
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (1,  cache, false));
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (2,  cache, false));
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (9,  cache, false));
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (10, cache, false));
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (0,  cache, true));
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (1,  cache, true));
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (2,  cache, true));
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (9,  cache, true));
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (10, cache, true));

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// - WriteBytesAndMovePtr ()
// - get_pData ()
test (ByteRingBuffer_WriteBytesAndMovePtr_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteRingBuffer* pRingBuffer = nullptr;
  bool result = ByteRingBuffer::Create (length, defaultValue, pRingBuffer);
  assertTrue (result);

  uint8_t cacheSize = 10;
  uint8_t cache[10];
  memset (&cache, 0x00, sizeof (cache));

  //---------- Act & Assert ----------
  pRingBuffer->Clear ();
  assertTrue (pRingBuffer->WriteBytesAndMovePtr (3, cache, false));
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->get_pData ()[index], index < 3 ? 0x00 : defaultValue);

  pRingBuffer->Clear ();
  assertTrue (pRingBuffer->WriteBytesAndMovePtr (7, cache, false));
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->get_pData ()[index], index < 1 || index >= 3 ? 0x00 : defaultValue);

  //---------- Arrange ----------
  for (uint16_t index = 0; index < cacheSize; index++)
    cache[index] = ((index + 8) % cacheSize) * 11;  // -> 88 99 00 11 22 33 44 55 66 77

  //---------- Act & Assert ----------
  pRingBuffer->Clear ();
  pRingBuffer->SetWritePointer (1);  // -> writing starts at address 0
  assertTrue (pRingBuffer->WriteBytesAndMovePtr (9, cache + 1, false));  // buffer: 77 99 00 11 22 33 44 55 66
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  assertEqual (pRingBuffer->get_pData ()[0], 77);
  assertEqual (pRingBuffer->get_pData ()[1], 99);
  assertEqual (pRingBuffer->get_pData ()[2], 00);
  assertEqual (pRingBuffer->get_pData ()[3], 11);
  assertEqual (pRingBuffer->get_pData ()[4], 22);
  assertEqual (pRingBuffer->get_pData ()[5], 33);
  assertEqual (pRingBuffer->get_pData ()[6], 44);
  assertEqual (pRingBuffer->get_pData ()[7], 55);
  assertEqual (pRingBuffer->get_pData ()[8], 66);

  pRingBuffer->Clear ();
  pRingBuffer->SetWritePointer (5);  // -> writeing starts at address 5
  assertTrue (pRingBuffer->WriteBytesAndMovePtr (4, cache + 3, true));  // buffer: AB AB AB AB AB 44 33 22 11
  assertTrue (pRingBuffer->WriteBytesAndMovePtr (2, cache    , true));  // buffer: 99 88 AB AB AB 44 33 22 11
  assertTrue (pRingBuffer->WriteBytesAndMovePtr (3, cache + 7, true));  // buffer: 99 88 77 66 55 44 33 22 11
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->get_pData ()[index], (uint8_t)((9 - index) * 11));
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)5);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// - ReadValueAndMovePtr ()
// - get_pData ()
test (ByteRingBuffer_ReadValueAndMovePtr_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteRingBuffer* pRingBuffer = nullptr;
  bool result = ByteRingBuffer::Create (length, defaultValue, pRingBuffer);
  assertTrue (result);

  for (uint16_t index = 0; index < length; index++)
    pRingBuffer->get_pData ()[index] = (uint8_t)((8 - index) * 11);  // -> 88 77 66 55 44 33 22 11 00  =  0x 58 4D 42 37 2C 21 16 0B 00

  //---------- Act & Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    bool valueB;
    assertEqual (pRingBuffer->get_CurrentReadAddress (), index);
    pRingBuffer->ReadValueAndMovePtr (valueB);
    assertEqual (valueB, index != length - 1);
  }

  for (uint16_t index = 0; index < length; index++)
  {
    uint8_t valueUI8;
    assertEqual (pRingBuffer->get_CurrentReadAddress (), index);
    pRingBuffer->ReadValueAndMovePtr (valueUI8);
    assertEqual (valueUI8, (uint8_t)((8 - index) * 11));
  }

  for (uint16_t index = 0; index < length; index++)
  {
    int8_t valueI8;
    assertEqual (pRingBuffer->get_CurrentReadAddress (), index);
    pRingBuffer->ReadValueAndMovePtr (valueI8);
    assertEqual (valueI8, (int8_t)((8 - index) * 11));
  }

  // 58 4D 42 37 2C 21 16 0B 00
  // [58][4D] --(little-endian => little end of number comes first in memory)--> 0x4D58
  uint16_t valueUI16;
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, false)); assertEqual (valueUI16, (uint16_t)0x4D58);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)2); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, false)); assertEqual (valueUI16, (uint16_t)0x3742);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)4); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, false)); assertEqual (valueUI16, (uint16_t)0x212C);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)6); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, false)); assertEqual (valueUI16, (uint16_t)0x0B16);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)8); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, false)); assertEqual (valueUI16, (uint16_t)0x5800);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)1); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, false)); assertEqual (valueUI16, (uint16_t)0x424D);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)3); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, false)); assertEqual (valueUI16, (uint16_t)0x2C37);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)5); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, false)); assertEqual (valueUI16, (uint16_t)0x1621);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)7); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, false)); assertEqual (valueUI16, (uint16_t)0x000B);

  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, true )); assertEqual (valueUI16, (uint16_t)0x584D);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)2); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, true )); assertEqual (valueUI16, (uint16_t)0x4237);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)4); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, true )); assertEqual (valueUI16, (uint16_t)0x2C21);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)6); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, true )); assertEqual (valueUI16, (uint16_t)0x160B);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)8); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, true )); assertEqual (valueUI16, (uint16_t)0x0058);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)1); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, true )); assertEqual (valueUI16, (uint16_t)0x4D42);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)3); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, true )); assertEqual (valueUI16, (uint16_t)0x372C);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)5); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, true )); assertEqual (valueUI16, (uint16_t)0x2116);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)7); assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI16, true )); assertEqual (valueUI16, (uint16_t)0x0B00);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);

  int16_t valueI16;
  uint16_t expectedUI16[9]     = { 0x4D58, 0x3742, 0x212C, 0x0B16, 0x5800, 0x424D, 0x2C37, 0x1621, 0x000B };
  uint16_t expectedUI16_inv[9] = { 0x584D, 0x4237, 0x2C21, 0x160B, 0x0058, 0x4D42, 0x372C, 0x2116, 0x0B00 };

  uint16_t address = 0;
  for (int index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_CurrentReadAddress (), address);
    assertTrue (pRingBuffer->ReadValueAndMovePtr (valueI16, false));
    assertEqual (valueI16, (int16_t)expectedUI16[index]);
    address = (address + 2) % length;
  }

  address = 0;
  for (int index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_CurrentReadAddress (), address);
    assertTrue (pRingBuffer->ReadValueAndMovePtr (valueI16, true));
    assertEqual (valueI16, (int16_t)expectedUI16_inv[index]);
    address = (address + 2) % length;
  }

  uint32_t valueUI32;
  int32_t  valueI32;
  uint32_t expectedUI32[9]     = { 0x37424D58, 0x0B16212C, 0x424D5800, 0x16212C37, 0x4D58000B, 0x212C3742, 0x58000B16, 0x2C37424D, 0x000B1621 };
  uint32_t expectedUI32_inv[9] = { 0x584D4237, 0x2C21160B, 0x00584D42, 0x372C2116, 0x0B00584D, 0x42372C21, 0x160B0058, 0x4D42372C, 0x21160B00 };

  address = 0;
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_CurrentReadAddress (), address);
    assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI32, false));
    assertEqual (valueUI32, expectedUI32[index]);
    address = (address + 4) % length;
  }

  address = 0;
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_CurrentReadAddress (), address);
    assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI32, true));
    assertEqual (valueUI32, expectedUI32_inv[index]);
    address = (address + 4) % length;
  }

  address = 0;
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_CurrentReadAddress (), address);
    assertTrue (pRingBuffer->ReadValueAndMovePtr (valueI32, false));
    assertEqual (valueI32, (int32_t)expectedUI32[index]);
    address = (address + 4) % length;
  }

  address = 0;
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_CurrentReadAddress (), address);
    assertTrue (pRingBuffer->ReadValueAndMovePtr (valueI32, true));
    assertEqual (valueI32, (int32_t)expectedUI32_inv[index]);
    address = (address + 4) % length;
  }
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_FromStart ()
test (ByteRingBuffer_WriteRange_FromStart_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteRingBuffer* pRingBuffer = nullptr;
  assertTrue (ByteRingBuffer::Create (length, defaultValue, pRingBuffer));

  //---------- Act & Assert ----------
  assertTrue  (pRingBuffer->WriteRange_FromStart ( 0,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_FromStart ( 1,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_FromStart ( 8,  0, 0x00));
  assertFalse (pRingBuffer->WriteRange_FromStart ( 9,  0, 0x00));
  assertFalse (pRingBuffer->WriteRange_FromStart (99,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_FromStart ( 0,  1, 0x00));
  assertTrue  (pRingBuffer->WriteRange_FromStart ( 0,  8, 0x00));
  assertTrue  (pRingBuffer->WriteRange_FromStart ( 0,  9, 0x00));
  assertFalse (pRingBuffer->WriteRange_FromStart ( 0, 10, 0x00));
  assertFalse (pRingBuffer->WriteRange_FromStart ( 0, 99, 0x00));

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_FromStart ()
// - Clear ()
test (ByteRingBuffer_WriteRange_FromStart_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint8_t newValue1 = 0x12;
  uint8_t newValue2 = 0x34;
  uint8_t newValue3 = 0x56;
  uint16_t length = 9;
  ByteRingBuffer* pRingBuffer = nullptr;
  assertTrue (ByteRingBuffer::Create (length, defaultValue, pRingBuffer));

  //---------- Act ----------
  assertTrue (pRingBuffer->WriteRange_FromStart (0, 1, newValue1));
  assertTrue (pRingBuffer->WriteRange_FromStart (3, 4, newValue2));

  //---------- Assert ----------
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue1);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue2);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue2);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue2);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue2);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);

  //---------- Act ----------
  pRingBuffer->Clear ();

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);

  //---------- Act ----------
  pRingBuffer->WriteRange_FromStart (2, 4, newValue3);

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->ReadByteAndMovePtr (), index >= 2 && index < 6 ? newValue3 : defaultValue);

  //---------- Act ----------
  pRingBuffer->WriteRange_FromStart (0, 9, newValue1);

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue1);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_ToEnd ()
test (ByteRingBuffer_WriteRange_ToEnd_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteRingBuffer* pRingBuffer = nullptr;
  assertTrue (ByteRingBuffer::Create (length, defaultValue, pRingBuffer));

  //---------- Act & Assert ----------
  assertTrue  (pRingBuffer->WriteRange_ToEnd ( 0,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_ToEnd ( 1,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_ToEnd ( 8,  0, 0x00));
  assertFalse (pRingBuffer->WriteRange_ToEnd ( 9,  0, 0x00));
  assertFalse (pRingBuffer->WriteRange_ToEnd (99,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_ToEnd ( 0,  1, 0x00));
  assertTrue  (pRingBuffer->WriteRange_ToEnd ( 0,  8, 0x00));
  assertTrue  (pRingBuffer->WriteRange_ToEnd ( 0,  9, 0x00));
  assertFalse (pRingBuffer->WriteRange_ToEnd ( 0, 10, 0x00));
  assertFalse (pRingBuffer->WriteRange_ToEnd ( 0, 99, 0x00));

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_ToEnd ()
// - Clear ()
test (ByteRingBuffer_WriteRange_ToEnd_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint8_t newValue1 = 0x12;
  uint8_t newValue2 = 0x34;
  uint8_t newValue3 = 0x56;
  uint16_t length = 9;
  ByteRingBuffer* pRingBuffer = nullptr;
  assertTrue (ByteRingBuffer::Create (length, defaultValue, pRingBuffer));

  //---------- Act ----------
  assertTrue (pRingBuffer->WriteRange_ToEnd (0, 1, newValue1));
  assertTrue (pRingBuffer->WriteRange_ToEnd (6, 4, newValue2));

  //---------- Assert ----------
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue2);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue2);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue2);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue2);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue1);

  //---------- Act ----------
  pRingBuffer->Clear ();

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);

  //---------- Act ----------
  pRingBuffer->WriteRange_ToEnd (2, 4, newValue3);

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->ReadByteAndMovePtr (), index < 2 || index > 6 ? newValue3 : defaultValue);

  //---------- Act ----------
  pRingBuffer->WriteRange_ToEnd (0, 9, newValue1);

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue1);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_StartToEnd ()
test (ByteRingBuffer_WriteRange_StartToEnd_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteRingBuffer* pRingBuffer = nullptr;
  assertTrue (ByteRingBuffer::Create (length, defaultValue, pRingBuffer));

  //---------- Act & Assert ----------
  assertTrue  (pRingBuffer->WriteRange_StartToEnd ( 0,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_StartToEnd ( 1,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_StartToEnd ( 8,  0, 0x00));
  assertFalse (pRingBuffer->WriteRange_StartToEnd ( 9,  0, 0x00));
  assertFalse (pRingBuffer->WriteRange_StartToEnd (99,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_StartToEnd ( 0,  1, 0x00));
  assertTrue  (pRingBuffer->WriteRange_StartToEnd ( 0,  8, 0x00));
  assertFalse (pRingBuffer->WriteRange_StartToEnd ( 0,  9, 0x00));
  assertFalse (pRingBuffer->WriteRange_StartToEnd ( 0, 99, 0x00));

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_StartToEnd ()
// - Clear ()
test (ByteRingBuffer_WriteRange_StartToEnd_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint8_t newValue1 = 0x12;
  uint8_t newValue2 = 0x34;
  uint8_t newValue3 = 0x56;
  uint16_t length = 9;
  ByteRingBuffer* pRingBuffer = nullptr;
  assertTrue (ByteRingBuffer::Create (length, defaultValue, pRingBuffer));

  //---------- Act ----------
  assertTrue (pRingBuffer->WriteRange_StartToEnd (0, 1, newValue1));
  assertTrue (pRingBuffer->WriteRange_StartToEnd (3, 7, newValue2));

  //---------- Assert ----------
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue1);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue2);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue2);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue2);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue2);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);

  //---------- Act ----------
  pRingBuffer->Clear ();

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);

  //---------- Act ----------
  pRingBuffer->WriteRange_StartToEnd (4, 2, newValue3);

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->ReadByteAndMovePtr (), index < 2 || index >= 4 ? newValue3 : defaultValue);

  //---------- Act ----------
  pRingBuffer->WriteRange_StartToEnd (2, 2, newValue1);

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue1);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
void setup ()
{
  Serial.begin (115200);
  delay (2000);
}

void loop ()
{
  aunit::TestRunner::run ();
}
