#include "stdafx.h"

#include "BasePreview.h"

namespace NGMC
{
	BasePreview::BasePreview(File& file)
		: m_File(file), m_IsSetup(false)
	{
	}

	bool BasePreview::IsSetup() const
	{
		return m_IsSetup;
	}

	bool BasePreview::Prepare()
	{
		return m_IsSetup;
	}
}