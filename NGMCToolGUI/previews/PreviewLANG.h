#pragma once

#include "BasePreview.h"

#include "loaders/LoaderLANG.h"

namespace NGMC
{
	class PreviewLANG final : public BasePreview
	{
	public:
		PreviewLANG(File& file);

		void OnRender() override;

	private:
		bool Prepare() override;

		void OnRenderChunkHeader(Chunk::Header& header);

		void OnRenderLANGHeader(LANG::Node& node);

		void OnRenderCTGPACKHeader(LANG::CTGPACKNode& node);
		
		void OnRenderSTRPACKHeader(LANG::STRPACKNode& node);

		LANG::Node m_Root;

		std::vector<std::pair<LANG::CTGPACKNode, std::vector<std::pair<LANG::STRPACKNode, std::vector<std::string>>>>> m_Content;
	};
}