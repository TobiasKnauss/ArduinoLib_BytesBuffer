#include <AUnit.h>

#include <BytesBuffer.h>

//--------------------------------------------------------------------
// Test of the functions
// - Create ()
test (BytesBuffer_RingBuffer_Create_Test1)
{
  //---------- Arrange ----------
  uint8_t value = 0;
  BytesBuffer* pRingBuffer = (BytesBuffer*)&value;

  //---------- Act ----------
  bool result = BytesBuffer::Create (20, 0xFF, true, pRingBuffer);

  //---------- Assert ----------
  assertFalse (result);
  assertEqual (pRingBuffer, &value);
}

//--------------------------------------------------------------------
// Test of the functions
// - Create ()
test (BytesBuffer_RingBuffer_Create_Test2)
{
  //---------- Arrange ----------
  BytesBuffer* pRingBuffer = nullptr;

  //---------- Act ----------
  bool result = BytesBuffer::Create (0, 0xFF, true, pRingBuffer);

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
test (BytesBuffer_RingBuffer_Create_Test3)
{
  //---------- Arrange ----------
  uint16_t length = 20;
  BytesBuffer* pRingBuffer = nullptr;

  //---------- Act ----------
  bool result = BytesBuffer::Create (length, 0xFF, true, pRingBuffer);

  //---------- Assert ----------
  assertTrue (result);
  assertNotEqual (pRingBuffer, nullptr);
  assertEqual (pRingBuffer->get_Length (), length);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - ReadValueAndMovePtr (uint8_t)
// - MoveReadPointer ()
// - SetReadPointer ()
// - get_CurrentReadAddress ()
// - get_pData ()
test (BytesBuffer_RingBuffer_MoveSetReadPointer_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint8_t newValue1 = 0x12;
  uint16_t length = 5;
  uint8_t valueUI8 = 0;
  BytesBuffer* pRingBuffer = nullptr;
  bool result = BytesBuffer::Create (length, defaultValue, true, pRingBuffer);
  assertTrue (result);

  //---------- Act & Assert ----------
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  for (uint16_t index = 0; index < 2 * length; index++)
  {
    assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, defaultValue);
    assertEqual (pRingBuffer->get_CurrentReadAddress (), (index + 1) % length);
    assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  }

  //---------- Arrange ----------
  pRingBuffer->get_pData ()[0] = newValue1;

  //---------- Act & Assert ----------
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);

  assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, newValue1);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)1);
  assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, defaultValue);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)2);
  assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, defaultValue);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)3);
  assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, defaultValue);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)4);
  assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, defaultValue);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);

  assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, newValue1);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)1);
  assertTrue  (pRingBuffer->MoveReadPointer ());
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)2);
  assertTrue  (pRingBuffer->MoveReadPointer ());
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)3);
  assertTrue  (pRingBuffer->MoveReadPointer (2));
  assertTrue  (pRingBuffer->MoveReadPointer (5));
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);
  assertFalse (pRingBuffer->MoveReadPointer (14));
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);

  assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, newValue1);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)1);
  assertTrue  (pRingBuffer->SetReadPointer (0));
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);

  assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, newValue1);
  assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, defaultValue);
  assertTrue  (pRingBuffer->MoveReadPointer (2));
  assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, defaultValue);
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);
  assertTrue  (pRingBuffer->SetReadPointer (0));
  assertEqual (pRingBuffer->get_CurrentReadAddress (), (uint16_t)0);
  assertTrue  (pRingBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, newValue1);

  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteValueAndMovePtr (uint8_t)
// - MoveWritePointer ()
// - SetWritePointer ()
// - get_CurrentWriteAddress ()
// - get_pData ()
test (BytesBuffer_RingBuffer_MoveSetWritePointer_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint8_t newValue1 = 0x12;
  uint8_t newValue2 = 0x34;
  uint8_t newValue3 = 0x56;
  uint16_t length = 9;
  BytesBuffer* pRingBuffer = nullptr;
  assertTrue (BytesBuffer::Create (length, defaultValue, true, pRingBuffer));

  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertEqual (pRingBuffer->get_pData ()[index], defaultValue);
    assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  }
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);

  //---------- Act ----------
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertTrue  (pRingBuffer->MoveWritePointer (3));
  assertFalse (pRingBuffer->MoveWritePointer (24));
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  assertTrue  (pRingBuffer->WriteValueAndMovePtr (newValue1));
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)4);
  assertTrue  (pRingBuffer->MoveWritePointer ());
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)5);
  assertTrue  (pRingBuffer->WriteValueAndMovePtr (newValue2));
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)6);
  assertTrue  (pRingBuffer->SetWritePointer (1));
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  assertTrue  (pRingBuffer->WriteValueAndMovePtr (newValue3));
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)2);

  //---------- Assert ----------
  for (uint16_t index = 0; index < length * 3; index++)
  {
    assertEqual (pRingBuffer->get_pData ()[index % length], index % length == 3 ? newValue1 : index % length == 5 ? newValue2 : index % length == 1 ? newValue3 : defaultValue);
    assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)2);
  }

  //---------- Act ----------
  assertTrue  (pRingBuffer->WriteValueAndMovePtr (newValue3));
  assertTrue  (pRingBuffer->WriteValueAndMovePtr (newValue3));
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)4);
  assertTrue  (pRingBuffer->SetWritePointer (7));
  assertTrue  (pRingBuffer->WriteValueAndMovePtr (newValue1));
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
  assertTrue  (pRingBuffer->SetWritePointer (3));
  assertEqual (pRingBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  for (uint16_t index = 0; index < length; index++)
    assertTrue (pRingBuffer->WriteValueAndMovePtr ((uint8_t)(index * 10)));

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->get_pData ()[index], (uint8_t)(((index + 6) % length) * 10));

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
