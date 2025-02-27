#include "stdafx.h"

#include "PreviewDatabin.h"

namespace NGMC
{
	PreviewDatabin::PreviewDatabin(File& file)
		: BasePreview(file),
		m_DatabinHeader()
	{
	}

	void PreviewDatabin::OnRender()
	{
		if (ImGui::BeginTable("tablePreviewerDatabin", 2, Preview::tableFlags))
		{
			ROW_VALUE("dat_00", m_DatabinHeader.dat_00);
			ROW_VALUE("dat_04", m_DatabinHeader.dat_04);
			ROW_VALUE("dat_08", m_DatabinHeader.dat_08);
			ROW_VALUE("dat_0C", m_DatabinHeader.dat_0C);
			ROW_SIZE("headerSize", m_DatabinHeader.headerSize);
			ROW_SIZE("fileDataOffset", m_DatabinHeader.fileDataOffset);
			ROW_VALUE("dat_18", m_DatabinHeader.dat_18);
			ROW_VALUE("dat_1C", m_DatabinHeader.dat_1C);
			ROW_VALUE("fileCount", m_DatabinHeader.fileCount);
			ROW_VALUE("fileIndicesOffset", m_DatabinHeader.fileIndicesOffset);
			ROW_VALUE("fileCount1", m_DatabinHeader.fileCount1);
			ROW_VALUE("dat_2C", m_DatabinHeader.dat_2C);

			ImGui::EndTable();
		}
	}

	bool PreviewDatabin::Prepare()
	{
		FileType type = m_File.GetType();
		if ((type.GetGame() == SIGMA_1 && type.GetId() == Databin::S1::databin) ||
			(type.GetGame() == SIGMA_2 && type.GetId() == Databin::S2::databin) ||
			(type.GetGame() == RE_3 && type.GetId() == Databin::RE::databin))
		{
			LoaderDatabin loader = LoaderDatabin(type.GetGame(), m_File);

			m_IsSetup = loader.LoadHeader(m_DatabinHeader);
		}

		return m_IsSetup;
	}
}
