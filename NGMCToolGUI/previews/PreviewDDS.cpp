#include "stdafx.h"

#include "PreviewDDS.h"

namespace NGMC
{
	PreviewDDS::PreviewDDS(File& file)
		: BasePreviewImage(file),
		m_DDS_HEADER(DDS_HEADER()), m_PreviewDDSMipMapIdx(0U)
	{
	}

	void PreviewDDS::OnRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
		if (ImGui::BeginTable("tablePreviewerDDSImage", 1, Preview::tableFlags, { (float)m_ImageDisplayWidth + 1, (float)m_ImageDisplayHeight + 1 }))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImVec2 cursorPos = ImGui::GetCursorPos();
			ImGui::SetCursorPos({ cursorPos.x + 0, cursorPos.y + 1 });
			ImGui::Image(m_Image, ImVec2(m_ImageDisplayWidth, m_ImageDisplayHeight));
			ImGui::EndTable();
		}
		ImGui::PopStyleVar();

		ImGui::PushItemWidth(100.0f);
		if (ImGui::InputInt("Mip map level", (int*)&m_PreviewDDSMipMapIdx))
		{
			m_PreviewDDSMipMapIdx = max(0, min((int)m_PreviewDDSMipMapIdx, (int32_t)m_DDS_HEADER.dwMipMapCount - 1));
			FreeTexture();
			Prepare();
		}
		ImGui::PopItemWidth();

		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		ImGui::Text("DDS_HEADER");

		if (ImGui::BeginTable("tablePreviewerDDS_HEADER", 2, Preview::tableFlags))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dwSize");
			ImGui::TableSetColumnIndex(1); ImGui::Text(GetPrettySize(m_DDS_HEADER.dwSize).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dwFlags");
			std::string flagsStr =
				std::string((m_DDS_HEADER.dwFlags & DDSD_CAPS ? "DDSD_CAPS | " : "")) +
				(m_DDS_HEADER.dwFlags & DDSD_HEIGHT ? "DDSD_HEIGHT | " : "") +
				(m_DDS_HEADER.dwFlags & DDSD_WIDTH ? "DDSD_WIDTH | " : "") +
				(m_DDS_HEADER.dwFlags & DDSD_PITCH ? "DDSD_PITCH | " : "") +
				(m_DDS_HEADER.dwFlags & DDSD_PIXELFORMAT ? "DDSD_PIXELFORMAT | " : "") +
				(m_DDS_HEADER.dwFlags & DDSD_MIPMAPCOUNT ? "DDSD_MIPMAPCOUNT | " : "") +
				(m_DDS_HEADER.dwFlags & DDSD_LINEARSIZE ? "DDSD_LINEARSIZE | " : "") +
				(m_DDS_HEADER.dwFlags & DDSD_DEPTH ? "DDSD_DEPTH | " : "");
			ImGui::TableSetColumnIndex(1); ImGui::Text((std::format("0x{:08X}", m_DDS_HEADER.dwFlags) + (m_DDS_HEADER.dwFlags ? std::format(" ({})", flagsStr.substr(0, flagsStr.length() ? flagsStr.length() - 3 : 0)) : "")).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dwHeight");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DDS_HEADER.dwHeight).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dwWidth");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DDS_HEADER.dwWidth).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dwPitchOrLinearSize");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DDS_HEADER.dwPitchOrLinearSize).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dwDepth");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DDS_HEADER.dwDepth).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dwMipMapCount");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DDS_HEADER.dwMipMapCount).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dwReserved1");
			ImGui::TableSetColumnIndex(1); ImGui::Text((std::string("\"") + (char*)m_DDS_HEADER.dwReserved1 + "\"").c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("ddspf");
			ImGui::TableSetColumnIndex(1);

			if (ImGui::BeginTable("tablePreviewerDDS_HEADERddspf", 2, Preview::tableFlags))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("dwSize");
				ImGui::TableSetColumnIndex(1); ImGui::Text(GetPrettySize(m_DDS_HEADER.ddspf.dwSize).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("dwFlags");
				std::string ddspfFlagsStr =
					std::string((m_DDS_HEADER.ddspf.dwFlags & DDPF_ALPHAPIXELS ? "DDPF_ALPHAPIXELS | " : "")) +
					(m_DDS_HEADER.ddspf.dwFlags & DDPF_ALPHA ? "DDPF_ALPHA | " : "") +
					(m_DDS_HEADER.ddspf.dwFlags & DDPF_FOURCC ? "DDPF_FOURCC | " : "") +
					(m_DDS_HEADER.ddspf.dwFlags & DDPF_RGB ? "DDPF_RGB | " : "") +
					(m_DDS_HEADER.ddspf.dwFlags & DDPF_YUV ? "DDPF_YUV | " : "") +
					(m_DDS_HEADER.ddspf.dwFlags & DDPF_LUMINANCE ? "DDPF_LUMINANCE | " : "");
				ImGui::TableSetColumnIndex(1); ImGui::Text((std::format("0x{:08X}", m_DDS_HEADER.ddspf.dwFlags) + (m_DDS_HEADER.ddspf.dwFlags ? std::format(" ({})", ddspfFlagsStr.substr(0, ddspfFlagsStr.length() ? ddspfFlagsStr.length() - 3 : 0)) : "")).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("dwFourCC");
				ImGui::TableSetColumnIndex(1); ImGui::Text((std::string("\"") + (char*)&m_DDS_HEADER.ddspf.dwFourCC + "\"").c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("dwRGBBitCount");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DDS_HEADER.ddspf.dwRGBBitCount).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("dwRBitMask");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", m_DDS_HEADER.ddspf.dwRBitMask).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("dwGBitMask");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", m_DDS_HEADER.ddspf.dwGBitMask).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("dwBBitMask");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", m_DDS_HEADER.ddspf.dwBBitMask).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("dwABitMask");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", m_DDS_HEADER.ddspf.dwABitMask).c_str());

				ImGui::EndTable();
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dwCaps");
			std::string capsStr =
				std::string((m_DDS_HEADER.dwCaps & DDSCAPS_COMPLEX ? "DDSCAPS_COMPLEX | " : "")) +
				(m_DDS_HEADER.dwCaps & DDSCAPS_MIPMAP ? "DDSCAPS_MIPMAP | " : "") +
				(m_DDS_HEADER.dwCaps & DDSCAPS_TEXTURE ? "DDSCAPS_TEXTURE | " : "");
			ImGui::TableSetColumnIndex(1); ImGui::Text((std::format("0x{:08X}", m_DDS_HEADER.dwCaps) + (m_DDS_HEADER.dwCaps ? std::format(" ({})", capsStr.substr(0, capsStr.length() ? capsStr.length() - 3 : 0)) : "")).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dwCaps2");
			std::string caps2Str =
				std::string((m_DDS_HEADER.dwCaps2 & DDSCAPS2_CUBEMAP ? "DDSCAPS2_CUBEMAP | " : "")) +
				(m_DDS_HEADER.dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEX ? "DDSCAPS2_CUBEMAP_POSITIVEX | " : "") +
				(m_DDS_HEADER.dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEX ? "DDSCAPS2_CUBEMAP_NEGATIVEX | " : "") +
				(m_DDS_HEADER.dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEY ? "DDSCAPS2_CUBEMAP_POSITIVEY | " : "") +
				(m_DDS_HEADER.dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEY ? "DDSCAPS2_CUBEMAP_NEGATIVEY | " : "") +
				(m_DDS_HEADER.dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEZ ? "DDSCAPS2_CUBEMAP_POSITIVEZ | " : "") +
				(m_DDS_HEADER.dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEZ ? "DDSCAPS2_CUBEMAP_NEGATIVEZ | " : "") +
				(m_DDS_HEADER.dwCaps2 & DDSCAPS2_VOLUME ? "DDSCAPS2_VOLUME | " : "");
			ImGui::TableSetColumnIndex(1); ImGui::Text((std::format("0x{:08X}", m_DDS_HEADER.dwCaps2) + (m_DDS_HEADER.dwCaps2 ? std::format(" ({})", caps2Str.substr(0, caps2Str.length() ? caps2Str.length() - 3 : 0)) : "")).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dwCaps3");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", m_DDS_HEADER.dwCaps3).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dwCaps4");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", m_DDS_HEADER.dwCaps4).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dwReserved2");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DDS_HEADER.dwReserved2).c_str());

			ImGui::EndTable();
		}
	}

	bool PreviewDDS::Prepare()
	{
		LoaderDDS loader(&m_File);

		if (loader.GetHeader(m_DDS_HEADER))
		{
			if (m_PreviewDDSMipMapIdx < m_DDS_HEADER.dwMipMapCount)
			{
				DDSFormat format = loader.GetPixelFormat();

				if (format != DDSFormat::unsupported)
				{
					MemoryBuffer imageDataBuffer;
					if (loader.GetMipData(imageDataBuffer, m_PreviewDDSMipMapIdx))
					{
						m_ImageDisplayWidth = m_DDS_HEADER.dwWidth;
						m_ImageDisplayHeight = m_DDS_HEADER.dwHeight;

						unsigned int denom = std::pow(2, m_PreviewDDSMipMapIdx);
						UploadTexture(
							(unsigned char*)imageDataBuffer.GetBaseAddress(),
							m_DDS_HEADER.dwWidth / denom,
							m_DDS_HEADER.dwHeight / denom,
							format
						);

						m_IsSetup = true;
					}
				}
			}
		}

		return m_IsSetup;
	}
}