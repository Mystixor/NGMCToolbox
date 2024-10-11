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

	bool LoaderTMC::GetTMCHeader(TMC::Header& outHeader)
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

	bool LoaderTMC::GetTMCVersion(TMC::ChunkVersion& outVersion)
	{
		std::streamoff pos = m_Reader.Tell();

		m_Reader.Seek(offsetof(ChunkHeader, ChunkHeader::version), MemoryBuffer::beg);

		bool isSuccess = m_Reader.ReadValue(outVersion);

		m_Reader.Seek(pos, MemoryBuffer::beg);

		return isSuccess;
	}

    bool LoaderTMC::GetTMCChildCount(uint32_t& outCount)
    {
        std::streamoff pos = m_Reader.Tell();

        m_Reader.Seek(offsetof(ChunkHeader, ChunkHeader::childCount), MemoryBuffer::beg);
        outCount = m_Reader.ReadUInt32();

        m_Reader.Seek(pos, MemoryBuffer::beg);

		return true;
    }

	bool LoaderTMC::GetTMCChildType(S1::ChunkType& outType, unsigned int index)
	{
		return false;
	}

	bool LoaderTMC::GetTMCChildType(S2::ChunkType& outType, unsigned int index)
	{
		return false;
	}

	bool LoaderTMC::GetTMCIdentifier(std::string& outIdentifier)
	{
		std::streamoff pos = m_Reader.Tell();

		m_Reader.Seek(sizeof(ChunkHeader) + 0x20, MemoryBuffer::beg);

		char identifier[16];
		m_Reader.ReadArray(identifier, 16);
		outIdentifier = identifier;

		m_Reader.Seek(pos, MemoryBuffer::beg);

		return true;
	}

	bool LoaderTMC::GetTMCU15(std::vector<float>& outU15)
	{
		std::streamoff pos = m_Reader.Tell();

		m_Reader.Seek(sizeof(Header), MemoryBuffer::beg);
				
		bool isSuccess = false;
		ChunkVersion version = ChunkVersion();
		if(this->GetTMCVersion(version))
		{
			switch (version)
			{
			case vSIGMA1:
			{
				float U15[12];
				m_Reader.ReadArray(U15, 12);
				outU15.assign(U15, U15 + 12);
				isSuccess = true;
				break;
			}
			case vSIGMA2:
			{
				float U15[36];
				m_Reader.ReadArray(U15, 36);
				outU15.assign(U15, U15 + 36);
				isSuccess = true;
				break;
			}
			}
		}

		m_Reader.Seek(pos, MemoryBuffer::beg);

		return isSuccess;
	}

	bool LoaderTMC::GetTMCChunkIDs(std::vector<TMC::S1::ChunkType>& outIDs)
	{
		TMC::ChunkVersion version;
		if(!(this->GetTMCVersion(version) && version == vSIGMA1))
			return false;

		std::streamoff pos = m_Reader.Tell();

		bool isSuccess = false;
		uint32_t childCount = 0;
		if (this->GetTMCChildCount(childCount) && childCount > 0)
		{
			m_Reader.Seek(sizeof(Header) + 12 * sizeof(float), MemoryBuffer::beg);

			outIDs.resize(childCount);

			m_Reader.ReadArray(&outIDs[0], childCount);
		}
		else
			return false;

		m_Reader.Seek(pos, MemoryBuffer::beg);

		isSuccess = true;

		return isSuccess;
	}

	bool LoaderTMC::GetTMCChunkIDs(std::vector<TMC::S2::ChunkType>& outIDs)
	{
		TMC::ChunkVersion version;
		if (!(this->GetTMCVersion(version) && version == vSIGMA2))
			return false;

		std::streamoff pos = m_Reader.Tell();

		bool isSuccess = false;
		uint32_t childCount = 0;
		if (this->GetTMCChildCount(childCount) && childCount > 0)
		{
			m_Reader.Seek(sizeof(Header) + 36 * sizeof(float), MemoryBuffer::beg);

			outIDs.resize(childCount);

			m_Reader.ReadArray(&outIDs[0], childCount);

			isSuccess = true;
		}

		m_Reader.Seek(pos, MemoryBuffer::beg);

		return isSuccess;
	}

	bool LoaderTMC::GetTMCChunkOffsets(std::vector<int32_t>& outOffsets)
	{
		TMC::Header header = TMC::Header();
		if (!this->GetTMCHeader(header))
			return false;

		outOffsets.resize(header.childCount);

		std::streamoff pos = m_Reader.Tell();

		bool isSuccess = false;
		if (header.childCount)
		{
			m_Reader.Seek(header.dataOffset, MemoryBuffer::beg);

			m_Reader.ReadArray(&outOffsets[0], header.childCount);

			isSuccess = true;
		}

		m_Reader.Seek(pos, MemoryBuffer::beg);

		return isSuccess;
	}
}

std::string NGMC::TMC::GetChunkTypeName(S1::ChunkType type)
{
	using namespace S1;

	std::string output = "";

	switch (type)
	{
	case ChunkType::MdlGeo:
		output += "MdlGeo"; break;
	case ChunkType::TTG:
		output += "TTG"; break;
	case ChunkType::VtxLay:
		output += "VtxLay"; break;
	case ChunkType::IdxLay:
		output += "IdxLay"; break;
	case ChunkType::MtrCol:
		output += "MtrCol"; break;
	case ChunkType::MdlInfo:
		output += "MdlInfo"; break;
	case ChunkType::HieLay:
		output += "HieLay"; break;
	case ChunkType::LHeader:
		output += "LHeader"; break;
	case ChunkType::NodeLay:
		output += "NodeLay"; break;
	case ChunkType::GlblMtx:
		output += "GlblMtx"; break;
	case ChunkType::BnOfsMtx:
		output += "BnOfsMtx"; break;
	case ChunkType::cpf:
		output += "cpf"; break;
	case ChunkType::MCAPACK:
		output += "MCAPACK"; break;
	case ChunkType::RENPACK:
		output += "RENPACK"; break;
	case ChunkType::COLLIDE:
		output += "COLLIDE"; break;
	case ChunkType::OBJ_TYPE_INFO:
		output += "OBJ_TYPE_INFO"; break;
	case ChunkType::MDL_TYPE_INFO:
		output += "MDL_TYPE_INFO"; break;
	case ChunkType::AcsPack:
		output += "AcsPack"; break;
	case ChunkType::MPH_LYR:
		output += "MPH_LYR"; break;
	case ChunkType::OBJHIT:
		output += "OBJHIT"; break;
	case ChunkType::EXTMCOL:
		output += "EXTMCOL"; break;
	case ChunkType::MISCDATA:
		output += "MISCDATA"; break;
	case ChunkType::EPM1:
		output += "EPM1"; break;
	case ChunkType::EMCinfo:
		output += "EMCinfo"; break;
	default:
		output += "unknown";
	}

	return output;
}

std::string NGMC::TMC::GetChunkTypeName(S2::ChunkType type)
{
	using namespace S2;

	std::string output = "";

	switch (type)
	{
	case ChunkType::MdlGeo:
		output += "MdlGeo"; break;
	case ChunkType::TTDM:
		output += "TTDM"; break;
	case ChunkType::VtxLay:
		output += "VtxLay"; break;
	case ChunkType::IdxLay:
		output += "IdxLay"; break;
	case ChunkType::MtrCol:
		output += "MtrCol"; break;
	case ChunkType::MdlInfo:
		output += "MdlInfo"; break;
	case ChunkType::HieLay:
		output += "HieLay"; break;
	case ChunkType::LHeader:
		output += "LHeader"; break;
	case ChunkType::NodeLay:
		output += "NodeLay"; break;
	case ChunkType::GlblMtx:
		output += "GlblMtx"; break;
	case ChunkType::BnOfsMtx:
		output += "BnOfsMtx"; break;
	case ChunkType::cpf:
		output += "cpf"; break;
	case ChunkType::MCAPACK:
		output += "MCAPACK"; break;
	case ChunkType::RENPACK:
		output += "RENPACK"; break;
	case ChunkType::Padding:
		output += "Padding"; break;
	case ChunkType::IA:
		output += "IA"; break;
	case ChunkType::COLLIDE:
		output += "COLLIDE"; break;
	case ChunkType::MTRLCHNG:
		output += "MTRLCHNG"; break;
	case ChunkType::EFFCNF:
		output += "EFFCNF"; break;
	case ChunkType::ACSCLS:
		output += "ACSCLS"; break;
	case ChunkType::EPM1:
		output += "EPM1"; break;
	default:
		output += "unknown";
	}

	return output;
}
