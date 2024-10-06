#include "stdafx.h"

#include "LoaderTMC.h"

#include "utility/Math.h"

namespace NGMC
{
	using namespace TMC;

	LoaderTMC::LoaderTMC(const wchar_t* filePath)
	{
		m_Reader.Init(filePath);
	}

	LoaderTMC::LoaderTMC(MemoryBuffer& memBuf)
	{
		m_Reader.Init(memBuf);
	}

	LoaderTMC::LoaderTMC(File& file)
	{
		m_Reader.Init(file);
	}

	LoaderTMC::~LoaderTMC()
	{
	}

	bool LoaderTMC::GetTMCHeader(TMC::ChunkHeader& outHeader)
	{
		bool isSuccess = false;

		{
			std::streamoff pos = m_Reader.Tell();

			m_Reader.Seek(0, MemoryBuffer::beg);
			m_Reader.ReadValue(outHeader);

			isSuccess = true;

			m_Reader.Seek(pos, MemoryBuffer::beg);
		}

		return isSuccess;
	}

    unsigned int LoaderTMC::GetTMCChunkCount()
    {
        std::streamoff pos = m_Reader.Tell();

        m_Reader.Seek(offsetof(S1::TMC, S1::TMC::header.childCount), MemoryBuffer::beg);
        uint32_t chunkCount = m_Reader.ReadUInt32();

        m_Reader.Seek(pos, MemoryBuffer::beg);

		return chunkCount;
    }

	bool LoaderTMC::GetTMCChunkType(S1::ChunkType outType, unsigned int index)
	{
		return false;
	}

	bool LoaderTMC::GetTMCChunkType(S2::ChunkType outType, unsigned int index)
	{
		return false;
	}
}