#pragma once

#include "DataReader.h"

namespace NGMC
{
	namespace TMC
	{
		struct ChunkVersion {
			uint8_t v[4];
		};

		struct ChunkHeader {
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

		struct Chunk
		{
		};

		struct ChunkH : Chunk
		{
			ChunkHeader header;
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
				EPM1 = 0x45504D31,	// 'EPM1'
				EMCinfo = 0x00000EFF
			};

			struct TMC : ChunkH {
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
				float U15[12];
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

			struct TMC : ChunkH {
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
				float U15[36];
			};
		}
	}


	class LoaderTMC
	{
	public:
		LoaderTMC(const wchar_t* filePath);

		LoaderTMC(MemoryBuffer& memBuf);

		LoaderTMC(File& file);

		~LoaderTMC();

		unsigned int GetTMCChunkCount();

		bool GetTMCChunkType(TMC::S1::ChunkType outType, unsigned int index);
		bool GetTMCChunkType(TMC::S2::ChunkType outType, unsigned int index);

	private:
		DataReader m_Reader;
	};
}