#pragma once

#include "stdafx.h"

#include "utility/Utility.h"
#include "MemoryBuffer.h"
#include "DataReader.h"

namespace NGMC
{
	//	The databin file header struct.
	struct DatabinHeader
	{
		int32_t dat_00, dat_04, dat_08, dat_0C;
		uint32_t headerSize;        // 0x20 ? less than the whole header idk
		uint32_t fileDataOffset;    // relative to the headerSize
		int32_t dat_18, dat_1C;
		uint32_t fileCount;
		uint32_t fileIndicesOffset; // relative to the headerSize
		uint32_t fileCount1;
		int32_t dat_2C;
	};

	namespace S1
	{
		//	The compressed databin item header struct from NGS1.
		struct DatabinItemHeader
		{
			uint32_t offset;
			int32_t dat_04;
			uint32_t size;
			uint32_t sizeCompressed;
			uint32_t dat_10;
			uint16_t indexLinkedFile0, indexLinkedFile1;
			uint8_t typeLinkedFile0, typeLinkedFile1;
			uint8_t type;
			uint8_t dat_1B;
			int32_t dat_1C;
		};
	}

	namespace S2
	{
		//	The compressed databin item header struct from NGS2.
		struct DatabinItemHeader
		{
			uint32_t offset;
			int32_t dat_04;
			uint32_t size;
			uint32_t sizeCompressed;
			uint32_t dat_10;
			uint16_t indexLinkedFile;
			uint8_t typeLinkedFile;
			uint8_t type;
		};
	}

	//	Class for loading data from a databin file.
	class LoaderDatabin
	{
	public:
		//	Constructs a Databin Loader object by specifying the wide char string file path to read the file at.
		LoaderDatabin(GAME game, const wchar_t* filePath);
		
		//	Constructs a Databin Loader object by specifying the memory buffer to read the file from.
		LoaderDatabin(GAME game, MemoryBuffer& memBuf);
		
		//	Constructs a Databin Loader object by specifying the file object to read from.
		LoaderDatabin(GAME game, File& file);

		//	Deconstructs the Databin Loader object.
		~LoaderDatabin();

		//	Returns the count of databin items contained in the databin.
		unsigned int GetFileCount();

		//	Returns the offset between the databin header and the databin item header at the specified index.
		uint32_t GetFileHeaderOffset(unsigned int index);

		//	Returns the FileType of the databin item at the specified index.
		FileType GetFileType(unsigned int index);

		//	Returns the size of the databin item at the specified index.
		std::uintmax_t GetFileSize(unsigned int index);

		//	Returns the compressed size of the databin item at the specified index.
		std::uintmax_t GetFileSizeCompressed(unsigned int index);


		//	Loads the Databin header into the specified outHeader, returns whether the operation was successful.
		bool LoadHeader(DatabinHeader& outHeader);

		//	Sigma 1: Loads the item header of the databin item at the specified index into the specified outItemHeader, returns whether the operation was successful.
		bool LoadItemHeader(S1::DatabinItemHeader& outItemHeader, unsigned int index);

		//	Sigma 2: Loads the item header of the databin item at the specified index into the specified outItemHeader, returns whether the operation was successful.
		bool LoadItemHeader(S2::DatabinItemHeader& outItemHeader, unsigned int index);

		//	Loads the item headers of all databin items into m_ItemHeaders, returns whether the operation was successful.
		bool LoadItemHeaders();

		//	Decompresses the databin item at the specified index, returns whether the operation was successful.
		bool DecompressItem(MemoryBuffer& outBuffer, unsigned int index);

	private:
		//	Unique identifier for the game to which the databin belongs. As of now, this cannot be infered purely from the file itself.
		GAME m_Game;

		//	The DataReader object responsible for reading data from the file associated with the loader object.
		DataReader m_Reader;

		//	List of the item header offsets, filled by the LoadItemHeaders functions.
		std::vector<uint32_t> m_FileHeaderOffsets;

		//	Sigma 1: List of the item headers, filled by the LoadItemHeaders functions.
		std::vector<S1::DatabinItemHeader> m_ItemHeadersS1;

		//	Sigma 2: List of the item headers, filled by the LoadItemHeaders functions.
		std::vector<S2::DatabinItemHeader> m_ItemHeadersS2;

		//	Loads the item header of the file at the specified index into the specified outItemHeader, returns whether the operation was successful.
		template<typename T_ItemHeader>
		bool LoadItemHeaderT(T_ItemHeader& outItemHeader, unsigned int index)
		{
			bool isSuccess = false;

			DatabinHeader header = DatabinHeader();
			m_Reader.Seek(0, MemoryBuffer::beg);
			m_Reader.ReadValue(header);

			if (index < header.fileCount)
			{
				m_Reader.Seek((std::streamoff)index * sizeof(uint32_t), MemoryBuffer::cur);

				uint32_t fileHeaderOffset = m_Reader.ReadUInt32();

				m_Reader.Seek((std::streamoff)header.headerSize + fileHeaderOffset);
				m_Reader.ReadValue(outItemHeader);

				isSuccess = true;
			}

			return isSuccess;
		}
	};
}