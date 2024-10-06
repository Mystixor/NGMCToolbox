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

        TMC::ChunkHeader m_TMCHeader;
    };
}