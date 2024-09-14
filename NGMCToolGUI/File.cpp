#include "stdafx.h"

#include "File.h"

#include "formats/LoaderDatabin.h"
#include "formats/LoaderGT1G.h"
#include "formats/LoaderDDS.h"

#include "popups/PopupSelectGame.h"

namespace NGMC
{
	extern bool g_ReadColorMapAsLuminance;
	extern PopupSelectGame g_PopupSelectGame;

	File::File()
		: m_IsFileInMemory(false), m_IsLoaded(false),
		m_FilePath(L""), m_Name(""),
		m_Type(FileType()),
		m_Childs(0),
		m_DataOffset(0ULL), m_Size(0ULL),
		m_IndexInParent(0U), m_Parent(nullptr),
		m_IsNodeOpened(false)
	{
	}

	File::File(const File& other)
		: m_IsFileInMemory(other.m_IsFileInMemory), m_IsLoaded(other.m_IsLoaded),
		m_FilePath(other.m_FilePath), m_Name(other.m_Name),
		m_Type(other.m_Type),
		m_Childs(other.m_Childs),
		m_DataOffset(other.m_DataOffset), m_Size(other.m_Size), m_MemBuf(other.m_MemBuf),
		m_IndexInParent(other.m_IndexInParent), m_Parent(other.m_Parent),
		m_IsNodeOpened(other.m_IsNodeOpened)
	{
		unsigned int childCount = GetChildCount();
		for (unsigned int i = 0; i < childCount; i++)
		{
			m_Childs[i].SetParent(this);
		}
	}

	File::File(const wchar_t* filePath, size_t dataOffset)
		: File()
	{
		m_FilePath = filePath;
		m_Name = GetFileNameFromPath(filePath);
		m_DataOffset = dataOffset;

		if (dataOffset == 0)
		{
			m_Type = DetectType();
		}
	}

	File::File(const wchar_t* filePath, const char* name, FileType type, size_t dataOffset)
		: File(filePath, dataOffset)
	{
		m_Name = name;
		m_Type = type;
	}

	File::File(const wchar_t* filePath, const char* name, std::uintmax_t size, FileType type, size_t dataOffset)
		: File(filePath, name, type, dataOffset)
	{
		m_Size = size;
		//m_DataOffset = dataOffset;
	}

	File::File(const wchar_t* filePath, const char* name, std::uintmax_t size, unsigned int indexInParent, File* p_Parent, FileType type, size_t dataOffset)
		: File(filePath, name, size, type, dataOffset)
	{
		m_IndexInParent = indexInParent;
		m_Parent = p_Parent;
	}

	File::File(MemoryBuffer& memBuf, const char* name, unsigned int indexInParent, File* p_Parent, FileType type)
		: File()
	{
		m_IndexInParent = indexInParent;
		m_Parent = p_Parent;

		size_t size = memBuf.GetSize();
		if (size)
		{
			m_MemBuf = memBuf;
			m_Size = size;
			m_IsFileInMemory = true;
			m_Name = name;
			m_Type = type;
		}
	}

	File::~File()
	{
		Unload();
	}

	bool File::IsLoaded() const
	{
		return m_IsLoaded;
	}

	bool File::IsFileInMemory() const
	{
		return m_IsFileInMemory;
	}

	const wchar_t* File::GetFilePath() const
	{
		return m_FilePath.c_str();
	}

	const char* File::GetName() const
	{
		return m_Name.c_str();
	}

	FileType File::GetType() const
	{
		return m_Type;
	}

	unsigned int File::GetChildCount() const
	{
		return m_Childs.size();
	}

	unsigned int File::GetDescendantCount() const
	{
		unsigned int descendantCount = 0;

		for (unsigned int i = 0; i < m_Childs.size(); i++)
		{
			descendantCount += m_Childs[i].GetDescendantCount() + 1;
		}

		return descendantCount;
	}

	const char* File::GetChildName(unsigned int index) const
	{
		if (this->GetChildCount() <= index)
		{
			return "[ERROR] Invalid Child Index";
		}

		return m_Childs[index].GetName();
	}

	File* File::GetChild(unsigned int index)
	{
		if (this->GetChildCount() <= index)
		{
			return nullptr;
		}

		return &m_Childs[index];
	}

	std::uintmax_t File::GetSize() const
	{
		return m_Size;
	}

	MemoryBuffer& File::GetMemoryBuffer()
	{
		return m_MemBuf;
	}

	unsigned int File::GetIndexInParent() const
	{
		return m_IndexInParent;
	}

	void File::UpdateStatus()
	{
		if (IsFileExisting(m_FilePath))
		{
			m_Size = GetFileSize(m_FilePath);
			m_IsFileInMemory = false;
		}
		else
		{
			m_Size = m_MemBuf.GetSize();
			m_IsFileInMemory = true;
		}
	}

	void File::SetName(const char* name)
	{
		m_Name = name;
	}

	void File::SetType(FileType type)
	{
		Unload();
		m_Type = type;
	}

	void File::SetParent(File* p_Parent)
	{
		m_Parent = p_Parent;
	}

	bool File::Load()
	{
		if (!m_IsLoaded)
		{
			switch (m_Type.GetGame())
			{
			case SIGMA_1:
			{
				using namespace S1;
				switch (m_Type.GetId())
				{
				case FileTypeId::databin:
				{
					m_IsLoaded = LoadDatabin(SIGMA_1);
					break;
				}
				case FileTypeId::databinItem:
				{
					m_IsLoaded = LoadDatabinItem(SIGMA_1);
					break;
				}
				case FileTypeId::GT1G_07:
				case FileTypeId::GT1G_13:
				{
					//bool g_ReadColorMapAsLuminanceBackup = g_ReadColorMapAsLuminance;
					//g_ReadColorMapAsLuminance = false;

					LoaderGT1G loader(this);

					unsigned int textureCount = loader.GetTextureCount();
					m_Childs = std::vector<File>(textureCount);
					for (unsigned int i = 0; i < textureCount; i++)
					{
						DDSFormat format;
						switch (loader.GetTexturePixelFormat(i))
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
							if ((i == 1) && (!g_ReadColorMapAsLuminance))
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
							return false;
						}
						}

						unsigned int mipMapCount = loader.GetTextureMipMapCount(i);

						size_t dataSize = 0;
						size_t* mipSizes = new size_t[mipMapCount];
						for (unsigned int j = 0; j < mipMapCount; j++)
						{
							mipSizes[j] = loader.GetMipDataSize(i, j);
							dataSize += mipSizes[j];
						}

						char* imageData = new char[dataSize];
						size_t offset = 0;
						for (unsigned int j = 0; j < mipMapCount; j++)
						{
							MemoryBuffer mipBuffer;
							loader.GetMipData(mipBuffer, i, j);
							memcpy((void*)((uintptr_t)imageData + offset), mipBuffer.GetBaseAddress(), mipSizes[j]);
							offset += mipSizes[j];
						}

						MemoryBuffer buffer;
						BuildDDS(
							buffer,
							imageData,
							loader.GetTextureMipMapWidth(i),
							loader.GetTextureMipMapHeight(i),
							mipMapCount,
							format,
							mipSizes
						);

						delete[] imageData;
						delete[] mipSizes;

						m_Childs[i] = File(buffer, std::format("{}.dds", i).c_str(), i, this, FileType(General::FileTypeId::DDS));
					}

					if (textureCount == 0)
					{
						std::cout << "GT1G file did not contain any textures." << std::endl;
					}
					m_IsLoaded = true;

					//g_ReadColorMapAsLuminance = g_ReadColorMapAsLuminanceBackup;

					break;
				}
				case FileTypeId::unknown:
				case FileTypeId::invalid:
				default:
				{
					std::cout << std::format("File type 0x{:02X} is not supported.", (char)m_Type.GetId()) << std::endl;
				}
				}
				break;
			}
			case SIGMA_2:
			{
				using namespace S2;
				switch (m_Type.GetId())
				{
				case FileTypeId::databin:
				{
					m_IsLoaded = LoadDatabin(SIGMA_2);
					break;
				}
				case FileTypeId::databinItem:
				{
					m_IsLoaded = LoadDatabinItem(SIGMA_2);
					break;
				}
				case FileTypeId::unknown:
				default:
				{
					std::cout << std::format("File type 0x{:02X} is not supported.", (char)m_Type.GetId()) << std::endl;
				}
				}
				break;
			}
			case RE_3:
			{
				using namespace RE;
				switch (m_Type.GetId())
				{
				case FileTypeId::databin:
				{
					m_IsLoaded = LoadDatabin(RE_3);
					break;
				}
				case FileTypeId::databinItem:
				{
					m_IsLoaded = LoadDatabinItem(RE_3);
					break;
				}
				case FileTypeId::unknown:
				default:
				{
					std::cout << std::format("File type 0x{:02X} is not supported.", (char)m_Type.GetId()) << std::endl;
				}
				}
				break;
			}
			default:
			{
				std::cout << "This file comes from an unsupported game." << std::endl;
			}
			}

			if (m_IsLoaded)
				m_IsNodeOpened = false;

			std::cout << "File \"" << m_Name << (m_IsLoaded ? "\" loaded." : "\" could not be loaded.") << std::endl;

			return m_IsLoaded;
		}
		return false;
	}

	bool File::Unload()
	{
		if (m_IsLoaded)
		{
			m_Childs.clear();
			m_Childs.shrink_to_fit();

			m_IsLoaded = false;

			std::cout << "File \"" << m_Name << "\" unloaded." << std::endl;
		}

		return m_IsLoaded;
	}

	bool File::Save(const wchar_t* filePath)
	{
		bool isSuccess = false;

		if (m_IsFileInMemory)
		{
			std::ofstream stream(filePath, std::ios::binary);

			if (stream)
			{
				stream.write(m_MemBuf.GetBaseAddress(), m_MemBuf.GetSize());

				isSuccess = true;
			}
		}

		std::wstring tempWString = filePath;
		std::string pathString = std::string(tempWString.begin(), tempWString.end());
		std::cout << "File \"" << m_Name << (isSuccess ? "\" was" : "\" could not be") << " saved to \"" << pathString << "\"." << std::endl;

		return isSuccess;
	}

	bool File::Extract(const wchar_t* directory)
	{
		bool isSuccess = true;
		unsigned int extractCount = 0U;

		switch (m_Type.GetGame())
		{
		case SIGMA_1:
		{
			using namespace S1;
			switch (m_Type.GetId())
			{
			case FileTypeId::databin:
			{
				extractCount = ExtractDatabin(SIGMA_1, directory);

				std::wstring filePath = directory;
				std::string dirString = std::string(filePath.begin(), filePath.end());
				std::cout << extractCount << " files were extracted to \"" << dirString << "\"." << std::endl;

				break;
			}
			default:
			{
				bool isLoaded = IsLoaded();
				if (isLoaded || Load())
				{
					for (unsigned int i = 0; i < GetChildCount(); i++)
					{
						File* p_Child = &m_Childs[i];

						std::string tempString = p_Child->GetName();
						std::wstring filePath = directory;
						filePath += L"\\" + std::wstring(tempString.begin(), tempString.end());

						p_Child->Save(filePath.c_str());
					}

					if (!isLoaded)
					{
						Unload();
					}
				}
				else
				{
					isSuccess = false;
				}
				break;
			}
			//default:
			//{
			//	std::cout << "Extraction not supported for file type " << GetTypeName(m_Type) << "." << std::endl;
			//	break;
			//}
			}
			break;
		}
		case SIGMA_2:
		{
			using namespace S2;
			switch (m_Type.GetId())
			{
			case FileTypeId::databin:
			{
				extractCount = ExtractDatabin(SIGMA_2, directory);

				std::wstring filePath = directory;
				std::string dirString = std::string(filePath.begin(), filePath.end());
				std::cout << extractCount << " files were extracted to \"" << dirString << "\"." << std::endl;

				break;
			}
			}
			break;
		}
		case RE_3:
		{
			using namespace RE;
			switch (m_Type.GetId())
			{
			case FileTypeId::databin:
			{
				extractCount = ExtractDatabin(RE_3, directory);

				std::wstring filePath = directory;
				std::string dirString = std::string(filePath.begin(), filePath.end());
				std::cout << extractCount << " files were extracted to \"" << dirString << "\"." << std::endl;

				break;
			}
			}
			break;
		}
		}

		return isSuccess;
	}

	bool File::IsNodeOpened()
	{
		bool isNodeOpened = m_IsNodeOpened;
		//m_IsNodeOpened = false;
		return isNodeOpened;
	}

	void File::SetNodeOpenness(bool isOpen)
	{
		m_IsNodeOpened = isOpen;
	}

	FileType File::DetectType()
	{
		FileType outType = FileType();

		size_t extensionOffset = m_Name.find_last_of('.') + 1;
		std::string extension = m_Name.substr(extensionOffset, m_Name.length() - extensionOffset);

		char magic[8];
		memset(magic, 0, sizeof(magic));

		if (m_IsFileInMemory)
		{
			uintptr_t pos = m_MemBuf.Tell();

			m_MemBuf.Seek(0, MemoryBuffer::beg);

			m_MemBuf.Read(magic, sizeof(magic));

			m_MemBuf.Seek(pos, MemoryBuffer::beg);
		}
		else
		{
			std::ifstream stream(m_FilePath, std::ios::binary);

			if (stream)
			{
				stream.seekg(0, std::ios_base::beg);

				stream.read(magic, sizeof(magic));
			}
		}

		if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::btl_dat).c_str()) == 0)
		{
			if (strnicmp(extension.c_str(), magic, 7) == 0)
				outType = S1::FileTypeId::btl_dat;
		}
		else if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::chr_dat2).c_str()) == 0)
		{
			if (strnicmp(extension.c_str(), magic, 8) == 0)
				outType = S1::FileTypeId::chr_dat2;
		}
		else if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::TMC_05).c_str()) == 0)
		{
			if (strnicmp(extension.c_str(), magic, 3) == 0)
				outType = S1::FileTypeId::TMC_05;
		}
		else if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::GT1G_07).c_str()) == 0)
		{
			if (strnicmp(extension.c_str(), magic, 4) == 0)
				outType = S1::FileTypeId::GT1G_07;
		}
		else if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::itm_dat2_08).c_str()) == 0)
		{
			if (strnicmp(extension.c_str(), magic, 8) == 0)
				outType = S1::FileTypeId::itm_dat2_08;
		}
		else if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::MESSTR).c_str()) == 0)
		{
			if (strnicmp(extension.c_str(), magic, 6) == 0)
				outType = S1::FileTypeId::MESSTR;
		}
		else if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::chr_dat).c_str()) == 0)
		{
			if (strnicmp(extension.c_str(), magic, 7) == 0)
				outType = S1::FileTypeId::chr_dat;
		}
		else if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::rtm_dat).c_str()) == 0)
		{
			if (strnicmp(extension.c_str(), magic, 7) == 0)
				outType = S1::FileTypeId::rtm_dat;
		}
		else if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::SND).c_str()) == 0)
		{
			if (strnicmp(extension.c_str(), magic, 3) == 0)
				outType = S1::FileTypeId::SND;
		}
		else if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::stry_dat).c_str()) == 0)
		{
			if (strnicmp(extension.c_str(), magic, 8) == 0)
				outType = S1::FileTypeId::stry_dat;
		}
		else if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::VtxLay).c_str()) == 0)
		{
			if (strnicmp(extension.c_str(), magic, 6) == 0)
				outType = S1::FileTypeId::VtxLay;
		}
		else if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::sprite).c_str()) == 0)
		{
			if (strnicmp(extension.c_str(), magic, 6) == 0)
				outType = S1::FileTypeId::sprite;
		}
		else if (stricmp(extension.c_str(), GetTypeName(General::FileTypeId::DDS).c_str()) == 0)
		{
			//char ddsMagic[4] = { ' ', 'S', 'D', 'D' };
			if (strnicmp("DDS ", magic, 4) == 0)
				outType = General::FileTypeId::DDS;
		}
		else if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::databin).c_str()) == 0)
		{
			// no magic => rely on file name as well as user to pick the correct game

			g_PopupSelectGame.Setup(this);
			g_PopupSelectGame.Open();

			outType = General::FileTypeId::databin;
		}
		else if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::tdpack).c_str()) == 0)
		{
			// no magic => rely on file name
			outType = S1::FileTypeId::tdpack;
		}
		else if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::type_02).c_str()) == 0)
		{
			// no magic => rely on file name
			outType = S1::FileTypeId::type_02;
		}
		else if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::type_04).c_str()) == 0)
		{
			// no magic => rely on file name
			outType = S1::FileTypeId::type_04;
		}
		else if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::type_06).c_str()) == 0)
		{
			// no magic => rely on file name
			outType = S1::FileTypeId::type_06;
		}
		else if (stricmp(extension.c_str(), GetTypeName(S1::FileTypeId::type_0A).c_str()) == 0)
		{
			// no magic => rely on file name
			outType = S1::FileTypeId::type_0A;
		}

		return outType;
	}

	bool File::LoadDatabin(GAME game)
	{
		bool isSuccess = false;

		LoaderDatabin loader(game, m_FilePath);

		if (loader.LoadItemHeaders())
		{
			unsigned int fileCount = loader.GetFileCount();
			m_Childs.reserve(m_Childs.size() + fileCount);

			FileType childType = FileType();
			switch (game)
			{
			case SIGMA_1:
				childType.SetType(S1::FileTypeId::databinItem);
				break;
			case SIGMA_2:
				childType.SetType(S2::FileTypeId::databinItem);
				break;
			case RE_3:
				childType.SetType(RE::FileTypeId::databinItem);
				break;
			case NON_GAME:
			default:
				return false;
			}

			for (unsigned int i = 0; i < fileCount; i++)
			{
				m_Childs.emplace_back(
					m_FilePath.c_str(),
					(std::format("{:05d}.", i) + loader.GetFileType(i).GetFileExtension() + ".compressed").c_str(),
					loader.GetFileSizeCompressed(i),
					i,
					this,
					childType,
					(size_t)loader.GetFileHeaderOffset(i)
				);
			}
			isSuccess = true;
		}

		return isSuccess;
	}

	bool File::LoadDatabinItem(GAME game)
	{
		bool isSuccess = false;

		LoaderDatabin loader(game, m_FilePath);

		if (loader.LoadItemHeaders())
		{
			if (m_IndexInParent < loader.GetFileCount())
			{
				MemoryBuffer decompressed;
				if (loader.DecompressItem(decompressed, m_IndexInParent))
				{
					FileType type = loader.GetFileType(m_IndexInParent);

					m_Childs.emplace_back(
						decompressed,
						(std::format("{:05d}.", m_IndexInParent) + type.GetFileExtension()).c_str(),
						//loader.GetFileSize(m_IndexInParent),
						0,
						this,
						type
					);

					isSuccess = true;
				}
			}
			else
			{
				std::cout << "[ERROR] loader.GetFileCount() <= m_IndexInParent" << std::endl;
			}
		}

		return isSuccess;
	}

	unsigned int File::ExtractDatabin(GAME game, const wchar_t* directory)
	{
		unsigned int extractCount = 0U;

		LoaderDatabin loader(game);

		if (m_IsFileInMemory)
		{
			if (loader.LoadItemHeaders(m_MemBuf))
			{
				for (unsigned int i = 0; i < loader.GetFileCount(); i++)
				{
					MemoryBuffer outBuf;

					bool extracted = false;
					File* p_File = GetChild(i);
					if (p_File != nullptr)
					{
						if (p_File->IsLoaded())
						{
							for (unsigned int j = 0; j < p_File->GetChildCount(); j++)
							{
								File* p_Child = p_File->GetChild(j);

								std::string tempString = std::format("{:05d}.", i) + p_Child->GetType().GetFileExtension();
								std::wstring filePath = directory;
								filePath += L"\\" + std::wstring(tempString.begin(), tempString.end());

								p_Child->Save(filePath.c_str());
							}
							extracted = true;
						}
					}

					if (!extracted)
					{
						loader.DecompressItem(outBuf, m_MemBuf, i);

						std::string tempString = std::format("{:05d}.", i) + loader.GetFileType(i).GetFileExtension();
						std::wstring filePath = directory;
						filePath += L"\\" + std::wstring(tempString.begin(), tempString.end());

						outBuf.SaveToFile(filePath.c_str());
					}

					extractCount++;
				}
			}
		}
		else
		{
			std::ifstream stream(m_FilePath, std::ios::binary);

			if (loader.LoadItemHeaders(stream))
			{
				for (unsigned int i = 0; i < loader.GetFileCount(); i++)
				{
					MemoryBuffer outBuf;

					bool extracted = false;
					File* p_File = GetChild(i);
					if (p_File != nullptr)
					{
						if (p_File->IsLoaded())
						{
							for (unsigned int j = 0; j < p_File->GetChildCount(); j++)
							{
								File* p_Child = p_File->GetChild(j);

								std::string tempString = std::format("{:05d}.", i) + p_Child->GetType().GetFileExtension();
								std::wstring filePath = directory;
								filePath += L"\\" + std::wstring(tempString.begin(), tempString.end());

								p_Child->Save(filePath.c_str());
							}
							extracted = true;
						}
					}

					if (!extracted)
					{
						loader.DecompressItem(outBuf, stream, i);

						std::string tempString = std::format("{:05d}.", i) + loader.GetFileType(i).GetFileExtension();
						std::wstring filePath = directory;
						filePath += L"\\" + std::wstring(tempString.begin(), tempString.end());

						outBuf.SaveToFile(filePath.c_str());
					}

					extractCount++;
				}
			}
		}

		return extractCount;
	}
}