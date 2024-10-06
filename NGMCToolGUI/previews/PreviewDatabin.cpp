#include "stdafx.h"

#include "PreviewDatabin.h"

namespace NGMC
{
	PreviewDatabin::PreviewDatabin(File& file)
		: BasePreview(file)
	{
	}

	void PreviewDatabin::OnRender()
	{
		if (ImGui::BeginTable("tablePreviewerDatabin", 2, Preview::tableFlags))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_00");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinHeader.dat_00).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_04");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinHeader.dat_04).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_08");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinHeader.dat_08).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_0C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinHeader.dat_0C).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("headerSize");
			ImGui::TableSetColumnIndex(1); ImGui::Text(GetPrettySize(m_DatabinHeader.headerSize).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("fileDataOffset");
			ImGui::TableSetColumnIndex(1); ImGui::Text(GetPrettySize(m_DatabinHeader.fileDataOffset).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_18");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinHeader.dat_18).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_1C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinHeader.dat_1C).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("fileCount");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinHeader.fileCount).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("fileIndicesOffset");
			ImGui::TableSetColumnIndex(1); ImGui::Text(GetPrettySize(m_DatabinHeader.fileIndicesOffset).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("fileCount1");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinHeader.fileCount1).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_2C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinHeader.dat_2C).c_str());

			ImGui::EndTable();
		}
	}

	bool PreviewDatabin::Prepare()
	{
		FileType type = m_File.GetType();
		if ((type.GetGame() == SIGMA_1 && type.GetId() == Databin::S1::databin) ||
			(type.GetGame() == SIGMA_2 && type.GetId() == Databin::S2::databin))
		{
			LoaderDatabin loader = LoaderDatabin(type.GetGame(), m_File);

			m_IsSetup = loader.LoadHeader(m_DatabinHeader);
		}

		return m_IsSetup;
	}
}