#include "stdafx.h"

#include "PreviewTMC.h"

namespace NGMC
{
	using namespace TMC;

	PreviewTMC::PreviewTMC(File& file)
		: BasePreview(file),
		m_Header(Header()), m_TMCIdentifier(""),
		m_TMC_U15(12),	//	12 is minimum length of U15 across all supported games
		m_TMC_S1_ChunkIDs(0), m_TMC_S2_ChunkIDs(0),
		m_TMC_ChunkOffsets(0)
	{
	}

	void PreviewTMC::OnRender()
	{
		ImGui::PushID(this);
		{
			ImGui::Dummy({});

			if (ImGui::BeginTable("tablePreviewerTMC", 2, Preview::tableFlags))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Header");
				ImGui::TableSetColumnIndex(1); OnRenderTMCChunkHeader(m_Header);

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Identifier");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("\"{}\"", m_Header.identifier).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("U15");
				ImGui::TableSetColumnIndex(1);

				if (ImGui::BeginTable("tablePreviewerTMCU15", 1, Preview::tableFlags))
				{
					for (unsigned int i = 0; i < m_TMC_U15.size(); i++)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text(std::format("{}", m_TMC_U15[i]).c_str());
					}

					ImGui::EndTable();
				}
				
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Chunks");
				ImGui::TableSetColumnIndex(1);

				if (ImGui::BeginTable("tablePreviewerTMCChunkIDs", 3, Preview::tableFlags))
				{
					ImGui::TableSetupColumn("ID");
					ImGui::TableSetupColumn("Name");
					ImGui::TableSetupColumn("Offset");
					ImGui::TableHeadersRow();

					switch (m_Header.version)
					{
					case vSIGMA1:
						for (unsigned int i = 0; i < m_TMC_S1_ChunkIDs.size(); i++)
						{
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0); ImGui::Text(std::format("0x{:08X}", (uint32_t)m_TMC_S1_ChunkIDs[i]).c_str());
							ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", GetChunkTypeName(m_TMC_S1_ChunkIDs[i])).c_str());
							ImGui::TableSetColumnIndex(2); ImGui::Text(GetPrettySize(m_TMC_ChunkOffsets[i]).c_str());
						}
						break;
					case vSIGMA2:
						for (unsigned int i = 0; i < m_TMC_S2_ChunkIDs.size(); i++)
						{
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0); ImGui::Text(std::format("0x{:08X}", (uint32_t)m_TMC_S2_ChunkIDs[i]).c_str());
							ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", GetChunkTypeName(m_TMC_S2_ChunkIDs[i])).c_str());
							ImGui::TableSetColumnIndex(2); ImGui::Text(GetPrettySize(m_TMC_ChunkOffsets[i]).c_str());
						}
						break;
					}

					ImGui::EndTable();
				}

				ImGui::EndTable();
			}
		}
		ImGui::PopID();
	}

	bool PreviewTMC::Prepare()
	{
		LoaderTMC loader(m_File);

		m_IsSetup = false;

		if (loader.GetTMCHeader(m_Header))
		{
			loader.GetTMCU15(m_TMC_U15);

			switch (m_Header.version)
			{
			case vSIGMA1:
				loader.GetTMCChunkIDs(m_TMC_S1_ChunkIDs); break;
			case vSIGMA2:
				loader.GetTMCChunkIDs(m_TMC_S2_ChunkIDs); break;
			}

			loader.GetTMCChunkOffsets(m_TMC_ChunkOffsets);
		}
		else
		{
			return false;
		}

		m_IsSetup = true;

		return m_IsSetup;
	}

	void PreviewTMC::OnRenderTMCChunkHeader(ChunkHeader& header)
	{
		uintptr_t headerAddr = (uintptr_t)&header;
		if (ImGui::BeginTable(std::vformat("header##{:08X}", std::make_format_args(headerAddr)).c_str(), 2, Preview::tableFlags))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Signature");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("\"{}\"", header.magic).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Version");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}.{}.{}.{}", header.version.v[0], header.version.v[1], header.version.v[2], header.version.v[3]).c_str());

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