#pragma once

#include "stdafx.h"

#include <shobjidl.h>

#include "MemoryBuffer.h"

namespace NGMC
{
	//	Logs a text encoded as UTF-8 to the console.
	static void Log(const std::string& text)
	{
		std::cout << text << std::endl;
	}

	//	Logs a text encoded as UTF-8 to the console. Text will be formatted in the same way as with std::format.
	template <class... _Types>
	_NODISCARD static void Log(const std::string_view formatText, _Types&&... _Args)
	{
		std::string text = std::vformat(formatText, std::make_format_args(_Args...));

		Log(text);
	}

	//	Logs a wide character text encoded as UTF-8 to the console.
	static void Log(const std::wstring& text)
	{
		int bufferSize = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, NULL, 0, NULL, NULL);
		char* buffer = new char[bufferSize];
		WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, buffer, bufferSize, NULL, NULL);
		Log(buffer);
		delete[] buffer;
	}

	//	Logs a wide character text encoded as UTF-8 to the console. Text will be formatted in the same way as with std::format.
	template <class... _Types>
	_NODISCARD static void Log(const std::wstring_view formatText, _Types&&... _Args)
	{
		std::wstring text = std::vformat(formatText, std::make_wformat_args(_Args...));

		int bufferSize = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, NULL, 0, NULL, NULL);
		char* buffer = new char[bufferSize];
		WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, buffer, bufferSize, NULL, NULL);
		Log(buffer);
		delete[] buffer;
	}


	static std::string GetPrettySize(size_t size)
	{
		std::string output = "";

		size_t order = size == 0 ? 0 : std::log2(size) / 10;

		std::string unit = "";
		switch (order)
		{
		case 0:
			break;
		case 1:
			unit += "kB";
			break;
		case 2:
			unit += "MB";
			break;
		case 3:
			unit += "GB";
			break;
		case 4:
		default:
			unit += "TB";
			break;
		}

		if (order == 0)
			output += std::format("{} bytes", size);
		else
		{
			double prettySize = std::round((double)size / std::pow(1024, order) * 1e2) / 1e2;
			output += std::vformat("{:.2f} {} ({} bytes)", std::make_format_args(prettySize, unit, size));
		}

		return output;
	}

	//	Returns the conversion of a string to a wide char string.
	static std::wstring GetWStringFromString(const char* string)
	{
		std::string tempString = string;
		
		return std::wstring(tempString.begin(), tempString.end());
	}
	
	//	Returns the conversion of a wide char string to a string (can lead to undesirable results, see GetStringFromWStringSimple for an alternative).
	static std::string GetStringFromWString(const wchar_t* wstring)
	{
		std::wstring tempString = wstring;
		
		return std::string(tempString.begin(), tempString.end());
	}

	//	Returns the conversion of a wide char string to a string by omitting data that might lead to some weird corruption (hacky).
	static std::string GetStringFromWStringSimple(const wchar_t* wstring)
	{
		std::string outString = "";

		unsigned int offset = 0;
		while (*(wstring + offset) != 0x0000)
		{
			//const wchar_t* test = wstring + offset;
			outString += *(char*)(wstring + offset++) & 0x7F;
		}

		return outString;
	}


	//	Unique identifier for each supported game.
	typedef char GAME;

	//	Unique identifier for an actively unknown game.
	constexpr GAME UNKNOWN_GAME = -1;

	//	Not-a-game identifier.
	constexpr GAME NON_GAME = 0;

	//	Unique identifier for Ninja Gaiden Sigma 1.
	constexpr GAME SIGMA_1 = 1;

	//	Unique identifier for Ninja Gaiden Sigma 2.
	constexpr GAME SIGMA_2 = 2;

	//	Unique identifier for Ninja Gaiden 3: Razor's Edge.
	constexpr GAME RE_3 = 3;
	

	namespace Databin
	{
		namespace General
		{
			//	Type Ids of general purpose files.
			enum FileTypeId
			{
				DDS,
				databin,	//	The databin file name is not enough to infer which game it belongs to. Until determined, this will be the file's type.
				unknown
			};
		}

		namespace S1
		{
			//	Type Ids of files found in NGS1.
			enum FileTypeId
			{
				type_00 = 0x00,
				btl_dat = 0x01,
				type_02 = 0x02,
				chr_dat2 = 0x03,
				type_04 = 0x04,
				TMC_05 = 0x05,
				type_06 = 0x06,
				GT1G_07 = 0x07,			//	texture container, for 3d models?
				itm_dat2_08 = 0x08,
				MESSTR = 0x09,
				type_0A = 0x0A,
				chr_dat = 0x0B,
				rtm_dat = 0x0C,
				SND = 0x0D,
				itm_dat2_0E = 0x0E,
				stry_dat = 0x0F,
				TMC_10 = 0x10,			//	
				TMCL = 0x11,
				sprite = 0x12,
				GT1G_13 = 0x13,			//	texture container, for sprites?
				invalid = 0x14,
				databin,
				databinItem,
				tdpack_00,
				unknown
			};
		}

		namespace S2
		{
			//	Type Ids of files found in NGS2.
			enum FileTypeId
			{
				type_00 = 0x00,
				type_01 = 0x01,
				type_02 = 0x02,
				TDP4ACT = 0x03,
				TDP4CLD = 0x04,
				type_05 = 0x05,
				type_06 = 0x06,
				type_07 = 0x07,
				TMC_08 = 0x08,
				type_09 = 0x09,
				type_0A = 0x0A,
				TMC_0B = 0x0B,
				type_0C = 0x0C,
				itm_dat2 = 0x0D,
				type_0E = 0x0E,
				type_0F = 0x0F,
				type_10 = 0x10,
				chr_dat = 0x11,
				rtm_dat = 0x12,
				tdpack = 0x13,
				TDP4SOB = 0x14,
				TDP4SOC = 0x15,
				sprpack = 0x16,
				STAGEETC = 0x17,
				TDP4STY = 0x18,
				TNF = 0x19,
				type_1A = 0x1A,
				TMCL = 0x1B,
				type_1C = 0x1C,
				type_1D = 0x1D,
				type_1E = 0x1E,
				invalid = 0x1F,
				databin,
				databinItem,
				LANG_00,
				tdpack_00,
				XWSFILE_1C,
				tdpack_1C,
				unknown
			};
		}

		namespace RE
		{
			//	Type Ids of files found in NG3RE.
			enum FileTypeId
			{
				type_00 = 0x00,
				type_01 = 0x01,
				type_02 = 0x02,
				chr_dat2 = 0x03,
				type_04 = 0x04,
				type_05 = 0x05,
				TMC_06 = 0x06,
				TMCL_07 = 0x07,
				type_08 = 0x08,
				type_09 = 0x09,
				TMC_0A = 0x0A,
				type_0B = 0x0B,
				IBLPK = 0x0C,
				itm_dat2 = 0x0D,
				type_0E = 0x0E,
				type_0F = 0x0F,
				tdpack_10 = 0x10,
				chr_dat = 0x11,
				type_12 = 0x12,
				tdpack_13 = 0x13,
				TDP4SOB = 0x14,
				TDP4SOC = 0x15,
				sprpack = 0x16,
				type_17 = 0x17,
				STAGEETC = 0x18,
				stry_dat = 0x19,
				TNF = 0x1A,
				type_1B = 0x1B,
				TMCL_1C = 0x1C,
				XWSFILE = 0x1D,
				databin,
				databinItem,
				unknown
			};
		}
	}

	//	Returns the name of this general purpose FileType as a string.
	static std::string GetTypeName(Databin::General::FileTypeId id)
	{
		using namespace Databin::General;

		std::string output = "";

		switch (id)
		{
		case FileTypeId::databin:
		{
			output += "databin";
			break;
		}
		case FileTypeId::DDS:
		{
			output += "DDS";
			break;
		}
		case FileTypeId::unknown:
		{
			output += "unknown";
			break;
		}
		default:
		{
			output += std::format("{:02X}", (int)id);
			break;
		}
		}

		return output;
	}

	//	Returns the name of this Ninja Gaiden Sigma 1 FileType as a string.
	static std::string GetTypeName(Databin::S1::FileTypeId id)
	{
		using namespace Databin::S1;

		std::string output = "";

		switch (id)
		{
		case FileTypeId::tdpack_00:
		{
			output += "tdpack";
			break;
		}
		case FileTypeId::btl_dat:
		{
			output += "btl_dat";
			break;
		}
		case FileTypeId::chr_dat2:
		{
			output += "chr_dat2";
			break;
		}
		case FileTypeId::TMC_05:
		case FileTypeId::TMC_10:
		{
			output += "TMC";
			break;
		}
		case FileTypeId::GT1G_07:
		case FileTypeId::GT1G_13:
		{
			output += "GT1G";
			break;
		}
		case FileTypeId::itm_dat2_08:
		case FileTypeId::itm_dat2_0E:
		{
			output += "itm_dat2";
			break;
		}
		case FileTypeId::MESSTR:
		{
			output += "MESSTR";
			break;
		}
		case FileTypeId::chr_dat:
		{
			output += "chr_dat";
			break;
		}
		case FileTypeId::rtm_dat:
		{
			output += "rtm_dat";
			break;
		}
		case FileTypeId::SND:
		{
			output += "SND";
			break;
		}
		case FileTypeId::stry_dat:
		{
			output += "stry_dat";
			break;
		}
		case FileTypeId::TMCL:
		{
			output += "TMCL";
			break;
		}
		case FileTypeId::sprite:
		{
			output += "sprite";
			break;
		}
		case FileTypeId::invalid:
		{
			output += "invalid";
			break;
		}
		case FileTypeId::databin:
		{
			output += "databin";
			break;
		}
		case FileTypeId::databinItem:
		{
			output += "databinCompressedFile";
			break;
		}
		case FileTypeId::unknown:
		{
			output += "unknown";
			break;
		}
		default:
		{
			output += std::format("{:02X}", (int)id);
			break;
		}
		}

		return output;
	}

	//	Returns the name of this Ninja Gaiden Sigma 2 FileType as a string.
	static std::string GetTypeName(Databin::S2::FileTypeId id)
	{
		using namespace Databin::S2;

		std::string output = "";

		switch (id)
		{
		case FileTypeId::LANG_00:
		{
			output += "LANG";
			break;
		}
		case FileTypeId::TDP4ACT:
		{
			output += "TDP4ACT";
			break;
		}
		case FileTypeId::TDP4CLD:
		{
			output += "TDP4CLD";
			break;
		}
		case FileTypeId::TMC_08:
		case FileTypeId::TMC_0B:
		{
			output += "TMC";
			break;
		}
		case FileTypeId::itm_dat2:
		{
			output += "itm_dat2";
			break;
		}
		case FileTypeId::chr_dat:
		{
			output += "chr_dat";
			break;
		}
		case FileTypeId::rtm_dat:
		{
			output += "rtm_dat";
			break;
		}
		case FileTypeId::tdpack_00:
		case FileTypeId::tdpack_1C:
		case FileTypeId::tdpack:
		{
			output += "tdpack";
			break;
		}
		case FileTypeId::TDP4SOB:
		{
			output += "TDP4SOB";
			break;
		}
		case FileTypeId::TDP4SOC:
		{
			output += "TDP4SOC";
			break;
		}
		case FileTypeId::sprpack:
		{
			output += "sprpack";
			break;
		}
		case FileTypeId::STAGEETC:
		{
			output += "STAGEETC";
			break;
		}
		case FileTypeId::TDP4STY:
		{
			output += "TDP4STY";
			break;
		}
		case FileTypeId::TNF:
		{
			output += "TNF";
			break;
		}
		case FileTypeId::TMCL:
		{
			output += "TMCL";
			break;
		}
		case FileTypeId::XWSFILE_1C:
		{
			output += "XWSFILE";
			break;
		}
		case FileTypeId::databin:
		{
			output += "databin";
			break;
		}
		case FileTypeId::databinItem:
		{
			output += "databinCompressedFile";
			break;
		}
		case FileTypeId::unknown:
		{
			output += "unknown";
			break;
		}
		default:
		{
			output += std::format("{:02X}", (int)id);
			break;
		}
		}

		return output;
	}

	//	Returns the name of this Ninja Gaiden Sigma 2 FileType as a string.
	static std::string GetTypeName(Databin::RE::FileTypeId id)
	{
		using namespace Databin::RE;

		std::string output = "";

		switch (id)
		{
		case FileTypeId::chr_dat2:
		{
			output += "chr_dat2";
			break;
		}
		case FileTypeId::TMC_06:
		case FileTypeId::TMC_0A:
		{
			output += "TMC";
			break;
		}
		case FileTypeId::TMCL_07:
		case FileTypeId::TMCL_1C:
		{
			output += "TMCL";
			break;
		}
		case FileTypeId::IBLPK:
		{
			output += "IBLPK";
			break;
		}
		case FileTypeId::itm_dat2:
		{
			output += "itm_dat2";
			break;
		}
		case FileTypeId::tdpack_10:
		case FileTypeId::tdpack_13:
		{
			output += "tdpack";
			break;
		}
		case FileTypeId::chr_dat:
		{
			output += "chr_dat";
			break;
		}
		case FileTypeId::TDP4SOB:
		{
			output += "TDP4SOB";
			break;
		}
		case FileTypeId::TDP4SOC:
		{
			output += "TDP4SOC";
			break;
		}
		case FileTypeId::sprpack:
		{
			output += "sprpack";
			break;
		}
		case FileTypeId::STAGEETC:
		{
			output += "STAGEETC";
			break;
		}
		case FileTypeId::stry_dat:
		{
			output += "stry_dat";
			break;
		}
		case FileTypeId::TNF:
		{
			output += "TNF";
			break;
		}
		case FileTypeId::XWSFILE:
		{
			output += "XWSFILE";
			break;
		}
		case FileTypeId::databin:
		{
			output += "databin";
			break;
		}
		case FileTypeId::databinItem:
		{
			output += "databinCompressedFile";
			break;
		}
		case FileTypeId::unknown:
		{
			output += "unknown";
			break;
		}
		default:
		{
			output += std::format("{:02X}", (int)id);
			break;
		}
		}

		return output;
	}

	//	Returns the file extension of a FileType as a string.
	template <typename FileTypeId>
	static std::string GetFileExtension(FileTypeId id)
	{
		std::string output = "";

		output += GetTypeName(id);

		return output;
	}
	
	//	A class holding information on the type of a file object as well as helper methods.
	class FileType
	{
	public:
		FileType()
			: m_Id(-1), m_Game(UNKNOWN_GAME)
		{}

		FileType(Databin::General::FileTypeId id)
			: m_Id(id), m_Game(NON_GAME)
		{}
		
		FileType(Databin::S1::FileTypeId id)
			: m_Id(id), m_Game(SIGMA_1)
		{}
		
		FileType(Databin::S2::FileTypeId id)
			: m_Id(id), m_Game(SIGMA_2)
		{}
		
		FileType(Databin::RE::FileTypeId id)
			: m_Id(id), m_Game(RE_3)
		{}

		bool operator==(FileType& other) const
		{
			if (m_Game == other.GetGame())
			{
				if (m_Id == other.GetId())
				{
					return true;
				}
			}
			return false;
		}

		bool operator!=(FileType& other) const
		{
			return !operator==(other);
		}

		void SetType(Databin::General::FileTypeId id)
		{
			m_Game = NON_GAME;
			m_Id = id;
		}
		
		void SetType(Databin::S1::FileTypeId id)
		{
			m_Game = SIGMA_1;
			m_Id = id;
		}
		
		void SetType(Databin::S2::FileTypeId id)
		{
			m_Game = SIGMA_2;
			m_Id = id;
		}
		
		void SetType(Databin::RE::FileTypeId id)
		{
			m_Game = RE_3;
			m_Id = id;
		}

		void SetGame(GAME game)
		{
			m_Game = game;
		}
		
		void SetId(int id)
		{
			m_Id = id;
		}

		bool IsUnknown() const
		{
			if (
				(m_Game == UNKNOWN_GAME) ||
				(m_Game == NON_GAME && m_Id == Databin::General::FileTypeId::unknown) ||
				(m_Game == SIGMA_1 && m_Id == Databin::S1::FileTypeId::unknown) ||
				(m_Game == SIGMA_2 && m_Id == Databin::S2::FileTypeId::unknown) ||
				(m_Game == RE_3 && m_Id == Databin::RE::FileTypeId::unknown)
				)
				return true;
			else
				return false;
		}

		int GetId() const { return m_Id; }
		GAME GetGame() const { return m_Game; }

		//	Returns the name of this FileType as a string.
		std::string GetTypeName() const
		{
			using namespace Databin;

			std::string output = "";

			switch (m_Game)
			{
			case UNKNOWN_GAME:
			case NON_GAME:
			{
				output += NGMC::GetTypeName((General::FileTypeId)m_Id);
				break;
			}
			case SIGMA_1:
			{
				output += NGMC::GetTypeName((S1::FileTypeId)m_Id);
				break;
			}
			case SIGMA_2:
			{
				output += NGMC::GetTypeName((S2::FileTypeId)m_Id);
				break;
			}
			case RE_3:
			{
				output += NGMC::GetTypeName((RE::FileTypeId)m_Id);
				break;
			}
			default:
				output += "unsupportedGame";
			}

			return output;
		}

		//	Returns the file extension of this FileType as a string (kinda redundant now)
		std::string GetFileExtension() const
		{
			std::string output = "";

			output += GetTypeName();

			return output;
		}

	private:
		int m_Id;
		GAME m_Game;
	};


	//	Returns whether a file exists with the specified wide char string file name.
	inline bool IsFileExisting(const std::wstring& name)
	{
		if (FILE* file = _wfopen(name.c_str(), L"r")) {
			fclose(file);
			return true;
		}
		else {
			return false;
		}
	}

	//	Returns the size of a file located at the specified wide char string path.
	inline std::uintmax_t GetFileSize(const std::wstring& name)
	{
		if (IsFileExisting(name))
		{
			return std::filesystem::file_size(name);
		}
		return 0ULL;
	}

	//	Returns a name for a file located at the specified string path.
	inline std::string GetFileNameFromPath(std::string const& path)
	{
		return path.substr(path.find_last_of("/\\") + 1);
	}

	//	Returns a name for a file located at the specified wide char string path.
	inline std::string GetFileNameFromPath(std::wstring const& path)
	{
		return GetFileNameFromPath(std::string(path.begin(), path.end()));
	}


	/**
	 * @brief Open a dialog to select item(s) or folder(s).
	 * @param paths Specifies the reference to the string vector that will receive the file or folder path(s). [IN]
	 * @param selectFolder Specifies whether to select folder(s) rather than file(s). (optional)
	 * @param multiSelect Specifies whether to allow the user to select multiple items. (optional)
	 * @note If no item(s) were selected, the function still returns true, and the given vector is unmodified.
	 * @note `<windows.h>`, `<string>`, `<vector>`, `<shobjidl.h>`
	 * @return Returns true if all the operations are successfully performed, false otherwise.
	 */
	static bool OpenFileDialog(std::vector<std::wstring>& paths, bool selectFolder = false, bool multiSelect = false)
	{
		IFileOpenDialog* p_file_open = nullptr;
		bool are_all_operation_success = false;
		while (!are_all_operation_success)
		{
			HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
				IID_IFileOpenDialog, reinterpret_cast<void**>(&p_file_open));
			if (FAILED(hr))
				break;

			if (selectFolder || multiSelect)
			{
				FILEOPENDIALOGOPTIONS options = 0;
				hr = p_file_open->GetOptions(&options);
				if (FAILED(hr))
					break;

				if (selectFolder)
					options |= FOS_PICKFOLDERS;
				if (multiSelect)
					options |= FOS_ALLOWMULTISELECT;

				hr = p_file_open->SetOptions(options);
				if (FAILED(hr))
					break;
			}

			hr = p_file_open->Show(NULL);
			if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) // No items were selected.
			{
				are_all_operation_success = true;
				break;
			}
			else if (FAILED(hr))
				break;

			IShellItemArray* p_items;
			hr = p_file_open->GetResults(&p_items);
			if (FAILED(hr))
				break;
			DWORD total_items = 0;
			hr = p_items->GetCount(&total_items);
			if (FAILED(hr))
				break;

			for (int i = 0; i < static_cast<int>(total_items); ++i)
			{
				IShellItem* p_item;
				p_items->GetItemAt(i, &p_item);
				if (SUCCEEDED(hr))
				{
					PWSTR path;
					hr = p_item->GetDisplayName(SIGDN_FILESYSPATH, &path);
					if (SUCCEEDED(hr))
					{
						paths.push_back(path);
						CoTaskMemFree(path);
					}
					p_item->Release();
				}
			}

			p_items->Release();
			are_all_operation_success = true;
		}

		if (p_file_open)
			p_file_open->Release();
		return are_all_operation_success;
	}

	/**
	 * @brief Open a dialog to save an item.
	 * @param path Specifies the reference to the string that will receive the target save path. [IN]
	 * @param defaultFileName Specifies the default save file name. (optional)
	 * @param pFilterInfo Specifies the pointer to the pair that contains filter information. (optional)
	 * @note If no path was selected, the function still returns true, and the given string is unmodified.
	 * @note `<windows.h>`, `<string>`, `<vector>`, `<shobjidl.h>`
	 * @return Returns true if all the operations are successfully performed, false otherwise.
	 */
	static bool SaveFileDialog(std::wstring& path, std::wstring defaultFileName = L"", std::pair<COMDLG_FILTERSPEC*, int>* pFilterInfo = nullptr)
	{
		IFileSaveDialog* p_file_save = nullptr;
		bool are_all_operation_success = false;
		while (!are_all_operation_success)
		{
			HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
				IID_IFileSaveDialog, reinterpret_cast<void**>(&p_file_save));
			if (FAILED(hr))
				break;

			if (!pFilterInfo)
			{
				COMDLG_FILTERSPEC save_filter[1];
				save_filter[0].pszName = L"All files";
				save_filter[0].pszSpec = L"*.*";
				hr = p_file_save->SetFileTypes(1, save_filter);
				if (FAILED(hr))
					break;
				hr = p_file_save->SetFileTypeIndex(1);
				if (FAILED(hr))
					break;
			}
			else
			{
				hr = p_file_save->SetFileTypes(pFilterInfo->second, pFilterInfo->first);
				if (FAILED(hr))
					break;
				hr = p_file_save->SetFileTypeIndex(1);
				if (FAILED(hr))
					break;
			}

			if (!defaultFileName.empty())
			{
				hr = p_file_save->SetFileName(defaultFileName.c_str());
				if (FAILED(hr))
					break;
			}

			hr = p_file_save->Show(NULL);
			if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) // No item was selected.
			{
				are_all_operation_success = true;
				break;
			}
			else if (FAILED(hr))
				break;

			IShellItem* p_item;
			hr = p_file_save->GetResult(&p_item);
			if (FAILED(hr))
				break;

			PWSTR item_path;
			hr = p_item->GetDisplayName(SIGDN_FILESYSPATH, &item_path);
			if (FAILED(hr))
				break;
			path = item_path;
			CoTaskMemFree(item_path);
			p_item->Release();

			are_all_operation_success = true;
		}

		if (p_file_save)
			p_file_save->Release();
		return are_all_operation_success;
	}

	/**
	 * @brief Open a dialog to open a folder, to save items to it.
	 * @param path Specifies the reference to the string that will receive the target folder path. [IN]
	 * @param pFilterInfo Specifies the pointer to the pair that contains filter information. (optional, unused with folders? idk)
	 * @note If no path was selected, the function still returns true, and the given string is unmodified.
	 * @note `<windows.h>`, `<string>`, `<vector>`, `<shobjidl.h>`
	 * @return Returns true if all the operations are successfully performed, false otherwise.
	 */
	static bool OpenFolderDialog(std::wstring& path, std::pair<COMDLG_FILTERSPEC*, int>* pFilterInfo = nullptr)
	{
		IFileDialog* p_folder_open = nullptr;
		bool are_all_operation_success = false;
		while (!are_all_operation_success)
		{
			HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
				IID_PPV_ARGS(&p_folder_open));
			if (FAILED(hr))
				break;

			DWORD dwFlags;
			hr = p_folder_open->GetOptions(&dwFlags);
			if (SUCCEEDED(hr))
			{
				hr = p_folder_open->SetOptions(dwFlags | FOS_PICKFOLDERS);
				if (FAILED(hr))
					break;
			}

			hr = p_folder_open->Show(NULL);
			if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) // No item was selected.
			{
				are_all_operation_success = true;
				break;
			}
			else if (FAILED(hr))
				break;

			IShellItem* p_item;
			hr = p_folder_open->GetResult(&p_item);
			if (FAILED(hr))
				break;

			PWSTR item_path;
			hr = p_item->GetDisplayName(SIGDN_FILESYSPATH, &item_path);
			if (FAILED(hr))
				break;
			path = item_path;
			CoTaskMemFree(item_path);
			p_item->Release();

			are_all_operation_success = true;
		}

		if (p_folder_open)
			p_folder_open->Release();
		return are_all_operation_success;
	}
}