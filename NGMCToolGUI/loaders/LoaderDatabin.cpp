#include "stdafx.h"

#include "LoaderDatabin.h"

#include "zlib.h"

namespace NGMC
{
	using namespace Databin;

	LoaderDatabin::LoaderDatabin(GAME game, const wchar_t* filePath)
		: m_Game(game)
	{
		m_Reader.Init(filePath);
	}

	LoaderDatabin::LoaderDatabin(GAME game, MemoryBuffer& memBuf)
		: m_Game(game)
	{
		m_Reader.Init(memBuf);
	}

	LoaderDatabin::LoaderDatabin(GAME game, File& file)
		: m_Game(game)
	{
		m_Reader.Init(file);
	}

	LoaderDatabin::~LoaderDatabin()
	{
	}

	unsigned int LoaderDatabin::GetFileCount()
	{
		std::streamoff pos = m_Reader.Tell();

		m_Reader.Seek(offsetof(Header, Header::fileCount), MemoryBuffer::beg);

		uint32_t fileCount = m_Reader.ReadUInt32();

		m_Reader.Seek(pos, MemoryBuffer::beg);

		return fileCount;
	}

	uint32_t LoaderDatabin::GetFileHeaderOffset(unsigned int index)
	{
		if (this->GetFileCount() <= index)
		{
			return 0U;
		}

		if(m_FileHeaderOffsets.size() >= index)
			return m_FileHeaderOffsets[index];

		std::streamoff pos = m_Reader.Tell();

		m_Reader.Seek(sizeof(Header) + index * sizeof(uint32_t), MemoryBuffer::beg);

		uint32_t fileHeaderOffset = m_Reader.ReadUInt32();

		m_Reader.Seek(pos, MemoryBuffer::beg);

		return fileHeaderOffset;
	}

	FileType LoaderDatabin::GetFileType(unsigned int index)
	{
		if (this->GetFileCount() <= index)
		{
			return FileType();
		}

		switch (m_Game)
		{
		case SIGMA_1:
			if (m_ItemHeadersS1.size() >= index)
				return FileType((S1::FileTypeId)m_ItemHeadersS1[index].type);
			else
			{
				S1::ItemHeader itemHeader = S1::ItemHeader();
				this->LoadItemHeader(itemHeader, index);
				return FileType((S1::FileTypeId)itemHeader.type);
			}
			break;
		case SIGMA_2:
			if (m_ItemHeadersS2.size() >= index)
				return FileType((S2::FileTypeId)m_ItemHeadersS2[index].type);
			else
			{
				S2::ItemHeader itemHeader = S2::ItemHeader();
				this->LoadItemHeader(itemHeader, index);
				return FileType((S2::FileTypeId)itemHeader.type);
			}
			break;
		default:
			return FileType();
		}
	}

	std::uintmax_t LoaderDatabin::GetFileSize(unsigned int index)
	{
		if (this->GetFileCount() <= index)
		{
			return 0ULL;
		}
		
		switch (m_Game)
		{
		case SIGMA_1:
			if (m_ItemHeadersS1.size() >= index)
				return m_ItemHeadersS1[index].size;
			else
			{
				S1::ItemHeader itemHeader = S1::ItemHeader();
				this->LoadItemHeader(itemHeader, index);
				return itemHeader.size;
			}
			break;
		case SIGMA_2:
			if (m_ItemHeadersS2.size() >= index)
				return m_ItemHeadersS2[index].size;
			else
			{
				S2::ItemHeader itemHeader = S2::ItemHeader();
				this->LoadItemHeader(itemHeader, index);
				return itemHeader.size;
			}
			break;
		default:
			return 0ULL;
		}
	}
	
	std::uintmax_t LoaderDatabin::GetFileSizeCompressed(unsigned int index)
	{
		if (this->GetFileCount() <= index)
		{
			return 0ULL;
		}

		switch (m_Game)
		{
		case SIGMA_1:
			if (m_ItemHeadersS1.size() >= index)
				return m_ItemHeadersS1[index].sizeCompressed;
			else
			{
				S1::ItemHeader itemHeader = S1::ItemHeader();
				this->LoadItemHeader(itemHeader, index);
				return itemHeader.sizeCompressed;
			}
			break;
		case SIGMA_2:
			if (m_ItemHeadersS2.size() >= index)
				return m_ItemHeadersS2[index].sizeCompressed;
			else
			{
				S2::ItemHeader itemHeader = S2::ItemHeader();
				this->LoadItemHeader(itemHeader, index);
				return itemHeader.sizeCompressed;
			}
			break;
		default:
			return 0ULL;
		}
	}

	bool LoaderDatabin::LoadHeader(Header& outHeader)
	{
		bool isSuccess = false;

		std::streamoff pos = m_Reader.Tell();

		{
			m_Reader.Seek(0, MemoryBuffer::beg);
			isSuccess = m_Reader.ReadValue(outHeader);
		}

		m_Reader.Seek(pos, MemoryBuffer::beg);

		return isSuccess;
	}
	
	bool LoaderDatabin::LoadItemHeader(S1::ItemHeader& outItemHeader, unsigned int index)
	{
		return LoadItemHeaderT(outItemHeader, index);
	}

	bool LoaderDatabin::LoadItemHeader(S2::ItemHeader& outItemHeader, unsigned int index)
	{
		return LoadItemHeaderT(outItemHeader, index);
	}

	bool LoaderDatabin::LoadItemHeaders()
	{
		if (m_Reader.GetFileSize() > 0)
		{
			Header header = Header();
			m_Reader.Seek(0, MemoryBuffer::beg);
			m_Reader.ReadValue(header);

			std::streamoff pos = m_Reader.Tell();

			unsigned int extractCount = 0;
			for (unsigned int i = 0; i < header.fileCount; i++)
			{
				m_Reader.Seek(pos + i * sizeof(uint32_t), MemoryBuffer::beg);

				m_FileHeaderOffsets.push_back(0U);

				m_FileHeaderOffsets[i] = m_Reader.ReadUInt32();

				m_Reader.Seek((std::streamoff)header.headerSize + m_FileHeaderOffsets[i]);

				switch (m_Game)
				{
				case SIGMA_1:
					m_ItemHeadersS1.push_back(S1::ItemHeader());
					m_Reader.ReadValue(m_ItemHeadersS1[i]);
					break;
				case SIGMA_2:
					m_ItemHeadersS2.push_back(S2::ItemHeader());
					m_Reader.ReadValue(m_ItemHeadersS2[i]);
					break;
				default:
					//	Unsupported game.
					return false;
				}
			}
			return true;
		}
		return false;
	}

	bool LoaderDatabin::DecompressItem(MemoryBuffer& outBuffer, unsigned int index)
	{
		bool isSuccess = false;

		if (m_Reader.GetFileSize() > 0)
		{
			m_Reader.Seek(0, MemoryBuffer::beg);

			Header header;
			m_Reader.ReadValue(header);

			if (index < header.fileCount)
			{
				m_Reader.Seek(index * sizeof(uint32_t), MemoryBuffer::cur);

				uint32_t fileHeaderOffset = m_Reader.ReadUInt32();

				m_Reader.Seek((std::streamoff)header.headerSize + fileHeaderOffset);

				uint32_t offset = 0U;
				uint32_t size = 0U;
				uint32_t sizeCompressed = 0U;
				switch (m_Game)
				{
				case SIGMA_1:
				{
					S1::ItemHeader fileHeader = S1::ItemHeader();
					m_Reader.ReadValue(fileHeader);
					offset = fileHeader.offset;
					size = fileHeader.size;
					sizeCompressed = fileHeader.sizeCompressed;
					break;
				}
				case SIGMA_2:
				{
					S2::ItemHeader fileHeader = S2::ItemHeader();
					m_Reader.ReadValue(fileHeader);
					offset = fileHeader.offset;
					size = fileHeader.size;
					sizeCompressed = fileHeader.sizeCompressed;
					break;
				}
				default:
					//	Unsupported game.
					return false;
				}

				char* fileDataRaw = new char[sizeCompressed];
				m_Reader.Seek((std::streamoff)header.headerSize + header.fileDataOffset + offset);
				m_Reader.ReadArray(fileDataRaw, sizeCompressed);


				int ret;
				//unsigned have;
				z_stream strm;

				strm.zalloc = Z_NULL;
				strm.zfree = Z_NULL;
				strm.opaque = Z_NULL;
				strm.avail_in = 0;
				strm.next_in = Z_NULL;
				ret = inflateInit(&strm);
				if (ret != Z_OK)
					return ret;

				strm.avail_in = sizeCompressed;
				strm.next_in = (Bytef*)fileDataRaw;

				strm.avail_out = size;
				strm.next_out = (Bytef*)outBuffer.PrepareBuffer(size);

				ret = inflate(&strm, Z_NO_FLUSH);

				isSuccess = true;

				//assert(ret != Z_STREAM_ERROR);
				switch (ret)
				{
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR; [[fallthrough]];
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
					isSuccess = false;
					//return ret;
				}

				(void)inflateEnd(&strm);

				delete[] fileDataRaw;
			}
		}
		return isSuccess;
	}
}