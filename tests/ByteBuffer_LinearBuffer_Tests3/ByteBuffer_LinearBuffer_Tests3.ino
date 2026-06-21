#include <AUnit.h>

#include <ByteBuffer.h>

//--------------------------------------------------------------------
// - WriteBytesAndMovePtr (array)
test (ByteBuffer_LinearBuffer_WriteBytesAndMovePtr_FromArray_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pLinearBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, false, pLinearBuffer);
  assertTrue (result);
  uint8_t cache[10];

  //---------- Act & Assert ----------
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (0,  (uint8_t*)nullptr, false));
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (1,  (uint8_t*)nullptr, false));
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (10, (uint8_t*)nullptr, false));
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (0,  cache, false));
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (2,  cache, false));
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (9,  cache, false));
  pLinearBuffer->SetWritePointer (4);
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (9,  cache, false));
  pLinearBuffer->SetWritePointer (0);
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (9,  cache, false));
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (10, cache, false));
  pLinearBuffer->SetWritePointer (0);
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (10, cache, false));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (0,  cache, true));
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (2,  cache, true));
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (9,  cache, true));
  pLinearBuffer->SetWritePointer (0);
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (9,  cache, true));
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (10, cache, true));
  pLinearBuffer->SetWritePointer (0);
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (10, cache, true));

  //---------- Cleanup ----------
  delete (pLinearBuffer);
}

//--------------------------------------------------------------------
// - WriteBytesAndMovePtr (array)
// - get_pData ()
test (ByteBuffer_LinearBuffer_WriteBytesAndMovePtr_FromArray_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pLinearBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, false, pLinearBuffer);
  assertTrue (result);

  uint8_t cacheSize = 10;
  uint8_t cache[10];
  memset (&cache, 0x00, sizeof (cache));

  //---------- Act & Assert ----------
  pLinearBuffer->Clear ();
  assertTrue (pLinearBuffer->WriteBytesAndMovePtr (3, cache, false));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pLinearBuffer->get_pData ()[index], index < 3 ? 0x00 : defaultValue);

  pLinearBuffer->Clear ();
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (7, cache, false));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pLinearBuffer->get_pData ()[index], defaultValue);

  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (6, cache, false));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)9);
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pLinearBuffer->get_pData ()[index], index >= 3 && index <= 9 ? 0x00 : defaultValue);

  //---------- Arrange ----------
  for (uint16_t index = 0; index < cacheSize; index++)
    cache[index] = ((index + 8) % cacheSize) * 11;  // -> 88 99 00 11 22 33 44 55 66 77

  //---------- Act & Assert ----------
  pLinearBuffer->Clear ();
  pLinearBuffer->SetWritePointer (1);  // -> writing starts at address 0
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (9, cache + 1, false));  // buffer: no change, all default
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (8, cache + 1, false));  // buffer: AB 99 00 11 22 33 44 55 66
  pLinearBuffer->SetWritePointer (0);
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (1, cache + 9, false));  // buffer: 77 99 00 11 22 33 44 55 66
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  assertEqual (pLinearBuffer->get_pData ()[0], 77);
  assertEqual (pLinearBuffer->get_pData ()[1], 99);
  assertEqual (pLinearBuffer->get_pData ()[2], 00);
  assertEqual (pLinearBuffer->get_pData ()[3], 11);
  assertEqual (pLinearBuffer->get_pData ()[4], 22);
  assertEqual (pLinearBuffer->get_pData ()[5], 33);
  assertEqual (pLinearBuffer->get_pData ()[6], 44);
  assertEqual (pLinearBuffer->get_pData ()[7], 55);
  assertEqual (pLinearBuffer->get_pData ()[8], 66);

  pLinearBuffer->Clear ();
  pLinearBuffer->SetWritePointer (5);  // -> writeing starts at address 5
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (4, cache + 3, true));  // buffer: AB AB AB AB AB 44 33 22 11
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (2, cache    , true));  // buffer: no change
  pLinearBuffer->SetWritePointer (0);
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (2, cache    , true));  // buffer: 99 88 AB AB AB 44 33 22 11
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (3, cache + 7, true));  // buffer: 99 88 77 66 55 44 33 22 11
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pLinearBuffer->get_pData ()[index], (uint8_t)((9 - index) * 11));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)5);

  //---------- Cleanup ----------
  delete (pLinearBuffer);
}

//--------------------------------------------------------------------
// - WriteBytesAndMovePtr (ByteBuffer)
test (ByteBuffer_LinearBuffer_WriteBytesAndMovePtr_FromLinearBuffer_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pLinearBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, false, pLinearBuffer);
  assertTrue (result);

  uint16_t srcLength = 8;
  ByteBuffer* pSrcBuffer = nullptr;
  assertTrue (ByteBuffer::Create (srcLength, defaultValue, false, pSrcBuffer));  // source is linear buffer
  for (uint16_t index = 0; index < srcLength; index++)
    pSrcBuffer->get_pData ()[index] = (uint8_t)((index + 1) * 11);  // -> 11 22 33 44 55 66 77 88  =  0x 0B 16 21 2C 37 42 4D 58

  //---------- Act & Assert ----------
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (0,  (ByteBuffer*)nullptr, false));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (1,  (ByteBuffer*)nullptr, false));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (10, (ByteBuffer*)nullptr, false));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  for (uint16_t index = 0; index < length; index++) assertEqual (pLinearBuffer->get_pData()[index], defaultValue);

  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (0, pSrcBuffer, false));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer   ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pLinearBuffer->get_pData()[index], defaultValue);

  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (1, pSrcBuffer, false));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)1);
  assertEqual (pSrcBuffer   ->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pLinearBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer   ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pLinearBuffer->get_pData()[index], index < 1 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, false));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer   ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pLinearBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (7, pSrcBuffer, false));
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (6, pSrcBuffer, false));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer   ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pLinearBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (5, pSrcBuffer, false));
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (1, pSrcBuffer, false));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)8);
  assertEqual (pSrcBuffer   ->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pLinearBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)8);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer   ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pLinearBuffer->get_pData()[index], index < 8 ? (uint8_t)((index + 1) * 11) : defaultValue);

  pLinearBuffer->Clear ();
  pLinearBuffer->SetWritePointer (3);
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, true));
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (1, pSrcBuffer, true));
  pSrcBuffer->SetReadPointer (1);
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, true));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)5);
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (5, pSrcBuffer, true));
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (4, pSrcBuffer, true));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)7);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)9);
  uint8_t expectedUI8_a[9] = { defaultValue, defaultValue, defaultValue, 33, 22, 77, 66, 55, 44 };
  for (uint16_t index = 0; index < length; index++) assertEqual (pLinearBuffer->get_pData()[index], expectedUI8_a[index]);

  pLinearBuffer->SetWritePointer (0);
  pSrcBuffer   ->SetReadPointer  (4);
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (3, pSrcBuffer, true));
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, true));
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (1, pSrcBuffer, true));
  uint8_t expectedUI8_b[9] = { 77, 66, 55, 88, 22, 77, 66, 55, 44 };
  for (uint16_t index = 0; index < length; index++) assertEqual (pLinearBuffer->get_pData()[index], expectedUI8_b[index]);

  //---------- Cleanup ----------
  delete (pLinearBuffer);
}

//--------------------------------------------------------------------
// - WriteBytesAndMovePtr (ByteBuffer)
test (ByteBuffer_LinearBuffer_WriteBytesAndMovePtr_FromLinearBuffer_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pLinearBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, false, pLinearBuffer);
  assertTrue (result);

  uint16_t srcLength = 10;
  ByteBuffer* pSrcBuffer = nullptr;
  assertTrue (ByteBuffer::Create (srcLength, defaultValue, false, pSrcBuffer));  // source is linear buffer
  for (uint16_t index = 0; index < srcLength; index++)
    pSrcBuffer->get_pData ()[index] = (uint8_t)((index + 1) * 11);  // -> 11 22 33 44 55 66 77 88 99 110 =  0x 0B 16 21 2C 37 42 4D 58 63 6E

  //---------- Act & Assert ----------
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (1, pSrcBuffer, false));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)1);
  assertEqual (pSrcBuffer   ->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pLinearBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer   ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pLinearBuffer->get_pData()[index], index < 1 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, false));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer   ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pLinearBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (7, pSrcBuffer, false));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer   ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pLinearBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (6, pSrcBuffer, false));
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (1, pSrcBuffer, false));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)9);
  assertEqual (pSrcBuffer   ->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pLinearBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)9);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer   ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pLinearBuffer->get_pData()[index], index < length ? (uint8_t)((index + 1) * 11) : defaultValue);

  pLinearBuffer->Clear ();
  pLinearBuffer->SetWritePointer (3);
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, true));
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (1, pSrcBuffer, true));
  pSrcBuffer->SetReadPointer (1);
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, true));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)6);
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (4, pSrcBuffer, true));
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (3, pSrcBuffer, true));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)6);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)9);
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (1, pSrcBuffer, true));
  uint8_t expectedUI8_a[9] = { defaultValue, defaultValue, defaultValue, 110, 33, 22, 66, 55, 44 };
  for (uint16_t index = 0; index < length; index++) assertEqual (pLinearBuffer->get_pData()[index], expectedUI8_a[index]);

  pLinearBuffer->SetWritePointer (0);
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, true));
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (3, pSrcBuffer, true));
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, true));
  uint8_t expectedUI8_b[9] = { 88, 77, 110, 99, 33, 22, 66, 55, 44 };
  for (uint16_t index = 0; index < length; index++) assertEqual (pLinearBuffer->get_pData()[index], expectedUI8_b[index]);

  //---------- Cleanup ----------
  delete (pLinearBuffer);
}

//--------------------------------------------------------------------
// - WriteBytesAndMovePtr (ByteBuffer)
test (ByteBuffer_LinearBuffer_WriteBytesAndMovePtr_FromRingBuffer_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pLinearBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, false, pLinearBuffer);
  assertTrue (result);

  uint16_t srcLength = 8;
  ByteBuffer* pSrcBuffer = nullptr;
  assertTrue (ByteBuffer::Create (srcLength, defaultValue, true, pSrcBuffer));  // source is ring buffer
  for (uint16_t index = 0; index < srcLength; index++)
    pSrcBuffer->get_pData ()[index] = (uint8_t)((index + 1) * 11);  // -> 11 22 33 44 55 66 77 88  =  0x 0B 16 21 2C 37 42 4D 58

  //---------- Act & Assert ----------
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (1, pSrcBuffer, false));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)1);
  assertEqual (pSrcBuffer   ->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pLinearBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer   ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pLinearBuffer->get_pData()[index], index < 1 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, false));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer   ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pLinearBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (7, pSrcBuffer, false));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer   ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pLinearBuffer->get_pData()[index], index < 3 ? (uint8_t)((index + 1) * 11) : defaultValue);

  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (6, pSrcBuffer, false));
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (1, pSrcBuffer, false));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)1);
  assertEqual (pSrcBuffer   ->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pLinearBuffer->get_CurrentReadAddress  (), (uint16_t)0);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)9);
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pSrcBuffer   ->get_pData()[index], (uint8_t)((index + 1) * 11));
  for (uint16_t index = 0; index < length;    index++) assertEqual (pLinearBuffer->get_pData()[index], (uint8_t)((index % srcLength + 1) * 11));

  pLinearBuffer->Clear ();
  pLinearBuffer->SetWritePointer (3);
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, true));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)5);
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (4, pSrcBuffer, true));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)7);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)9);
  assertFalse (pLinearBuffer->WriteBytesAndMovePtr (2, pSrcBuffer, true));
  pLinearBuffer->SetWritePointer (0);
  assertTrue  (pLinearBuffer->WriteBytesAndMovePtr (4, pSrcBuffer, true));
  assertEqual (pSrcBuffer   ->get_CurrentReadAddress  (), (uint16_t)3);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)4);
  uint8_t expectedUI8[9] = { 33, 22, 11, 88, 22, 77, 66, 55, 44 };
  for (uint16_t index = 0; index < srcLength; index++) assertEqual (pLinearBuffer->get_pData()[index], expectedUI8[index]);

  //---------- Cleanup ----------
  delete (pLinearBuffer);
}

//--------------------------------------------------------------------
// - WriteValueAndMovePtr ()
// - get_pData ()
test (ByteBuffer_LinearBuffer_WriteValueAndMovePtr_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pLinearBuffer = nullptr;
  bool result = ByteBuffer::Create (length, defaultValue, false, pLinearBuffer);
  assertTrue (result);

  //---------- Act & Assert ----------
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pLinearBuffer->get_CurrentWriteAddress (), index);
    pLinearBuffer->WriteValueAndMovePtr (index % 2 > 0);
  }
  bool valueB = false;
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pLinearBuffer->get_pData ()[index] > 0, valueB);
    valueB = !valueB;
  }

  pLinearBuffer->Clear ();
  pLinearBuffer->SetWritePointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pLinearBuffer->get_CurrentWriteAddress (), index);
    pLinearBuffer->WriteValueAndMovePtr ((uint8_t)((8 - index) * 11));
  }
  uint8_t valueUI8 = 88;
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pLinearBuffer->get_pData ()[index], valueUI8);
    valueUI8 -= 11;
  }

  pLinearBuffer->Clear ();
  pLinearBuffer->SetWritePointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pLinearBuffer->get_CurrentWriteAddress (), index);
    pLinearBuffer->WriteValueAndMovePtr ((int8_t)((8 - index) * 11));
  }
  int8_t valueI8 = 88;
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pLinearBuffer->get_pData ()[index], valueI8);
    valueI8 -= 11;
  }

  pLinearBuffer->Clear ();
  uint16_t valueUI16 = 0x0111;
  pLinearBuffer->SetWritePointer (0);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)0); assertTrue (pLinearBuffer->WriteValueAndMovePtr (valueUI16, false)); valueUI16 += 0x0211;
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)2); assertTrue (pLinearBuffer->WriteValueAndMovePtr (valueUI16, false)); valueUI16 += 0x0311;
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)4); assertTrue (pLinearBuffer->WriteValueAndMovePtr (valueUI16, false)); valueUI16 += 0x0411;
  assertEqual (pLinearBuffer->get_pData ()[0], 0x11); assertEqual (pLinearBuffer->get_pData ()[1], 0x01);
  assertEqual (pLinearBuffer->get_pData ()[2], 0x22); assertEqual (pLinearBuffer->get_pData ()[3], 0x03);
  assertEqual (pLinearBuffer->get_pData ()[4], 0x33); assertEqual (pLinearBuffer->get_pData ()[5], 0x06);
  assertEqual (pLinearBuffer->get_pData ()[6], 0xAB); assertEqual (pLinearBuffer->get_pData ()[7], 0xAB);
  assertEqual (pLinearBuffer->get_pData ()[8], 0xAB);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)6); assertTrue  (pLinearBuffer->WriteValueAndMovePtr (valueUI16, false)); valueUI16 += 0x0511;
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)8); assertFalse (pLinearBuffer->WriteValueAndMovePtr (valueUI16, false)); valueUI16 += 0x0611;
  pLinearBuffer->SetWritePointer (1);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)1); assertTrue  (pLinearBuffer->WriteValueAndMovePtr (valueUI16, false)); valueUI16 += 0x0711;
  assertEqual (pLinearBuffer->get_pData ()[0], 0x11); assertEqual (pLinearBuffer->get_pData ()[1], 0x66);
  assertEqual (pLinearBuffer->get_pData ()[2], 0x15); assertEqual (pLinearBuffer->get_pData ()[3], 0x03);
  assertEqual (pLinearBuffer->get_pData ()[4], 0x33); assertEqual (pLinearBuffer->get_pData ()[5], 0x06);
  assertEqual (pLinearBuffer->get_pData ()[6], 0x44); assertEqual (pLinearBuffer->get_pData ()[7], 0x0A);
  assertEqual (pLinearBuffer->get_pData ()[8], 0xAB);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)3); assertTrue (pLinearBuffer->WriteValueAndMovePtr (valueUI16, false)); valueUI16 += 0x0811;
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)5); assertTrue (pLinearBuffer->WriteValueAndMovePtr (valueUI16, false)); valueUI16 += 0x0911;
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)7); assertTrue (pLinearBuffer->WriteValueAndMovePtr (valueUI16, false));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)9);
  assertEqual (pLinearBuffer->get_pData ()[0], 0x11); assertEqual (pLinearBuffer->get_pData ()[1], 0x66);
  assertEqual (pLinearBuffer->get_pData ()[2], 0x15); assertEqual (pLinearBuffer->get_pData ()[3], 0x77);
  assertEqual (pLinearBuffer->get_pData ()[4], 0x1C); assertEqual (pLinearBuffer->get_pData ()[5], 0x88);
  assertEqual (pLinearBuffer->get_pData ()[6], 0x24); assertEqual (pLinearBuffer->get_pData ()[7], 0x99);
  assertEqual (pLinearBuffer->get_pData ()[8], 0x2D);

  pLinearBuffer->Clear ();
  valueUI16 = 0x0111;
  pLinearBuffer->SetWritePointer (0);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)0); assertTrue (pLinearBuffer->WriteValueAndMovePtr (valueUI16, true)); valueUI16 += 0x0211;
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)2); assertTrue (pLinearBuffer->WriteValueAndMovePtr (valueUI16, true)); valueUI16 += 0x0311;
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)4); assertTrue (pLinearBuffer->WriteValueAndMovePtr (valueUI16, true)); valueUI16 += 0x0411;
  assertEqual (pLinearBuffer->get_pData ()[0], 0x01); assertEqual (pLinearBuffer->get_pData ()[1], 0x11);
  assertEqual (pLinearBuffer->get_pData ()[2], 0x03); assertEqual (pLinearBuffer->get_pData ()[3], 0x22);
  assertEqual (pLinearBuffer->get_pData ()[4], 0x06); assertEqual (pLinearBuffer->get_pData ()[5], 0x33);
  assertEqual (pLinearBuffer->get_pData ()[6], 0xAB); assertEqual (pLinearBuffer->get_pData ()[7], 0xAB);
  assertEqual (pLinearBuffer->get_pData ()[8], 0xAB);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)6); assertTrue  (pLinearBuffer->WriteValueAndMovePtr (valueUI16, true)); valueUI16 += 0x0511;
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)8); assertFalse (pLinearBuffer->WriteValueAndMovePtr (valueUI16, true)); valueUI16 += 0x0611;
  pLinearBuffer->SetWritePointer (1);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)1); assertTrue  (pLinearBuffer->WriteValueAndMovePtr (valueUI16, true)); valueUI16 += 0x0711;
  assertEqual (pLinearBuffer->get_pData ()[0], 0x01); assertEqual (pLinearBuffer->get_pData ()[1], 0x15);
  assertEqual (pLinearBuffer->get_pData ()[2], 0x66); assertEqual (pLinearBuffer->get_pData ()[3], 0x22);
  assertEqual (pLinearBuffer->get_pData ()[4], 0x06); assertEqual (pLinearBuffer->get_pData ()[5], 0x33);
  assertEqual (pLinearBuffer->get_pData ()[6], 0x0A); assertEqual (pLinearBuffer->get_pData ()[7], 0x44);
  assertEqual (pLinearBuffer->get_pData ()[8], 0xAB);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)3); assertTrue (pLinearBuffer->WriteValueAndMovePtr (valueUI16, true)); valueUI16 += 0x0811;
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)5); assertTrue (pLinearBuffer->WriteValueAndMovePtr (valueUI16, true)); valueUI16 += 0x0911;
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)7); assertTrue (pLinearBuffer->WriteValueAndMovePtr (valueUI16, true));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)9);
  assertEqual (pLinearBuffer->get_pData ()[0], 0x01); assertEqual (pLinearBuffer->get_pData ()[1], 0x15);
  assertEqual (pLinearBuffer->get_pData ()[2], 0x66); assertEqual (pLinearBuffer->get_pData ()[3], 0x1C);
  assertEqual (pLinearBuffer->get_pData ()[4], 0x77); assertEqual (pLinearBuffer->get_pData ()[5], 0x24);
  assertEqual (pLinearBuffer->get_pData ()[6], 0x88); assertEqual (pLinearBuffer->get_pData ()[7], 0x2D);
  assertEqual (pLinearBuffer->get_pData ()[8], 0x99);

  uint8_t expected_UI16[27]     = { 0x34, 0x12,   0x78, 0x56,   0xBC, 0x9A,   0x00, 0xDE,   0xAB, 0x34,   0x88, 0x66,   0xCC, 0xAA,   0x10, 0xEE,   0x54, 0x32,   0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB };
  uint8_t expected_UI16_Inv[27] = { 0x12, 0x34,   0x56, 0x78,   0x9A, 0xBC,   0xDE, 0x00,   0xAB, 0x12,   0x66, 0x88,   0xAA, 0xCC,   0xEE, 0x10,   0x32, 0x54,   0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB };

  pLinearBuffer->Clear ();
  uint16_t address = 0;
  int16_t valueI16 = 0x1234;
  pLinearBuffer->SetWritePointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pLinearBuffer->get_CurrentWriteAddress (), address);
    if (index == 4)
    {
      assertFalse (pLinearBuffer->WriteValueAndMovePtr (valueI16, false));
      pLinearBuffer->SetWritePointer (1);
    }
    else
    {
      assertTrue  (pLinearBuffer->WriteValueAndMovePtr (valueI16, false));
    }
    uint16_t offset = index * 2;
    for (uint16_t check = 0; check < 2; check++)
    {
      assertEqual (pLinearBuffer->get_pData ()[(offset + check    ) % length], expected_UI16[ offset + check               ]);
      assertEqual (pLinearBuffer->get_pData ()[(offset + check + 2) % length], expected_UI16[(offset + check + 2 + 18) % 27]);
    }
    valueI16 = valueI16 + 0x0044 & 0x00FF | valueI16 + 0x4400 & 0xFF00;
    address = (address + 2) % length;
  }

  pLinearBuffer->Clear ();
  address = 0;
  valueI16 = 0x1234;
  pLinearBuffer->SetWritePointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pLinearBuffer->get_CurrentWriteAddress (), address);
    if (index == 4)
    {
      assertFalse (pLinearBuffer->WriteValueAndMovePtr (valueI16, true));
      pLinearBuffer->SetWritePointer (1);
    }
    else
    {
      assertTrue  (pLinearBuffer->WriteValueAndMovePtr (valueI16, true));
    }
    uint16_t offset = index * 2;
    for (uint16_t check = 0; check < 2; check++)
    {
      assertEqual (pLinearBuffer->get_pData ()[(offset + check    ) % length], expected_UI16_Inv[ offset + check               ]);
      assertEqual (pLinearBuffer->get_pData ()[(offset + check + 2) % length], expected_UI16_Inv[(offset + check + 2 + 18) % 27]);
    }
    valueI16 = valueI16 + 0x0044 & 0x00FF | valueI16 + 0x4400 & 0xFF00;
    address = (address + 2) % length;
  }

  uint8_t expected_UI32[45]     = { 0x78, 0x56, 0x34, 0x12,   0x00, 0xDE, 0xBC, 0x9A,   0xAB, 0x78, 0x56, 0x34,   0x10, 0xEE, 0xCC, 0xAA,   0x9A, 0xAB, 0x78, 0x56,   0x20, 0xFE, 0xDC, 0xBA,   0xAA, 0x9A, 0xAB, 0x78,   0x30, 0x0E, 0xEC, 0xCA,   0xB8, 0x96, 0x74, 0x52,   0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB };
  uint8_t expected_UI32_Inv[45] = { 0x12, 0x34, 0x56, 0x78,   0x9A, 0xBC, 0xDE, 0x00,   0xAB, 0x12, 0x34, 0x56,   0xAA, 0xCC, 0xEE, 0x10,   0x00, 0xAB, 0x12, 0x34,   0xBA, 0xDC, 0xFE, 0x20,   0x10, 0x00, 0xAB, 0x12,   0xCA, 0xEC, 0x0E, 0x30,   0x52, 0x74, 0x96, 0xB8,   0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB };

  pLinearBuffer->Clear ();
  address = 0;
  uint32_t valueUI32 = 0x12345678;
  pLinearBuffer->SetWritePointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pLinearBuffer->get_CurrentWriteAddress (), address);
    if (pLinearBuffer->get_CurrentWriteAddress () + 4 > length)
    {
      assertFalse (pLinearBuffer->WriteValueAndMovePtr (valueUI32, false));
      pLinearBuffer->SetWritePointer ((pLinearBuffer->get_CurrentWriteAddress () + 4) % length);
    }
    else
    {
      assertTrue  (pLinearBuffer->WriteValueAndMovePtr (valueUI32, false));
    }
    uint16_t offset = index * 4;
    for (uint16_t check = 0; check < 2; check++)
    {
      assertEqual (pLinearBuffer->get_pData ()[(offset + check    ) % length], expected_UI32[ offset + check               ]);
      assertEqual (pLinearBuffer->get_pData ()[(offset + check + 4) % length], expected_UI32[(offset + check + 4 + 36) % 45]);
    }
    valueUI32 = valueUI32 + 0x00000088 & 0x000000FF | valueUI32 + 0x00008800 & 0x0000FF00 | valueUI32 + 0x00880000 & 0x00FF0000 | valueUI32 + 0x88000000 & 0xFF000000;
    address = (address + 4) % length;
  }

  pLinearBuffer->Clear ();
  address = 0;
  valueUI32 = 0x12345678;
  pLinearBuffer->SetWritePointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pLinearBuffer->get_CurrentWriteAddress (), address);
    if (pLinearBuffer->get_CurrentWriteAddress () + 4 > length)
    {
      assertFalse (pLinearBuffer->WriteValueAndMovePtr (valueUI32, true));
      pLinearBuffer->SetWritePointer ((pLinearBuffer->get_CurrentWriteAddress () + 4) % length);
    }
    else
    {
      assertTrue  (pLinearBuffer->WriteValueAndMovePtr (valueUI32, true));
    }
    uint16_t offset = index * 4;
    for (uint16_t check = 0; check < 2; check++)
    {
      assertEqual (pLinearBuffer->get_pData ()[(offset + check    ) % length], expected_UI32_Inv[ offset + check               ]);
      assertEqual (pLinearBuffer->get_pData ()[(offset + check + 4) % length], expected_UI32_Inv[(offset + check + 4 + 36) % 45]);
    }
    valueUI32 = valueUI32 + 0x00000088 & 0x000000FF | valueUI32 + 0x00008800 & 0x0000FF00 | valueUI32 + 0x00880000 & 0x00FF0000 | valueUI32 + 0x88000000 & 0xFF000000;
    address = (address + 4) % length;
  }

  pLinearBuffer->Clear ();
  address = 0;
  int32_t valueI32 = 0x12345678;
  pLinearBuffer->SetWritePointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pLinearBuffer->get_CurrentWriteAddress (), address);
    if (pLinearBuffer->get_CurrentWriteAddress () + 4 > length)
    {
      assertFalse (pLinearBuffer->WriteValueAndMovePtr (valueI32, false));
      pLinearBuffer->SetWritePointer ((pLinearBuffer->get_CurrentWriteAddress () + 4) % length);
    }
    else
    {
      assertTrue  (pLinearBuffer->WriteValueAndMovePtr (valueI32, false));
    }
    uint16_t offset = index * 4;
    for (uint16_t check = 0; check < 2; check++)
    {
      assertEqual (pLinearBuffer->get_pData ()[(offset + check    ) % length], expected_UI32[ offset + check               ]);
      assertEqual (pLinearBuffer->get_pData ()[(offset + check + 4) % length], expected_UI32[(offset + check + 4 + 36) % 45]);
    }
    valueI32 = valueI32 + 0x00000088 & 0x000000FF | valueI32 + 0x00008800 & 0x0000FF00 | valueI32 + 0x00880000 & 0x00FF0000 | valueI32 + 0x88000000 & 0xFF000000;
    address = (address + 4) % length;
  }

  pLinearBuffer->Clear ();
  address = 0;
  valueI32 = 0x12345678;
  pLinearBuffer->SetWritePointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pLinearBuffer->get_CurrentWriteAddress (), address);
    if (pLinearBuffer->get_CurrentWriteAddress () + 4 > length)
    {
      assertFalse (pLinearBuffer->WriteValueAndMovePtr (valueI32, true));
      pLinearBuffer->SetWritePointer ((pLinearBuffer->get_CurrentWriteAddress () + 4) % length);
    }
    else
    {
      assertTrue  (pLinearBuffer->WriteValueAndMovePtr (valueI32, true));
    }
    uint16_t offset = index * 4;
    for (uint16_t check = 0; check < 2; check++)
    {
      assertEqual (pLinearBuffer->get_pData ()[(offset + check    ) % length], expected_UI32_Inv[ offset + check               ]);
      assertEqual (pLinearBuffer->get_pData ()[(offset + check + 4) % length], expected_UI32_Inv[(offset + check + 4 + 36) % 45]);
    }
    valueI32 = valueI32 + 0x00000088 & 0x000000FF | valueI32 + 0x00008800 & 0x0000FF00 | valueI32 + 0x00880000 & 0x00FF0000 | valueI32 + 0x88000000 & 0xFF000000;
    address = (address + 4) % length;
  }
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_From ()
test (ByteBuffer_LinearBuffer_WriteRange_From_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pLinearBuffer = nullptr;
  assertTrue (ByteBuffer::Create (length, defaultValue, false, pLinearBuffer));

  //---------- Act & Assert ----------
  assertTrue  (pLinearBuffer->WriteRange_From ( 0,  0, 0x00));
  assertTrue  (pLinearBuffer->WriteRange_From ( 1,  0, 0x00));
  assertTrue  (pLinearBuffer->WriteRange_From ( 8,  0, 0x00));
  assertFalse (pLinearBuffer->WriteRange_From ( 9,  0, 0x00));
  assertFalse (pLinearBuffer->WriteRange_From (99,  0, 0x00));
  assertTrue  (pLinearBuffer->WriteRange_From ( 0,  1, 0x00));
  assertTrue  (pLinearBuffer->WriteRange_From ( 0,  8, 0x00));
  assertTrue  (pLinearBuffer->WriteRange_From ( 0,  9, 0x00));
  assertFalse (pLinearBuffer->WriteRange_From ( 0, 10, 0x00));
  assertFalse (pLinearBuffer->WriteRange_From ( 0, 99, 0x00));
  assertTrue  (pLinearBuffer->WriteRange_From ( 6,  1, 0x00));
  assertFalse (pLinearBuffer->WriteRange_From ( 6,  8, 0x00));
  assertFalse (pLinearBuffer->WriteRange_From ( 6,  9, 0x00));
  assertFalse (pLinearBuffer->WriteRange_From ( 6, 10, 0x00));
  assertFalse (pLinearBuffer->WriteRange_From ( 6, 99, 0x00));

  //---------- Cleanup ----------
  delete (pLinearBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_From ()
// - Clear ()
test (ByteBuffer_LinearBuffer_WriteRange_From_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint8_t newValue1 = 0x12;
  uint8_t newValue2 = 0x34;
  uint8_t newValue3 = 0x56;
  uint16_t length = 9;
  uint8_t valueUI8 = 0;
  ByteBuffer* pLinearBuffer = nullptr;
  assertTrue (ByteBuffer::Create (length, defaultValue, false, pLinearBuffer));

  uint8_t expected_UI8[9] = { newValue1, defaultValue, defaultValue, newValue2, newValue2, newValue2, newValue2, defaultValue, defaultValue };

  //---------- Act ----------
  assertTrue (pLinearBuffer->WriteRange_From (0, 1, newValue1));
  assertTrue (pLinearBuffer->WriteRange_From (3, 4, newValue2));

  //---------- Assert ----------
  pLinearBuffer->SetReadPointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue (pLinearBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, expected_UI8[index]);
  }

  //---------- Act ----------
  pLinearBuffer->Clear ();

  //---------- Assert ----------
  pLinearBuffer->SetReadPointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, defaultValue);
  }

  //---------- Act ----------
  assertTrue (pLinearBuffer->WriteRange_From (2, 4, newValue3));

  //---------- Assert ----------
  pLinearBuffer->SetReadPointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, index >= 2 && index < 6 ? newValue3 : defaultValue);
  }

  //---------- Act ----------
  assertFalse (pLinearBuffer->WriteRange_From (6, 6, newValue2));

  //---------- Assert ----------
  pLinearBuffer->SetReadPointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, index >= 2 && index < 6 ? newValue3 : defaultValue);
  }

  //---------- Act ----------
  assertTrue (pLinearBuffer->WriteRange_From (0, 9, newValue1));

  //---------- Assert ----------
  pLinearBuffer->SetReadPointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue (pLinearBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, newValue1);
  }
  //---------- Cleanup ----------
  delete (pLinearBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_To ()
test (ByteBuffer_LinearBuffer_WriteRange_To_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pLinearBuffer = nullptr;
  assertTrue (ByteBuffer::Create (length, defaultValue, false, pLinearBuffer));

  //---------- Act & Assert ----------
  assertTrue  (pLinearBuffer->WriteRange_To ( 0,  0, 0x00));
  assertTrue  (pLinearBuffer->WriteRange_To ( 1,  0, 0x00));
  assertTrue  (pLinearBuffer->WriteRange_To ( 9,  0, 0x00));
  assertFalse (pLinearBuffer->WriteRange_To (10,  0, 0x00));
  assertFalse (pLinearBuffer->WriteRange_To (99,  0, 0x00));
  assertFalse (pLinearBuffer->WriteRange_To ( 0,  1, 0x00));
  assertFalse (pLinearBuffer->WriteRange_To ( 0,  8, 0x00));
  assertFalse (pLinearBuffer->WriteRange_To ( 0,  9, 0x00));
  assertFalse (pLinearBuffer->WriteRange_To ( 0, 10, 0x00));
  assertFalse (pLinearBuffer->WriteRange_To ( 0, 99, 0x00));
  assertTrue  (pLinearBuffer->WriteRange_To ( 6,  1, 0x00));
  assertFalse (pLinearBuffer->WriteRange_To ( 6,  8, 0x00));
  assertFalse (pLinearBuffer->WriteRange_To ( 6,  9, 0x00));
  assertFalse (pLinearBuffer->WriteRange_To ( 6, 10, 0x00));
  assertFalse (pLinearBuffer->WriteRange_To ( 6, 99, 0x00));

  //---------- Cleanup ----------
  delete (pLinearBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_To ()
// - Clear ()
test (ByteBuffer_LinearBuffer_WriteRange_To_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint8_t newValue1 = 0x12;
  uint8_t newValue2 = 0x34;
  uint8_t newValue3 = 0x56;
  uint16_t length = 9;
  uint8_t valueUI8 = 0;
  ByteBuffer* pLinearBuffer = nullptr;
  assertTrue (ByteBuffer::Create (length, defaultValue, false, pLinearBuffer));

  uint8_t expected_UI8[9] = { defaultValue, defaultValue, newValue2, newValue2, newValue2, newValue2, defaultValue, defaultValue, defaultValue };

  //---------- Act ----------
  assertFalse (pLinearBuffer->WriteRange_To (0, 1, newValue1));
  assertTrue  (pLinearBuffer->WriteRange_To (6, 4, newValue2));

  //---------- Assert ----------
  pLinearBuffer->SetReadPointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue (pLinearBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, expected_UI8[index]);
  }

  //---------- Act ----------
  pLinearBuffer->Clear ();

  //---------- Assert ----------
  pLinearBuffer->SetReadPointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, defaultValue);
  }

  //---------- Act ----------
  assertFalse (pLinearBuffer->WriteRange_To (2, 4, newValue3));

  //---------- Assert ----------
  pLinearBuffer->SetReadPointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, defaultValue);
  }

  //---------- Act ----------
  assertTrue (pLinearBuffer->WriteRange_To (8, 6, newValue2));

  //---------- Assert ----------
  pLinearBuffer->SetReadPointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, index >= 2 && index < 8 ? newValue2 : defaultValue);
  }

  //---------- Act ----------
  assertTrue (pLinearBuffer->WriteRange_To (9, 9, newValue1));

  //---------- Assert ----------
  pLinearBuffer->SetReadPointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, newValue1);
  }

  //---------- Cleanup ----------
  delete (pLinearBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_FromTo ()
test (ByteBuffer_LinearBuffer_WriteRange_FromTo_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  ByteBuffer* pLinearBuffer = nullptr;
  assertTrue (ByteBuffer::Create (length, defaultValue, false, pLinearBuffer));

  //---------- Act & Assert ----------
  assertTrue  (pLinearBuffer->WriteRange_FromTo ( 0,  0, 0x00));
  assertFalse (pLinearBuffer->WriteRange_FromTo ( 1,  0, 0x00));
  assertFalse (pLinearBuffer->WriteRange_FromTo ( 8,  0, 0x00));
  assertFalse (pLinearBuffer->WriteRange_FromTo ( 9,  0, 0x00));
  assertFalse (pLinearBuffer->WriteRange_FromTo (99,  0, 0x00));
  assertTrue  (pLinearBuffer->WriteRange_FromTo ( 0,  1, 0x00));
  assertTrue  (pLinearBuffer->WriteRange_FromTo ( 0,  9, 0x00));
  assertFalse (pLinearBuffer->WriteRange_FromTo ( 0, 10, 0x00));
  assertFalse (pLinearBuffer->WriteRange_FromTo ( 0, 99, 0x00));
  assertFalse (pLinearBuffer->WriteRange_FromTo ( 6,  1, 0x00));
  assertTrue  (pLinearBuffer->WriteRange_FromTo ( 6,  8, 0x00));
  assertTrue  (pLinearBuffer->WriteRange_FromTo ( 6,  9, 0x00));
  assertFalse (pLinearBuffer->WriteRange_FromTo ( 6, 99, 0x00));

  //---------- Cleanup ----------
  delete (pLinearBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteRange_FromTo ()
// - Clear ()
test (ByteBuffer_LinearBuffer_WriteRange_FromTo_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint8_t newValue1 = 0x12;
  uint8_t newValue2 = 0x34;
  uint8_t newValue3 = 0x56;
  uint16_t length = 9;
  uint8_t valueUI8 = 0;
  ByteBuffer* pLinearBuffer = nullptr;
  assertTrue (ByteBuffer::Create (length, defaultValue, false, pLinearBuffer));

  uint8_t expected_UI8[9] = { newValue1, defaultValue, defaultValue, newValue2, newValue2, newValue2, newValue2, defaultValue, defaultValue };

  //---------- Act ----------
  assertTrue (pLinearBuffer->WriteRange_FromTo (0, 1, newValue1));
  assertTrue (pLinearBuffer->WriteRange_FromTo (3, 7, newValue2));

  //---------- Assert ----------
  pLinearBuffer->SetReadPointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue (pLinearBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, expected_UI8[index]);
  }

  //---------- Act ----------
  pLinearBuffer->Clear ();

  //---------- Assert ----------
  pLinearBuffer->SetReadPointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, defaultValue);
  }

  //---------- Act ----------
  assertFalse (pLinearBuffer->WriteRange_FromTo (4, 2, newValue3));

  //---------- Assert ----------
  pLinearBuffer->SetReadPointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, defaultValue);
  }

  //---------- Act ----------
  assertTrue (pLinearBuffer->WriteRange_FromTo (2, 4, newValue2));

  //---------- Assert ----------
  pLinearBuffer->SetReadPointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, index >= 2 && index < 4 ? newValue2 : defaultValue);
  }

  //---------- Act ----------
  assertTrue (pLinearBuffer->WriteRange_FromTo (0, 9, newValue1));

  //---------- Assert ----------
  pLinearBuffer->SetReadPointer (0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8));
    assertEqual (valueUI8, newValue1);
  }

  //---------- Cleanup ----------
  delete (pLinearBuffer);
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
