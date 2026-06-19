#include <FastCRC.h>
#include "MemoryTools_Memory.h"
#include "ByteBuffer.h"

using namespace MemoryTools;

//--------------------------------------------------------------------
bool ByteBuffer::Create ( uint16_t      i_DataLength,
                          uint8_t       i_DefaultValue,
                          bool          i_IsRingBuffer,
                          ByteBuffer*&  o_pByteBuffer)
{
  if (i_DataLength < 1
  ||  i_DataLength > c_MaxBufferSize)
    return false;
  if (o_pByteBuffer != nullptr)
    return false;

  uint8_t* pData = nullptr;
  if (!Memory::Allocate (pData, i_DataLength, i_DefaultValue))
    return false;

  o_pByteBuffer = new ByteBuffer (pData, i_DataLength, i_DefaultValue, i_IsRingBuffer);
  return true;
}

//--------------------------------------------------------------------
ByteBuffer::~ByteBuffer ()
{
  delete (m_pData);
  m_pData         = nullptr;
  m_pAfterData    = nullptr;
  m_pCurrentRead  = nullptr;
  m_pCurrentWrite = nullptr;
}

//--------------------------------------------------------------------
ByteBuffer::ByteBuffer (uint8_t*  i_pData,
                        uint16_t  i_DataLength,
                        uint8_t   i_DefaultValue,
                        bool      i_IsRingBuffer)
{
  m_pData         = i_pData;
  m_DataLength    = i_DataLength;
  m_pCurrentRead  = i_pData;
  m_pCurrentWrite = i_pData;
  m_pAfterData    = i_pData + i_DataLength;
  m_DefaultValue  = i_DefaultValue;
  m_IsRingBuffer  = i_IsRingBuffer;
}

//--------------------------------------------------------------------
uint16_t ByteBuffer::get_CurrentReadAddress ()
{
  return m_pCurrentRead - m_pData;
}

//--------------------------------------------------------------------
uint16_t ByteBuffer::get_CurrentWriteAddress ()
{
  return m_pCurrentWrite - m_pData;
}

//--------------------------------------------------------------------
uint8_t* ByteBuffer::get_pData ()
{
  return m_pData;
}

//--------------------------------------------------------------------
uint16_t ByteBuffer::get_IsRingBuffer ()
{
  return m_IsRingBuffer;
}

//--------------------------------------------------------------------
uint16_t ByteBuffer::get_Length ()
{
  return m_DataLength;
}

//--------------------------------------------------------------------
void ByteBuffer::Clear ()
{
  memset (m_pData, m_DefaultValue, m_DataLength);
}

//--------------------------------------------------------------------
bool ByteBuffer::Clear_From ( uint16_t  i_StartAddress,
                              uint16_t  i_ByteCount)
{
  return WriteRange_From (i_StartAddress, i_ByteCount, m_DefaultValue);
}

//--------------------------------------------------------------------
bool ByteBuffer::Clear_To ( uint16_t  i_EndAddress,
                            uint16_t  i_ByteCount)
{
  return WriteRange_To (i_EndAddress, i_ByteCount, m_DefaultValue);
}

//--------------------------------------------------------------------
bool ByteBuffer::Clear_FromTo ( uint16_t  i_StartAddress,
                                uint16_t  i_EndAddress)
{
  return WriteRange_FromTo (i_StartAddress, i_EndAddress, m_DefaultValue);
}

//--------------------------------------------------------------------
bool ByteBuffer::MoveReadPointer (uint16_t i_Count)
{
  uint8_t* pCurrentRead = m_pCurrentRead + i_Count;

  if (!m_IsRingBuffer)  // Notice: The pointer of a linear buffer may point to the first position after the buffer, but not to any position beyond that.
  {
    if (pCurrentRead > m_pAfterData)
      return false;

    m_pCurrentRead = pCurrentRead;
    return true;
  }

  if (pCurrentRead >= m_pAfterData)
    pCurrentRead -= m_DataLength;
  if (pCurrentRead >= m_pAfterData)
    return false;

  m_pCurrentRead = pCurrentRead;
  return true;
}

//--------------------------------------------------------------------
bool ByteBuffer::MoveWritePointer (uint16_t i_Count)
{
  uint8_t* pCurrentWrite = m_pCurrentWrite + i_Count;

  if (!m_IsRingBuffer)  // Notice: The pointer of a linear buffer may point to the first position after the buffer, but not to any position beyond that.
  {
    if (pCurrentWrite > m_pAfterData)
      return false;

    m_pCurrentWrite = pCurrentWrite;
    return true;
  }

  if (pCurrentWrite >= m_pAfterData)
    pCurrentWrite -= m_DataLength;
  if (pCurrentWrite >= m_pAfterData)
    return false;

  m_pCurrentWrite = pCurrentWrite;
  return true;
}

//--------------------------------------------------------------------
bool ByteBuffer::SetReadPointer ( uint16_t  i_Address,
                                  bool      i_WrapIfNeeded)
{
  if (!m_IsRingBuffer)  // Notice: The pointer of a linear buffer may point to the first position after the buffer, but not to any position beyond that.
  {
    if (i_Address > m_DataLength)
      return false;

    m_pCurrentRead = m_pData + i_Address;
    return true;
  }

  if (i_Address >= m_DataLength
  &&  i_WrapIfNeeded)
    i_Address -= m_DataLength;
  if (i_Address >= m_DataLength)
    return false;

  m_pCurrentRead = m_pData + i_Address;
  return true;
}

//--------------------------------------------------------------------
bool ByteBuffer::SetWritePointer (uint16_t  i_Address,
                                  bool      i_WrapIfNeeded)
{
  if (!m_IsRingBuffer)  // Notice: The pointer of a linear buffer may point to the first position after the buffer, but not to any position beyond that.
  {
    if (i_Address > m_DataLength)
      return false;

    m_pCurrentWrite = m_pData + i_Address;
    return true;
  }

  if (i_Address >= m_DataLength
  &&  i_WrapIfNeeded)
    i_Address -= m_DataLength;
  if (i_Address >= m_DataLength)
    return false;

  m_pCurrentWrite = m_pData + i_Address;
  return true;
}

//--------------------------------------------------------------------
bool ByteBuffer::ReadBytesAndMovePtr (uint16_t  i_ByteCount,
                                      uint8_t*  i_pDestination,
                                      bool      i_InvertByteOrder)
{
  if (i_pDestination == nullptr)
    return false;
  if (i_ByteCount == 0)
    return true;
  if (i_ByteCount > m_DataLength)
    return false;

  uint16_t bytesUntilEnd = m_pAfterData - m_pCurrentRead;
  if (!m_IsRingBuffer
  &&  i_ByteCount > bytesUntilEnd)
    return false;

  if (i_InvertByteOrder)
  {
    for (uint16_t index = 1; index <= i_ByteCount; index++)
    {
      uint8_t valueUI8;
      if (!ReadValueAndMovePtr (valueUI8))
        return false;  // should not happen because of previous check
      i_pDestination[i_ByteCount - index] = valueUI8;
    }
    return true;
  }

  if (i_ByteCount <= bytesUntilEnd)
  {
    memcpy (i_pDestination, m_pCurrentRead, i_ByteCount);
    return MoveReadPointer (i_ByteCount);
  }

  uint16_t sizeOf2ndBlock = i_ByteCount - bytesUntilEnd;
  memcpy (i_pDestination                , m_pCurrentRead, bytesUntilEnd);
  memcpy (i_pDestination + bytesUntilEnd, m_pData       , sizeOf2ndBlock);
  m_pCurrentRead = m_pData + sizeOf2ndBlock;
  return true;
}

//--------------------------------------------------------------------
//                                   Start   2nd   Copy   Remarks
// Example 1)  Len = 3                      Block  Steps
// Source,      L = 10: ____ABC___     4
// Destination, L =  9: ______ABC      6      0      1    0x 2nd-block > 0
// Destination, L = 10: ______ABC_     6      0      1    0x 2nd-block > 0
// Destination, L = 11: ______ABC__    6      0      1    0x 2nd-block > 0
//
// Example 2)  Len = 5
// Source,      L = 10: ____ABCDE_     4      0
// Destination, L =  7: DE__ABC        4      2      2    1x 2nd-block > 0
// Destination, L =  7: CDE__AB        5      3      2    1x 2nd-block > 0
// Destination, L =  8: E___ABCD       4      1      2    1x 2nd-block > 0
// Destination, L =  8: DE___ABC       5      2      2    1x 2nd-block > 0
// Destination, L =  9: ____ABCDE      4      0      1    0x 2nd-block > 0
// Destination, L =  9: E____ABCD      5      1      2    1x 2nd-block > 0
// Destination, L = 11: ______ABCDE    6      0      1    0x 2nd-block > 0
// Destination, L = 11: E______ABCD    7      1      2    1x 2nd-block > 0
//
// Example 3)  Len = 7
// Source,      L =  9: DEFG__ABC      6      4
// Destination, L =  8: DEFG_ABC       5      4      2    2x 2nd-block > 0, equal-sized
// Destination, L =  9: DEFG__ABC      6      4      2    2x 2nd-block > 0, equal-sized
// Destination, L = 10: DEFG___ABC     7      4      2    2x 2nd-block > 0, equal-sized
// Destination, L = 10: ___ABCDEFG     3      0      2    1x 2nd-block > 0
// Destination, L = 10: FG___ABCDE     5      2      3    2x 2nd-block > 0, different-sized
// Destination, L = 10: CDEFG___AB     8      5      3    2x 2nd-block > 0, different-sized
//
bool ByteBuffer::ReadBytesAndMovePtr (uint16_t    i_ByteCount,
                                      ByteBuffer* i_pDestination,
                                      bool        i_InvertByteOrder)
{
  if (i_pDestination == nullptr)
    return false;
  if (i_ByteCount == 0)
    return true;
  if (i_ByteCount > m_DataLength)
    return false;
  if (i_ByteCount > i_pDestination->get_Length ())
    return false;

  uint16_t bytesUntilEndInSource = m_pAfterData - m_pCurrentRead;
  if (!m_IsRingBuffer
  &&  i_ByteCount > bytesUntilEndInSource)
    return false;

  uint16_t bytesUntilEndInDestination = i_pDestination->get_Length () - i_pDestination->get_CurrentWriteAddress ();
  if (!i_pDestination->m_IsRingBuffer
  &&  i_ByteCount > bytesUntilEndInDestination)
    return false;

  if (i_InvertByteOrder)
  {
    for (uint16_t index = 1; index <= i_ByteCount; index++)
    {
      uint8_t valueUI8;
      if (!ReadValueAndMovePtr (valueUI8))
        return false;  // should not happen because of previous check
      if (!i_pDestination->WriteValueAndMovePtr (valueUI8))
        return false;  // should not happen because of previous check
    }
    return true;
  }

  for (int pass = 1; pass <= 3; pass++)
  {
    bytesUntilEndInSource      = m_pAfterData                  - m_pCurrentRead;
    bytesUntilEndInDestination = i_pDestination->get_Length () - i_pDestination->get_CurrentWriteAddress ();
    uint16_t sizeOf2ndBlockInSource      = i_ByteCount > bytesUntilEndInSource      ? i_ByteCount - bytesUntilEndInSource      : 0;
    uint16_t sizeOf2ndBlockInDestination = i_ByteCount > bytesUntilEndInDestination ? i_ByteCount - bytesUntilEndInDestination : 0;
    uint16_t copySize = i_ByteCount - max (sizeOf2ndBlockInSource, sizeOf2ndBlockInDestination);

    memcpy (i_pDestination->m_pCurrentWrite, m_pCurrentRead, copySize);
    bool result = MoveReadPointer (i_ByteCount)
               && i_pDestination->MoveWritePointer (i_ByteCount);
    if (!result) return result;

    i_ByteCount -= copySize;
    if (i_ByteCount == 0)
      return true;
  }

  return false;  // should not happen
}

//--------------------------------------------------------------------
bool ByteBuffer::ReadValueAndMovePtr (bool& o_Value)
{
  o_Value = m_pCurrentRead < m_pAfterData
          ? *m_pCurrentRead != 0
          : false;
  return MoveReadPointer ();
}

//--------------------------------------------------------------------
bool ByteBuffer::ReadValueAndMovePtr (uint8_t& o_Value)
{
  o_Value = m_pCurrentRead < m_pAfterData
          ? *m_pCurrentRead
          : 0;
  return MoveReadPointer ();
}

//--------------------------------------------------------------------
bool ByteBuffer::ReadValueAndMovePtr (int8_t& o_Value)
{
  o_Value = m_pCurrentRead < m_pAfterData
          ? (int8_t)*m_pCurrentRead
          : 0;
  return MoveReadPointer ();
}

//--------------------------------------------------------------------
bool ByteBuffer::ReadValueAndMovePtr (uint16_t& o_Value,
                                      bool      i_InvertByteOrder)
{
  return ReadBytesAndMovePtr (sizeof (uint16_t), (uint8_t*)&o_Value, i_InvertByteOrder);
}

//--------------------------------------------------------------------
bool ByteBuffer::ReadValueAndMovePtr (int16_t&  o_Value,
                                      bool      i_InvertByteOrder)
{
  return ReadBytesAndMovePtr (sizeof (int16_t), (uint8_t*)&o_Value, i_InvertByteOrder);
}

//--------------------------------------------------------------------
bool ByteBuffer::ReadValueAndMovePtr (uint32_t& o_Value,
                                      bool      i_InvertByteOrder)
{
  return ReadBytesAndMovePtr (sizeof (uint32_t), (uint8_t*)&o_Value, i_InvertByteOrder);
}

//--------------------------------------------------------------------
bool ByteBuffer::ReadValueAndMovePtr (int32_t&  o_Value,
                                      bool      i_InvertByteOrder)
{
  return ReadBytesAndMovePtr (sizeof (int32_t), (uint8_t*)&o_Value, i_InvertByteOrder);
}

//--------------------------------------------------------------------
bool ByteBuffer::WriteBytesAndMovePtr ( uint16_t  i_ByteCount,
                                        uint8_t*  i_pSource,
                                        bool      i_InvertByteOrder)
{
  if (i_pSource == nullptr)
    return false;
  if (i_ByteCount == 0)
    return true;
  if (i_ByteCount > m_DataLength)
    return false;

  uint16_t bytesUntilEnd = m_pAfterData - m_pCurrentWrite;
  if (!m_IsRingBuffer
  &&  i_ByteCount > bytesUntilEnd)
    return false;

  if (i_InvertByteOrder)
  {
    for (uint16_t index = 1; index <= i_ByteCount; index++)
    {
      if (!WriteValueAndMovePtr (i_pSource[i_ByteCount - index]))
        return false;  // should not happen because of previous check
    }
    return true;
  }

  if (i_ByteCount <= bytesUntilEnd)
  {
    memcpy (m_pCurrentWrite, i_pSource, i_ByteCount);
    return MoveWritePointer (i_ByteCount);
  }

  uint16_t sizeOf2ndBlock = i_ByteCount - bytesUntilEnd;
  memcpy (m_pCurrentWrite, i_pSource                , bytesUntilEnd);
  memcpy (m_pData        , i_pSource + bytesUntilEnd, sizeOf2ndBlock);
  m_pCurrentWrite = m_pData + sizeOf2ndBlock;
  return true;
}

//--------------------------------------------------------------------
//                                   Start   2nd   Copy   Remarks
// Example 1)  Len = 3                      Block  Steps
// Destination, L = 10: ____ABC___     4
// Source,      L =  9: ______ABC      6      0      1    0x 2nd-block > 0
// Source,      L = 10: ______ABC_     6      0      1    0x 2nd-block > 0
// Source,      L = 11: ______ABC__    6      0      1    0x 2nd-block > 0
//
// Example 2)  Len = 5
// Destination, L = 10: ____ABCDE_     4      0
// Source,      L =  7: DE__ABC        4      2      2    1x 2nd-block > 0
// Source,      L =  7: CDE__AB        5      3      2    1x 2nd-block > 0
// Source,      L =  8: E___ABCD       4      1      2    1x 2nd-block > 0
// Source,      L =  8: DE___ABC       5      2      2    1x 2nd-block > 0
// Source,      L =  9: ____ABCDE      4      0      1    0x 2nd-block > 0
// Source,      L =  9: E____ABCD      5      1      2    1x 2nd-block > 0
// Source,      L = 11: ______ABCDE    6      0      1    0x 2nd-block > 0
// Source,      L = 11: E______ABCD    7      1      2    1x 2nd-block > 0
//
// Example 3)  Len = 7
// Destination, L =  9: DEFG__ABC      6      4
// Source,      L =  8: DEFG_ABC       5      4      2    2x 2nd-block > 0, equal-sized
// Source,      L =  9: DEFG__ABC      6      4      2    2x 2nd-block > 0, equal-sized
// Source,      L = 10: DEFG___ABC     7      4      2    2x 2nd-block > 0, equal-sized
// Source,      L = 10: ___ABCDEFG     3      0      2    1x 2nd-block > 0
// Source,      L = 10: FG___ABCDE     5      2      3    2x 2nd-block > 0, different-sized
// Source,      L = 10: CDEFG___AB     8      5      3    2x 2nd-block > 0, different-sized
//
bool ByteBuffer::WriteBytesAndMovePtr ( uint16_t    i_ByteCount,
                                        ByteBuffer* i_pSource,
                                        bool        i_InvertByteOrder)
{
  if (i_pSource == nullptr)
    return false;
  if (i_ByteCount == 0)
    return true;
  if (i_ByteCount > m_DataLength)
    return false;
  if (i_ByteCount > i_pSource->get_Length ())
    return false;

  uint16_t bytesUntilEndInDestination = m_pAfterData - m_pCurrentWrite;
  if (!m_IsRingBuffer
  &&  i_ByteCount > bytesUntilEndInDestination)
    return false;

  uint16_t bytesUntilEndInSource = i_pSource->get_Length () - i_pSource->get_CurrentReadAddress ();
  if (!i_pSource->m_IsRingBuffer
  &&  i_ByteCount > bytesUntilEndInSource)
    return false;

  if (i_InvertByteOrder)
  {
    for (uint16_t index = 1; index <= i_ByteCount; index++)
    {
      uint8_t valueUI8;
      if (!i_pSource->ReadValueAndMovePtr (valueUI8))
        return false;  // should not happen because of previous check
      if (!WriteValueAndMovePtr (valueUI8))
        return false;  // should not happen because of previous check
    }
    return true;
  }

  for (int pass = 1; pass <= 3; pass++)
  {
    bytesUntilEndInDestination = m_pAfterData             - m_pCurrentWrite;
    bytesUntilEndInSource      = i_pSource->get_Length () - i_pSource->get_CurrentReadAddress ();
    uint16_t sizeOf2ndBlockInDestination = i_ByteCount > bytesUntilEndInDestination ? i_ByteCount - bytesUntilEndInDestination : 0;
    uint16_t sizeOf2ndBlockInSource      = i_ByteCount > bytesUntilEndInSource      ? i_ByteCount - bytesUntilEndInSource      : 0;
    uint16_t copySize = i_ByteCount - max (sizeOf2ndBlockInDestination, sizeOf2ndBlockInSource);

    memcpy (m_pCurrentWrite, i_pSource->m_pCurrentRead, copySize);
    bool result = MoveWritePointer (i_ByteCount)
               && i_pSource->MoveReadPointer (i_ByteCount);
    if (!result) return result;

    i_ByteCount -= copySize;
    if (i_ByteCount == 0)
      return true;
  }

  return false;  // should not happen
}

//--------------------------------------------------------------------
bool ByteBuffer::WriteValueAndMovePtr (bool i_Value)
{
  if (m_pCurrentWrite < m_pAfterData)
    *m_pCurrentWrite = i_Value ? 1 : 0;
  return MoveWritePointer ();
}

//--------------------------------------------------------------------
bool ByteBuffer::WriteValueAndMovePtr (uint8_t i_Value)
{
  if (m_pCurrentWrite < m_pAfterData)
    *m_pCurrentWrite = i_Value;
  return MoveWritePointer ();
}

//--------------------------------------------------------------------
bool ByteBuffer::WriteValueAndMovePtr (int8_t i_Value)
{
  if (m_pCurrentWrite < m_pAfterData)
    *m_pCurrentWrite = i_Value;
  return MoveWritePointer ();
}

//--------------------------------------------------------------------
bool ByteBuffer::WriteValueAndMovePtr ( uint16_t  i_Value,
                                        bool      i_InvertByteOrder)
{
  return WriteBytesAndMovePtr (sizeof (uint16_t), (uint8_t*)&i_Value, i_InvertByteOrder);
}

//--------------------------------------------------------------------
bool ByteBuffer::WriteValueAndMovePtr ( int16_t   i_Value,
                                        bool      i_InvertByteOrder)
{
  return WriteBytesAndMovePtr (sizeof (int16_t), (uint8_t*)&i_Value, i_InvertByteOrder);
}

//--------------------------------------------------------------------
bool ByteBuffer::WriteValueAndMovePtr ( uint32_t  i_Value,
                                        bool      i_InvertByteOrder)
{
  return WriteBytesAndMovePtr (sizeof (uint32_t), (uint8_t*)&i_Value, i_InvertByteOrder);
}

//--------------------------------------------------------------------
bool ByteBuffer::WriteValueAndMovePtr ( int32_t   i_Value,
                                        bool      i_InvertByteOrder)
{
  return WriteBytesAndMovePtr (sizeof (int32_t), (uint8_t*)&i_Value, i_InvertByteOrder);
}

//--------------------------------------------------------------------
bool ByteBuffer::WriteRange_From (uint16_t  i_StartAddress,
                                  uint16_t  i_ByteCount,
                                  uint8_t   i_Value)
{
  if (i_StartAddress >= m_DataLength)
    return false;
  if (i_ByteCount == 0)
    return true;
  if (i_ByteCount > m_DataLength)
    return false;

  uint16_t bytesUntilEnd = m_DataLength - i_StartAddress;
  if (i_ByteCount <= bytesUntilEnd)
  {
    memset (m_pData + i_StartAddress, i_Value, i_ByteCount);
    return true;
  }

  if (!m_IsRingBuffer)
    return false;

  uint16_t sizeOf2ndBlock = i_ByteCount - bytesUntilEnd;
  memset (m_pData + i_StartAddress, i_Value, bytesUntilEnd);
  memset (m_pData                 , i_Value, sizeOf2ndBlock);
  return true;
}

//--------------------------------------------------------------------
bool ByteBuffer::WriteRange_To (uint16_t  i_EndAddress,  //                ex.1: 6    ex.2: 3    --> ex.1: writing ___xxx__
                                uint16_t  i_ByteCount,   //                ex.1: 3    ex.2: 6        ex.2: writing xxx__xxx
                                uint8_t   i_Value)       // m_DataLength:  ex.1: 8    ex.2: 8
{
  if (m_IsRingBuffer)
  {
    if (i_EndAddress >= m_DataLength)  // ex.1,2: m_DataLength == 8  --> i_EndAddress may be 0 to 7.
      return false;
    if (i_EndAddress == 0)
      i_EndAddress = m_DataLength;
  }
  else
  {
    if (i_EndAddress > m_DataLength)
      return false;
  }
  if (i_ByteCount == 0)  // nothing to do.
    return true;
  if (i_ByteCount > m_DataLength)  // cannot write more values than the buffer actually has.
    return false;

  if (i_ByteCount <= i_EndAddress)  // the start is before the end, write in one pass (like in ex.1)
  {
    memset (m_pData + i_EndAddress - i_ByteCount, i_Value, i_ByteCount);  // ex.1:  start offset = 6 - 3 = 3  --> writing the bytes 3,4,5.
    return true;
  }

  if (!m_IsRingBuffer)
    return false;

  // the start is after the end, write in two passes (like in ex.2)
  uint16_t sizeOf2ndBlock = i_ByteCount - i_EndAddress;  // ex.2: 6 - 3 = 3
  memset (m_pData,                                 i_Value, i_EndAddress);    // ex.2: writing the bytes 0,1,2
  memset (m_pData + m_DataLength - sizeOf2ndBlock, i_Value, sizeOf2ndBlock);  // ex.2: start offset = 8 - 3 = 5  --> writing the bytes 5,6,7.
  return true;
}

//--------------------------------------------------------------------
bool ByteBuffer::WriteRange_FromTo (uint16_t  i_StartAddress,  //                ex.1: 3    ex.2: 6    --> ex.1: writing ___xxx__
                                    uint16_t  i_EndAddress,    //                ex.1: 6    ex.2: 3        ex.2: writing xxx___xx
                                    uint8_t   i_Value)         // m_DataLength:  ex.1: 8    ex.2: 8
{
  if (i_StartAddress >= m_DataLength)  // ex.1,2: m_DataLength == 8  --> i_StartAddress may be 0 to 7.
    return false;
  if (m_IsRingBuffer)
  {
    if (i_EndAddress >= m_DataLength)  // ex.1,2: m_DataLength == 8  --> i_EndAddress may be 0 to 7.
      return false;
  }
  else
  {
    if (i_EndAddress > m_DataLength)  // ex.1,2: m_DataLength == 8  --> i_EndAddress may be 0 to 8.
      return false;
  }
  if (i_EndAddress == i_StartAddress)
    return true;

  if (i_EndAddress > i_StartAddress)  // the start is before the end, write in one pass (like in ex.1)
  {
    memset (m_pData + i_StartAddress, i_Value, i_EndAddress - i_StartAddress);  // ex.1: start offset = 3, length = 6 - 3 = 3 -->  writing the bytes 3,4,5
    return true;
  }

  if (!m_IsRingBuffer)
    return false;

  // the start is after the end, write in two passes (like in ex.2)
  uint16_t bytesUntilEnd = m_DataLength - i_StartAddress;     // ex.2: 8 - 6 = 2
  memset (m_pData + i_StartAddress, i_Value, bytesUntilEnd);  // ex.2: writing the bytes 6,7.
  memset (m_pData                 , i_Value, i_EndAddress);   // ex.2: writing the bytes 0,1,2.
  return true;
}

//--------------------------------------------------------------------
bool ByteBuffer::CalcChecksumCRC16_From ( uint16_t  i_StartAddress,
                                          uint16_t  i_ByteCount,
                                          uint16_t& o_Checksum)
{
  o_Checksum = 0;
  if (i_StartAddress >= m_DataLength)
    return false;
  if (i_ByteCount == 0)
    return true;
  if (i_ByteCount > m_DataLength)
    return false;

  FastCRC16 crc16;

  uint16_t bytesUntilEnd = m_DataLength - i_StartAddress;
  if (i_ByteCount <= bytesUntilEnd)
  {
    o_Checksum = crc16.modbus (m_pData + i_StartAddress, i_ByteCount);
    return true;
  }

  if (!m_IsRingBuffer)
    return false;

  uint16_t sizeOf2ndBlock = i_ByteCount - bytesUntilEnd;
               crc16.modbus     (m_pData + i_StartAddress, bytesUntilEnd);
  o_Checksum = crc16.modbus_upd (m_pData                 , sizeOf2ndBlock);
  return true;
}

//--------------------------------------------------------------------
bool ByteBuffer::CalcChecksumCRC16_To ( uint16_t  i_EndAddress,
                                        uint16_t  i_ByteCount,
                                        uint16_t& o_Checksum)
{
  o_Checksum = 0;
  if (m_IsRingBuffer)
  {
    if (i_EndAddress >= m_DataLength)
      return false;
    if (i_EndAddress == 0)
      i_EndAddress = m_DataLength;
  }
  else
  {
    if (i_EndAddress > m_DataLength)
      return false;
  }
  if (i_ByteCount == 0)
    return true;
  if (i_ByteCount > m_DataLength)
    return false;

  FastCRC16 crc16;

  if (i_ByteCount <= i_EndAddress)
  {
    o_Checksum = crc16.modbus (m_pData + i_EndAddress - i_ByteCount, i_ByteCount);
    return true;
  }

  if (!m_IsRingBuffer)
    return false;

  uint16_t sizeOf2ndBlock = i_ByteCount - i_EndAddress;
               crc16.modbus     (m_pData,                                 i_EndAddress);
  o_Checksum = crc16.modbus_upd (m_pData + m_DataLength - sizeOf2ndBlock, sizeOf2ndBlock);
  return true;
}

//--------------------------------------------------------------------
bool ByteBuffer::CalcChecksumCRC16_FromTo ( uint16_t  i_StartAddress,
                                            uint16_t  i_EndAddress,
                                            uint16_t& o_Checksum)
{
  if (i_StartAddress >= m_DataLength)
    return false;
  if (m_IsRingBuffer)
  {
    if (i_EndAddress >= m_DataLength)
      return false;
  }
  else
  {
    if (i_EndAddress > m_DataLength)
      return false;
  }
  if (i_EndAddress == i_StartAddress)
    return true;

  FastCRC16 crc16;

  if (i_EndAddress > i_StartAddress)
  {
    o_Checksum = crc16.modbus (m_pData + i_StartAddress, i_EndAddress - i_StartAddress);
    return true;
  }

  if (!m_IsRingBuffer)
    return false;

  uint16_t bytesUntilEnd = m_DataLength - i_StartAddress;
               crc16.modbus     (m_pData + i_StartAddress, bytesUntilEnd);
  o_Checksum = crc16.modbus_upd (m_pData                 , i_EndAddress);
  return true;
}

//--------------------------------------------------------------------
bool ByteBuffer::Print (Stream* i_pOutput,
                        bool    i_AppendNewLine)
{
  return Print (i_pOutput, 0, m_DataLength, i_AppendNewLine);
}

//--------------------------------------------------------------------
bool ByteBuffer::Print (Stream*   i_pOutput,
                        uint16_t  i_StartAddress,
                        uint16_t  i_ByteCount,
                        bool      i_AppendNewLine)
{
  if (i_pOutput == nullptr)
    return false;
  if (i_StartAddress >= m_DataLength)
    return false;
  if (i_ByteCount == 0)
    return true;
  if (i_ByteCount > m_DataLength)
    return false;

  for (uint16_t index = 0; index < i_ByteCount; index++)
  {
    uint16_t address = i_StartAddress + index;
    if (address >= m_DataLength)
      address -= m_DataLength;
    i_pOutput->print (m_pData[address], HEX2);
  }
  if (i_AppendNewLine)
    i_pOutput->println ();

  return true;
}
