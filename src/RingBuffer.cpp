#include "MemoryTools_Memory.h"
#include "MemoryTools_RingBuffer.h"

using namespace MemoryTools;

//--------------------------------------------------------------------
bool RingBuffer::Create ( uint16_t      i_DataLength,
                          uint8_t       i_DefaultValue,
                          RingBuffer*&  o_pRingBuffer)
{
  if (i_DataLength < 1
  ||  i_DataLength > c_MaxBufferSize)
    return false;
  if (o_pRingBuffer != nullptr)
    return false;

  uint8_t* pData = nullptr;
  if (!Memory::Allocate (pData, i_DataLength, i_DefaultValue))
    return false;

  o_pRingBuffer = new RingBuffer (pData, i_DataLength, i_DefaultValue);
  return true;
}

//--------------------------------------------------------------------
RingBuffer::~RingBuffer ()
{
  delete (m_pData);
  m_pData         = nullptr;
  m_pAfterData    = nullptr;
  m_pCurrentRead  = nullptr;
  m_pCurrentWrite = nullptr;
}

//--------------------------------------------------------------------
RingBuffer::RingBuffer (uint8_t*  i_pData,
                        uint16_t  i_DataLength,
                        uint8_t   i_DefaultValue)
{
  m_pData         = i_pData;
  m_DataLength    = i_DataLength;
  m_pCurrentRead  = i_pData;
  m_pCurrentWrite = i_pData;
  m_pAfterData    = i_pData + i_DataLength;
  m_DefaultValue  = i_DefaultValue;
}

//--------------------------------------------------------------------
uint16_t RingBuffer::get_CurrentReadOffset ()
{
  return m_pCurrentRead - m_pData;
}

//--------------------------------------------------------------------
uint16_t RingBuffer::get_CurrentWriteOffset ()
{
  return m_pCurrentWrite - m_pData;
}

//--------------------------------------------------------------------
uint8_t* RingBuffer::get_pData ()
{
  return m_pData;
}

//--------------------------------------------------------------------
uint16_t RingBuffer::get_Length ()
{
  return m_DataLength;
}

//--------------------------------------------------------------------
void RingBuffer::Clear ()
{
  memset (m_pData, m_DefaultValue, m_DataLength);
}

//--------------------------------------------------------------------
void RingBuffer::MoveReadPointer (uint16_t i_Count)
{
  m_pCurrentRead += i_Count;
  if (m_pCurrentRead >= m_pAfterData)
    m_pCurrentRead -= m_DataLength;
}

//--------------------------------------------------------------------
void RingBuffer::MoveWritePointer (uint16_t i_Count)
{
  m_pCurrentWrite += i_Count;
  if (m_pCurrentWrite >= m_pAfterData)
    m_pCurrentWrite -= m_DataLength;
}

//--------------------------------------------------------------------
bool RingBuffer::SetReadPointer ( uint16_t  i_Offset,
                                  bool      i_WrapIfNeeded)
{
  if (i_Offset >= m_DataLength
  &&  i_WrapIfNeeded)
    i_Offset -= m_DataLength;
  if (i_Offset >= m_DataLength)
    return false;

  m_pCurrentRead = m_pData + i_Offset;
  return true;
}

//--------------------------------------------------------------------
bool RingBuffer::SetWritePointer (uint16_t  i_Offset,
                                  bool      i_WrapIfNeeded)
{
  if (i_Offset >= m_DataLength
  &&  i_WrapIfNeeded)
    i_Offset -= m_DataLength;
  if (i_Offset >= m_DataLength)
    return false;

  m_pCurrentWrite = m_pData + i_Offset;
  return true;
}

//--------------------------------------------------------------------
uint8_t RingBuffer::ReadByteAndMovePtr ()
{
  uint8_t value = *m_pCurrentRead;
  MoveReadPointer ();
  return value;
}

//--------------------------------------------------------------------
bool RingBuffer::ReadBytesAndMovePtr ( uint16_t  i_ByteCount,
                                      uint8_t*  i_pDestination,
                                      bool      i_InvertByteOrder)
{
  if (i_pDestination == nullptr)
    return false;
  if (i_ByteCount == 0)
    return true;

  if (i_ByteCount == 1)
  {
    *i_pDestination = ReadByteAndMovePtr ();
    return true;
  }

  if (i_ByteCount > m_DataLength)
    return false;

  if (i_InvertByteOrder)
  {
    for (uint16_t index = 1; index <= i_ByteCount; index++)
      i_pDestination[i_ByteCount - index] = ReadByteAndMovePtr ();
    return true;
  }

  uint16_t bytesUntilEnd = m_pAfterData - m_pCurrentRead;
  if (i_ByteCount <= bytesUntilEnd)
  {
    memcpy (i_pDestination, m_pCurrentRead, i_ByteCount);
    MoveReadPointer (i_ByteCount);
    return true;
  }

  uint16_t sizeOf2ndBlock = i_ByteCount - bytesUntilEnd;
  memcpy (i_pDestination                , m_pCurrentRead, bytesUntilEnd);
  memcpy (i_pDestination + bytesUntilEnd, m_pData       , sizeOf2ndBlock);
  m_pCurrentRead = m_pData + sizeOf2ndBlock;
  return true;
}

//--------------------------------------------------------------------
void RingBuffer::ReadValueAndMovePtr (bool& o_Value)
{
  o_Value = ReadByteAndMovePtr () != 0;
}

//--------------------------------------------------------------------
void RingBuffer::ReadValueAndMovePtr (uint8_t& o_Value)
{
  o_Value = ReadByteAndMovePtr ();
}

//--------------------------------------------------------------------
void RingBuffer::ReadValueAndMovePtr (int8_t& o_Value)
{
  o_Value = (int8_t)ReadByteAndMovePtr ();
}

//--------------------------------------------------------------------
bool RingBuffer::ReadValueAndMovePtr ( uint16_t& o_Value,
                                      bool      i_InvertByteOrder)
{
  return ReadBytesAndMovePtr (sizeof (uint16_t), (uint8_t*)&o_Value, i_InvertByteOrder);
}

//--------------------------------------------------------------------
bool RingBuffer::ReadValueAndMovePtr ( int16_t&  o_Value,
                                      bool      i_InvertByteOrder)
{
  return ReadBytesAndMovePtr (sizeof (int16_t), (uint8_t*)&o_Value, i_InvertByteOrder);
}

//--------------------------------------------------------------------
bool RingBuffer::ReadValueAndMovePtr ( uint32_t& o_Value,
                                      bool      i_InvertByteOrder)
{
  return ReadBytesAndMovePtr (sizeof (uint32_t), (uint8_t*)&o_Value, i_InvertByteOrder);
}

//--------------------------------------------------------------------
bool RingBuffer::ReadValueAndMovePtr ( int32_t&  o_Value,
                                      bool      i_InvertByteOrder)
{
  return ReadBytesAndMovePtr (sizeof (int32_t), (uint8_t*)&o_Value, i_InvertByteOrder);
}

//--------------------------------------------------------------------
void RingBuffer::WriteByteAndMovePtr (uint8_t i_Value)
{
  *m_pCurrentWrite = i_Value;
  MoveWritePointer ();
}

//--------------------------------------------------------------------
bool RingBuffer::WriteBytesAndMovePtr ( uint16_t  i_ByteCount,
                                        uint8_t*  i_pSource,
                                        bool      i_InvertByteOrder)
{
  if (i_pSource == nullptr)
    return false;
  if (i_ByteCount == 0)
    return true;

  if (i_ByteCount == 1)
  {
    WriteByteAndMovePtr (*i_pSource);
    return true;
  }

  if (i_ByteCount > m_DataLength)
    return false;

  if (i_InvertByteOrder)
  {
    for (uint16_t index = 1; index <= i_ByteCount; index++)
      WriteByteAndMovePtr (i_pSource[i_ByteCount - index]);
    return true;
  }

  uint16_t bytesUntilEnd = m_pAfterData - m_pCurrentWrite;
  if (i_ByteCount <= bytesUntilEnd)
  {
    memcpy (m_pCurrentWrite, i_pSource, i_ByteCount);
    MoveWritePointer (i_ByteCount);
    return true;
  }

  uint16_t sizeOf2ndBlock = i_ByteCount - bytesUntilEnd;
  memcpy (m_pCurrentWrite, i_pSource                , bytesUntilEnd);
  memcpy (m_pData        , i_pSource + bytesUntilEnd, sizeOf2ndBlock);
  m_pCurrentWrite = m_pData + sizeOf2ndBlock;
  return true;
}

//--------------------------------------------------------------------
void RingBuffer::WriteValueAndMovePtr (bool i_Value)
{
  WriteByteAndMovePtr (i_Value ? 1 : 0);
}

//--------------------------------------------------------------------
void RingBuffer::WriteValueAndMovePtr (uint8_t i_Value)
{
  WriteByteAndMovePtr (i_Value);
}

//--------------------------------------------------------------------
void RingBuffer::WriteValueAndMovePtr (int8_t i_Value)
{
  WriteByteAndMovePtr ((uint8_t)i_Value);
}

//--------------------------------------------------------------------
bool RingBuffer::WriteValueAndMovePtr ( uint16_t  i_Value,
                                        bool      i_InvertByteOrder)
{
  return WriteBytesAndMovePtr (sizeof (uint16_t), (uint8_t*)&i_Value, i_InvertByteOrder);
}

//--------------------------------------------------------------------
bool RingBuffer::WriteValueAndMovePtr ( int16_t   i_Value,
                                        bool      i_InvertByteOrder)
{
  return WriteBytesAndMovePtr (sizeof (int16_t), (uint8_t*)&i_Value, i_InvertByteOrder);
}

//--------------------------------------------------------------------
bool RingBuffer::WriteValueAndMovePtr ( uint32_t  i_Value,
                                        bool      i_InvertByteOrder)
{
  return WriteBytesAndMovePtr (sizeof (uint32_t), (uint8_t*)&i_Value, i_InvertByteOrder);
}

//--------------------------------------------------------------------
bool RingBuffer::WriteValueAndMovePtr ( int32_t   i_Value,
                                        bool      i_InvertByteOrder)
{
  return WriteBytesAndMovePtr (sizeof (int32_t), (uint8_t*)&i_Value, i_InvertByteOrder);
}

//--------------------------------------------------------------------
bool RingBuffer::WriteRange_FromStart ( uint16_t  i_StartOffset,
                                        uint16_t  i_ByteCount,
                                        uint8_t   i_Value)
{
  if (i_StartOffset >= m_DataLength)
    return false;
  if (i_ByteCount == 0)
    return true;
  if (i_ByteCount > m_DataLength)
    return false;

  if (i_ByteCount == m_DataLength)
  {
    memset (m_pData, i_Value, m_DataLength);
    return true;
  }

  uint16_t bytesUntilEnd = m_DataLength - i_StartOffset;
  if (i_ByteCount <= bytesUntilEnd)
  {
    memset (m_pData + i_StartOffset, i_Value, i_ByteCount);
    return true;
  }

  uint16_t sizeOf2ndBlock = i_ByteCount - bytesUntilEnd;
  memset (m_pData + i_StartOffset, i_Value, bytesUntilEnd);
  memset (m_pData                , i_Value, sizeOf2ndBlock);
  return true;
}

//--------------------------------------------------------------------
bool RingBuffer::WriteRange_FromEnd ( uint16_t  i_EndOffset,  //                ex.1: 6    ex.2: 3
                                      uint16_t  i_ByteCount,  //                ex.1: 3    ex.2: 6
                                      uint8_t   i_Value)      // m_DataLength:  ex.1: 8    ex.2: 8
{
  if (i_EndOffset >= m_DataLength)  // ex.1,2: m_DataLength == 8  --> i_EndOffset may be 0 to 7.
    return false;
  if (i_ByteCount == 0)  // nothing to do.
    return true;
  if (i_ByteCount > m_DataLength)  // cannot write more values than the buffer actually has.
    return false;

  if (i_ByteCount == m_DataLength)  // write the entire buffer
  {
    memset (m_pData, i_Value, m_DataLength);
    return true;
  }

  if (i_ByteCount <= i_EndOffset)  // the start is before the end, write in one pass (like in ex.1)
  {
    memset (m_pData + i_EndOffset - i_ByteCount, i_Value, i_ByteCount);  // ex.1:  start offset = 6 - 3 = 3  --> writing the bytes 3,4,5.
    return true;
  }

  // the start is after the end, write in two passes (like in ex.2)
  uint16_t sizeOf2ndBlock = i_ByteCount - i_EndOffset;  // ex.2: 6 - 3 = 3
  memset (m_pData,                                 i_Value, i_EndOffset);     // ex.2: writing the bytes 0,1,2
  memset (m_pData + m_DataLength - sizeOf2ndBlock, i_Value, sizeOf2ndBlock);  // ex.2: start offset = 8 - 3 = 5  --> writing the bytes 5,6,7.
  return true;
}

//--------------------------------------------------------------------
bool RingBuffer::WriteRange_StartToEnd (uint16_t  i_StartOffset,  //                ex.1: 3    ex.2: 6
                                        uint16_t  i_EndOffset,    //                ex.1: 6    ex.2: 3
                                        uint8_t   i_Value)        // m_DataLength:  ex.1: 8    ex.2: 8
{
  if (i_StartOffset >= m_DataLength)  // ex.1,2: m_DataLength == 8  --> i_StartOffset may be 0 to 7.
    return false;
  if (i_EndOffset >= m_DataLength)  // ex.1,2: m_DataLength == 8  --> i_EndOffset may be 0 to 7.
    return false;

  if (i_StartOffset == i_EndOffset)  // write the entire buffer
  {
    memset (m_pData, i_Value, m_DataLength);
    return true;
  }

  if (i_EndOffset > i_StartOffset)  // the start is before the end, write in one pass (like in ex.1)
  {
    memset (m_pData + i_StartOffset, i_Value, i_EndOffset - i_StartOffset);  // ex.1: start offset = 3, length = 6 - 3 = 3 -->  writing the bytes 3,4,5
    return true;
  }

  // the start is after the end, write in two passes (like in ex.2)
  uint16_t bytesUntilEnd = m_DataLength - i_StartOffset;    // ex.2: 8 - 6 = 2
  memset (m_pData + i_StartOffset, i_Value, bytesUntilEnd); // ex.2: writing the bytes 6,7.
  memset (m_pData                , i_Value, i_EndOffset);   // ex.2: writing the bytes 0,1,2.
  return true;
}
