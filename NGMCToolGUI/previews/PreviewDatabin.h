#pragma once

#include "BasePreview.h"

#include "loaders/LoaderDatabin.h"

namespace NGMC
{
    class PreviewDatabin final : public BasePreview
    {
    public:
        PreviewDatabin(File& file);

        void OnRender() override;

    private:
        bool Prepare() override;

		//	The header of a databin file.
		Databin::Header m_DatabinHeader;
    };
}