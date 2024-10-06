#pragma once

#include "BasePreviewImage.h"

#include "loaders/LoaderDDS.h"

namespace NGMC
{
    class PreviewDDS final : public BasePreviewImage
    {
    public:
        PreviewDDS(File& file);

        void OnRender() override;

    private:
        bool Prepare() override;

        //	The header of a DDS file.
        DDS_HEADER m_DDS_HEADER;

        //	The mip level of a DDS texture which is being currently previewed.
        unsigned int m_PreviewDDSMipMapIdx;
    };
}