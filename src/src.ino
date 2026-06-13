#include "ByteRingBuffer.h"

ByteRingBuffer* m_pRingBuffer = nullptr;
uint16_t  m_RingBufferLength  = 0;
uint8_t*  m_pDestination      = nullptr;
uint8_t*  m_pSource           = new uint8_t[0];
uint16_t  m_ByteCount         = 0;
uint16_t  m_StartOffset       = 0;
uint16_t  m_EndOffset         = 0;
uint16_t  m_Checksum          = 0;

bool      m_ValueBOOL = false;
uint8_t   m_ValueUI8  = 0;
int8_t    m_ValueI8   = 0;
uint16_t  m_ValueUI16 = 0;
int16_t   m_ValueI16  = 0;
uint32_t  m_ValueUI32 = 0;
int32_t   m_ValueI32  = 0;

void setup ()
{
  ByteRingBuffer::Create (m_RingBufferLength, 0xFF, m_pRingBuffer);

  m_pRingBuffer->Clear_FromStart  (m_StartOffset, m_ByteCount);
  m_pRingBuffer->Clear_ToEnd      (m_EndOffset,   m_ByteCount);
  m_pRingBuffer->Clear_StartToEnd (m_StartOffset, m_EndOffset);

  m_pRingBuffer->ReadBytesAndMovePtr (m_ByteCount, m_pDestination, false);
  m_pRingBuffer->ReadValueAndMovePtr (m_ValueBOOL);
  m_pRingBuffer->ReadValueAndMovePtr (m_ValueUI8);
  m_pRingBuffer->ReadValueAndMovePtr (m_ValueI8);
  m_pRingBuffer->ReadValueAndMovePtr (m_ValueUI16, false);
  m_pRingBuffer->ReadValueAndMovePtr (m_ValueI16, false);
  m_pRingBuffer->ReadValueAndMovePtr (m_ValueUI32, false);
  m_pRingBuffer->ReadValueAndMovePtr (m_ValueI32, false);
  m_pRingBuffer->MoveReadPointer ();
  m_pRingBuffer->SetReadPointer (1);

  m_pRingBuffer->WriteBytesAndMovePtr (m_ByteCount, m_pSource, false);
  m_pRingBuffer->WriteValueAndMovePtr (m_ValueBOOL);
  m_pRingBuffer->WriteValueAndMovePtr (m_ValueUI8);
  m_pRingBuffer->WriteValueAndMovePtr (m_ValueI8);
  m_pRingBuffer->WriteValueAndMovePtr (m_ValueUI16, false);
  m_pRingBuffer->WriteValueAndMovePtr (m_ValueI16, false);
  m_pRingBuffer->WriteValueAndMovePtr (m_ValueUI32, false);
  m_pRingBuffer->WriteValueAndMovePtr (m_ValueI32, false);
  m_pRingBuffer->MoveWritePointer ();
  m_pRingBuffer->SetWritePointer (1);

  m_pRingBuffer->WriteRange_FromStart  (m_StartOffset, m_ByteCount, m_ValueUI8);
  m_pRingBuffer->WriteRange_ToEnd      (m_EndOffset,   m_ByteCount, m_ValueUI8);
  m_pRingBuffer->WriteRange_StartToEnd (m_StartOffset, m_EndOffset, m_ValueUI8);

  m_pRingBuffer->CalcChecksumCRC16_FromStart  (m_StartOffset, m_ByteCount, m_Checksum);
  m_pRingBuffer->CalcChecksumCRC16_ToEnd      (m_EndOffset,   m_ByteCount, m_Checksum);
  m_pRingBuffer->CalcChecksumCRC16_StartToEnd (m_StartOffset, m_EndOffset, m_Checksum);

  m_pRingBuffer->Print (&Serial);
  m_pRingBuffer->Print (&Serial, 1, 2);
}

void loop ()
{
}
