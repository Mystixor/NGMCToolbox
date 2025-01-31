#include "stdafx.h"

#include "File.h"

#include "loaders/LoaderDatabin.h"
#include "loaders/LoaderGT1G.h"
#include "loaders/LoaderDDS.h"
#include "loaders/LoaderLANG.h"

#include "popups/PopupSelectGame.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

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

		m_MemBuf = memBuf;
		m_Size = memBuf.GetSize();
		m_IsFileInMemory = true;
		m_Name = name;
		m_Type = type;
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
			using namespace Databin;
			switch (m_Type.GetGame())
			{
			case SIGMA_1:
			{
				using namespace S1;
				switch (m_Type.GetId())
				{
				case FileTypeId::databin:
				{
					m_IsLoaded = LoadDatabin();
					break;
				}
				case FileTypeId::databinItem:
				{
					m_IsLoaded = LoadDatabinItem();
					break;
				}
				case FileTypeId::GT1G_07:
				case FileTypeId::GT1G_13:
				{
					using namespace GT1G;

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
							Log("This texture format is not yet supported.");
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
						Log("GT1G file did not contain any textures.");
					}
					m_IsLoaded = true;

					//g_ReadColorMapAsLuminance = g_ReadColorMapAsLuminanceBackup;

					break;
				}
				case FileTypeId::unknown:
				case FileTypeId::invalid:
				default:
				{
					m_IsLoaded = LoadUnsupported();
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
					m_IsLoaded = LoadDatabin();
					break;
				}
				case FileTypeId::databinItem:
				{
					m_IsLoaded = LoadDatabinItem();
					break;
				}
				case FileTypeId::LANG_00:
				{

				}
				case FileTypeId::unknown:
				default:
				{
					m_IsLoaded = LoadUnsupported();
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
					m_IsLoaded = LoadDatabin();
					break;
				}
				case FileTypeId::databinItem:
				{
					m_IsLoaded = LoadDatabinItem();
					break;
				}
				case FileTypeId::unknown:
				default:
				{
					m_IsLoaded = LoadUnsupported();
				}
				}
				break;
			}
			default:
			{
				Log("This file comes from an unsupported game.");
			}
			}

			if (m_IsLoaded)
				m_IsNodeOpened = false;

			Log("File \"{}\"{} loaded.", m_Name, m_IsLoaded ? "" : " could not be");

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

			Log("File \"{}\" unloaded.", m_Name);
		}

		return m_IsLoaded;
	}

	bool File::Save(const wchar_t* filePath, bool isPrintResult)
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

		if(isPrintResult)
			Log(L"File \"{}\" {} saved to \"{}\".", std::wstring(m_Name.begin(), m_Name.end()), isSuccess ? L"was" : L"could not be", filePath);

		return isSuccess;
	}

	bool File::Extract(const wchar_t* directory)
	{
		bool isSuccess = true;
		unsigned int extractCount = 0U;

		{
			using namespace Databin;

			switch (m_Type.GetGame())
			{
			case SIGMA_1:
			{
				using namespace S1;
				switch (m_Type.GetId())
				{
				case FileTypeId::databin:
				{
					extractCount = ExtractDatabin(directory);
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

							std::string name = p_Child->GetName();
							std::wstring filePath = directory;
							filePath += L"\\" + std::wstring(name.begin(), name.end());

							p_Child->Save(filePath.c_str(), false);
							extractCount++;
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
					extractCount = ExtractDatabin(directory);
					break;
				}
				case FileTypeId::LANG_00:
				{
					extractCount = ExtractLANG(directory);
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

							std::string name = p_Child->GetName();
							std::wstring filePath = directory;
							filePath += L"\\" + std::wstring(name.begin(), name.end());

							p_Child->Save(filePath.c_str(), false);
							extractCount++;
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
					extractCount = ExtractDatabin(directory);
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

							std::string name = p_Child->GetName();
							std::wstring filePath = directory;
							filePath += L"\\" + std::wstring(name.begin(), name.end());

							p_Child->Save(filePath.c_str(), false);
							extractCount++;
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
				}
				break;
			}
			}
		}

		Log(L"{} file{} extracted to \"{}\".", extractCount, extractCount != 1 ? L"s were" : L" was", directory);

		return isSuccess;
	}

	bool File::IsNodeOpened() const
	{
		return m_IsNodeOpened;
	}

	void File::SetNodeOpenness(bool isOpen)
	{
		m_IsNodeOpened = isOpen;
	}

	bool File::CheckExtension(FileType checkType, const char* extension)
	{
		std::string typeName = checkType.GetTypeName();
		if (stricmp(extension, typeName.c_str()) == 0)
		{
			return true;
		}
		return false;
	}

	bool File::CheckMagic(FileType checkType, char magic[8])
	{
		std::string typeName = checkType.GetTypeName();
		if (strnicmp(typeName.c_str(), magic, min(8, typeName.length())) == 0)
		{
			return true;
		}
		return false;
	}

	bool File::CheckExtensionMagic(FileType checkType, const char* extension, char magic[8])
	{
		std::string typeName = checkType.GetTypeName();
		if (stricmp(extension, typeName.c_str()) == 0)
		{
			if (strnicmp(typeName.c_str(), magic, min(8, typeName.length())) == 0)
				return true;
		}
		return false;
	}

	FileType File::DetectType(FileType typeBias)
	{
		FileType outType = FileType();
		outType.SetGame(typeBias.GetGame());

		if (this->GetSize() < 8)
			outType.SetId(typeBias.GetId());

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

		{
			using namespace Databin;

			GAME gameBias = typeBias.GetGame();
			int idBias = typeBias.GetId();
			if (gameBias == NON_GAME || gameBias == UNKNOWN_GAME)
			{
				using namespace General;

				if (CheckExtensionMagic(FileTypeId::DDS, extension.c_str(), magic))
				{
					outType = FileTypeId::DDS;
				}
				else if (CheckExtension(FileTypeId::databin, extension.c_str()))
				{
					g_PopupSelectGame.Setup(this);
					g_PopupSelectGame.Open();

					outType = FileTypeId::databin;
				}

				if (!outType.IsUnknown())
					return outType;
			}
			if (gameBias == SIGMA_1 || gameBias == UNKNOWN_GAME)
			{
				using namespace S1;

				if (CheckExtensionMagic(FileTypeId::tdpack_00, extension.c_str(), magic))
				{
					outType = FileTypeId::tdpack_00;
				}
				else if (CheckExtension(FileTypeId::type_00, extension.c_str()))
				{
					if (CheckMagic(FileTypeId::tdpack_00, magic))
						outType = FileTypeId::tdpack_00;
					else
						outType = FileTypeId::type_00;
				}
				else if (CheckExtensionMagic(FileTypeId::btl_dat, extension.c_str(), magic))
				{
					outType = FileTypeId::btl_dat;
				}
				else if (CheckExtension(FileTypeId::type_02, extension.c_str()))
				{
					outType = FileTypeId::type_02;
				}
				else if (CheckExtensionMagic(FileTypeId::chr_dat2, extension.c_str(), magic))
				{
					outType = FileTypeId::chr_dat2;
				}
				else if (CheckExtension(FileTypeId::type_04, extension.c_str()))
				{
					outType = FileTypeId::type_04;
				}
				else if (CheckExtensionMagic(FileTypeId::TMC_05, extension.c_str(), magic))
				{
					outType = FileTypeId::TMC_05;
					if (idBias == FileTypeId::TMC_10)
						outType = FileTypeId::TMC_10;
				}
				else if (CheckExtension(FileTypeId::type_06, extension.c_str()))
				{
					outType = FileTypeId::type_06;
				}
				else if (CheckExtensionMagic(FileTypeId::GT1G_07, extension.c_str(), magic))
				{
					outType = FileTypeId::GT1G_07;
					if (idBias == FileTypeId::GT1G_13)
						outType = FileTypeId::GT1G_13;
				}
				else if (CheckExtensionMagic(FileTypeId::itm_dat2_08, extension.c_str(), magic))
				{
					outType = FileTypeId::itm_dat2_08;
					if (idBias == FileTypeId::itm_dat2_0E)
						outType = FileTypeId::itm_dat2_0E;
				}
				else if (CheckExtensionMagic(FileTypeId::MESSTR, extension.c_str(), magic))
				{
					outType = FileTypeId::MESSTR;
				}
				else if (CheckExtension(FileTypeId::type_0A, extension.c_str()))
				{
					outType = FileTypeId::type_0A;
				}
				else if (CheckExtensionMagic(FileTypeId::chr_dat, extension.c_str(), magic))
				{
					outType = FileTypeId::chr_dat;
				}
				else if (CheckExtensionMagic(FileTypeId::rtm_dat, extension.c_str(), magic))
				{
					outType = FileTypeId::rtm_dat;
				}
				else if (CheckExtensionMagic(FileTypeId::SND, extension.c_str(), magic))
				{
					outType = FileTypeId::SND;
				}
				else if (CheckExtensionMagic(FileTypeId::stry_dat, extension.c_str(), magic))
				{
					outType = FileTypeId::stry_dat;
				}
				else if (CheckExtension(FileTypeId::TMCL, extension.c_str()))
				{
					outType = FileTypeId::TMCL;
				}
				else if (CheckExtensionMagic(FileTypeId::sprite, extension.c_str(), magic))
				{
					outType = FileTypeId::sprite;
				}

				if (!outType.IsUnknown())
					return outType;
			}
			if (gameBias == SIGMA_2 || gameBias == UNKNOWN_GAME)
			{
				using namespace S2;

				if (CheckExtensionMagic(FileTypeId::LANG_00, extension.c_str(), magic))
				{
					outType = FileTypeId::LANG_00;
				}
				else if (CheckExtension(FileTypeId::type_00, extension.c_str()))
				{
					if (CheckMagic(FileTypeId::LANG_00, magic))
						outType = FileTypeId::LANG_00;
					else if (CheckMagic(FileTypeId::tdpack_00, magic))
						outType = FileTypeId::tdpack_00;
					else
						outType = FileTypeId::type_00;
				}
				else if (CheckExtension(FileTypeId::type_01, extension.c_str()))
				{
					outType = FileTypeId::type_01;
				}
				else if (CheckExtension(FileTypeId::type_02, extension.c_str()))
				{
					outType = FileTypeId::type_02;
				}
				else if (CheckExtensionMagic(FileTypeId::TDP4ACT, extension.c_str(), magic))
				{
					outType = FileTypeId::TDP4ACT;
				}
				else if (CheckExtensionMagic(FileTypeId::TDP4CLD, extension.c_str(), magic))
				{
					outType = FileTypeId::TDP4CLD;
				}
				else if (CheckExtension(FileTypeId::type_05, extension.c_str()))
				{
					outType = FileTypeId::type_05;
				}
				else if (CheckExtension(FileTypeId::type_06, extension.c_str()))
				{
					outType = FileTypeId::type_06;
				}
				else if (CheckExtension(FileTypeId::type_07, extension.c_str()))
				{
					outType = FileTypeId::type_07;
				}
				else if (CheckExtensionMagic(FileTypeId::TMC_08, extension.c_str(), magic))
				{
					outType = FileTypeId::TMC_08;
					if (idBias == FileTypeId::TMC_0B)
						outType = FileTypeId::TMC_0B;
				}
				else if (CheckExtension(FileTypeId::type_09, extension.c_str()))
				{
					outType = FileTypeId::type_09;
				}
				else if (CheckExtension(FileTypeId::type_0A, extension.c_str()))
				{
					outType = FileTypeId::type_0A;
				}
				else if (CheckExtension(FileTypeId::type_0C, extension.c_str()))
				{
					outType = FileTypeId::type_0C;
				}
				else if (CheckExtensionMagic(FileTypeId::itm_dat2, extension.c_str(), magic))
				{
					outType = FileTypeId::itm_dat2;
				}
				else if (CheckExtension(FileTypeId::type_0E, extension.c_str()))
				{
					outType = FileTypeId::type_0E;
				}
				else if (CheckExtension(FileTypeId::type_0F, extension.c_str()))
				{
					outType = FileTypeId::type_0F;
				}
				else if (CheckExtension(FileTypeId::type_10, extension.c_str()))
				{
					outType = FileTypeId::type_10;
				}
				else if (CheckExtensionMagic(FileTypeId::chr_dat, extension.c_str(), magic))
				{
					outType = FileTypeId::chr_dat;
				}
				else if (CheckExtensionMagic(FileTypeId::rtm_dat, extension.c_str(), magic))
				{
					outType = FileTypeId::rtm_dat;
				}
				else if (CheckExtensionMagic(FileTypeId::tdpack, extension.c_str(), magic))
				{
					outType = FileTypeId::tdpack_00;
					if (idBias == FileTypeId::tdpack)
						outType = FileTypeId::tdpack;
				}
				else if (CheckExtensionMagic(FileTypeId::TDP4SOB, extension.c_str(), magic))
				{
					outType = FileTypeId::TDP4SOB;
				}
				else if (CheckExtensionMagic(FileTypeId::TDP4SOC, extension.c_str(), magic))
				{
					outType = FileTypeId::TDP4SOC;
				}
				else if (CheckExtensionMagic(FileTypeId::sprpack, extension.c_str(), magic))
				{
					outType = FileTypeId::sprpack;
				}
				else if (CheckExtensionMagic(FileTypeId::STAGEETC, extension.c_str(), magic))
				{
					outType = FileTypeId::STAGEETC;
				}
				else if (CheckExtensionMagic(FileTypeId::TDP4STY, extension.c_str(), magic))
				{
					outType = FileTypeId::TDP4STY;
				}
				else if (CheckExtensionMagic(FileTypeId::TNF, extension.c_str(), magic))
				{
					outType = FileTypeId::TNF;
				}
				else if (CheckExtension(FileTypeId::type_1A, extension.c_str()))
				{
					outType = FileTypeId::type_1A;
				}
				else if (CheckExtension(FileTypeId::TMCL, extension.c_str()))
				{
					outType = FileTypeId::TMCL;
				}
				else if (CheckExtensionMagic(FileTypeId::XWSFILE_1C, extension.c_str(), magic))
				{
					outType = FileTypeId::XWSFILE_1C;
				}
				else if (CheckExtensionMagic(FileTypeId::tdpack_1C, extension.c_str(), magic))
				{
					outType = FileTypeId::tdpack_1C;
				}
				else if (CheckExtension(FileTypeId::type_1C, extension.c_str()))
				{
					if (CheckMagic(FileTypeId::XWSFILE_1C, magic))
						outType = FileTypeId::XWSFILE_1C;
					else if (CheckMagic(FileTypeId::tdpack_1C, magic))
						outType = FileTypeId::tdpack_1C;
					else
						outType = FileTypeId::type_1C;
				}
				else if (CheckExtension(FileTypeId::type_1D, extension.c_str()))
				{
					outType = FileTypeId::type_1D;
				}
				else if (CheckExtension(FileTypeId::type_1E, extension.c_str()))
				{
					outType = FileTypeId::type_1E;
				}

				if (!outType.IsUnknown())
					return outType;
			}
		}

		return outType;
	}

	bool File::LoadDatabin()
	{
		bool isSuccess = false;

		LoaderDatabin loader(m_Type.GetGame(), *this);

		if (loader.LoadItemHeaders())
		{
			using namespace Databin;

			unsigned int fileCount = loader.GetFileCount();
			m_Childs.reserve(m_Childs.size() + fileCount);

			FileType childType = FileType();
			switch (m_Type.GetGame())
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

	bool File::LoadDatabinItem()
	{
		bool isSuccess = false;

		LoaderDatabin loader(m_Type.GetGame(), *this);

		if (m_IndexInParent < loader.GetFileCount())
		{
			MemoryBuffer decBuf;
			if (loader.DecompressItem(decBuf, m_IndexInParent))
			{
				FileType type = loader.GetFileType(m_IndexInParent);

				m_Childs.emplace_back(
					decBuf,
					(std::format("{:05d}.", m_IndexInParent) + type.GetFileExtension()).c_str(),
					0,
					this,
					type
				);

				m_Childs.back().SetType(m_Childs.back().DetectType(type));
				m_Childs.back().SetName((std::format("{:05d}.", m_IndexInParent) + m_Childs.back().GetType().GetFileExtension()).c_str());

				isSuccess = true;
			}
		}
		else
		{
			Log("[ERROR] loader.GetFileCount() <= m_IndexInParent");
		}

		return isSuccess;
	}

	bool File::LoadUnsupported()
	{
		Log("File type \"{}\" is not supported.", m_Type.GetTypeName());

		return false;
	}

	unsigned int File::ExtractDatabin(const wchar_t* directory)
	{
		unsigned int extractCount = 0U;

		LoaderDatabin loader(m_Type.GetGame(), *this);

		if (loader.LoadItemHeaders())
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
					loader.DecompressItem(outBuf, /*m_MemBuf, */ i);

					FileType childType = loader.GetFileType(i);
					File childFile = File(outBuf, (std::format("{:05d}.", i) + childType.GetFileExtension()).c_str(), i, this);
					childFile.SetType(childFile.DetectType(childType));

					std::string tempString = std::format("{:05d}.", i) + childFile.GetType().GetFileExtension();
					std::wstring filePath = directory;
					filePath += L"\\" + std::wstring(tempString.begin(), tempString.end());

					outBuf.SaveToFile(filePath.c_str());
				}

				extractCount++;
			}
		}

		return extractCount;
	}

	unsigned int File::ExtractLANG(const wchar_t* directory)
	{
		unsigned int extractCount = 0U;

		LoaderLANG loader(*this);

		LANG::Node root;
		std::vector<std::pair<LANG::CTGPACKNode, std::vector<std::pair<LANG::STRPACKNode, std::vector<std::string>>>>> content;

		if (loader.GetRoot(root))
		{
			if (loader.ParseLANG(content))
			{
				rapidjson::Document d;
				d.SetObject();

				rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
				
				rapidjson::Value rootName;
				rootName.SetString(root.name, strlen(root.name), allocator);
				d.AddMember("name", rootName, allocator);

				rapidjson::Value CTGPACKs(rapidjson::kArrayType);

				for (int i = 0; i < content.size(); i++)
				{
					rapidjson::Value CTGPACK;
					CTGPACK.SetObject();

					rapidjson::Value ctgpackName;
					ctgpackName.SetString(content[i].first.name, strlen(content[i].first.name), allocator);
					CTGPACK.AddMember("name", ctgpackName, allocator);

					rapidjson::Value STRPACKs(rapidjson::kArrayType);

					for (int j = 0; j < content[i].first.header.childCount; j++)
					{
						rapidjson::Value STRPACK;
						STRPACK.SetObject();

						rapidjson::Value strpackName;
						strpackName.SetString(content[i].second[j].first.name, strlen(content[i].second[j].first.name), allocator);
						STRPACK.AddMember("name", strpackName, allocator);

						rapidjson::Value dat_50;
						dat_50.SetUint(content[i].second[j].first.dat_50);
						STRPACK.AddMember("dat_50", dat_50, allocator);

						rapidjson::Value strings(rapidjson::kArrayType);

						for (int k = 0; k < content[i].second[j].first.header.childCount; k++)
						{
							rapidjson::Value text(rapidjson::kStringType);
							text.SetString(content[i].second[j].second[k].c_str(), allocator);
							strings.PushBack(text, allocator);
						}
						STRPACK.AddMember("strings", strings, allocator);

						STRPACKs.PushBack(STRPACK, allocator);
					}
					CTGPACK.AddMember("STRPACKs", STRPACKs, allocator);

					CTGPACKs.PushBack(CTGPACK, allocator);
				}
				d.AddMember("CTGPACKs", CTGPACKs, allocator);

				rapidjson::StringBuffer buffer;
				rapidjson::PrettyWriter<rapidjson::StringBuffer> pretty_writer(buffer);
				d.Accept(pretty_writer);


				std::string tempString = std::format("{}.json", m_Name);
				std::wstring filePath = directory;
				filePath += L"\\" + std::wstring(tempString.begin(), tempString.end());

				std::ofstream stream(filePath);
				if (stream)
				{
					stream.write(buffer.GetString(), buffer.GetSize());
				}

				extractCount++;
			}
		}

		return extractCount;
	}
}