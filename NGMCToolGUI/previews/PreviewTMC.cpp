#include "stdafx.h"

#include "PreviewTMC.h"

namespace NGMC
{
	PreviewTMC::PreviewTMC(File& file)
		: BasePreview(file),
		m_TMCHeader(TMC::ChunkHeader())
	{
	}

	void PreviewTMC::OnRender()
	{
		ImGui::Dummy({});

		ImGui::Text("TMC Header:");
		OnRenderTMCChunkHeader(m_TMCHeader);
	}

	bool PreviewTMC::Prepare()
	{
		LoaderTMC loader(m_File);

		{
			m_IsSetup = true;

			if (!loader.GetTMCHeader(m_TMCHeader))
				m_IsSetup = false;
		}

		return m_IsSetup;
	}

	void PreviewTMC::OnRenderTMCChunkHeader(TMC::ChunkHeader& header)
	{
		uintptr_t headerAddr = (uintptr_t)&header;
		if (ImGui::BeginTable(std::vformat("tablePreviewerTMC##{:08X}", std::make_format_args(headerAddr)).c_str(), 2, Preview::tableFlags))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Signature");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("\"{}\"", header.magic).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Version");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}.{}.{}.{}", m_TMCHeader.version.v[0], m_TMCHeader.version.v[1], m_TMCHeader.version.v[2], m_TMCHeader.version.v[3]).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("U01");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", header.U01).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("chunkSize");
			ImGui::TableSetColumnIndex(1); ImGui::Text(GetPrettySize(header.chunkSize).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("childCount");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", header.childCount).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("U03");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", header.U03).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("U04");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", header.U04).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("U05");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", header.U05).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("U06");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", header.U06).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("U07");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", header.U07).c_str());

			ImGui::EndTable();
		}
	}
}