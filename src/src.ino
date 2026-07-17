#include "BytesBuffer.h"

BytesBuffer*  m_pBytesBuffer        = nullptr;
uint16_t      m_BytesBufferLength   = 0;
uint8_t*      m_pDestinationArray   = nullptr;
BytesBuffer*  m_pDestinationBuffer  = nullptr;
uint8_t*      m_pSourceArray        = new uint8_t[0];
BytesBuffer*  m_pSourceBuffer       = nullptr;
uint16_t      m_ByteCount           = 0;
uint16_t      m_StartOffset         = 0;
uint16_t      m_EndOffset           = 0;
uint16_t      m_Checksum            = 0;

bool      m_ValueBOOL = false;
uint8_t   m_ValueUI8  = 0;
int8_t    m_ValueI8   = 0;
uint16_t  m_ValueUI16 = 0;
int16_t   m_ValueI16  = 0;
uint32_t  m_ValueUI32 = 0;
int32_t   m_ValueI32  = 0;

void setup ()
{
  BytesBuffer::Create (m_BytesBufferLength, 0xFF, true, m_pBytesBuffer);

  m_pBytesBuffer->Clear_From   (m_StartOffset, m_ByteCount);
  m_pBytesBuffer->Clear_To     (m_EndOffset,   m_ByteCount);
  m_pBytesBuffer->Clear_FromTo (m_StartOffset, m_EndOffset);

  m_pBytesBuffer->ReadBytesAndMovePtr (m_ByteCount, m_pDestinationArray, false);
  m_pBytesBuffer->ReadBytesAndMovePtr (m_ByteCount, m_pDestinationBuffer, false);
  m_pBytesBuffer->ReadValueAndMovePtr (m_ValueBOOL);
  m_pBytesBuffer->ReadValueAndMovePtr (m_ValueUI8);
  m_pBytesBuffer->ReadValueAndMovePtr (m_ValueI8);
  m_pBytesBuffer->ReadValueAndMovePtr (m_ValueUI16, false);
  m_pBytesBuffer->ReadValueAndMovePtr (m_ValueI16, false);
  m_pBytesBuffer->ReadValueAndMovePtr (m_ValueUI32, false);
  m_pBytesBuffer->ReadValueAndMovePtr (m_ValueI32, false);
  m_pBytesBuffer->MovePtrBackAndReadValue (m_ValueUI8);
  m_pBytesBuffer->MoveReadPointer ();
  m_pBytesBuffer->MoveReadPointerBackward ();
  m_pBytesBuffer->SetReadPointer (1);

  m_pBytesBuffer->WriteBytesAndMovePtr (m_ByteCount, m_pSourceArray, false);
  m_pBytesBuffer->WriteBytesAndMovePtr (m_ByteCount, m_pSourceBuffer, false);
  m_pBytesBuffer->WriteValueAndMovePtr (m_ValueBOOL);
  m_pBytesBuffer->WriteValueAndMovePtr (m_ValueUI8);
  m_pBytesBuffer->WriteValueAndMovePtr (m_ValueI8);
  m_pBytesBuffer->WriteValueAndMovePtr (m_ValueUI16, false);
  m_pBytesBuffer->WriteValueAndMovePtr (m_ValueI16, false);
  m_pBytesBuffer->WriteValueAndMovePtr (m_ValueUI32, false);
  m_pBytesBuffer->WriteValueAndMovePtr (m_ValueI32, false);
  m_pBytesBuffer->MovePtrBackAndWriteValue (m_ValueUI8);
  m_pBytesBuffer->MoveWritePointer ();
  m_pBytesBuffer->MoveWritePointerBackward ();
  m_pBytesBuffer->SetWritePointer (1);

  m_pBytesBuffer->WriteRange_From   (m_StartOffset, m_ByteCount, m_ValueUI8);
  m_pBytesBuffer->WriteRange_To     (m_EndOffset,   m_ByteCount, m_ValueUI8);
  m_pBytesBuffer->WriteRange_FromTo (m_StartOffset, m_EndOffset, m_ValueUI8);

  m_pBytesBuffer->CalcChecksumCRC16_From   (m_StartOffset, m_ByteCount, m_Checksum);
  m_pBytesBuffer->CalcChecksumCRC16_To     (m_EndOffset,   m_ByteCount, m_Checksum);
  m_pBytesBuffer->CalcChecksumCRC16_FromTo (m_StartOffset, m_EndOffset, m_Checksum);

  m_pBytesBuffer->Print (&Serial);
  m_pBytesBuffer->Print (&Serial, 1, 2);
}

void loop ()
{
}
