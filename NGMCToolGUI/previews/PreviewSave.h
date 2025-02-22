#pragma once

#include "BasePreview.h"

#include "loaders/LoaderSave.h"

namespace NGMC
{
    /*class SubPreviewSave
    {
    public:
        SubPreviewSave(File& file);

        virtual ~SubPreviewSave() = default;

        bool IsSetup() const;

        virtual void OnRender() = 0;

        virtual bool Prepare();

    private:
        File& m_File;

        bool m_IsSetup;
    };*/

    
    class PreviewSave final : public BasePreview
    {
    public:
        PreviewSave(File& file);

        ~PreviewSave() override;

        void OnRender() override;

    private:
        bool Prepare() override;

        SaveFile::Type m_SaveType;

        BasePreview* mp_SubPreview;
    };
}