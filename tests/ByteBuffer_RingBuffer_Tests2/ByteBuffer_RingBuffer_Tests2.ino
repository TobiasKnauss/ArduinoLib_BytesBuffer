#include <AUnit.h>

#include <ByteBuffer.h>

//--------------------------------------------------------------------
// - ReadBytesAndMovePtr (array)
test (ByteBuffer_RingBuffer_ReadBytesAndMovePtr_ToArray_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pRingBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, true, pRingBuffer);
  assertTrue (result);
  uint8_t cache[10];

  //---------- Act & Assert ----------
  assertFalse (pRingBuffer->ReadBytesAndMovePtr (0,  (uint8_t*)nullptr, false));
  assertFalse (pRingBuffer->ReadBytesAndMovePtr (1,  (uint8_t*)nullptr, false));
  assertFalse (pRingBuffer->ReadBytesAndMovePtr (10, (uint8_t*)nullptr, false));
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
// - ReadBytesAndMovePtr (array)
// - get_pData ()
test (ByteBuffer_RingBuffer_ReadBytesAndMovePtr_ToArray_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pRingBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, true, pRingBuffer);
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
  assertTrue  (pRingBuffer->SetReadPointer (0));  // -> reading starts at address 0
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (9, cache + 1, false));  // cache: 00 88 99 11 22 33 44 55 66 77
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
  assertTrue (pRingBuffer->SetReadPointer (5));  // -> reading starts at address 5
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
// - ReadBytesAndMovePtr (ByteBuffer)
test (ByteBuffer_RingBuffer_ReadBytesAndMovePtr_ToLinearBuffer_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pRingBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, true, pRingBuffer);
  assertTrue (result);
  for (uint16_t index = 0; index < length; index++)
    pRingBuffer->get_pData ()[index] = (uint8_t)((index + 1) * 11);  // -> 11 22 33 44 55 66 77 88 99  =  0x 0B 16 21 2C 37 42 4D 58 63

  uint16_t destLength = 8;
  ByteBuffer* pDestBuffer = nullptr;
  assertTrue (ByteBuffer::Create (destLength, defaultValue, false, pDestBuffer));  // destination is linear buffer

  //---------- Act & Assert ----------
  assertFalse (pRingBuffer->ReadBytesAndMovePtr (0,  (ByteBuffer*)nullptr, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);
  assertFalse (pRingBuffer->ReadBytesAndMovePtr (1,  (ByteBuffer*)nullptr, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);
  assertFalse (pRingBuffer->ReadBytesAndMovePtr (10, (ByteBuffer*)nullptr, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);
  for (uint16_t index = 0; index < length; index++) assertEqual (pRingBuffer->get_pData()[index], (uint8_t)((index + 1) * 11));

  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (0, pDestBuffer, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  for (uint16_t index = 0; index < length;     index++) assertEqual (pRingBuffer->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < destLength; index++) assertEqual (pDestBuffer->get_pData()[index], defaultValue);

  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (1, pDestBuffer, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)1);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pDestBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  for (uint16_t index = 0; index < length;     index++) assertEqual (pRingBuffer->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < destLength; index++) assertEqual (pDestBuffer->get_pData()[index], index < 1 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (2, pDestBuffer, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < length;     index++) assertEqual (pRingBuffer->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < destLength; index++) assertEqual (pDestBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertFalse (pRingBuffer->ReadBytesAndMovePtr (7, pDestBuffer, false));
  assertFalse (pRingBuffer->ReadBytesAndMovePtr (6, pDestBuffer, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < length;     index++) assertEqual (pRingBuffer->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < destLength; index++) assertEqual (pDestBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (5, pDestBuffer, false));
  assertFalse (pRingBuffer->ReadBytesAndMovePtr (1, pDestBuffer, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)8);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pDestBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)8);
  for (uint16_t index = 0; index < length;     index++) assertEqual (pRingBuffer->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < destLength; index++) assertEqual (pDestBuffer->get_pData()[index], (uint8_t)((index + 1) * 11));

  pDestBuffer->Clear ();
  pDestBuffer->SetWritePointer (4);
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (2, pDestBuffer, true));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)1);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)6);
  assertFalse (pRingBuffer->ReadBytesAndMovePtr (3, pDestBuffer, true));
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (2, pDestBuffer, true));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)8);
  uint8_t expectedUI8[8] = { defaultValue, defaultValue, defaultValue, defaultValue, 11, 99, 33, 22 };
  for (uint16_t index = 0; index < destLength; index++) assertEqual (pDestBuffer->get_pData()[index], expectedUI8[index]);

  pDestBuffer->SetWritePointer (2);
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (6, pDestBuffer, true));
  for (uint16_t index = 0; index < destLength; index++) assertEqual (pDestBuffer->get_pData()[index], index < 2 ? defaultValue : (uint8_t)((11 - index) * 11));

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// - ReadBytesAndMovePtr (ByteBuffer)
test (ByteBuffer_RingBuffer_ReadBytesAndMovePtr_ToLinearBuffer_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pRingBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, true, pRingBuffer);
  assertTrue (result);
  for (uint16_t index = 0; index < length; index++)
    pRingBuffer->get_pData ()[index] = (uint8_t)((index + 1) * 11);  // -> 11 22 33 44 55 66 77 88 99  =  0x 0B 16 21 2C 37 42 4D 58 63

  uint16_t destLength = 10;
  ByteBuffer* pDestBuffer = nullptr;
  assertTrue (ByteBuffer::Create (destLength, defaultValue, false, pDestBuffer));  // destination is linear buffer

  //---------- Act & Assert ----------
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (1, pDestBuffer, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)1);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pDestBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  for (uint16_t index = 0; index < length;     index++) assertEqual (pRingBuffer->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < destLength; index++) assertEqual (pDestBuffer->get_pData()[index], index < 1 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (2, pDestBuffer, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < length;     index++) assertEqual (pRingBuffer->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < destLength; index++) assertEqual (pDestBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertFalse (pRingBuffer->ReadBytesAndMovePtr (8, pDestBuffer, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < length;     index++) assertEqual (pRingBuffer->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < destLength; index++) assertEqual (pDestBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (7, pDestBuffer, false));
  assertFalse (pRingBuffer->ReadBytesAndMovePtr (1, pDestBuffer, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)1);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pDestBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)10);
  for (uint16_t index = 0; index < length;     index++) assertEqual (pRingBuffer->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < destLength; index++) assertEqual (pDestBuffer->get_pData()[index], (uint8_t)((index % length + 1) * 11));

  pDestBuffer->Clear ();
  pRingBuffer->SetReadPointer (5);
  pDestBuffer->SetWritePointer (2);
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (3, pDestBuffer, true));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)8);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)5);
  assertFalse (pRingBuffer->ReadBytesAndMovePtr (6, pDestBuffer, true));
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (5, pDestBuffer, true));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)4);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)10);
  uint8_t expectedUI8[10] = { defaultValue, defaultValue, 88, 77, 66, 44, 33, 22, 11, 99 };
  for (uint16_t index = 0; index < destLength; index++) assertEqual (pDestBuffer->get_pData()[index], expectedUI8[index]);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// - ReadBytesAndMovePtr (ByteBuffer)
test (ByteBuffer_RingBuffer_ReadBytesAndMovePtr_ToRingBuffer_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pRingBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, true, pRingBuffer);
  assertTrue (result);
  for (uint16_t index = 0; index < length; index++)
    pRingBuffer->get_pData ()[index] = (uint8_t)((index + 1) * 11);  // -> 11 22 33 44 55 66 77 88 99  =  0x 0B 16 21 2C 37 42 4D 58 63

  uint16_t destLength = 8;
  ByteBuffer* pDestBuffer = nullptr;
  assertTrue (ByteBuffer::Create (destLength, defaultValue, true, pDestBuffer));  // destination is ring buffer

  //---------- Act & Assert ----------
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (1, pDestBuffer, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)1);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pDestBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  for (uint16_t index = 0; index < length;     index++) assertEqual (pRingBuffer->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < destLength; index++) assertEqual (pDestBuffer->get_pData()[index], index < 1 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (2, pDestBuffer, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < length;     index++) assertEqual (pRingBuffer->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < destLength; index++) assertEqual (pDestBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertFalse (pRingBuffer->ReadBytesAndMovePtr (9, pDestBuffer, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < length;     index++) assertEqual (pRingBuffer->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < destLength; index++) assertEqual (pDestBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (8, pDestBuffer, false));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)2);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pDestBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  uint8_t expectedUI8_a[8] = { 99, 11, 22, 44, 55, 66, 77, 88 };
  for (uint16_t index = 0; index < length;     index++) assertEqual (pRingBuffer->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < destLength; index++) assertEqual (pDestBuffer->get_pData()[index], expectedUI8_a[index]);

  pDestBuffer->Clear ();
  pDestBuffer->SetWritePointer (4);
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (2, pDestBuffer, true));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)4);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)6);
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (3, pDestBuffer, true));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)7);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  assertTrue  (pRingBuffer->ReadBytesAndMovePtr (2, pDestBuffer, true));
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pDestBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  uint8_t expectedUI8_b[8] = { 55, 99, 88, defaultValue, 44, 33, 77, 66 };
  for (uint16_t index = 0; index < destLength; index++) assertEqual (pDestBuffer->get_pData()[index], expectedUI8_b[index]);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// - ReadValueAndMovePtr ()
// - get_pData ()
test (ByteBuffer_RingBuffer_ReadValueAndMovePtr_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pRingBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, true, pRingBuffer);
  assertTrue (result);

  for (uint16_t index = 0; index < length; index++)
    pRingBuffer->get_pData ()[index] = (uint8_t)((8 - index) * 11);  // -> 88 77 66 55 44 33 22 11 00  =  0x 58 4D 42 37 2C 21 16 0B 00

  //---------- Act & Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    bool valueB;
    assertEqual (pRingBuffer->get_CurrentReadAddress (), index);
    assertTrue (pRingBuffer->ReadValueAndMovePtr (valueB));
    assertEqual (valueB, index != length - 1);
  }

  for (uint16_t index = 0; index < length; index++)
  {
    uint8_t valueUI8;
    assertEqual (pRingBuffer->get_CurrentReadAddress (), index);
    assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, (uint8_t)((8 - index) * 11));
  }

  for (uint16_t index = 0; index < length; index++)
  {
    int8_t valueI8;
    assertEqual (pRingBuffer->get_CurrentReadAddress (), index);
    assertTrue (pRingBuffer->ReadValueAndMovePtr (valueI8));
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
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_CurrentReadAddress (), address);
    assertTrue (pRingBuffer->ReadValueAndMovePtr (valueI16, false));
    assertEqual (valueI16, (int16_t)expectedUI16[index]);
    address = (address + 2) % length;
  }

  address = 0;
  for (uint16_t index = 0; index < length; index++)
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
void setup ()
{
  Serial.begin (115200);
  delay (2000);
}

void loop ()
{
  aunit::TestRunner::run ();
}
