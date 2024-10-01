#include "stdafx.h"

#include "MemoryBuffer.h"

#include "utility/Utility.h"

namespace NGMC
{
	MemoryBuffer::MemoryBuffer()
		: m_Buffer(nullptr), m_Size(0ULL), m_IsMemoryAllocated(false), m_Position(0ULL)
	{
	}

	MemoryBuffer::MemoryBuffer(const MemoryBuffer& other)
		: MemoryBuffer()
	{
		//*this = MemoryBuffer();

		fillMemory(other.GetBaseAddress(), other.GetSize());
	}

	MemoryBuffer::MemoryBuffer(const char* buffer, size_t size)
		: MemoryBuffer()
	{
		//*this = MemoryBuffer();

		fillMemory(buffer, size);
	}

	MemoryBuffer::MemoryBuffer(std::ifstream& stream, size_t size)
		: MemoryBuffer()
	{
		//*this = MemoryBuffer();

		LoadFromStream(stream, size);
	}

	MemoryBuffer::MemoryBuffer(const wchar_t* filePath)
		: MemoryBuffer()
	{
		size_t size = GetFileSize(filePath);
		std::ifstream stream(filePath, std::ios::binary);

		if (stream)
		{
			LoadFromStream(stream, size);
		}
	}

	MemoryBuffer::~MemoryBuffer()
	{
		if (m_IsMemoryAllocated)
		{
			m_IsMemoryAllocated = false;
			delete[] m_Buffer;
		}
	}

	MemoryBuffer& MemoryBuffer::operator=(const MemoryBuffer& other)
	{
		fillMemory(other.GetBaseAddress(), other.GetSize());

		return *this;
	}
	
	char* MemoryBuffer::GetBaseAddress() const
	{
		return m_Buffer;
	}

	size_t MemoryBuffer::GetSize() const
	{
		return m_Size;
	}

	char* MemoryBuffer::PrepareBuffer(size_t size)
	{
		if (m_IsMemoryAllocated)
		{
			m_IsMemoryAllocated = false;
			delete[] m_Buffer;
		}

		m_Size = size;
		m_Buffer = new char[size];
		memset(m_Buffer, 0, size);
		m_Position = 0;
		m_IsMemoryAllocated = true;

		return m_Buffer;
	}

	bool MemoryBuffer::LoadFromBuffer(const char* buffer, size_t size)
	{
		fillMemory(buffer, size);

		return true;
	}

	bool MemoryBuffer::LoadFromStream(std::ifstream& stream, size_t size)
	{
		if (stream)
		{
			PrepareBuffer(size);

			stream.read(m_Buffer, m_Size);

			return true;
		}
		return false;
	}

	bool MemoryBuffer::SaveToFile(const wchar_t* filePath)
	{
		std::ofstream stream(filePath, std::ios::binary);

		if (stream && m_Size > 0)
		{
			stream.write(m_Buffer, m_Size);

			return true;
		}
		return false;
	}

	bool MemoryBuffer::Seek(uintptr_t pos)
	{
		if (0 <= pos && pos < m_Size)
		{
			m_Position = pos;
			return true;
		}
		m_Position = m_Size;
		return false;
	}

	bool MemoryBuffer::Seek(intptr_t off, seekdir way)
	{
		switch (way)
		{
		case beg:
			return Seek(off);
		case cur:
			if (m_Position + off < m_Size && 0 <= m_Position + off)
			{
				m_Position += off;
				return true;
			} break;
		case end:
			if (off < 0 && off <= -(intptr_t)m_Size)
			{
				m_Position = m_Size + off;
				return true;
			} break;
		}
		m_Position = m_Size;
		return false;
	}

	uintptr_t MemoryBuffer::Tell() const
	{
		return m_Position;
	}

	bool MemoryBuffer::Read(char* out, size_t count)
	{
		if (m_Position + count <= m_Size)
		{
			memcpy(out, m_Buffer + m_Position, count);
			Seek(count, cur);
			return true;
		}
		else
		{
			m_Position = m_Size;
			return false;
		}
	}

	bool MemoryBuffer::Write(const char* buf, size_t count)
	{
		if (m_Position + count <= m_Size)
		{
			memcpy(m_Buffer + m_Position, buf, count);
			Seek(count, cur);
			return true;
		}
		else
		{
			m_Position = m_Size;
			return false;
		}
	}

	void MemoryBuffer::fillMemory(const char* buffer, size_t size)
	{
		PrepareBuffer(size);

		memcpy(m_Buffer, buffer, size);
	}
}