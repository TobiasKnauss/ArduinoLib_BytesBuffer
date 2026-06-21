#include <AUnit.h>

#include <ByteBuffer.h>

//--------------------------------------------------------------------
// - WriteBytesAndMovePtr (array)
test (ByteBuffer_RingBuffer_WriteBytesAndMovePtr_FromArray_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pRingBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, true, pRingBuffer);
  assertTrue (result);
  uint8_t cache[10];

  //---------- Act & Assert ----------
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (0,  (uint8_t*)nullptr, false));
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (1,  (uint8_t*)nullptr, false));
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (10, (uint8_t*)nullptr, false));
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
// - WriteBytesAndMovePtr (array)
// - get_pData ()
test (ByteBuffer_RingBuffer_WriteBytesAndMovePtr_FromArray_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pRingBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, true, pRingBuffer);
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
// - WriteBytesAndMovePtr (ByteBuffer)
test (ByteBuffer_RingBuffer_WriteBytesAndMovePtr_FromLinearBuffer_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pRingBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, true, pRingBuffer);
  assertTrue (result);

  uint16_t srcLength = 8;
  ByteBuffer* pSrcBuffer = nullptr;
  assertTrue (ByteBuffer::Create (srcLength, defaultValue, false, pSrcBuffer));  // source is linear buffer
  for (uint16_t index = 0; index < srcLength; index++)
    pSrcBuffer->get_pData ()[index] = (uint8_t)((index + 1) * 11);  // -> 11 22 33 44 55 66 77 88  =  0x 0B 16 21 2C 37 42 4D 58

  //---------- Act & Assert ----------
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (0,  (ByteBuffer*)nullptr, false));
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (1,  (ByteBuffer*)nullptr, false));
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (10, (ByteBuffer*)nullptr, false));
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  for (uint16_t index = 0; index < length; index++) assertEqual (pRingBuffer->get_pData()[index], defaultValue);

  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (0, pSrcBuffer, false));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pRingBuffer->get_pData()[index], defaultValue);

  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (1, pSrcBuffer, false));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)1);
  assertEqual (pSrcBuffer ->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pRingBuffer->get_pData()[index], index < 1 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, false));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pRingBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertFalse (pRingBuffer->WriteBytesAndMovePtr (7, pSrcBuffer, false));
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (6, pSrcBuffer, false));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pRingBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (5, pSrcBuffer, false));
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (1, pSrcBuffer, false));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)8);
  assertEqual (pSrcBuffer ->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)8);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pRingBuffer->get_pData()[index], index < 8 ? (uint8_t)((index + 1) * 11) : defaultValue);

  pRingBuffer->Clear ();
  pRingBuffer->SetWritePointer (3);
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, true));
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (1, pSrcBuffer, true));
  pSrcBuffer->SetReadPointer (1);
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, true));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)5);
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (6, pSrcBuffer, true));
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (5, pSrcBuffer, true));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)8);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  uint8_t expectedUI8_a[9] = { 44, defaultValue, defaultValue, 33, 22, 88, 77, 66, 55 };
  for (uint16_t index = 0; index < length; index++) assertEqual (pRingBuffer->get_pData()[index], expectedUI8_a[index]);

  pSrcBuffer->SetReadPointer (2);
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (6, pSrcBuffer, true));
  uint8_t expectedUI8_b[9] = { 44, 88, 77, 66, 55, 44, 33, 66, 55 };
  for (uint16_t index = 0; index < length; index++) assertEqual (pRingBuffer->get_pData()[index], expectedUI8_b[index]);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// - WriteBytesAndMovePtr (ByteBuffer)
test (ByteBuffer_RingBuffer_WriteBytesAndMovePtr_FromLinearBuffer_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pRingBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, true, pRingBuffer);
  assertTrue (result);

  uint16_t srcLength = 10;
  ByteBuffer* pSrcBuffer = nullptr;
  assertTrue (ByteBuffer::Create (srcLength, defaultValue, false, pSrcBuffer));  // source is linear buffer
  for (uint16_t index = 0; index < srcLength; index++)
    pSrcBuffer->get_pData ()[index] = (uint8_t)((index + 1) * 11);  // -> 11 22 33 44 55 66 77 88 99 110 =  0x 0B 16 21 2C 37 42 4D 58 63 6E

  //---------- Act & Assert ----------
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (1, pSrcBuffer, false));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)1);
  assertEqual (pSrcBuffer ->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pRingBuffer->get_pData()[index], index < 1 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, false));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pRingBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertFalse (pRingBuffer->WriteBytesAndMovePtr (8, pSrcBuffer, false));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pRingBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (7, pSrcBuffer, false));
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (1, pSrcBuffer, false));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)10);
  assertEqual (pSrcBuffer ->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pRingBuffer->get_pData()[index], (uint8_t)((index + (index < 1 ? 10 : 1)) * 11));

  pRingBuffer->Clear ();
  pRingBuffer->SetWritePointer (5);
  pSrcBuffer ->SetReadPointer (2);
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (3, pSrcBuffer, true));
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)8);
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)5);
  assertFalse (pRingBuffer->WriteBytesAndMovePtr (6, pSrcBuffer, true));
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (5, pSrcBuffer, true));
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)4);
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)10);
  uint8_t expectedUI8[9] = { 99, 88, 77, 66, defaultValue, 55, 44, 33, 110 };
  for (uint16_t index = 0; index < length; index++) assertEqual (pRingBuffer->get_pData()[index], expectedUI8[index]);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// - WriteBytesAndMovePtr (ByteBuffer)
test (ByteBuffer_RingBuffer_WriteBytesAndMovePtr_FromRingBuffer_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pRingBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, true, pRingBuffer);
  assertTrue (result);

  uint16_t srcLength = 8;
  ByteBuffer* pSrcBuffer = nullptr;
  assertTrue (ByteBuffer::Create (srcLength, defaultValue, true, pSrcBuffer));  // source is ring buffer
  for (uint16_t index = 0; index < srcLength; index++)
    pSrcBuffer->get_pData ()[index] = (uint8_t)((index + 1) * 11);  // -> 11 22 33 44 55 66 77 88  =  0x 0B 16 21 2C 37 42 4D 58

  //---------- Act & Assert ----------
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (1, pSrcBuffer, false));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)1);
  assertEqual (pSrcBuffer ->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pRingBuffer->get_pData()[index], index < 1 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, false));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pRingBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertFalse (pRingBuffer->WriteBytesAndMovePtr (9, pSrcBuffer, false));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pRingBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (8, pSrcBuffer, false));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pSrcBuffer ->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pRingBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)2);
  uint8_t expectedUI8_a[9] = { 22, 33, 33, 44, 55, 66, 77, 88, 11 };
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pRingBuffer->get_pData()[index], expectedUI8_a[index]);

  pRingBuffer->Clear ();
  pRingBuffer->SetWritePointer (4);
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, true));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)5);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)6);
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (4, pSrcBuffer, true));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)1);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  assertTrue  (pRingBuffer->WriteBytesAndMovePtr (2 , pSrcBuffer, true));
  assertEqual (pSrcBuffer ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  uint8_t expectedUI8_b[9] = { 66, 33, 22, defaultValue, 55, 44, 11, 88, 77 };
  for (uint16_t index = 0; index < length; index++) assertEqual (pRingBuffer->get_pData()[index], expectedUI8_b[index]);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// - WriteValueAndMovePtr ()
// - get_pData ()
test (ByteBuffer_RingBuffer_WriteValueAndMovePtr_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pRingBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, true, pRingBuffer);
  assertTrue (result);

  //---------- Act & Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_CurrentWriteAddress (), index);
    pRingBuffer->WriteValueAndMovePtr (index % 2 > 0);
  }
  bool valueB = false;
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_pData ()[index] > 0, valueB);
    valueB = !valueB;
  }

  pRingBuffer->Clear ();
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_CurrentWriteAddress (), index);
    pRingBuffer->WriteValueAndMovePtr ((uint8_t)((8 - index) * 11));
  }
  uint8_t valueUI8 = 88;
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_pData ()[index], valueUI8);
    valueUI8 -= 11;
  }

  pRingBuffer->Clear ();
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_CurrentWriteAddress (), index);
    pRingBuffer->WriteValueAndMovePtr ((int8_t)((8 - index) * 11));
  }
  int8_t valueI8 = 88;
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_pData ()[index], valueI8);
    valueI8 -= 11;
  }

  pRingBuffer->Clear ();
  uint16_t valueUI16 = 0x0111;
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, false)); valueUI16 += 0x0211;
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)2); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, false)); valueUI16 += 0x0311;
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)4); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, false)); valueUI16 += 0x0411;
  assertEqual (pRingBuffer->get_pData ()[0], 0x11); assertEqual (pRingBuffer->get_pData ()[1], 0x01);
  assertEqual (pRingBuffer->get_pData ()[2], 0x22); assertEqual (pRingBuffer->get_pData ()[3], 0x03);
  assertEqual (pRingBuffer->get_pData ()[4], 0x33); assertEqual (pRingBuffer->get_pData ()[5], 0x06);
  assertEqual (pRingBuffer->get_pData ()[6], 0xAB); assertEqual (pRingBuffer->get_pData ()[7], 0xAB);
  assertEqual (pRingBuffer->get_pData ()[8], 0xAB);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)6); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, false)); valueUI16 += 0x0511;
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)8); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, false)); valueUI16 += 0x0611;
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)1); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, false)); valueUI16 += 0x0711;
  assertEqual (pRingBuffer->get_pData ()[0], 0x0F); assertEqual (pRingBuffer->get_pData ()[1], 0x66);
  assertEqual (pRingBuffer->get_pData ()[2], 0x15); assertEqual (pRingBuffer->get_pData ()[3], 0x03);
  assertEqual (pRingBuffer->get_pData ()[4], 0x33); assertEqual (pRingBuffer->get_pData ()[5], 0x06);
  assertEqual (pRingBuffer->get_pData ()[6], 0x44); assertEqual (pRingBuffer->get_pData ()[7], 0x0A);
  assertEqual (pRingBuffer->get_pData ()[8], 0x55);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)3); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, false)); valueUI16 += 0x0811;
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)5); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, false)); valueUI16 += 0x0911;
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)7); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, false));
  assertEqual (pRingBuffer->get_pData ()[0], 0x0F); assertEqual (pRingBuffer->get_pData ()[1], 0x66);
  assertEqual (pRingBuffer->get_pData ()[2], 0x15); assertEqual (pRingBuffer->get_pData ()[3], 0x77);
  assertEqual (pRingBuffer->get_pData ()[4], 0x1C); assertEqual (pRingBuffer->get_pData ()[5], 0x88);
  assertEqual (pRingBuffer->get_pData ()[6], 0x24); assertEqual (pRingBuffer->get_pData ()[7], 0x99);
  assertEqual (pRingBuffer->get_pData ()[8], 0x2D);

  pRingBuffer->Clear ();
  valueUI16 = 0x0111;
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, true)); valueUI16 += 0x0211;
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)2); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, true)); valueUI16 += 0x0311;
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)4); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, true)); valueUI16 += 0x0411;
  assertEqual (pRingBuffer->get_pData ()[0], 0x01); assertEqual (pRingBuffer->get_pData ()[1], 0x11);
  assertEqual (pRingBuffer->get_pData ()[2], 0x03); assertEqual (pRingBuffer->get_pData ()[3], 0x22);
  assertEqual (pRingBuffer->get_pData ()[4], 0x06); assertEqual (pRingBuffer->get_pData ()[5], 0x33);
  assertEqual (pRingBuffer->get_pData ()[6], 0xAB); assertEqual (pRingBuffer->get_pData ()[7], 0xAB);
  assertEqual (pRingBuffer->get_pData ()[8], 0xAB);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)6); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, true)); valueUI16 += 0x0511;
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)8); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, true)); valueUI16 += 0x0611;
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)1); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, true)); valueUI16 += 0x0711;
  assertEqual (pRingBuffer->get_pData ()[0], 0x55); assertEqual (pRingBuffer->get_pData ()[1], 0x15);
  assertEqual (pRingBuffer->get_pData ()[2], 0x66); assertEqual (pRingBuffer->get_pData ()[3], 0x22);
  assertEqual (pRingBuffer->get_pData ()[4], 0x06); assertEqual (pRingBuffer->get_pData ()[5], 0x33);
  assertEqual (pRingBuffer->get_pData ()[6], 0x0A); assertEqual (pRingBuffer->get_pData ()[7], 0x44);
  assertEqual (pRingBuffer->get_pData ()[8], 0x0F);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)3); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, true)); valueUI16 += 0x0811;
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)5); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, true)); valueUI16 += 0x0911;
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)7); assertTrue (pRingBuffer->WriteValueAndMovePtr (valueUI16, true));
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pRingBuffer->get_pData ()[0], 0x55); assertEqual (pRingBuffer->get_pData ()[1], 0x15);
  assertEqual (pRingBuffer->get_pData ()[2], 0x66); assertEqual (pRingBuffer->get_pData ()[3], 0x1C);
  assertEqual (pRingBuffer->get_pData ()[4], 0x77); assertEqual (pRingBuffer->get_pData ()[5], 0x24);
  assertEqual (pRingBuffer->get_pData ()[6], 0x88); assertEqual (pRingBuffer->get_pData ()[7], 0x2D);
  assertEqual (pRingBuffer->get_pData ()[8], 0x99);

  uint8_t expected_UI16[27]     = { 0x34, 0x12,   0x78, 0x56,   0xBC, 0x9A,   0x00, 0xDE,   0x44, 0x22,   0x88, 0x66,   0xCC, 0xAA,   0x10, 0xEE,   0x54, 0x32,   0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB };
  uint8_t expected_UI16_Inv[27] = { 0x12, 0x34,   0x56, 0x78,   0x9A, 0xBC,   0xDE, 0x00,   0x22, 0x44,   0x66, 0x88,   0xAA, 0xCC,   0xEE, 0x10,   0x32, 0x54,   0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB };

  pRingBuffer->Clear ();
  uint16_t address = 0;
  int16_t valueI16 = 0x1234;
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_CurrentWriteAddress (), address);
    assertTrue  (pRingBuffer->WriteValueAndMovePtr (valueI16, false));
    uint16_t offset = index * 2;
    for (uint16_t check = 0; check < 2; check++)
    {
      assertEqual (pRingBuffer->get_pData ()[(offset + check    ) % length], expected_UI16[ offset + check               ]);
      assertEqual (pRingBuffer->get_pData ()[(offset + check + 2) % length], expected_UI16[(offset + check + 2 + 18) % 27]);
    }
    valueI16 = valueI16 + 0x0044 & 0x00FF | valueI16 + 0x4400 & 0xFF00;
    address = (address + 2) % length;
  }

  pRingBuffer->Clear ();
  address = 0;
  valueI16 = 0x1234;
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_CurrentWriteAddress (), address);
    assertTrue  (pRingBuffer->WriteValueAndMovePtr (valueI16, true));
    uint16_t offset = index * 2;
    for (uint16_t check = 0; check < 2; check++)
    {
      assertEqual (pRingBuffer->get_pData ()[(offset + check    ) % length], expected_UI16_Inv[ offset + check               ]);
      assertEqual (pRingBuffer->get_pData ()[(offset + check + 2) % length], expected_UI16_Inv[(offset + check + 2 + 18) % 27]);
    }
    valueI16 = valueI16 + 0x0044 & 0x00FF | valueI16 + 0x4400 & 0xFF00;
    address = (address + 2) % length;
  }

  uint8_t expected_UI32[45]     = { 0x78, 0x56, 0x34, 0x12,   0x00, 0xDE, 0xBC, 0x9A,   0x88, 0x66, 0x44, 0x22,   0x10, 0xEE, 0xCC, 0xAA,   0x98, 0x76, 0x54, 0x32,   0x20, 0xFE, 0xDC, 0xBA,   0xA8, 0x86, 0x64, 0x42,   0x30, 0x0E, 0xEC, 0xCA,   0xB8, 0x96, 0x74, 0x52,   0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB };
  uint8_t expected_UI32_Inv[45] = { 0x12, 0x34, 0x56, 0x78,   0x9A, 0xBC, 0xDE, 0x00,   0x22, 0x44, 0x66, 0x88,   0xAA, 0xCC, 0xEE, 0x10,   0x32, 0x54, 0x76, 0x98,   0xBA, 0xDC, 0xFE, 0x20,   0x42, 0x64, 0x86, 0xA8,   0xCA, 0xEC, 0x0E, 0x30,   0x52, 0x74, 0x96, 0xB8,   0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB };

  pRingBuffer->Clear ();
  address = 0;
  uint32_t valueUI32 = 0x12345678;
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_CurrentWriteAddress (), address);
    assertTrue  (pRingBuffer->WriteValueAndMovePtr (valueUI32, false));
    uint16_t offset = index * 4;
    for (uint16_t check = 0; check < 2; check++)
    {
      assertEqual (pRingBuffer->get_pData ()[(offset + check    ) % length], expected_UI32[ offset + check               ]);
      assertEqual (pRingBuffer->get_pData ()[(offset + check + 4) % length], expected_UI32[(offset + check + 4 + 36) % 45]);
    }
    valueUI32 = valueUI32 + 0x00000088 & 0x000000FF | valueUI32 + 0x00008800 & 0x0000FF00 | valueUI32 + 0x00880000 & 0x00FF0000 | valueUI32 + 0x88000000 & 0xFF000000;
    address = (address + 4) % length;
  }

  pRingBuffer->Clear ();
  address = 0;
  valueUI32 = 0x12345678;
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_CurrentWriteAddress (), address);
    assertTrue  (pRingBuffer->WriteValueAndMovePtr (valueUI32, true));
    uint16_t offset = index * 4;
    for (uint16_t check = 0; check < 2; check++)
    {
      assertEqual (pRingBuffer->get_pData ()[(offset + check    ) % length], expected_UI32_Inv[ offset + check               ]);
      assertEqual (pRingBuffer->get_pData ()[(offset + check + 4) % length], expected_UI32_Inv[(offset + check + 4 + 36) % 45]);
    }
    valueUI32 = valueUI32 + 0x00000088 & 0x000000FF | valueUI32 + 0x00008800 & 0x0000FF00 | valueUI32 + 0x00880000 & 0x00FF0000 | valueUI32 + 0x88000000 & 0xFF000000;
    address = (address + 4) % length;
  }

  pRingBuffer->Clear ();
  address = 0;
  int32_t valueI32 = 0x12345678;
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_CurrentWriteAddress (), address);
    assertTrue  (pRingBuffer->WriteValueAndMovePtr (valueI32, false));
    uint16_t offset = index * 4;
    for (uint16_t check = 0; check < 2; check++)
    {
      assertEqual (pRingBuffer->get_pData ()[(offset + check    ) % length], expected_UI32[ offset + check               ]);
      assertEqual (pRingBuffer->get_pData ()[(offset + check + 4) % length], expected_UI32[(offset + check + 4 + 36) % 45]);
    }
    valueI32 = valueI32 + 0x00000088 & 0x000000FF | valueI32 + 0x00008800 & 0x0000FF00 | valueI32 + 0x00880000 & 0x00FF0000 | valueI32 + 0x88000000 & 0xFF000000;
    address = (address + 4) % length;
  }

  pRingBuffer->Clear ();
  address = 0;
  valueI32 = 0x12345678;
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_CurrentWriteAddress (), address);
    assertTrue  (pRingBuffer->WriteValueAndMovePtr (valueI32, true));
    uint16_t offset = index * 4;
    for (uint16_t check = 0; check < 2; check++)
    {
      assertEqual (pRingBuffer->get_pData ()[(offset + check    ) % length], expected_UI32_Inv[ offset + check               ]);
      assertEqual (pRingBuffer->get_pData ()[(offset + check + 4) % length], expected_UI32_Inv[(offset + check + 4 + 36) % 45]);
    }
    valueI32 = valueI32 + 0x00000088 & 0x000000FF | valueI32 + 0x00008800 & 0x0000FF00 | valueI32 + 0x00880000 & 0x00FF0000 | valueI32 + 0x88000000 & 0xFF000000;
    address = (address + 4) % length;
  }
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_From ()
test (ByteBuffer_RingBuffer_WriteRange_From_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pRingBuffer = nullptr;
  assertTrue (ByteBuffer::Create (length, defaultValue, true, pRingBuffer));

  //---------- Act & Assert ----------
  assertTrue  (pRingBuffer->WriteRange_From ( 0,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_From ( 1,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_From ( 8,  0, 0x00));
  assertFalse (pRingBuffer->WriteRange_From ( 9,  0, 0x00));
  assertFalse (pRingBuffer->WriteRange_From (99,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_From ( 0,  1, 0x00));
  assertTrue  (pRingBuffer->WriteRange_From ( 0,  8, 0x00));
  assertTrue  (pRingBuffer->WriteRange_From ( 0,  9, 0x00));
  assertFalse (pRingBuffer->WriteRange_From ( 0, 10, 0x00));
  assertFalse (pRingBuffer->WriteRange_From ( 0, 99, 0x00));
  assertTrue  (pRingBuffer->WriteRange_From ( 6,  1, 0x00));
  assertTrue  (pRingBuffer->WriteRange_From ( 6,  8, 0x00));
  assertTrue  (pRingBuffer->WriteRange_From ( 6,  9, 0x00));
  assertFalse (pRingBuffer->WriteRange_From ( 6, 10, 0x00));
  assertFalse (pRingBuffer->WriteRange_From ( 6, 99, 0x00));

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_From ()
// - Clear ()
test (ByteBuffer_RingBuffer_WriteRange_From_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint8_t newValue1 = 0x12;
  uint8_t newValue2 = 0x34;
  uint8_t newValue3 = 0x56;
  uint16_t length = 9;
  uint8_t valueUI8 = 0;
  ByteBuffer* pRingBuffer = nullptr;
  assertTrue (ByteBuffer::Create (length, defaultValue, true, pRingBuffer));

  uint8_t expected_UI8[9] = { newValue1, defaultValue, defaultValue, newValue2, newValue2, newValue2, newValue2, defaultValue, defaultValue };

  //---------- Act ----------
  assertTrue (pRingBuffer->WriteRange_From (0, 1, newValue1));
  assertTrue (pRingBuffer->WriteRange_From (3, 4, newValue2));

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, expected_UI8[index]);
  }

  //---------- Act ----------
  pRingBuffer->Clear ();

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, defaultValue);
  }

  //---------- Act ----------
  assertTrue (pRingBuffer->WriteRange_From (2, 4, newValue3));

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, index >= 2 && index < 6 ? newValue3 : defaultValue);
  }

  //---------- Act ----------
  assertTrue (pRingBuffer->WriteRange_From (6, 6, newValue2));

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, index >= 6 || index < 3 ? newValue2 : newValue3);
  }

  //---------- Act ----------
  assertTrue (pRingBuffer->WriteRange_From (0, 9, newValue1));

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, newValue1);
  }
  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_To ()
test (ByteBuffer_RingBuffer_WriteRange_To_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pRingBuffer = nullptr;
  assertTrue (ByteBuffer::Create (length, defaultValue, true, pRingBuffer));

  //---------- Act & Assert ----------
  assertTrue  (pRingBuffer->WriteRange_To ( 0,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_To ( 1,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_To ( 8,  0, 0x00));
  assertFalse (pRingBuffer->WriteRange_To ( 9,  0, 0x00));
  assertFalse (pRingBuffer->WriteRange_To (99,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_To ( 0,  1, 0x00));
  assertTrue  (pRingBuffer->WriteRange_To ( 0,  8, 0x00));
  assertTrue  (pRingBuffer->WriteRange_To ( 0,  9, 0x00));
  assertFalse (pRingBuffer->WriteRange_To ( 0, 10, 0x00));
  assertFalse (pRingBuffer->WriteRange_To ( 0, 99, 0x00));
  assertTrue  (pRingBuffer->WriteRange_To ( 6,  1, 0x00));
  assertTrue  (pRingBuffer->WriteRange_To ( 6,  8, 0x00));
  assertTrue  (pRingBuffer->WriteRange_To ( 6,  9, 0x00));
  assertFalse (pRingBuffer->WriteRange_To ( 6, 10, 0x00));
  assertFalse (pRingBuffer->WriteRange_To ( 6, 99, 0x00));

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_To ()
// - Clear ()
test (ByteBuffer_RingBuffer_WriteRange_To_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint8_t newValue1 = 0x12;
  uint8_t newValue2 = 0x34;
  uint8_t newValue3 = 0x56;
  uint16_t length = 9;
  uint8_t valueUI8 = 0;
  ByteBuffer* pRingBuffer = nullptr;
  assertTrue (ByteBuffer::Create (length, defaultValue, true, pRingBuffer));

  uint8_t expected_UI8[9] = { defaultValue, defaultValue, newValue2, newValue2, newValue2, newValue2, defaultValue, defaultValue, newValue1 };

  //---------- Act ----------
  assertTrue (pRingBuffer->WriteRange_To (0, 1, newValue1));
  assertTrue (pRingBuffer->WriteRange_To (6, 4, newValue2));

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, expected_UI8[index]);
  }

  //---------- Act ----------
  pRingBuffer->Clear ();

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, defaultValue);
  }

  //---------- Act ----------
  assertTrue (pRingBuffer->WriteRange_To (2, 4, newValue3));

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, index < 2 || index > 6 ? newValue3 : defaultValue);
  }

  //---------- Act ----------
  assertTrue (pRingBuffer->WriteRange_To (8, 6, newValue2));

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, index >= 2 && index < 8 ? newValue2 : newValue3);
  }

  //---------- Act ----------
  assertTrue (pRingBuffer->WriteRange_To (0, 9, newValue1));

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, newValue1);
  }

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_FromTo ()
test (ByteBuffer_RingBuffer_WriteRange_FromTo_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pRingBuffer = nullptr;
  assertTrue (ByteBuffer::Create (length, defaultValue, true, pRingBuffer));

  //---------- Act & Assert ----------
  assertTrue  (pRingBuffer->WriteRange_FromTo ( 0,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_FromTo ( 1,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_FromTo ( 8,  0, 0x00));
  assertFalse (pRingBuffer->WriteRange_FromTo ( 9,  0, 0x00));
  assertFalse (pRingBuffer->WriteRange_FromTo (99,  0, 0x00));
  assertTrue  (pRingBuffer->WriteRange_FromTo ( 0,  1, 0x00));
  assertTrue  (pRingBuffer->WriteRange_FromTo ( 0,  8, 0x00));
  assertFalse (pRingBuffer->WriteRange_FromTo ( 0,  9, 0x00));
  assertFalse (pRingBuffer->WriteRange_FromTo ( 0, 99, 0x00));
  assertTrue  (pRingBuffer->WriteRange_FromTo ( 6,  1, 0x00));
  assertTrue  (pRingBuffer->WriteRange_FromTo ( 6,  8, 0x00));
  assertFalse (pRingBuffer->WriteRange_FromTo ( 6,  9, 0x00));
  assertFalse (pRingBuffer->WriteRange_FromTo ( 6, 99, 0x00));

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_FromTo ()
// - Clear ()
test (ByteBuffer_RingBuffer_WriteRange_FromTo_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint8_t newValue1 = 0x12;
  uint8_t newValue2 = 0x34;
  uint8_t newValue3 = 0x56;
  uint16_t length = 9;
  uint8_t valueUI8 = 0;
  ByteBuffer* pRingBuffer = nullptr;
  assertTrue (ByteBuffer::Create (length, defaultValue, true, pRingBuffer));

  uint8_t expected_UI8[9] = { newValue1, defaultValue, defaultValue, newValue2, newValue2, newValue2, newValue2, defaultValue, defaultValue };

  //---------- Act ----------
  assertTrue (pRingBuffer->WriteRange_FromTo (0, 1, newValue1));
  assertTrue (pRingBuffer->WriteRange_FromTo (3, 7, newValue2));

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue (pRingBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, expected_UI8[index]);
  }

  //---------- Act ----------
  pRingBuffer->Clear ();

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, defaultValue);
  }

  //---------- Act ----------
  assertTrue (pRingBuffer->WriteRange_FromTo (4, 2, newValue3));

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, index < 2 || index >= 4 ? newValue3 : defaultValue);
  }

  //---------- Act ----------
  assertTrue (pRingBuffer->WriteRange_FromTo (2, 4, newValue1));

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, index < 2 || index >= 4 ? newValue3 : newValue1);
  }

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
