#include <AUnit.h>

#include <MemoryTools_RingBuffer.h>

using namespace MemoryTools;

//--------------------------------------------------------------------
// Test of the functions
// - Create ()
test (RingBuffer_Create_Test1)
{
  //---------- Arrange ----------
  uint8_t value = 0;
  RingBuffer* pRingBuffer = (RingBuffer*)&value;

  //---------- Act ----------
  bool result = RingBuffer::Create (20, 0xFF, pRingBuffer);

  //---------- Assert ----------
  assertFalse (result);
  assertEqual (pRingBuffer, &value);
}

//--------------------------------------------------------------------
// Test of the functions
// - Create ()
test (RingBuffer_Create_Test2)
{
  //---------- Arrange ----------
  RingBuffer* pRingBuffer = nullptr;

  //---------- Act ----------
  bool result = RingBuffer::Create (0, 0xFF, pRingBuffer);

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
test (RingBuffer_Create_Test3)
{
  //---------- Arrange ----------
  uint16_t length = 20;
  RingBuffer* pRingBuffer = nullptr;

  //---------- Act ----------
  bool result = RingBuffer::Create (length, 0xFF, pRingBuffer);

  //---------- Assert ----------
  assertTrue (result);
  assertNotEqual (pRingBuffer, nullptr);
  assertEqual (pRingBuffer->get_Length (), length);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - GetByteAndMovePtr ()
test (RingBuffer_GetByteAndMovePtr_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  RingBuffer* pRingBuffer = nullptr;
  bool result = RingBuffer::Create (length, defaultValue, pRingBuffer);
  assertTrue (result);

  //---------- Act & Assert ----------
  for (uint16_t index = 0; index < 2 * length; index++)
    assertEqual (pRingBuffer->GetByteAndMovePtr (), defaultValue);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - SetByteAndMovePtr ()
// - MovePtr ()
test (RingBuffer_SetByteAndMovePtr_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  RingBuffer* pRingBuffer = nullptr;
  assertTrue (RingBuffer::Create (length, defaultValue, pRingBuffer));

  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->GetByteAndMovePtr (), defaultValue);

  //---------- Act ----------
  pRingBuffer->MovePtr (3);
  pRingBuffer->SetByteAndMovePtr (0x12);
  pRingBuffer->ResetPointer ();

  //---------- Assert ----------
  for (uint16_t index = 0; index < length * 3; index++)
    assertEqual (pRingBuffer->GetByteAndMovePtr (), index % length == 3 ? 0x12 : defaultValue);

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->GetByteAndMovePtr (), defaultValue);

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->GetByteAndMovePtr (), defaultValue);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
test (RingBuffer_SetValue_FromStart_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  RingBuffer* pRingBuffer = nullptr;
  assertTrue (RingBuffer::Create (length, defaultValue, pRingBuffer));

  //---------- Act & Assert ----------
  assertTrue  (pRingBuffer->SetValue_FromStart ( 0,  0, 0x00));
  assertTrue  (pRingBuffer->SetValue_FromStart ( 1,  0, 0x00));
  assertTrue  (pRingBuffer->SetValue_FromStart ( 8,  0, 0x00));
  assertFalse (pRingBuffer->SetValue_FromStart ( 9,  0, 0x00));
  assertFalse (pRingBuffer->SetValue_FromStart (99,  0, 0x00));
  assertTrue  (pRingBuffer->SetValue_FromStart ( 0,  1, 0x00));
  assertTrue  (pRingBuffer->SetValue_FromStart ( 0,  8, 0x00));
  assertTrue  (pRingBuffer->SetValue_FromStart ( 0,  9, 0x00));
  assertFalse (pRingBuffer->SetValue_FromStart ( 0, 10, 0x00));
  assertFalse (pRingBuffer->SetValue_FromStart ( 0, 99, 0x00));

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
test (RingBuffer_SetValue_FromStart_Test2)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xAB;
  uint16_t length = 9;
  RingBuffer* pRingBuffer = nullptr;
  assertTrue (RingBuffer::Create (length, defaultValue, pRingBuffer));

  //---------- Act ----------
  uint8_t newLength = 1;
  uint8_t newValue = 0x12;
  pRingBuffer->SetValue_FromStart (0, newLength, newValue);

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->GetByteAndMovePtr (), index < newLength ? newValue : defaultValue);

  //---------- Act ----------
  pRingBuffer->Clear ();

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->GetByteAndMovePtr (), defaultValue);

  //---------- Act ----------
  newLength = 4;
  newValue = 0x23;
  pRingBuffer->SetValue_FromStart (0, newLength, newValue);

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->GetByteAndMovePtr (), index < newLength ? newValue : defaultValue);

  //---------- Act ----------
  newLength = 9;
  newValue = 0x56;
  pRingBuffer->SetValue_FromStart (0, newLength, newValue);

  //---------- Assert ----------
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->GetByteAndMovePtr (), newValue);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
test (RingBuffer_GetSetValueAndMovePtr_Test1)
{
  //---------- Arrange ----------
  uint8_t defaultValue = 0xFF;
  uint16_t length = 9;
  RingBuffer* pRingBuffer = nullptr;
  assertTrue (RingBuffer::Create (length, defaultValue, pRingBuffer));

  //---------- Act ----------
  pRingBuffer->set
  //---------- Assert ----------

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
