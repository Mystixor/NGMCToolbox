#pragma once

#include "BasePreview.h"

#include "loaders/LoaderTMC.h"

namespace NGMC
{
    class PreviewTMC final : public BasePreview
    {
    public:
        PreviewTMC(File& file);

        void OnRender() override;

    private:
        bool Prepare() override;

        void OnRenderTMCChunkHeader(TMC::ChunkHeader& header);

        TMC::Header m_Header;

        std::string m_TMCIdentifier;

        std::vector<float> m_TMC_U15;

        std::vector<TMC::S1::ChunkType> m_TMC_S1_ChunkIDs;
        std::vector<TMC::S2::ChunkType> m_TMC_S2_ChunkIDs;

        std::vector<int32_t> m_TMC_ChunkOffsets;
    };
}