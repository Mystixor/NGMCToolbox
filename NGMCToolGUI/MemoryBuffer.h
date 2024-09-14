#pragma once

#include "stdafx.h"

namespace NGMC
{
	class MemoryBuffer
	{
	public:
		//	The seekdir defines the operation executed by the Seek member function. Possible values are beg (0), cur (1), end (2).
		using seekdir = int;

		//	Constructs a MemoryBuffer object with a size of 0 and no memory allocated.
		MemoryBuffer();

		//	Constructs a MemoryBuffer object and populate the buffer by copying the entire size of another MemoryBuffer object into memory.
		MemoryBuffer(const MemoryBuffer& other);

		//	Constructs a MemoryBuffer object and populate the buffer by copying the specified size in bytes of a buffer array into memory.
		MemoryBuffer(const char* buffer, size_t size);

		//	Constructs a MemoryBuffer object and populate the buffer by reading the specified size in bytes of an input file stream into memory.
		MemoryBuffer(std::ifstream& stream, size_t size);

		//	Constructs a MemoryBuffer object and populate the buffer by copying the contents of an entire file into memory.
		MemoryBuffer(const wchar_t* filePath);

		//	Deallocates the memory in the buffer, if any had been allocated.
		~MemoryBuffer();

		//	The assignment operator to copy another MemoryBuffer's memory into this one's.
		MemoryBuffer& operator=(const MemoryBuffer& other);

		//	Returns the address of the buffer at position 0.
		char* GetBaseAddress() const;

		//	Returns the size of the currently allocated buffer.
		size_t GetSize() const;

		//	Allocates a specified size of bytes in the buffer and returns the base address. If any memory had been allocated already, it is first deallocated.
		char* PrepareBuffer(size_t size);

		//	Populates the buffer by copying the specified size in bytes of a buffer array into memory.
		bool LoadFromBuffer(const char* buffer, size_t size);

		//	Populates the buffer by reading the specified size in bytes of an input file stream into memory.
		bool LoadFromStream(std::ifstream& stream, size_t size);

		//	Saves the entire buffer to a file on disk. Returns whether the operation succeeded.
		bool SaveToFile(const wchar_t* filePath);

		//	Seeks to an absolute position. Returns whether there is at least 1 more byte available to read before reaching the end of the buffer.
		bool Seek(uintptr_t pos);

		//	Seeks by an offset relative to the beginning (way = MemoryBuffer::beg), relative to the current position (way = MemoryBuffer::cur), or relative to the end (way = MemoryBuffer::end). Returns whether there is at least 1 more byte available to read before reaching the end of the buffer.
		bool Seek(intptr_t off, seekdir way);

		//	Returns the current position in the buffer.
		uintptr_t Tell() const;

		//	Reads a count of bytes from the buffer into the out array, if the operation stays within the bounds of the buffer. Returns whether the operation succeeded. If false is returned, the position is set to the first byte after the buffer.
		bool Read(char* out, size_t count);

		//	Writes a count of bytes from the buf array into the buffer, if the operation stays within the bounds of the buffer. Returns whether the operation succeeded. If false is returned, the position is set to the first byte after the buffer.
		bool Write(const char* buf, size_t count);

		//	For seeking relative to the beginning of the buffer.
		static constexpr int beg = 0;

		//	For seeking relative to the current position in the buffer.
		static constexpr int cur = 1;

		//	For seeking relative to the end of the buffer.
		static constexpr int end = 2;

	private:
		//	Fills the buffer by copying the entire size of the parameter buffer array.
		void fillMemory(const char* buffer, size_t size);

		//	The underlying char array which is managed by this class.
		char* m_Buffer;

		//	The size of the buffer.
		size_t m_Size;

		//	Whether any memory is allocated at m_Buffer or not.
		bool m_IsMemoryAllocated;

		//	The current position in the buffer used by the Read/Write/Seek member functions.
		uintptr_t m_Position;
	};
}