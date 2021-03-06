#ifndef _MEDUSA_BINARY_STREAM_
#define _MEDUSA_BINARY_STREAM_

#include "medusa/namespace.hpp"
#include "medusa/types.hpp"
#include "medusa/endian.hpp"
#include "medusa/exception.hpp"
#include "medusa/export.hpp"

#include <string>
#include <cstring>
#include <memory>

#include <boost/type_traits.hpp>

#ifdef _MSC_VER
# pragma warning(disable: 4251)
#endif

MEDUSA_NAMESPACE_BEGIN

//! BinaryStream is a generic class to handle memory access.
class Medusa_EXPORT BinaryStream // TODO: disable copy...
{
public:
  typedef std::shared_ptr<BinaryStream> SharedPtr;

  BinaryStream(void);
  virtual ~BinaryStream(void);

  //! This method returns the current endianness.
  EEndianness GetEndianness(void) const             { return m_Endianness;        }

  //! This method sets the desired endianness.
  void        SetEndianness(EEndianness Endianness) { m_Endianness = Endianness;  }

  //! This method reads according to the size of rData and performs a swap if needed.
  bool Read(TOffset Position, s8  &rData) const
  { return ReadGeneric(Position, rData); }

  //! This method reads according to the size of rData and performs a swap if needed.
  bool Read(TOffset Position, u8  &rData) const
  { return ReadGeneric(Position, rData); }

  //! This method reads according to the size of rData and performs a swap if needed.
  bool Read(TOffset Position, s16 &rData) const
  { return ReadGeneric(Position, rData); }

  //! This method reads according to the size of rData and performs a swap if needed.
  bool Read(TOffset Position, u16 &rData) const
  { return ReadGeneric(Position, rData); }

  //! This method reads according to the size of rData and performs a swap if needed.
  bool Read(TOffset Position, s32 &rData) const
  { return ReadGeneric(Position, rData); }

  //! This method reads according to the size of rData and performs a swap if needed.
  bool Read(TOffset Position, u32 &rData) const
  { return ReadGeneric(Position, rData); }

  //! This method reads according to the size of rData and performs a swap if needed.
  bool Read(TOffset Position, s64 &rData) const
  { return ReadGeneric(Position, rData); }

  //! This method reads according to the size of rData and performs a swap if needed.
  bool Read(TOffset Position, u64 &rData) const
  { return ReadGeneric(Position, rData); }

  template<typename T, size_t N>
  bool Read(TOffset Position, T (&rData)[N]) const
  {
    for (size_t i = 0; i < N; ++i)
    {
      if (Read(Position, rData[i]) == false)
        return false;
      Position += sizeof(T);
    }
    return true;
  }

  //! This method reads a buffer, no swap will be performed.
  bool Read(TOffset Position, void* pData, size_t Length) const
  {
    return Read(Position, static_cast<u8*>(pData), Length);
  }

  //! This method reads a buffer, no swap will be performed.
  bool Read(TOffset Position, u8* pData, size_t Length) const
  {
    if (m_pBuffer == nullptr)
      return false;

    if (Position + Length < Position || Position + Length > m_Size)
      return false;

    u8 const* pDataPosition = reinterpret_cast<u8 const*>(m_pBuffer) + Position;
    memcpy(pData, pDataPosition, Length);
    return true;
  }

  bool Write(TOffset Position, s8  const& rData)
  { return WriteGeneric(Position, rData); }
  bool Write(TOffset Position, u8  const& rData)
  { return WriteGeneric(Position, rData); }

  bool Write(TOffset Position, s16 const& rData)
  { return WriteGeneric(Position, rData); }
  bool Write(TOffset Position, u16 const& rData)
  { return WriteGeneric(Position, rData); }

  bool Write(TOffset Position, s32 const& rData)
  { return WriteGeneric(Position, rData); }
  bool Write(TOffset Position, u32 const& rData)
  { return WriteGeneric(Position, rData); }

  bool Write(TOffset Position, s64 const& rData)
  { return WriteGeneric(Position, rData); }
  bool Write(TOffset Position, u64 const& rData)
  { return WriteGeneric(Position, rData); }

  template<typename T, size_t N>
  bool Write(TOffset Position, T const (&rData)[N]) const
  {
    for (size_t i = 0; i < N; ++i)
    {
      if (Write(Position, rData[i]) == false)
        return false;
      Position += sizeof(T);
    }

    return true;
  }

  bool Write(TOffset Position, void const* pData, size_t Length)
  {
    return Write(Position, static_cast<u8 const*>(pData), Length);
  }

  bool Write(TOffset Position, u8 const* pData, size_t Length)
  {
    if (m_pBuffer == nullptr)
      return false;

    if (Position + Length < Position || Position + Length > m_Size)
      return false;

    u8* pDataPosition = reinterpret_cast<u8*>(m_pBuffer) + Position;
    memcpy(pDataPosition, pData, Length);
    return true;
  }

  u32         GetSize(void)   const { return m_Size;    }
  void const* GetBuffer(void) const { return m_pBuffer; }

protected:
  template <typename DataType>
  bool ReadGeneric(TOffset Position, DataType& rData) const
  {
    if (m_pBuffer == nullptr)
      return false;

    if (Position + sizeof(DataType) > m_Size)
      return false;

    u8 const* pDataPosition = reinterpret_cast<u8 const*>(m_pBuffer) + Position;

    rData = *reinterpret_cast<DataType const*>(pDataPosition);
    if (TestEndian(m_Endianness))
      EndianSwap(rData);
    return true;
  }

  template <typename DataType>
  bool WriteGeneric(TOffset Position, DataType& rData)
  {
    if (m_pBuffer == nullptr)
      return false;

    if (Position + sizeof(DataType) > m_Size)
      return false;

    typename boost::remove_const<DataType>::type* pDataPosition
      = reinterpret_cast< typename boost::remove_const<DataType>::type* >(m_pBuffer) + Position;

    *pDataPosition = rData;
    if (TestEndian(m_Endianness))
      EndianSwap(pDataPosition);
    return true;
  }

  void*         m_pBuffer;
  u32           m_Size;
  EEndianness   m_Endianness;
};

//! FileBinaryStream is a generic class for file access.
class Medusa_EXPORT FileBinaryStream : public BinaryStream
{
public:
  FileBinaryStream(void);
  FileBinaryStream(std::wstring const& rFilePath);
  virtual ~FileBinaryStream(void);

  void Open(std::wstring const& rFilePath);
  void Close(void);

protected:
  std::wstring  m_FileName;
  TFileHandle   m_FileHandle;
  TMapHandle    m_MapHandle;
};

//! MemoryBinaryStream is similar to BinaryStream.
class Medusa_EXPORT MemoryBinaryStream : public BinaryStream
{
public:
  MemoryBinaryStream(void);
  MemoryBinaryStream(void const* pMem, u32 MemSize);
  virtual ~MemoryBinaryStream(void);

  void Open(void const* pMem, u32 MemSize);
  void Close(void);
};

MEDUSA_NAMESPACE_END

#endif // _MEDUSA_BINARY_STREAM_