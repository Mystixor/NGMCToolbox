#pragma once

#include "BasePreview.h"

#include "loaders/LoaderDatabin.h"

namespace NGMC
{
    class PreviewDatabinItem final : public BasePreview
    {
	public:
		PreviewDatabinItem(File& file);

		void OnRender() override;

	private:
		bool Prepare() override;

		//	The header of a compressed databin item from Ninja Gaiden Sigma 1.
		Databin::S1::ItemHeader m_DatabinItemHeaderS1;
		
		//	The header of a compressed databin item from Ninja Gaiden Sigma 2.
		Databin::S2::ItemHeader m_DatabinItemHeaderS2;
	};
}