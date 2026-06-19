#include "ByteBuffer.h"

ByteBuffer* m_pByteBuffer         = nullptr;
uint16_t    m_ByteBufferLength    = 0;
uint8_t*    m_pDestinationArray   = nullptr;
ByteBuffer* m_pDestinationBuffer  = nullptr;
uint8_t*    m_pSourceArray        = new uint8_t[0];
ByteBuffer* m_pSourceBuffer       = nullptr;
uint16_t    m_ByteCount           = 0;
uint16_t    m_StartOffset         = 0;
uint16_t    m_EndOffset           = 0;
uint16_t    m_Checksum            = 0;

bool      m_ValueBOOL = false;
uint8_t   m_ValueUI8  = 0;
int8_t    m_ValueI8   = 0;
uint16_t  m_ValueUI16 = 0;
int16_t   m_ValueI16  = 0;
uint32_t  m_ValueUI32 = 0;
int32_t   m_ValueI32  = 0;

void setup ()
{
  ByteBuffer::Create (m_ByteBufferLength, 0xFF, true, m_pByteBuffer);

  m_pByteBuffer->Clear_From   (m_StartOffset, m_ByteCount);
  m_pByteBuffer->Clear_To     (m_EndOffset,   m_ByteCount);
  m_pByteBuffer->Clear_FromTo (m_StartOffset, m_EndOffset);

  m_pByteBuffer->ReadBytesAndMovePtr (m_ByteCount, m_pDestinationArray, false);
  m_pByteBuffer->ReadBytesAndMovePtr (m_ByteCount, m_pDestinationBuffer, false);
  m_pByteBuffer->ReadValueAndMovePtr (m_ValueBOOL);
  m_pByteBuffer->ReadValueAndMovePtr (m_ValueUI8);
  m_pByteBuffer->ReadValueAndMovePtr (m_ValueI8);
  m_pByteBuffer->ReadValueAndMovePtr (m_ValueUI16, false);
  m_pByteBuffer->ReadValueAndMovePtr (m_ValueI16, false);
  m_pByteBuffer->ReadValueAndMovePtr (m_ValueUI32, false);
  m_pByteBuffer->ReadValueAndMovePtr (m_ValueI32, false);
  m_pByteBuffer->MoveReadPointer ();
  m_pByteBuffer->SetReadPointer (1);

  m_pByteBuffer->WriteBytesAndMovePtr (m_ByteCount, m_pSourceArray, false);
  m_pByteBuffer->WriteBytesAndMovePtr (m_ByteCount, m_pSourceBuffer, false);
  m_pByteBuffer->WriteValueAndMovePtr (m_ValueBOOL);
  m_pByteBuffer->WriteValueAndMovePtr (m_ValueUI8);
  m_pByteBuffer->WriteValueAndMovePtr (m_ValueI8);
  m_pByteBuffer->WriteValueAndMovePtr (m_ValueUI16, false);
  m_pByteBuffer->WriteValueAndMovePtr (m_ValueI16, false);
  m_pByteBuffer->WriteValueAndMovePtr (m_ValueUI32, false);
  m_pByteBuffer->WriteValueAndMovePtr (m_ValueI32, false);
  m_pByteBuffer->MoveWritePointer ();
  m_pByteBuffer->SetWritePointer (1);

  m_pByteBuffer->WriteRange_From   (m_StartOffset, m_ByteCount, m_ValueUI8);
  m_pByteBuffer->WriteRange_To     (m_EndOffset,   m_ByteCount, m_ValueUI8);
  m_pByteBuffer->WriteRange_FromTo (m_StartOffset, m_EndOffset, m_ValueUI8);

  m_pByteBuffer->CalcChecksumCRC16_From   (m_StartOffset, m_ByteCount, m_Checksum);
  m_pByteBuffer->CalcChecksumCRC16_To     (m_EndOffset,   m_ByteCount, m_Checksum);
  m_pByteBuffer->CalcChecksumCRC16_FromTo (m_StartOffset, m_EndOffset, m_Checksum);

  m_pByteBuffer->Print (&Serial);
  m_pByteBuffer->Print (&Serial, 1, 2);
}

void loop ()
{
}
