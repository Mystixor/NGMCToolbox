#pragma once

#include "DataReader.h"

namespace NGMC
{
	namespace TMC
	{
		struct ChunkVersion
		{
			uint8_t v[4];

			ChunkVersion() : v({}) {}

			operator uint32_t() const { return *(uint32_t*)this; }
		};

		constexpr inline uint32_t vSIGMA1 = 0x00010000;
		constexpr inline uint32_t vSIGMA2 = 0x01010000;

		struct ChunkHeader
		{
			char magic[8];
			ChunkVersion version;
			int32_t U01;
			uint32_t chunkSize;
			int32_t childCount;
			int32_t U03;
			int32_t U04;
			int32_t dataOffset;
			int32_t U05;
			int32_t U06;
			int32_t U07;
		};

		//	TMC header common between Sigma 1 and 2.
		struct Header : ChunkHeader
		{
			uint16_t U07_0;
			uint16_t U07_1;
			uint32_t U08;
			uint32_t U09;
			uint32_t U10;
			uint32_t U11;
			uint32_t U12;
			uint32_t U13;
			uint32_t U14;
			char identifier[16];
		};

		namespace S1
		{
			enum ChunkType : uint32_t
			{
				MdlGeo = 0x80000001,
				TTG = 0x80000002,
				VtxLay = 0x80000003,
				IdxLay = 0x80000004,
				MtrCol = 0x80000005,
				MdlInfo = 0x80000006,
				HieLay = 0x80000010,
				LHeader = 0x80000020,
				NodeLay = 0x80000030,
				GlblMtx = 0x80000040,
				BnOfsMtx = 0x80000050,
				cpf = 0x80000060,
				MCAPACK = 0x80000070,
				RENPACK = 0x80000080,
				COLLIDE = 0x00000000,
				OBJ_TYPE_INFO = 0x00000001,
				MDL_TYPE_INFO = 0x00000002,
				AcsPack = 0x00000010,
				MPH_LYR = 0x00000011,
				OBJHIT = 0x00000012,
				EXTMCOL = 0x00000015,
				MISCDATA = 0x00000020,
				EPM1 = 0x45504D31,	// 'EPM1'
				EMCinfo = 0x00000EFF
			};
		}

		namespace S2
		{
			enum ChunkType : uint32_t
			{
				MdlGeo = 0x80000001,
				TTDM = 0x80000002,
				VtxLay = 0x80000003,
				IdxLay = 0x80000004,
				MtrCol = 0x80000005,
				MdlInfo = 0x80000006,
				HieLay = 0x80000010,
				LHeader = 0x80000020,
				NodeLay = 0x80000030,
				GlblMtx = 0x80000040,
				BnOfsMtx = 0x80000050,
				cpf = 0x80000060,
				MCAPACK = 0x80000070,
				RENPACK = 0x80000080,
				Padding = 0x00000002,	// ?
				IA = 0x00000003,	// ?
				COLLIDE = 0x00000004,
				MTRLCHNG = 0x00000005,
				EFFCNF = 0x00000006,	// sometimes COLLIDE...?
				ACSCLS = 0x0000000E,
				EPM1 = 0x45504D31	// 'EPM1'
			};
		}

		std::string GetChunkTypeName(S1::ChunkType type);

		std::string GetChunkTypeName(S2::ChunkType type);
	}


	class LoaderTMC
	{
	public:
		LoaderTMC(const wchar_t* filePath);

		LoaderTMC(MemoryBuffer& memBuf);

		LoaderTMC(File& file);

		~LoaderTMC();

		bool GetTMCHeader(TMC::Header& outHeader);

		bool GetTMCVersion(TMC::ChunkVersion& outVersion);

		bool GetTMCChildCount(uint32_t& outCount);

		bool GetTMCChildType(TMC::S1::ChunkType& outType, unsigned int index);
		bool GetTMCChildType(TMC::S2::ChunkType& outType, unsigned int index);

		bool GetTMCIdentifier(std::string& outIdentifier);

		bool GetTMCU15(std::vector<float>& outU15);

		bool GetTMCChunkIDs(std::vector<TMC::S1::ChunkType>& outIDs);
		bool GetTMCChunkIDs(std::vector<TMC::S2::ChunkType>& outIDs);

		bool GetTMCChunkOffsets(std::vector<int32_t>& outOffsets);

	private:
		DataReader m_Reader;
	};
}