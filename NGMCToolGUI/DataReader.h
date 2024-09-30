#pragma once

#include "MemoryBuffer.h"
#include "File.h"

namespace NGMC
{
	//	Class for reading data equally from a file on disk as from a MemoryBuffer.
	class DataReader
	{
	public:
		//	Constructs a DataReader object.
		DataReader();

		//	Deconstructs the DataReader object.
		~DataReader();

		//	Initializes a DataReader object using the path to the file which will be read. The file will be opened in an Input File Stream until this DataReader object is deconstructed.
		bool Init(const wchar_t* filePath);

		//	Initializes a DataReader object using a reference to the MemoryBuffer of the file which will be read.
		bool Init(MemoryBuffer& inBuffer);
		
		//	Initializes a DataReader object using a reference to the file object which will be read from.
		bool Init(File& file);

		//	Returns the offset in bytes to the beginnning of the file of where the reading head is currently at.
		std::streamoff Tell();

		//	Seeks by an offset in bytes relative to the position defined by the parameter way.
		void Seek(std::streamoff offset, MemoryBuffer::seekdir way = MemoryBuffer::beg);

		//	Returns the size of either the Input File Stream or the MemoryBuffer.
		std::streamsize GetFileSize() const;

		//	Reads and returns an 8-bit unsigned integer.
		uint8_t ReadUInt8();

		//	Reads and returns an 8-bit signed integer.
		int8_t ReadInt8();

		//	Reads and returns a 16-bit unsigned integer.
		uint16_t ReadUInt16();

		//	Reads and returns a 16-bit signed integer.
		int16_t ReadInt16();

		//	Reads and returns a 32-bit unsigned integer.
		uint32_t ReadUInt32();

		//	Reads and returns a 32-bit signed integer.
		int32_t ReadInt32();

		//	Reads and returns a 64-bit unsigned integer.
		uint64_t ReadUInt64();

		//	Reads and returns a 64-bit signed integer.
		int64_t ReadInt64();

		//	Reads and returns a float.
		float ReadFloat();

		//	Reads and returns a double.
		double ReadDouble();

		//	Reads and returns a string of the given length. If the length is negative, the string will grow until a NULL-termination character is hit.
		std::string ReadString(int length = -1);

		//	Reads and returns a wide character string of the given length. If the length is negative, the string will grow until a NULL-termination character is hit.
		std::wstring ReadWString(int length = -1);

		//	Reads a value of type T. The value is stored and returned via the given outValue.
		template<typename T>
		bool ReadValue(T& outVal)
		{
			if (m_IsFileInMemory)
			{
				return m_InBuffer->Read((char*)&outVal, sizeof(T));
			}
			else
			{
				m_InStream.read((char*)&outVal, sizeof(T));
				return true;
			}
		}

		//	Reads an array of type T with the given count of elements. The array is stored and returned via the given outArray.
		template<typename T>
		void ReadArray(T* outArray, unsigned int count)
		{
			for (unsigned int i = 0; i < count; i++)
			{
				ReadValue(*(outArray + i));
			}
		}

	private:
		//	Whether the file is loaded from memory or disk.
		bool m_IsFileInMemory;

		//	The Memory Buffer to read the file from, unused if m_IsFileInMemory == false
		MemoryBuffer* m_InBuffer;

		//	The Input File Stream to read the file from, unused if m_IsFileInMemory == true
		std::ifstream m_InStream;

		//	The size of either the Input File Stream or the MemoryBuffer.
		std::streamsize m_Size;
	};
}