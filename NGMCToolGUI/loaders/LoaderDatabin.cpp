#include "stdafx.h"

#include "LoaderDatabin.h"

#include "zlib.h"

namespace NGMC
{
	LoaderDatabin::LoaderDatabin(GAME game, std::wstring& filePath)
		: m_Game(game)
	{
		m_Reader.Init(filePath.c_str());
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

	unsigned int LoaderDatabin::GetFileCount() const
	{
		return m_FileHeaderOffsets.size();
	}

	uint32_t LoaderDatabin::GetFileHeaderOffset(unsigned int index) const
	{
		if (this->GetFileCount() <= index)
		{
			return 0U;
		}
		return m_FileHeaderOffsets[index];
	}

	FileType LoaderDatabin::GetFileType(unsigned int index) const
	{
		if (this->GetFileCount() <= index)
		{
			return FileType();
		}

		switch (m_Game)
		{
		case SIGMA_1:
			return FileType((S1::FileTypeId)m_ItemHeadersS1[index].type);
		case SIGMA_2:
			return FileType((S2::FileTypeId)m_ItemHeadersS2[index].type);
		default:
			return FileType();
		}
	}

	std::uintmax_t LoaderDatabin::GetFileSize(unsigned int index) const
	{
		if (this->GetFileCount() <= index)
		{
			return 0ULL;
		}

		switch (m_Game)
		{
		case SIGMA_1:
			return m_ItemHeadersS1[index].size;
		case SIGMA_2:
			return m_ItemHeadersS2[index].size;
		default:
			return 0ULL;
		}
	}
	
	std::uintmax_t LoaderDatabin::GetFileSizeCompressed(unsigned int index) const
	{
		if (this->GetFileCount() <= index)
		{
			return 0ULL;
		}

		switch (m_Game)
		{
		case SIGMA_1:
			return m_ItemHeadersS1[index].sizeCompressed;
		case SIGMA_2:
			return m_ItemHeadersS2[index].sizeCompressed;
		default:
			return 0ULL;
		}
	}

	bool LoaderDatabin::LoadHeader(DatabinHeader& outHeader)
	{
		return m_Reader.ReadValue(outHeader);
	}
	
	bool LoaderDatabin::LoadItemHeader(S1::DatabinItemHeader& outItemHeader, unsigned int index)
	{
		return LoadItemHeaderT(outItemHeader, index);
	}

	bool LoaderDatabin::LoadItemHeader(S2::DatabinItemHeader& outItemHeader, unsigned int index)
	{
		return LoadItemHeaderT(outItemHeader, index);
	}

	bool LoaderDatabin::LoadItemHeaders()
	{
		if (m_Reader.GetFileSize() > 0)
		{
			DatabinHeader header = DatabinHeader();
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
					m_ItemHeadersS1.push_back(S1::DatabinItemHeader());
					m_Reader.ReadValue(m_ItemHeadersS1[i]);
					break;
				case SIGMA_2:
					m_ItemHeadersS2.push_back(S2::DatabinItemHeader());
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

			DatabinHeader header;
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
					S1::DatabinItemHeader fileHeader = S1::DatabinItemHeader();
					m_Reader.ReadValue(fileHeader);
					offset = fileHeader.offset;
					size = fileHeader.size;
					sizeCompressed = fileHeader.sizeCompressed;
					break;
				}
				case SIGMA_2:
				{
					S2::DatabinItemHeader fileHeader = S2::DatabinItemHeader();
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