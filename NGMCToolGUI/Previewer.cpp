#include "stdafx.h"

#include "Previewer.h"

namespace NGMC
{
	extern bool g_ReadColorMapAsLuminance;

	Previewer::Previewer()
		: m_File(nullptr), m_PrevType(FileType()), m_IsSetup(false),
		m_Image(nullptr), m_ImageInUse(false),
		m_ImageWidth(0), m_ImageHeight(0), m_ImageDisplayWidth(0), m_ImageDisplayHeight(0),
		m_DatabinHeader(DatabinHeader()),
		m_DatabinItemHeaderS1(S1::DatabinItemHeader()),
		m_DatabinItemHeaderS2(S2::DatabinItemHeader()),
		m_GT1GHeader(GT1GHeader()), m_GT1GTextures(0), m_PreviewGT1GTexIdx(0U), m_PreviewGT1GMipMapIdx(0U),
		m_DDS_HEADER(DDS_HEADER()), m_PreviewDDSMipMapIdx(0U)
	{
	}

	Previewer::~Previewer()
	{
	}

	void Previewer::SetFile(File* p_File)
	{
		if (m_File != p_File)
		{
			//	Prepare Preview content for new file
			FreeTexture();
			*this = Previewer();

			m_File = p_File;

			//Prepare();
		}
	}

	void Previewer::OnRender()
	{
		if (m_File != nullptr)
		{
			ImGui::Text("GENERAL");

			FileType type = m_File->GetType();

			if (type != m_PrevType)
			{
				Prepare();
				m_PrevType = type;
			}

			std::string typeName = type.GetTypeName();

			ImGuiTableFlags flags =
				ImGuiTableFlags_RowBg |
				ImGuiTableFlags_SizingFixedFit |
				ImGuiTableFlags_NoHostExtendX |
				ImGuiTableFlags_BordersInnerV |
				ImGuiTableFlags_BordersOuter;

			GAME game = m_File->GetType().GetGame();
			std::string gameName = "";
			switch (game)
			{
			case NON_GAME:
				gameName = "General";
				break;
			case SIGMA_1:
				gameName = "NGS1";
				break;
			case SIGMA_2:
				gameName = "NGS2";
				break;
			case RE_3:
				gameName = "NG3RE";
				break;
			}

			if (ImGui::BeginTable("tablePreviewerGeneral", 2, flags))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Name");
				ImGui::TableSetColumnIndex(1); ImGui::Text(m_File->GetName());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Type");
				ImGui::TableSetColumnIndex(1); ImGui::Text("%s > %s", gameName.c_str(), typeName.c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Is file in memory");
				ImGui::TableSetColumnIndex(1); ImGui::Text(m_File->IsFileInMemory() ? "true" : "false");

				if (!m_File->IsFileInMemory())
				{
					std::wstring filePath = m_File->GetFilePath();

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Path");
					ImGui::TableSetColumnIndex(1); ImGui::Text(std::string(filePath.begin(), filePath.end()).c_str());
				}
				
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Is file loaded");
				ImGui::TableSetColumnIndex(1); ImGui::Text(m_File->IsLoaded() ? "true" : "false");

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Size");
				ImGui::TableSetColumnIndex(1); ImGui::Text(GetPrettySize(m_File->GetSize()).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Index in Parent");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_File->GetIndexInParent()).c_str());
				
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Child count");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_File->GetChildCount()).c_str());

				ImGui::EndTable();
			}

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Text("FORMAT SPECIFIC (%s > %s)", gameName.c_str(), typeName.c_str());

			if (m_IsSetup)
			{
				switch (game)
				{
				case NON_GAME:
				{
					using namespace General;
					switch (type.GetId())
					{
					case FileTypeId::DDS:
						OnRenderDDS();
						break;
					}
					break;
				}
				case SIGMA_1:
				{
					using namespace S1;
					switch (type.GetId())
					{
					case FileTypeId::databin:
						OnRenderDatabin();
						break;
					case FileTypeId::databinItem:
						OnRenderDatabinItem();
						break;
					case FileTypeId::GT1G_07:
					case FileTypeId::GT1G_13:
						OnRenderGT1G();
						break;
					}
					break;
				}
				case SIGMA_2:
				{
					using namespace S2;
					switch (type.GetId())
					{
					case FileTypeId::databin:
						OnRenderDatabin();
						break;
					case FileTypeId::databinItem:
						OnRenderDatabinItem();
						break;
					}
					break;
				}
				case RE_3:
				{
					using namespace RE;
					switch (type.GetId())
					{
					case FileTypeId::databin:
						OnRenderDatabin();
						break;
					case FileTypeId::databinItem:
						OnRenderDatabinItem();
						break;
					}
					break;
				}
				}
			}
			else
			{
				if (type.IsUnknown())
					ImGui::Text("Format unknown. Right-click the file and set a type manually.");
				else
					ImGui::Text("This format is not yet supported for previewing.");
			}
		}
		else
		{
			ImGui::Text("Select an element in the content viewer to see a preview of its data here.");
		}
	}

	bool Previewer::Prepare()
	{
		if (m_File != nullptr)
		{
			FileType type = m_File->GetType();
			switch (type.GetGame())
			{
			case NON_GAME:
			{
				using namespace General;
				switch (type.GetId())
				{
				case FileTypeId::DDS:
				{
					m_IsSetup = PrepareDDS();
					break;
				}
				}
				break;
			}
			case SIGMA_1:
			{
				using namespace S1;
				switch (type.GetId())
				{
				case FileTypeId::databin:
				{
					m_IsSetup = PrepareDatabin();
					break;
				}
				case FileTypeId::databinItem:
				{
					m_IsSetup = PrepareDatabinItem();
					break;
				}
				case FileTypeId::GT1G_07:
				case FileTypeId::GT1G_13:
				{
					m_IsSetup = PrepareGT1G();
					break;
				}
				}
				break;
			}
			case SIGMA_2:
			{
				using namespace S2;
				switch (type.GetId())
				{
				case FileTypeId::databin:
				{
					m_IsSetup = PrepareDatabin();
					break;
				}
				case FileTypeId::databinItem:
				{
					m_IsSetup = PrepareDatabinItem();
					break;
				}
				}
				break;
			}
			}
		}

		return m_IsSetup;
	}

	bool Previewer::PrepareDatabin()
	{
		bool isSuccess = false;

		LoaderDatabin loader = LoaderDatabin(m_File->GetType().GetGame());

		if (m_File->IsFileInMemory())
		{
			isSuccess = loader.LoadHeader(m_DatabinHeader, m_File->GetMemoryBuffer());
		}
		else
		{
			loader.SetFilePath(m_File->GetFilePath());
			isSuccess = loader.LoadHeader(m_DatabinHeader);
		}

		return isSuccess;
	}

	bool Previewer::PrepareDatabinItem()
	{
		bool isSuccess = false;

		GAME game = m_File->GetType().GetGame();
		LoaderDatabin loader = LoaderDatabin(game);

		if (m_File->IsFileInMemory())
		{
			switch (game)
			{
			case SIGMA_1:
				isSuccess = loader.LoadItemHeader(m_DatabinItemHeaderS1, m_File->GetMemoryBuffer(), m_File->GetIndexInParent());
				break;
			case SIGMA_2:
				isSuccess = loader.LoadItemHeader(m_DatabinItemHeaderS2, m_File->GetMemoryBuffer(), m_File->GetIndexInParent());
				break;
			}
		}
		else
		{
			loader.SetFilePath(m_File->GetFilePath());
			switch (game)
			{
			case SIGMA_1:
				isSuccess = loader.LoadItemHeader(m_DatabinItemHeaderS1, m_File->GetIndexInParent());
				break;
			case SIGMA_2:
				isSuccess = loader.LoadItemHeader(m_DatabinItemHeaderS2, m_File->GetIndexInParent());
				break;
			}
		}

		return isSuccess;
	}

	bool Previewer::PrepareGT1G()
	{
		bool isSuccess = false;

		LoaderGT1G loader = LoaderGT1G(m_File);

		{
			loader.GetHeader(m_GT1GHeader);

			m_GT1GTextures.reserve(m_GT1GTextures.size() + m_GT1GHeader.textureCount);

			unsigned int* mipMapCounts = new unsigned int[m_GT1GHeader.textureCount];
			PixelFormat* formats = new PixelFormat[m_GT1GHeader.textureCount];
			unsigned int* widths = new unsigned int[m_GT1GHeader.textureCount];
			unsigned int* heights = new unsigned int[m_GT1GHeader.textureCount];
			uint32_t* flags = new uint32_t[m_GT1GHeader.textureCount];
			uint32_t* extraFlags0s = new uint32_t[m_GT1GHeader.textureCount];
			uint32_t* extraFlags1s = new uint32_t[m_GT1GHeader.textureCount];
			uint32_t* extraFlags2s = new uint32_t[m_GT1GHeader.textureCount];

			loader.GetTexturesInfo(m_GT1GHeader, mipMapCounts, formats, widths, heights, flags, extraFlags0s, extraFlags1s, extraFlags2s);

			for (unsigned int i = 0; i < m_GT1GHeader.textureCount; i++)
			{
				//unsigned int mipMapCount = loader.GetTextureMipMapCount(i);
				//PixelFormat format = loader.GetTexturePixelFormat(i);
				//uint32_t flags = loader.GetTextureFlags(i);
				//uint32_t extraFlags0 = 0;
				//uint32_t extraFlags1 = 0;
				//uint32_t extraFlags2 = 0;
				//if ((flags >> 24) == 0x10)
				//{
				//	loader.GetTextureExtraFlags(i, &extraFlags0, &extraFlags1, &extraFlags2);
				//}
				//m_GT1GTextures.emplace_back(mipMapCount, format, flags, extraFlags0, extraFlags1, extraFlags2, std::vector<GT1GMipMap>(mipMapCount));
				m_GT1GTextures.emplace_back(mipMapCounts[i], formats[i], flags[i], extraFlags0s[i], extraFlags1s[i], extraFlags2s[i], std::vector<GT1GMipMap>(mipMapCounts[i]));

				unsigned int denom = 1;
				for (unsigned int j = 0; j < mipMapCounts[i]; j++)
				{
					//m_GT1GTextures[i].MipMaps[j].Width = loader.GetTextureMipMapWidth(i, j);
					//m_GT1GTextures[i].MipMaps[j].Height = loader.GetTextureMipMapHeight(i, j);
					m_GT1GTextures[i].MipMaps[j].Width = widths[i] / denom;
					m_GT1GTextures[i].MipMaps[j].Height = heights[i] / denom;
					denom *= 2;
				}
			}

			delete[] extraFlags2s;
			delete[] extraFlags1s;
			delete[] extraFlags0s;
			delete[] flags;
			delete[] heights;
			delete[] widths;
			delete[] formats;
			delete[] mipMapCounts;

			isSuccess = PrepareGT1GTexture(m_PreviewGT1GTexIdx);
		}

		return isSuccess;
	}

	bool Previewer::PrepareGT1GTexture(unsigned int textureIndex)
	{
		bool isSuccess = false;

		LoaderGT1G loader = LoaderGT1G(m_File);

		if (m_PreviewGT1GTexIdx < loader.GetTextureCount())
		{
			unsigned int textureIndex = m_PreviewGT1GTexIdx;
			unsigned int mipLevel = m_PreviewGT1GMipMapIdx;

			MemoryBuffer imageDataBuffer;
			if (loader.GetMipData(imageDataBuffer, textureIndex, mipLevel))
			{
				DDSFormat format = DDSFormat::unsupported;

				switch (m_GT1GTextures[textureIndex].Format)
				{
				case PixelFormat::RGBA8_BGRA_u8:
					format = DDSFormat::BGRA8;
					break;
				case PixelFormat::RGBA8_RGBA_u8:
					format = DDSFormat::RGBA8;
					break;
				case PixelFormat::CompressedRgbS3tcDxt1Ext_06:
				case PixelFormat::CompressedRgbS3tcDxt1Ext_59:
					format = DDSFormat::DXT1;
					break;
				case PixelFormat::CompressedRgbaS3tcDxt5Ext_08:
				case PixelFormat::CompressedRgbaS3tcDxt5Ext_5B:
					format = DDSFormat::DXT5;
					break;
				case PixelFormat::ColorMap_u8:
					if (g_ReadColorMapAsLuminance)
					{
						format = DDSFormat::LUMINANCE;
					}
					else
					{
						format = DDSFormat::BGRA8;
					}
					break;
				}

				if (format != DDSFormat::unsupported)
				{
					m_ImageDisplayWidth = m_GT1GTextures[textureIndex].MipMaps[0].Width;
					m_ImageDisplayHeight = m_GT1GTextures[textureIndex].MipMaps[0].Height;

					UploadTexture(
						(unsigned char*)imageDataBuffer.GetBaseAddress(),
						m_GT1GTextures[textureIndex].MipMaps[mipLevel].Width,
						m_GT1GTextures[textureIndex].MipMaps[mipLevel].Height,
						format
					);

					isSuccess = true;
				}
			}
		}

		return isSuccess;
	}

	bool Previewer::PrepareDDS()
	{
		bool isSuccess = false;

		LoaderDDS loader(m_File);

		if (loader.GetHeader(m_DDS_HEADER))
		{
			if (m_PreviewDDSMipMapIdx < m_DDS_HEADER.dwMipMapCount)
			{
				DDSFormat format = loader.GetPixelFormat();

				if (format != PixelFormat::unsupportedFormat)
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

						isSuccess = true;
					}
				}
			}
		}

		return isSuccess;
	}

	void Previewer::UploadTexture(unsigned char* imageData, int imageWidth, int imageHeight, DDSFormat pixelFormat)
	{
		// Create a OpenGL texture identifier
		GLuint imageTexture;
		glGenTextures(1, &imageTexture);
		glBindTexture(GL_TEXTURE_2D, imageTexture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//	linear when zoomed out for nicer visuals
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	//	nearest when zoomed in for pixel accuracy

		// Setup texture wrapping parameters so the texture does not repeat
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// Upload pixels into texture
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

		GLint internalFormat = GL_RGBA;
		GLenum format = GL_RGBA;
		GLenum type = GL_UNSIGNED_BYTE;

		switch (pixelFormat)
		{
		case DDSFormat::BGRA8:
			internalFormat = GL_RGBA;
			format = GL_BGRA;
			type = GL_UNSIGNED_BYTE;
			break;
		case DDSFormat::RGBA8:
			internalFormat = GL_RGBA;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;
		case DDSFormat::DXT1:
			internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			break;
		case DDSFormat::DXT5:
			internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		case DDSFormat::LUMINANCE:
			internalFormat = GL_LUMINANCE;
			format = GL_LUMINANCE;
			break;
		}

		switch (pixelFormat)
		{
		case DDSFormat::BGRA8:
		case DDSFormat::RGBA8:
		case DDSFormat::LUMINANCE:
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, imageWidth, imageHeight, 0, format, type, imageData);
			break;
		case DDSFormat::DXT1:
		{
			GLsizei imageSize = max(1, ((imageWidth + 3) / 4)) * max(1, ((imageHeight + 3) / 4)) * 8;
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, internalFormat, imageWidth, imageHeight, 0, imageSize, imageData);
			break;
		}
		case DDSFormat::DXT5:
		{
			GLsizei imageSize = max(1, ((imageWidth + 3) / 4)) * max(1, ((imageHeight + 3) / 4)) * 16;
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, internalFormat, imageWidth, imageHeight, 0, imageSize, imageData);
			break;
		}
		}


		m_Image = (void*)(intptr_t)imageTexture;
		m_ImageWidth = imageWidth;
		m_ImageHeight = imageHeight;
		m_ImageInUse = true;
	}

	void Previewer::FreeTexture()
	{
		if (m_ImageInUse)
		{
			GLuint imageTexture = (GLuint)(intptr_t)m_Image;
			glDeleteTextures(1, &imageTexture);

			m_ImageInUse = false;
		}
	}

	void Previewer::OnRenderDatabin()
	{
		ImGuiTableFlags flags =
			ImGuiTableFlags_RowBg |
			ImGuiTableFlags_SizingFixedFit |
			ImGuiTableFlags_NoHostExtendX |
			ImGuiTableFlags_BordersInnerV |
			ImGuiTableFlags_BordersOuter;

		if (ImGui::BeginTable("tablePreviewerDatabin", 2, flags))
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

	void Previewer::OnRenderDatabinItem()
	{
		ImGuiTableFlags flags =
			ImGuiTableFlags_RowBg |
			ImGuiTableFlags_SizingFixedFit |
			ImGuiTableFlags_NoHostExtendX |
			ImGuiTableFlags_BordersInnerV |
			ImGuiTableFlags_BordersOuter;

		if (ImGui::BeginTable("tablePreviewerDatabinItem", 2, flags))
		{
			GAME game = m_File->GetType().GetGame();

			uint32_t offset = 0U;
			int32_t dat_04 = 0;
			uint32_t size = 0U;
			uint32_t sizeCompressed = 0U;
			uint32_t dat_10 = 0U;
			switch (game)
			{
			case SIGMA_1:
			{
				offset = m_DatabinItemHeaderS1.offset;
				dat_04 = m_DatabinItemHeaderS1.dat_04;
				size = m_DatabinItemHeaderS1.size;
				sizeCompressed = m_DatabinItemHeaderS1.sizeCompressed;
				dat_10 = m_DatabinItemHeaderS1.dat_10;
				break;
			}
			case SIGMA_2:
			{
				offset = m_DatabinItemHeaderS2.offset;
				dat_04 = m_DatabinItemHeaderS2.dat_04;
				size = m_DatabinItemHeaderS2.size;
				sizeCompressed = m_DatabinItemHeaderS2.sizeCompressed;
				dat_10 = m_DatabinItemHeaderS2.dat_10;
				break;
			}
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("offset");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", offset).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_04");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", dat_04).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("size");
			ImGui::TableSetColumnIndex(1); ImGui::Text(GetPrettySize(size).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("sizeCompressed");
			ImGui::TableSetColumnIndex(1); ImGui::Text(GetPrettySize(sizeCompressed).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_10");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", dat_10).c_str());

			switch (game)
			{
			case SIGMA_1:
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("indexLinkedFile0");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinItemHeaderS1.indexLinkedFile0).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("indexLinkedFile1");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinItemHeaderS1.indexLinkedFile1).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("typeLinkedFile0");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:02X} ({})", m_DatabinItemHeaderS1.typeLinkedFile0, GetFileExtension((S1::FileTypeId)m_DatabinItemHeaderS1.typeLinkedFile0)).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("typeLinkedFile1");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:02X} ({})", m_DatabinItemHeaderS1.typeLinkedFile1, GetFileExtension((S1::FileTypeId)m_DatabinItemHeaderS1.typeLinkedFile1)).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("type");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:02X} ({})", m_DatabinItemHeaderS1.type, GetFileExtension((S1::FileTypeId)m_DatabinItemHeaderS1.type)).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("dat_1B");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinItemHeaderS1.dat_1B).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("dat_1C");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinItemHeaderS1.dat_1C).c_str());

				break;
			}
			case SIGMA_2:
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("indexLinkedFile");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_DatabinItemHeaderS2.indexLinkedFile).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("typeLinkedFile");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:02X} ({})", m_DatabinItemHeaderS2.typeLinkedFile, GetFileExtension((S2::FileTypeId)m_DatabinItemHeaderS2.typeLinkedFile)).c_str());

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("type");
				ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:02X} ({})", m_DatabinItemHeaderS2.type, GetFileExtension((S2::FileTypeId)m_DatabinItemHeaderS2.type)).c_str());

				break;
			}
			}

			ImGui::EndTable();
		}
	}

	void Previewer::OnRenderGT1G()
	{
		ImGuiTableFlags flags =
			ImGuiTableFlags_RowBg |
			ImGuiTableFlags_SizingFixedFit |
			ImGuiTableFlags_NoHostExtendX |
			ImGuiTableFlags_BordersInnerV |
			ImGuiTableFlags_BordersOuter;

		if (ImGui::BeginTable("tablePreviewerGT1G", 2, flags))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Version");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}.{}.{}.{}", (char)m_GT1GHeader.version.major, (char)m_GT1GHeader.version.submajor, (char)m_GT1GHeader.version.minor, (char)m_GT1GHeader.version.patch).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("fileSize");
			std::string prettyFileSize = GetPrettySize(m_GT1GHeader.fileSize);
			ImGui::TableSetColumnIndex(1); ImGui::Text(prettyFileSize.c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("textureOffsetTableAddress");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", m_GT1GHeader.textureOffsetTableAddress).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("textureCount");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_GT1GHeader.textureCount).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_14");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_GT1GHeader.dat_14).c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("dat_18");
			ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_GT1GHeader.dat_18).c_str());

			ImGui::EndTable();
		}

		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		if (m_GT1GHeader.textureCount)
		{
			ImGui::Text("Preview texture");

			ImGui::PushItemWidth(100.0f);
			if (ImGui::InputInt("Texture index", (int*)&m_PreviewGT1GTexIdx))
			{
				m_PreviewGT1GTexIdx = max(0, min((int)m_PreviewGT1GTexIdx, (int32_t)m_GT1GHeader.textureCount - 1));
				m_PreviewGT1GMipMapIdx = 0;
				FreeTexture();
				PrepareGT1GTexture(m_PreviewGT1GTexIdx);
			}
			if (ImGui::InputInt("Mip map level", (int*)&m_PreviewGT1GMipMapIdx))
			{
				m_PreviewGT1GMipMapIdx = max(0, min((int)m_PreviewGT1GMipMapIdx, (int32_t)m_GT1GTextures[m_PreviewGT1GTexIdx].MipMapCount - 1));
				FreeTexture();
				PrepareGT1GTexture(m_PreviewGT1GTexIdx);
			}
			ImGui::PopItemWidth();

			if (m_Image != nullptr)
			{
				if (m_GT1GTextures[m_PreviewGT1GTexIdx].Format == PixelFormat::ColorMap_u8)
				{
					if (ImGui::Checkbox("Render Color Map as raw data (color index)", &g_ReadColorMapAsLuminance))
					{
						FreeTexture();
						PrepareGT1GTexture(m_PreviewGT1GTexIdx);
					}
				}

				//ImGui::Text("pointer = %x", m_Image);
				//ImGui::Text("size = %d x %d", m_ImageWidth, m_ImageHeight);

				//	Save Texture Button
				/*if (ImGui::Button("Save Texture"))
				{
					//	Saving DDS to disk
					DDSFormat format;
					switch (m_GT1GTextures[m_PreviewGT1GTexIdx].Format)
					{
					case PixelFormat::RGBA8_BGRA_u8:
					{
						format = DDSFormat::BGRA8;
						break;
					}
					case PixelFormat::RGBA8_RGBA_u8:
					{
						format = DDSFormat::RGBA8;
						break;
					}
					case PixelFormat::CompressedRgbS3tcDxt1Ext_06:
					case PixelFormat::CompressedRgbS3tcDxt1Ext_59:
					{
						format = DDSFormat::DXT1;
						break;
					}
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_08:
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_5B:
					{
						format = DDSFormat::DXT5;
						break;
					}
					case PixelFormat::ColorMap_u8:
					{
						if ((m_PreviewGT1GTexIdx == 1) && (!g_ReadColorMapAsLuminance))
						{
							format = DDSFormat::BGRA8;
						}
						else
						{
							format = DDSFormat::LUMINANCE;
						}
						break;
					}
					default:
					{
						std::cout << "This texture format is not yet supported." << std::endl;
					}
					}

					LoaderGT1G loader(m_File);

					unsigned int mipMapCount = loader.GetMipMapCount(m_PreviewGT1GTexIdx);

					size_t dataSize = 0;
					size_t* mipSizes = new size_t[mipMapCount];
					for (unsigned int i = 0; i < mipMapCount; i++)
					{
						mipSizes[i] = loader.GetMipDataSize(m_PreviewGT1GTexIdx, i);
						dataSize += mipSizes[i];
					}

					char* imageData = new char[dataSize];
					size_t offset = 0;
					for (unsigned int i = 0; i < mipMapCount; i++)
					{
						MemoryBuffer mipBuffer;
						loader.GetImageData(mipBuffer, m_PreviewGT1GTexIdx, i);
						memcpy((void*)((uintptr_t)imageData + offset), mipBuffer.GetBaseAddress(), mipSizes[i]);
						offset += mipSizes[i];
					}

					SaveDDSTexture(
						(GetWString(m_File->GetName()) + L".dds").c_str(),
						imageData,
						m_GT1GTextures[m_PreviewGT1GTexIdx].MipMaps[0].Width,
						m_GT1GTextures[m_PreviewGT1GTexIdx].MipMaps[0].Height,
						m_GT1GTextures[m_PreviewGT1GTexIdx].MipMapCount,
						format,
						mipSizes
					);

					delete[] imageData;
					delete[] mipSizes;
				}*/

				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
				if (ImGui::BeginTable("tablePreviewerGT1GImage", 1, flags, { (float)m_ImageDisplayWidth + 1, (float)m_ImageDisplayHeight + 1 }))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImVec2 cursorPos = ImGui::GetCursorPos();
					ImGui::SetCursorPos({ cursorPos.x + 0, cursorPos.y + 1 });
					ImGui::Image(m_Image, ImVec2(m_ImageDisplayWidth, m_ImageDisplayHeight));
					ImGui::EndTable();
				}
				ImGui::PopStyleVar();
			}

			for (unsigned int i = 0; i < m_GT1GHeader.textureCount; i++)
			{
				ImGui::Dummy(ImVec2(0.0f, 5.0f));
				ImGui::Separator();
				ImGui::Dummy(ImVec2(0.0f, 5.0f));
				ImGui::Text(std::format("Texture #{}", i).c_str());

				if (ImGui::BeginTable(std::format("tablePreviewerGT1GTexture{}", i).c_str(), 2, flags))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Pixel format");
					ImGui::TableSetColumnIndex(1);

					switch (m_GT1GTextures[i].Format)
					{
					case PixelFormat::RGBA8_BGRA_u8:
						ImGui::Text("BGRA"); break;
					case PixelFormat::RGBA8_RGBA_u8:
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_08:
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_5B:
						ImGui::Text("RGBA"); break;
					case PixelFormat::CompressedRgbS3tcDxt1Ext_06:
					case PixelFormat::CompressedRgbS3tcDxt1Ext_59:
						ImGui::Text("RGB"); break;
					case PixelFormat::ColorMap_u8:
						ImGui::Text("Color Map index"); break;
					default:
						ImGui::Text("unknown");
					}
					
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Compression");
					ImGui::TableSetColumnIndex(1);

					switch (m_GT1GTextures[i].Format)
					{
					case PixelFormat::RGBA8_BGRA_u8:
					case PixelFormat::RGBA8_RGBA_u8:
					case PixelFormat::ColorMap_u8:
						ImGui::Text(std::format("raw (0x{:02X})", (unsigned char)m_GT1GTextures[i].Format).c_str()); break;
					case PixelFormat::CompressedRgbS3tcDxt1Ext_06:
					case PixelFormat::CompressedRgbS3tcDxt1Ext_59:
						ImGui::Text(std::format("S3TC DXT1 (0x{:02X})", (unsigned char)m_GT1GTextures[i].Format).c_str()); break;
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_08:
					case PixelFormat::CompressedRgbaS3tcDxt5Ext_5B:
						ImGui::Text(std::format("S3TC DXT5 (0x{:02X})", (unsigned char)m_GT1GTextures[i].Format).c_str()); break;
					default:
						ImGui::Text("unknown");
					}
					
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Flags");
					ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", m_GT1GTextures[i].Flags).c_str());

					if ((m_GT1GTextures[i].Flags >> 24) == 0x10)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0); ImGui::Text("ExtraFlags0");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", m_GT1GTextures[i].ExtraFlags0).c_str());

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0); ImGui::Text("ExtraFlags1");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", m_GT1GTextures[i].ExtraFlags1).c_str());

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0); ImGui::Text("ExtraFlags2");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("0x{:08X}", m_GT1GTextures[i].ExtraFlags2).c_str());
					}

					ImGui::EndTable();
				}

				if (ImGui::BeginTable(std::format("tablePreviewerGT1GTexture{}MipMaps", i).c_str(), 1 + m_GT1GTextures[i].MipMapCount, flags))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("MipMap level");
					for (unsigned int j = 0; j < m_GT1GTextures[i].MipMapCount; j++)
					{
						ImGui::TableSetColumnIndex(1 + j); ImGui::Text(std::format("{}", j).c_str());
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Width");
					for (unsigned int j = 0; j < m_GT1GTextures[i].MipMapCount; j++)
					{
						ImGui::TableSetColumnIndex(1 + j); ImGui::Text(std::format("{}", m_GT1GTextures[i].MipMaps[j].Width).c_str());
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Height");
					for (unsigned int j = 0; j < m_GT1GTextures[i].MipMapCount; j++)
					{
						ImGui::TableSetColumnIndex(1 + j); ImGui::Text(std::format("{}", m_GT1GTextures[i].MipMaps[j].Height).c_str());
					}

					ImGui::EndTable();
				}
			}
		}
		else
		{
			ImGui::Text("No textures were found in this file. This is unexpected.");
		}
	}

	void Previewer::OnRenderDDS()
	{
		ImGuiTableFlags flags =
			ImGuiTableFlags_RowBg |
			ImGuiTableFlags_SizingFixedFit |
			ImGuiTableFlags_NoHostExtendX |
			ImGuiTableFlags_BordersInnerV |
			ImGuiTableFlags_BordersOuter;

		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
		if (ImGui::BeginTable("tablePreviewerDDSImage", 1, flags, { (float)m_ImageDisplayWidth + 1, (float)m_ImageDisplayHeight + 1 }))
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
			PrepareDDS();
		}
		ImGui::PopItemWidth();

		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		ImGui::Text("DDS_HEADER");

		if (ImGui::BeginTable("tablePreviewerDDS_HEADER", 2, flags))
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

			if (ImGui::BeginTable("tablePreviewerDDS_HEADERddspf", 2, flags))
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
}