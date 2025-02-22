#include "stdafx.h"

#include "PreviewLANG.h"

namespace NGMC
{
	PreviewLANG::PreviewLANG(File& file)
		: BasePreview(file)
	{
	}

	void PreviewLANG::OnRender()
	{
		ImGui::PushID(this);
		{
			bool isBigEndian = m_Root.header.version.v[0] == 255;

			ImGui::Dummy({});

			ImGui::Text(std::format("This file is encoded as {}-endian.", isBigEndian ? "big" : "little").c_str());
			
			ImGui::Dummy({});

			ImGui::Text(std::format("LANG \"{}\"", m_Root.name).c_str());

			if (ImGui::BeginTable("tablePreviewerLANG", 1, Preview::tableFlags))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				if (ImGui::TreeNodeEx("Header", ImGuiTreeNodeFlags_SpanAvailWidth))
				{
					OnRenderLANGHeader(m_Root);

					ImGui::TreePop();
				}

				int32_t rootChildCount = SwapBytesIf((unsigned long)m_Root.header.childCount, isBigEndian);
				for (unsigned int i = 0; i < rootChildCount; i++)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					if (ImGui::TreeNodeEx(std::format("CTGPACK #{} \"{}\"", i, m_Content[i].first.name).c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
					{
						if (ImGui::BeginTable("tablePreviewerLANGCTGPACK", 1, Preview::tableFlags))
						{
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);

							if (ImGui::TreeNodeEx("Header", ImGuiTreeNodeFlags_SpanAvailWidth))
							{
								OnRenderCTGPACKHeader(m_Content[i].first);

								ImGui::TreePop();
							}

							int32_t ctgpackChildCount = SwapBytesIf((unsigned long)m_Content[i].first.header.childCount, isBigEndian);
							for (unsigned int j = 0; j < ctgpackChildCount; j++)
							{
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0);

								if (ImGui::TreeNodeEx(std::format("STRPACK #{} \"{}\"", j, m_Content[i].second[j].first.name).c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
								{
									if (ImGui::BeginTable("tablePreviewerLANGCTGPACKSTRPACK", 1, Preview::tableFlags))
									{
										ImGui::TableNextRow();
										ImGui::TableSetColumnIndex(0);

										if (ImGui::TreeNodeEx("Header", ImGuiTreeNodeFlags_SpanAvailWidth))
										{
											OnRenderSTRPACKHeader(m_Content[i].second[j].first);

											ImGui::TreePop();
										}

										int32_t strpackChildCount = SwapBytesIf((unsigned long)m_Content[i].second[j].first.header.childCount, isBigEndian);
										for (unsigned int k = 0; k < strpackChildCount; k++)
										{
											ImGui::TableNextRow();

											ImGui::TableSetColumnIndex(0); ImGui::Text(m_Content[i].second[j].second[k].c_str());
										}

										ImGui::EndTable();
									}

									ImGui::TreePop();
								}
							}

							ImGui::EndTable();
						}

						ImGui::TreePop();
					}
				}

				ImGui::EndTable();
			}
		}
		ImGui::PopID();
	}

	bool PreviewLANG::Prepare()
	{
		m_IsSetup = true;

		LoaderLANG loader(m_File);

		if (!loader.GetRoot(m_Root))
		{
			m_IsSetup = false;
		}
		else
		{
			if (!loader.ParseLANG(m_Content))
				m_IsSetup = false;
		}

		return m_IsSetup;
	}

	void PreviewLANG::OnRenderChunkHeader(Chunk::Header& header)
	{
		uintptr_t headerAddr = (uintptr_t)&header;
		if (ImGui::BeginTable(std::vformat("header##{:08X}", std::make_format_args(headerAddr)).c_str(), 2, Preview::tableFlags))
		{
			bool isBigEndian = header.version.v[0] == 255;

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Signature");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("\"{}\"", header.magic).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Version");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}.{}.{}.{}", header.version.v[0], header.version.v[1], header.version.v[2], header.version.v[3]).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_0C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)header.dat_0C, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("chunkSize");
			ImGui::TableSetColumnIndex(1); ImGui::Text(GetPrettySize(SwapBytesIf((unsigned long)header.chunkSize, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("childCount");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)header.childCount, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_18");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)header.dat_18, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_1C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)header.dat_1C, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("childOffsetsOffset");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)header.childOffsetsOffset, isBigEndian)).c_str());
			
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("extraDataOffset");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)header.extraDataOffset, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_28");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)header.dat_28, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_2C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)header.dat_2C, isBigEndian)).c_str());

			ImGui::EndTable();
		}
	}

	void PreviewLANG::OnRenderLANGHeader(LANG::Node& node)
	{
		if (ImGui::BeginTable("tablePreviewerLANG2", 2, Preview::tableFlags))
		{
			bool isBigEndian = node.header.version.v[0] == 255;

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Chunk Header");
			ImGui::TableSetColumnIndex(1); OnRenderChunkHeader(node.header);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Identifier");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("\"{}\"", node.name).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_40");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_40, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_44");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_44, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_48");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_48, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_4C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_4C, isBigEndian)).c_str());

			ImGui::EndTable();
		}
	}

	void PreviewLANG::OnRenderCTGPACKHeader(LANG::CTGPACKNode& node)
	{
		if (ImGui::BeginTable("tablePreviewerLANGCTGPACK2", 2, Preview::tableFlags))
		{
			bool isBigEndian = node.header.version.v[0] == 255;

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Header");
			ImGui::TableSetColumnIndex(1); OnRenderChunkHeader(node.header);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Identifier");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("\"{}\"", node.name).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_40");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_40, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_44");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_44, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_48");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_48, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_4C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_4C, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_50");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_50, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_54");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_54, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_58");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_58, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_5C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_5C, isBigEndian)).c_str());

			ImGui::EndTable();
		}
	}

	void PreviewLANG::OnRenderSTRPACKHeader(LANG::STRPACKNode& node)
	{
		if (ImGui::BeginTable("tablePreviewerLANGCTGPACKSTRPACK2", 2, Preview::tableFlags))
		{
			bool isBigEndian = node.header.version.v[0] == 255;

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Header");
			ImGui::TableSetColumnIndex(1); OnRenderChunkHeader(node.header);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Identifier");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("\"{}\"", node.name).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_40");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_40, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_44");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_44, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_48");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_48, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_4C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_4C, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_50");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_50, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_54");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_54, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_58");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_58, isBigEndian)).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_5C");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", SwapBytesIf((unsigned long)node.dat_5C, isBigEndian)).c_str());

			ImGui::EndTable();
		}
	}
}