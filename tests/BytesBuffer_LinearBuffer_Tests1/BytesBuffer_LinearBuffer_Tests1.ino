#include <AUnit.h>

#include <BytesBuffer.h>

//--------------------------------------------------------------------
// Test of the functions
// - Create ()
test (BytesBuffer_LinearBuffer_Create_Test1)
{
  //---------- Arrange ----------
  uint8_t value = 0;
  BytesBuffer* pLinearBuffer = (BytesBuffer*)&value;

  //---------- Act ----------
  bool result = BytesBuffer::Create (20, 0xFF, false, pLinearBuffer);

  //---------- Assert ----------
  assertFalse (result);
  assertEqual (pLinearBuffer, &value);
}

//--------------------------------------------------------------------
// Test of the functions
// - Create ()
test (BytesBuffer_LinearBuffer_Create_Test2)
{
  //---------- Arrange ----------
  BytesBuffer* pLinearBuffer = nullptr;

  //---------- Act ----------
  bool result = BytesBuffer::Create (0, 0xFF, false, pLinearBuffer);

  //---------- Assert ----------
  assertFalse (result);
  assertEqual (pLinearBuffer, nullptr);

  //---------- Cleanup ----------
  delete (pLinearBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - Create ()
// - get_Length ()
test (BytesBuffer_LinearBuffer_Create_Test3)
{
  //---------- Arrange ----------
  uint16_t length = 20;
  BytesBuffer* pLinearBuffer = nullptr;

  //---------- Act ----------
  bool result = BytesBuffer::Create (length, 0xFF, false, pLinearBuffer);

  //---------- Assert ----------
  assertTrue (result);
  assertNotEqual (pLinearBuffer, nullptr);
  assertEqual (pLinearBuffer->get_Length (), length);

  //---------- Cleanup ----------
  delete (pLinearBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - ReadValueAndMovePtr (uint8_t)
// - MoveReadPointer ()
// - SetReadPointer ()
// - get_CurrentReadAddress ()
// - get_pData ()
test (BytesBuffer_LinearBuffer_MoveSetReadPointer_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint8_t newValue1 = 0x12;
  uint16_t length = 5;
  uint8_t valueUI8 = 0;
  BytesBuffer* pLinearBuffer = nullptr;
  assertTrue (BytesBuffer::Create (length, defaultValue, false, pLinearBuffer));

  //---------- Act & Assert ----------
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)0);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, defaultValue);
    assertEqual (pLinearBuffer->get_CurrentReadAddress (), (index + 1));
    assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  }
  assertFalse (pLinearBuffer->ReadValueAndMovePtr (valueUI8));

  //---------- Arrange ----------
  pLinearBuffer->get_pData ()[0] = newValue1;

  //---------- Act & Assert ----------
  assertTrue  (pLinearBuffer->SetReadPointer (0));
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)0);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertTrue  (pLinearBuffer->SetReadPointer (1));
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)1);
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertTrue  (pLinearBuffer->SetReadPointer (5));
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)5);
  assertTrue  (pLinearBuffer->SetReadPointer (2));
  assertFalse (pLinearBuffer->SetReadPointer (14));
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)2);

  assertTrue  (pLinearBuffer->SetReadPointer (0));
  assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, newValue1);
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)1);
  assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, defaultValue);
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)2);
  assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, defaultValue);
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)3);
  assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, defaultValue);
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)4);
  assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, defaultValue);
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)5);
  assertFalse (pLinearBuffer->ReadValueAndMovePtr (valueUI8));
  assertFalse (pLinearBuffer->MoveReadPointer ());
  assertFalse (pLinearBuffer->MoveReadPointer (1));
  assertFalse (pLinearBuffer->MoveReadPointer (5));

  assertTrue  (pLinearBuffer->SetReadPointer (0));
  assertFalse (pLinearBuffer->MoveReadPointer (14));
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)0);
  assertTrue  (pLinearBuffer->MoveReadPointer ());
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)1);
  assertTrue  (pLinearBuffer->MoveReadPointer ());
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)2);
  assertTrue  (pLinearBuffer->MoveReadPointer ());
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)3);
  assertFalse (pLinearBuffer->MoveReadPointer (5));
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)3);
  assertTrue  (pLinearBuffer->MoveReadPointer (2));
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)5);
  assertFalse (pLinearBuffer->MoveReadPointer (14));
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)5);

  assertTrue  (pLinearBuffer->SetReadPointer (0));
  assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, newValue1);
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)1);
  assertTrue  (pLinearBuffer->SetReadPointer (2));
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)2);
  assertTrue  (pLinearBuffer->MoveReadPointer (2));
  assertTrue  (pLinearBuffer->ReadValueAndMovePtr (valueUI8)); assertEqual (valueUI8, defaultValue);
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)5);

  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)0);

  //---------- Cleanup ----------
  delete (pLinearBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteValueAndMovePtr (uint8_t)
// - MoveWritePointer ()
// - SetWritePointer ()
// - get_CurrentWriteAddress ()
// - get_pData ()
test (BytesBuffer_LinearBuffer_MoveSetWritePointer_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint8_t newValue1 = 0x12;
  uint8_t newValue2 = 0x34;
  uint8_t newValue3 = 0x56;
  uint16_t length = 9;
  BytesBuffer* pLinearBuffer = nullptr;
  assertTrue (BytesBuffer::Create (length, defaultValue, false, pLinearBuffer));

  //---------- Act & Assert ----------
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)0);
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pLinearBuffer->WriteValueAndMovePtr (newValue1));;
    assertEqual (pLinearBuffer->get_pData ()[index], newValue1);
    assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (index + 1));
    assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)0);
  }
  assertFalse (pLinearBuffer->WriteValueAndMovePtr (newValue1));

  //---------- Act & Assert ----------
  assertTrue  (pLinearBuffer->SetWritePointer (0));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)0);
  assertTrue  (pLinearBuffer->SetWritePointer (1));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  assertEqual (pLinearBuffer->get_CurrentReadAddress (), (uint16_t)0);
  assertTrue  (pLinearBuffer->SetWritePointer (5));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)5);
  assertTrue  (pLinearBuffer->SetWritePointer (2));
  assertFalse (pLinearBuffer->SetWritePointer (14));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)2);

  assertTrue  (pLinearBuffer->SetWritePointer (0));
  for (uint16_t index = 0; index < length; index++)
  {
    assertTrue  (pLinearBuffer->WriteValueAndMovePtr (newValue2));
    for (uint16_t check = 0; check < length; check++)
      assertEqual (pLinearBuffer->get_pData ()[check], check <= index ? newValue2 : newValue1);
    assertEqual (pLinearBuffer->get_CurrentWriteAddress (), index + 1);
  }
  assertFalse (pLinearBuffer->MoveWritePointer ());
  assertFalse (pLinearBuffer->MoveWritePointer (1));
  assertFalse (pLinearBuffer->MoveWritePointer (5));

  assertTrue  (pLinearBuffer->SetWritePointer (0));
  assertFalse (pLinearBuffer->MoveWritePointer (14));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)0);
  assertTrue  (pLinearBuffer->MoveWritePointer ());
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  assertTrue  (pLinearBuffer->MoveWritePointer ());
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)2);
  assertTrue  (pLinearBuffer->MoveWritePointer ());
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  assertFalse (pLinearBuffer->MoveWritePointer (9));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)3);
  assertTrue  (pLinearBuffer->MoveWritePointer (5));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)8);
  assertFalse (pLinearBuffer->MoveWritePointer (14));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)8);

  pLinearBuffer->Clear ();
  assertTrue  (pLinearBuffer->SetWritePointer (3));
  assertTrue  (pLinearBuffer->WriteValueAndMovePtr (newValue1));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)4);
  assertTrue  (pLinearBuffer->MoveWritePointer ());
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)5);
  assertTrue  (pLinearBuffer->WriteValueAndMovePtr (newValue2));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)6);
  assertTrue  (pLinearBuffer->SetWritePointer (1));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)1);
  assertTrue  (pLinearBuffer->WriteValueAndMovePtr (newValue3));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)2);

  //---------- Assert ----------
  for (uint16_t index = 0; index < length * 3; index++)
  {
    assertEqual (pLinearBuffer->get_pData ()[index % length], index % length == 3 ? newValue1 : index % length == 5 ? newValue2 : index % length == 1 ? newValue3 : defaultValue);
    assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)2);
  }

  //---------- Act ----------
  assertTrue  (pLinearBuffer->WriteValueAndMovePtr (newValue3));
  assertTrue  (pLinearBuffer->WriteValueAndMovePtr (newValue3));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)4);
  assertTrue  (pLinearBuffer->SetWritePointer (7));
  assertTrue  (pLinearBuffer->WriteValueAndMovePtr (newValue1));
  assertEqual (pLinearBuffer->get_CurrentWriteAddress (), (uint16_t)8);

  //---------- Assert ----------
  assertEqual (pLinearBuffer->get_pData ()[0], defaultValue);
  assertEqual (pLinearBuffer->get_pData ()[1], newValue3);
  assertEqual (pLinearBuffer->get_pData ()[2], newValue3);
  assertEqual (pLinearBuffer->get_pData ()[3], newValue3);
  assertEqual (pLinearBuffer->get_pData ()[4], defaultValue);
  assertEqual (pLinearBuffer->get_pData ()[5], newValue2);
  assertEqual (pLinearBuffer->get_pData ()[6], defaultValue);
  assertEqual (pLinearBuffer->get_pData ()[7], newValue1);
  assertEqual (pLinearBuffer->get_pData ()[8], defaultValue);

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
