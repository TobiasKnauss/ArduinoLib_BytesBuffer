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
  for (uint16_t index = 0; index < 2 * length; index++)
    assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);

  //---------- Arrange ----------
  pRingBuffer->WriteByteAndMovePtr (newValue1);

  //---------- Act & Assert ----------
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue1);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue1);
  pRingBuffer->MoveReadPointer ();
  pRingBuffer->MoveReadPointer ();
  pRingBuffer->MoveReadPointer (2);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue1);
  pRingBuffer->SetReadPointer (0);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue1);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  pRingBuffer->MoveReadPointer (2);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  pRingBuffer->SetReadPointer (0);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue1);

  //---------- Cleanup ----------
  delete (pRingBuffer);
}

//--------------------------------------------------------------------
// Test of the functions
// - WriteByteAndMovePtr ()
// - MoveWritePointer ()
// - SetWritePointer ()
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

  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);

  //---------- Act ----------
  pRingBuffer->MoveWritePointer (3);
  pRingBuffer->WriteByteAndMovePtr (newValue1);
  pRingBuffer->MoveWritePointer (1);
  pRingBuffer->WriteByteAndMovePtr (newValue2);
  pRingBuffer->SetWritePointer (1);
  pRingBuffer->WriteByteAndMovePtr (newValue3);

  //---------- Assert ----------
  for (uint16_t index = 0; index < length * 3; index++)
    assertEqual (pRingBuffer->ReadByteAndMovePtr (), index % length == 3 ? newValue1 : index % length == 5 ? newValue2 : index % length == 1 ? newValue3 : defaultValue);

  //---------- Act ----------
  pRingBuffer->WriteByteAndMovePtr (newValue3);
  pRingBuffer->WriteByteAndMovePtr (newValue3);
  pRingBuffer->SetWritePointer (7);
  pRingBuffer->WriteByteAndMovePtr (newValue1);

  //---------- Assert ----------
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue3);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue3);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue3);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue2);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), newValue1);
  assertEqual (pRingBuffer->ReadByteAndMovePtr (), defaultValue);

  //---------- Act ----------
  pRingBuffer->SetWritePointer (3);
  for (uint16_t index = 0; index < length; index++)
    pRingBuffer->WriteByteAndMovePtr (index * 10);

  //---------- Assert ----------
  pRingBuffer->SetReadPointer (0);
  for (uint16_t index = 0; index < length; index++)
    assertEqual (pRingBuffer->ReadByteAndMovePtr (), (uint8_t)(((index + 6) % length) * 10));

  //---------- Cleanup ----------
  delete (pRingBuffer);
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
