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

		ImGui::TextUnformatted("DDS_HEADER");

		if (ImGui::BeginTable("tablePreviewerDDS_HEADER", 2, Preview::tableFlags))
		{
			ROW_SIZE("dwSize", m_DDS_HEADER.dwSize);

			std::string flagsStr =
				std::string((m_DDS_HEADER.dwFlags & DDSD_CAPS ? "DDSD_CAPS | " : "")) +
				(m_DDS_HEADER.dwFlags & DDSD_HEIGHT ? "DDSD_HEIGHT | " : "") +
				(m_DDS_HEADER.dwFlags & DDSD_WIDTH ? "DDSD_WIDTH | " : "") +
				(m_DDS_HEADER.dwFlags & DDSD_PITCH ? "DDSD_PITCH | " : "") +
				(m_DDS_HEADER.dwFlags & DDSD_PIXELFORMAT ? "DDSD_PIXELFORMAT | " : "") +
				(m_DDS_HEADER.dwFlags & DDSD_MIPMAPCOUNT ? "DDSD_MIPMAPCOUNT | " : "") +
				(m_DDS_HEADER.dwFlags & DDSD_LINEARSIZE ? "DDSD_LINEARSIZE | " : "") +
				(m_DDS_HEADER.dwFlags & DDSD_DEPTH ? "DDSD_DEPTH | " : "");
			ROW_STRING("dwFlags", std::format("0x{:08X}", m_DDS_HEADER.dwFlags) + (m_DDS_HEADER.dwFlags ? std::format(" ({})", flagsStr.substr(0, flagsStr.length() ? flagsStr.length() - 3 : 0)) : ""));

			ROW_VALUE("dwHeight", m_DDS_HEADER.dwHeight);
			ROW_VALUE("dwWidth", m_DDS_HEADER.dwWidth);
			ROW_VALUE("dwPitchOrLinearSize", m_DDS_HEADER.dwPitchOrLinearSize);
			ROW_VALUE("dwDepth", m_DDS_HEADER.dwDepth);
			ROW_VALUE("dwMipMapCount", m_DDS_HEADER.dwMipMapCount);
			ROW_STRING("dwReserved1", std::string("\"") + (char*)m_DDS_HEADER.dwReserved1 + "\"");

			ROW_BEGIN("ddspf");
			if (ImGui::BeginTable("tablePreviewerDDS_HEADERddspf", 2, Preview::tableFlags))
			{
				ROW_SIZE("dwSize", m_DDS_HEADER.ddspf.dwSize);

				std::string ddspfFlagsStr =
					std::string((m_DDS_HEADER.ddspf.dwFlags & DDPF_ALPHAPIXELS ? "DDPF_ALPHAPIXELS | " : "")) +
					(m_DDS_HEADER.ddspf.dwFlags & DDPF_ALPHA ? "DDPF_ALPHA | " : "") +
					(m_DDS_HEADER.ddspf.dwFlags & DDPF_FOURCC ? "DDPF_FOURCC | " : "") +
					(m_DDS_HEADER.ddspf.dwFlags & DDPF_RGB ? "DDPF_RGB | " : "") +
					(m_DDS_HEADER.ddspf.dwFlags & DDPF_YUV ? "DDPF_YUV | " : "") +
					(m_DDS_HEADER.ddspf.dwFlags & DDPF_LUMINANCE ? "DDPF_LUMINANCE | " : "");
				ROW_STRING("dwFlags", std::format("0x{:08X}", m_DDS_HEADER.ddspf.dwFlags) + (m_DDS_HEADER.ddspf.dwFlags ? std::format(" ({})", ddspfFlagsStr.substr(0, ddspfFlagsStr.length() ? ddspfFlagsStr.length() - 3 : 0)) : ""));

				ROW_STRING("dwFourCC", std::string("\"") + (char*)&m_DDS_HEADER.ddspf.dwFourCC + "\"");
				ROW_VALUE("dwRGBBitCount", m_DDS_HEADER.ddspf.dwRGBBitCount);
				ROW_VALUE("dwRBitMask", m_DDS_HEADER.ddspf.dwRBitMask);
				ROW_VALUE("dwGBitMask", m_DDS_HEADER.ddspf.dwGBitMask);
				ROW_VALUE("dwBBitMask", m_DDS_HEADER.ddspf.dwBBitMask);
				ROW_VALUE("dwABitMask", m_DDS_HEADER.ddspf.dwABitMask);

				ImGui::EndTable();
			}

			std::string capsStr =
				std::string((m_DDS_HEADER.dwCaps & DDSCAPS_COMPLEX ? "DDSCAPS_COMPLEX | " : "")) +
				(m_DDS_HEADER.dwCaps & DDSCAPS_MIPMAP ? "DDSCAPS_MIPMAP | " : "") +
				(m_DDS_HEADER.dwCaps & DDSCAPS_TEXTURE ? "DDSCAPS_TEXTURE | " : "");
			ROW_STRING("dwCaps", std::format("0x{:08X}", m_DDS_HEADER.dwCaps) + (m_DDS_HEADER.dwCaps ? std::format(" ({})", capsStr.substr(0, capsStr.length() ? capsStr.length() - 3 : 0)) : ""));

			std::string caps2Str =
				std::string((m_DDS_HEADER.dwCaps2 & DDSCAPS2_CUBEMAP ? "DDSCAPS2_CUBEMAP | " : "")) +
				(m_DDS_HEADER.dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEX ? "DDSCAPS2_CUBEMAP_POSITIVEX | " : "") +
				(m_DDS_HEADER.dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEX ? "DDSCAPS2_CUBEMAP_NEGATIVEX | " : "") +
				(m_DDS_HEADER.dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEY ? "DDSCAPS2_CUBEMAP_POSITIVEY | " : "") +
				(m_DDS_HEADER.dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEY ? "DDSCAPS2_CUBEMAP_NEGATIVEY | " : "") +
				(m_DDS_HEADER.dwCaps2 & DDSCAPS2_CUBEMAP_POSITIVEZ ? "DDSCAPS2_CUBEMAP_POSITIVEZ | " : "") +
				(m_DDS_HEADER.dwCaps2 & DDSCAPS2_CUBEMAP_NEGATIVEZ ? "DDSCAPS2_CUBEMAP_NEGATIVEZ | " : "") +
				(m_DDS_HEADER.dwCaps2 & DDSCAPS2_VOLUME ? "DDSCAPS2_VOLUME | " : "");
			ROW_STRING("dwCaps2", std::format("0x{:08X}", m_DDS_HEADER.dwCaps2) + (m_DDS_HEADER.dwCaps2 ? std::format(" ({})", caps2Str.substr(0, caps2Str.length() ? caps2Str.length() - 3 : 0)) : ""));

			ROW_VALUE("dwCaps3", m_DDS_HEADER.dwCaps3);
			ROW_VALUE("dwCaps4", m_DDS_HEADER.dwCaps4);
			ROW_VALUE("dwReserved2", m_DDS_HEADER.dwReserved2);

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
