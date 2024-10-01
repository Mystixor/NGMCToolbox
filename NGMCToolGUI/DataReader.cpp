#include "stdafx.h"

#include "DataReader.h"

#include "utility/Utility.h"

namespace NGMC
{
	DataReader::DataReader()
		: m_IsFileInMemory(true), m_Size(0LL),
		m_InBuffer(nullptr)
	{
	}

	DataReader::~DataReader()
	{
	}

	bool DataReader::Init(const wchar_t* filePath)
	{
		m_IsFileInMemory = false;
		m_InStream.open(filePath, std::ios::binary);
		if (m_InStream)
		{
			m_InStream.seekg(0, std::ios::end);
			std::streampos end = m_InStream.tellg();
			m_InStream.seekg(0, std::ios::beg);
			m_Size = end - m_InStream.tellg();
			return true;
		}
		else
			return false;
	}

	bool DataReader::Init(MemoryBuffer& inBuffer)
	{
		m_IsFileInMemory = true;
		m_InBuffer = &inBuffer;
		m_Size = m_InBuffer->GetSize();
		m_InBuffer->Seek(0, MemoryBuffer::beg);

		return true;
	}

	bool DataReader::Init(File& file)
	{
		if (file.IsFileInMemory())
			return DataReader::Init(file.GetMemoryBuffer());
		else
			return DataReader::Init(file.GetFilePath());
	}

	std::streamoff DataReader::Tell()
	{
		if (m_IsFileInMemory)
			return m_InBuffer->Tell();
		else
			return m_InStream.tellg();
	}
	
	void DataReader::Seek(std::streamoff offset, MemoryBuffer::seekdir way)
	{
		if (m_IsFileInMemory)
			m_InBuffer->Seek(offset, way);
		else
		{
			switch (way)
			{
			case MemoryBuffer::beg:
				m_InStream.seekg(offset, std::ios::beg);
				break;
			case MemoryBuffer::cur:
				m_InStream.seekg(offset, std::ios::cur);
				break;
			case MemoryBuffer::end:
				m_InStream.seekg(offset, std::ios::end);
				break;
			}
		}
	}

	std::streamsize DataReader::GetFileSize() const
	{
		return m_Size;
	}
	
	uint8_t DataReader::ReadUInt8()
	{
		uint8_t val = 0;
		ReadValue(val);
		return val;
	}
	
	int8_t DataReader::ReadInt8()
	{
		int8_t val = 0;
		ReadValue(val);
		return val;
	}
	
	uint16_t DataReader::ReadUInt16()
	{
		uint16_t val = 0;
		ReadValue(val);
		return val;
	}
	
	int16_t DataReader::ReadInt16()
	{
		int16_t val = 0;
		ReadValue(val);
		return val;
	}
	
	uint32_t DataReader::ReadUInt32()
	{
		uint32_t val = 0;
		ReadValue(val);
		return val;
	}
	
	int32_t DataReader::ReadInt32()
	{
		int32_t val = 0;
		ReadValue(val);
		return val;
	}
	
	uint64_t DataReader::ReadUInt64()
	{
		uint64_t val = 0;
		ReadValue(val);
		return val;
	}
	
	int64_t DataReader::ReadInt64()
	{
		int64_t val = 0;
		ReadValue(val);
		return val;
	}
	
	float DataReader::ReadFloat()
	{
		float val = 0;
		ReadValue(val);
		return val;
	}
	
	double DataReader::ReadDouble()
	{
		double val = 0;
		ReadValue(val);
		return val;
	}
	
	std::string DataReader::ReadString(int length)
	{
		std::string text = "";
		if (m_IsFileInMemory)
		{
			for(int i = 0; length < 0 || i < length; i++)
			{
				char val;
				if (!m_InBuffer->Read(&val, sizeof(val)))
					throw;
				if (val == 0x0)
					return text;
				text += val;
			}
		}
		else
		{
			for (int i = 0; length < 0 || i < length; i++)
			{
				char val = 0;
				m_InStream.read(&val, sizeof(val));
				if (val == 0x0)
					return text;
				text += val;
			}
		}
	}
	
	std::wstring DataReader::ReadWString(int length)
	{
		std::wstring text = L"";
		if (m_IsFileInMemory)
		{
			for (int i = 0; length < 0 || i < length; i++)
			{
				wchar_t val = 0;
				if (!m_InBuffer->Read((char*)&val, sizeof(val)))
					throw;
				if (val == 0x0)
					return text;
				text += val;
			}
		}
		else
		{
			for (int i = 0; length < 0 || i < length; i++)
			{
				wchar_t val;
				m_InStream.read((char*)&val, sizeof(val));
				if (val == 0x0)
					return text;
				text += val;
			}
		}
	}
}