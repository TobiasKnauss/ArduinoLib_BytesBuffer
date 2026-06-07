#ifndef ByteRingBuffer_h
#define ByteRingBuffer_h

#include <Arduino.h>

class ByteRingBuffer
{
//==================== Fields ====================
private:
  //-------------------- static --------------------

  static const uint16_t c_MaxBufferSize = 65500;

  //-------------------- instance --------------------

  uint8_t* m_pData;
  uint16_t m_DataLength;
  uint8_t* m_pCurrentRead;
  uint8_t* m_pCurrentWrite;
  uint8_t* m_pAfterData;
  uint8_t  m_DefaultValue;

//==================== Constructors ====================
public:
  //-------------------- static --------------------

  static bool Create (uint16_t          i_RingBufferLength,
                      uint8_t           i_DefaultValue,
                      ByteRingBuffer*&  o_pRingBuffer);

  //-------------------- instance --------------------

  ~ByteRingBuffer ();

private:
  //-------------------- instance --------------------

  ByteRingBuffer (uint8_t*  i_pData,
                  uint16_t  i_DataLength,
                  uint8_t   i_DefaultValue);

//==================== Properties ====================
public:
  //-------------------- instance --------------------

  uint16_t get_CurrentReadOffset ();
  uint16_t get_CurrentWriteOffset ();

  uint8_t* get_pData ();

  uint16_t get_Length ();

//==================== Public Methods ====================
public:
  //-------------------- instance --------------------

  // Reset the entire buffer to its default value.
  void Clear ();


  //--------------------------------------------------------------------
  // Pointer editing

  // Move the read pointer by the given number.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  void MoveReadPointer (uint16_t i_Count = 1);

  // Move the write pointer by the given number.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  void MoveWritePointer (uint16_t i_Count = 1);

  // Set the read pointer to the given offset.
  // If the end of the ring buffer is reached the the wrap option is set, wrap the pointer around to the buffer start.
  bool SetReadPointer ( uint16_t  i_Offset,
                        bool      i_WrapIfNeeded = false);

  // Set the write pointer to the given offset.
  // If the end of the ring buffer is reached the the wrap option is set, wrap the pointer around to the buffer start.
  bool SetWritePointer ( uint16_t  i_Offset,
                        bool      i_WrapIfNeeded = false);

  //--------------------------------------------------------------------
  // ReadByteAndMovePtr, ReadBytesAndMovePtr

  uint8_t ReadByteAndMovePtr ();

  // Read the specified number of bytes from the ring buffer to the given destination pointer and move the pointer forward.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  bool ReadBytesAndMovePtr (uint16_t  i_ByteCount,
                            uint8_t*  i_pDestination,
                            bool      i_InvertByteOrder);

  //--------------------------------------------------------------------
  // ReadValueAndMovePtr

  // Read a BOOL value from the ring buffer and move the pointer forward.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  void ReadValueAndMovePtr (bool& o_Value);

  // Read an UINT8 value from the ring buffer and move the pointer forward.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  void ReadValueAndMovePtr (uint8_t& o_Value);

  // Read an INT8 value from the ring buffer and move the pointer forward.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  void ReadValueAndMovePtr (int8_t& o_Value);

  // Read an UINT16 value from the ring buffer and move the pointer forward.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  bool ReadValueAndMovePtr (uint16_t& o_Value,
                            bool      i_InvertByteOrder);

  // Read an INT16 value from the ring buffer and move the pointer forward.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  bool ReadValueAndMovePtr (int16_t&  o_Value,
                            bool      i_InvertByteOrder);

  // Read an UINT32 value from the ring buffer and move the pointer forward.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  bool ReadValueAndMovePtr (uint32_t& o_Value,
                            bool      i_InvertByteOrder);

  // Read an INT32 value from the ring buffer and move the pointer forward.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  bool ReadValueAndMovePtr (int32_t&  o_Value,
                            bool      i_InvertByteOrder);

  //--------------------------------------------------------------------
  // WriteByteAndMovePtr, WriteBytesAndMovePtr

  void WriteByteAndMovePtr (uint8_t i_Value);

  // Write the specified number of bytes from the given source pointer to the ring buffer and move the pointer forward.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  bool WriteBytesAndMovePtr ( uint16_t  i_ByteCount,
                            uint8_t*  i_pSource,
                            bool      i_InvertByteOrder);

  //--------------------------------------------------------------------
  // WriteValueAndMovePtr

  // Write a BOOL value into the ring buffer and move the pointer forward.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  void WriteValueAndMovePtr (bool i_Value);

  // Write an UINT8 value into the ring buffer and move the pointer forward.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  void WriteValueAndMovePtr (uint8_t i_Value);

  // Write an INT8 value into the ring buffer and move the pointer forward.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  void WriteValueAndMovePtr (int8_t i_Value);

  // Write an UINT16 value into the ring buffer and move the pointer forward.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  bool WriteValueAndMovePtr ( uint16_t  i_Value,
                              bool      i_InvertByteOrder);

  // Write an INT16 value into the ring buffer and move the pointer forward.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  bool WriteValueAndMovePtr ( int16_t   i_Value,
                              bool      i_InvertByteOrder);

  // Write an UINT32 value into the ring buffer and move the pointer forward.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  bool WriteValueAndMovePtr ( uint32_t  i_Value,
                              bool      i_InvertByteOrder);

  // Write an INT32 value into the ring buffer and move the pointer forward.
  // If the end of the ring buffer is reached, wrap the pointer around to the buffer start.
  bool WriteValueAndMovePtr ( int32_t   i_Value,
                              bool      i_InvertByteOrder);

  //--------------------------------------------------------------------
  // WriteRange_...

  // Write the given value to the specified number of bytes in the ring buffer, starting at the given buffer address.
  // i_StartAddress:      The relative position in the ring buffer from which on the value is written.
  // i_ByteCount:         The number of bytes that are written in the buffer.
  // i_Value:             The value that is written into the buffer.
  bool WriteRange_FromStart ( uint16_t  i_StartAddress,
                              uint16_t  i_ByteCount,
                              uint8_t   i_Value);

  // Write the given value to the specified number of bytes in the ring buffer, ending at the given buffer address.
  // i_EndAddress:        The relative position in the ring buffer up to which the value is written. This position is NOT included!
  // i_ByteCount:         The number of bytes that are written in the buffer.
  // i_Value:             The value that is written into the buffer.
  bool WriteRange_ToEnd ( uint16_t  i_EndAddress,
                          uint16_t  i_ByteCount,
                          uint8_t   i_Value);

  // Write the given value to the ring buffer, starting and ending at the given buffer addresses.
  // i_StartAddress:      The relative position in the ring buffer from which on the value is written.
  // i_EndAddress:        The relative position in the ring buffer up to which the value is written. This position is NOT included!
  //                      If start offset == end offset, the entire buffer will be written.
  // i_Value:             The value that is written into the buffer.
  bool WriteRange_StartToEnd (uint16_t  i_StartAddress,
                              uint16_t  i_EndAddress,
                              uint8_t   i_Value);
};

#endif
