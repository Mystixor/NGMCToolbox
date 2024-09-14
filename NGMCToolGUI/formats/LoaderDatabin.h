#pragma once

#include "stdafx.h"

#include "Utility.h"
#include "MemoryBuffer.h"

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
		//	Constructs a Databin Loader object.
		LoaderDatabin(GAME game);

		//	Constructs a Databin Loader object by specifying the wide char string file path to read the file at.
		LoaderDatabin(GAME game, std::wstring& filePath);

		//	Deconstructs the Databin Loader object.
		~LoaderDatabin();

		//	Returns the count of files contained in the Databin file.
		unsigned int GetFileCount() const;

		//	Returns the offset between the Databin file header and the item header of the file at the specified index.
		uint32_t GetFileHeaderOffset(unsigned int index) const;

		//	Returns the FileType of the file at the specified index.
		FileType GetFileType(unsigned int index) const;

		//	Returns the size of the file at the specified index.
		std::uintmax_t GetFileSize(unsigned int index) const;

		//	Returns the compressed size of the file at the specified index.
		std::uintmax_t GetFileSizeCompressed(unsigned int index) const;


		//	Sets the file path to be reading the Databin file from to the specified wide char string file path.
		void SetFilePath(const wchar_t* filePath);

		//	Loads the Databin file header into the specified outHeader from the file path saved in the Databin Loader object, returns whether the operation was successful.
		bool LoadHeader(DatabinHeader& outHeader);

		//	Loads the Databin file header into the specified outHeader from the specified inStream, returns whether the operation was successful.
		bool LoadHeader(DatabinHeader& outHeader, std::ifstream& inStream);

		//	Loads the Databin file header into the specified outHeader from the specified inBuffer, returns whether the operation was successful.
		bool LoadHeader(DatabinHeader& outHeader, MemoryBuffer& inBuffer);

		//	Sigma 1: Loads the item header of the file at the specified index into the specified outItemHeader, returns whether the operation was successful.
		bool LoadItemHeader(S1::DatabinItemHeader& outItemHeader, unsigned int index);

		//	Sigma 1: Loads the item header of the file at the specified index into the specified outItemHeader from the specified inStream, returns whether the operation was successful.
		bool LoadItemHeader(S1::DatabinItemHeader& outItemHeader, std::ifstream& inStream, unsigned int index);

		//	Sigma 1: Loads the item header of the file at the specified index into the specified outItemHeader from the specified inBuffer, returns whether the operation was successful.
		bool LoadItemHeader(S1::DatabinItemHeader& outItemHeader, MemoryBuffer& inBuffer, unsigned int index);
		
		//	Sigma 2: Loads the item header of the file at the specified index into the specified outItemHeader, returns whether the operation was successful.
		bool LoadItemHeader(S2::DatabinItemHeader& outItemHeader, unsigned int index);

		//	Sigma 2: Loads the item header of the file at the specified index into the specified outItemHeader from the specified inStream, returns whether the operation was successful.
		bool LoadItemHeader(S2::DatabinItemHeader& outItemHeader, std::ifstream& inStream, unsigned int index);

		//	Sigma 2: Loads the item header of the file at the specified index into the specified outItemHeader from the specified inBuffer, returns whether the operation was successful.
		bool LoadItemHeader(S2::DatabinItemHeader& outItemHeader, MemoryBuffer& inBuffer, unsigned int index);

		//	Loads the item headers of all files into m_FileHeaders from the specified inStream, returns whether the operation was successful.
		bool LoadItemHeaders(std::ifstream& inStream);

		//	Loads the item headers of all files into m_FileHeaders from the specified inBuffer, returns whether the operation was successful.
		bool LoadItemHeaders(MemoryBuffer& inBuffer);

		//	Loads the item headers of all files into m_FileHeaders from the file path saved in the Databin Loader object, returns whether the operation was successful.
		bool LoadItemHeaders();

		//	Decompresses the item at the specified index by reading from the specified inStream into the specified outBuffer, returns whether the operation was successful.
		bool DecompressItem(MemoryBuffer& outBuffer, std::ifstream& inStream, unsigned int index);

		//	Decompresses the item at the specified index by reading from the specified inBuffer into the specified outBuffer, returns whether the operation was successful.
		bool DecompressItem(MemoryBuffer& outBuffer, MemoryBuffer& inBuffer, unsigned int index);

		//	Decompresses the item at the specified index by reading from the file path saved in the Databin Loader object, returns whether the operation was successful.
		bool DecompressItem(MemoryBuffer& outBuffer, unsigned int index);

	private:
		GAME m_Game;

		//	The wide char string file path of the Databin file to read from, unused if functions specifying an inStream or inBuffer are called.
		std::wstring m_FilePath;

		//	List of the item header offsets, filled by the LoadItemHeaders functions.
		std::vector<uint32_t> m_FileHeaderOffsets;


		//	Sigma 1: List of the item headers, filled by the LoadItemHeaders functions.
		std::vector<S1::DatabinItemHeader> m_FileHeadersS1;

		//	Sigma 2: List of the item headers, filled by the LoadItemHeaders functions.
		std::vector<S2::DatabinItemHeader> m_FileHeadersS2;
	};
}