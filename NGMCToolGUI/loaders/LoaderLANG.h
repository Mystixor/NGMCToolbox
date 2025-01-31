#pragma once

#include "formats/Node.h"

#include "DataReader.h"

namespace NGMC
{
	namespace LANG
	{
		struct STRPACKNode : Chunk::SizedChildrenNode<char16_t>
		{
			char name[16];
			int32_t dat_40;
			int32_t dat_44;
			int32_t dat_48;
			int32_t dat_4C;
			int32_t dat_50;
			int32_t dat_54;
			int32_t dat_58;
			int32_t dat_5C;
		};

		struct CTGPACKNode : Chunk::SizedChildrenNode<STRPACKNode>
		{
			char name[16];
			int32_t dat_40;
			int32_t dat_44;
			int32_t dat_48;
			int32_t dat_4C;
			int32_t dat_50;
			int32_t dat_54;
			int32_t dat_58;
			int32_t dat_5C;
		};

		struct Node : Chunk::SizedChildrenNode<CTGPACKNode>
		{
			char name[16];
			int32_t dat_40;
			int32_t dat_44;
			int32_t dat_48;
			int32_t dat_4C;
		};
	}

	bool ImportFileFromJSON(const wchar_t* filePath);

	class LoaderLANG
	{
	public:
		LoaderLANG(const wchar_t* filePath);

		LoaderLANG(MemoryBuffer& memBuf);

		LoaderLANG(File& file);

		~LoaderLANG();

		bool GetRoot(LANG::Node& outRoot);

		bool ParseLANG(std::vector<std::pair<LANG::CTGPACKNode, std::vector<std::pair<LANG::STRPACKNode, std::vector<std::string>>>>>& outLANG);

	private:
		MemoryBuffer m_MemBuf;
	};
}