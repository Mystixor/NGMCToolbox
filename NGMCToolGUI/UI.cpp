#include "stdafx.h"

#include "UI.h"

#include "utility/Utility.h"
#include "Previewer.h"

#include "popups/PopupBuildGT1G.h"
#include "popups/PopupRenameFile.h"
#include "popups/PopupFilePicker.h"
#include "popups/PopupAbout.h"
#include "popups/PopupSelectGame.h"

#include "loaders/LoaderLANG.h"


namespace NGMC
{
	bool g_ReadColorMapAsLuminance = false;

	//	The FileManager object managing all files loaded on the top-level in the application.
	FileManager fileManager;

	//	The Previewer object handling the contents of a Previewer window.
	Previewer previewer;


	//	The Popup object handling the building of a GT1G file.
	PopupBuildGT1G g_PopupBuildGT1G;

	//	The Popup object handling the renaming of a file in the content viewer.
	PopupRenameFile g_PopupRenameFile;

	//	The Popup object handling the display of general information concerning the application.
	PopupAbout g_PopupAbout;

	//	The Popup object handling the selection of a game for a file of which the game could not be infered.
	PopupSelectGame g_PopupSelectGame;

	//	The function handling the rendering of all top-level Popups.
	static void HandlePopups()
	{
		g_PopupBuildGT1G.OnRender();
		g_PopupRenameFile.OnRender();
		g_PopupAbout.OnRender();
		g_PopupSelectGame.OnRender();
	}


	// Our state

	//	Whether the ImGui Demo Window is being shown.
	static bool show_demo_window = false;

	//	The class defining the behavior of windows in the dockspace.
	static ImGuiWindowClass dockspaceWindowClass;

	//	The Flags defining the behavior of windows in the dockspace.
	static ImGuiWindowFlags dockspaceWindowFlags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove;

	//	The function drawing an ImGui::MenuItem for each FileType in the tool, loading a type into the specified variable if the respective MenuItem is clicked.
	static void OnImGuiNGMCFileTypeMenuItems(FileType& type)
	{
		if (ImGui::BeginMenu("NGS"))
		{
			using namespace Databin::S1;
			if (ImGui::MenuItem("databin"))
			{
				type.SetType(FileTypeId::databin);
			}
			if (ImGui::MenuItem("tdpack"))
			{
				type.SetType(FileTypeId::tdpack_00);
			}
			if (ImGui::MenuItem("btl_dat"))
			{
				type.SetType(FileTypeId::btl_dat);
			}
			if (ImGui::MenuItem("type_02"))
			{
				type.SetType(FileTypeId::type_02);
			}
			if (ImGui::MenuItem("chr_dat2"))
			{
				type.SetType(FileTypeId::chr_dat2);
			}
			if (ImGui::MenuItem("type_04"))
			{
				type.SetType(FileTypeId::type_04);
			}
			if (ImGui::MenuItem("TMC_05"))
			{
				type.SetType(FileTypeId::TMC_05);
			}
			if (ImGui::MenuItem("type_06"))
			{
				type.SetType(FileTypeId::type_06);
			}
			if (ImGui::MenuItem("GT1G_07"))
			{
				type.SetType(FileTypeId::GT1G_07);
			}
			if (ImGui::MenuItem("itm_dat2_08"))
			{
				type.SetType(FileTypeId::itm_dat2_08);
			}
			if (ImGui::MenuItem("MESSTR"))
			{
				type.SetType(FileTypeId::MESSTR);
			}
			if (ImGui::MenuItem("type_0A"))
			{
				type.SetType(FileTypeId::type_0A);
			}
			if (ImGui::MenuItem("chr_dat"))
			{
				type.SetType(FileTypeId::chr_dat);
			}
			if (ImGui::MenuItem("rtm_dat"))
			{
				type.SetType(FileTypeId::rtm_dat);
			}
			if (ImGui::MenuItem("SND"))
			{
				type.SetType(FileTypeId::SND);
			}
			if (ImGui::MenuItem("itm_dat2_0E"))
			{
				type.SetType(FileTypeId::itm_dat2_0E);
			}
			if (ImGui::MenuItem("stry_dat"))
			{
				type.SetType(FileTypeId::stry_dat);
			}
			if (ImGui::MenuItem("TMC_10"))
			{
				type.SetType(FileTypeId::TMC_10);
			}
			if (ImGui::MenuItem("TMCL"))
			{
				type.SetType(FileTypeId::TMCL);
			}
			if (ImGui::MenuItem("sprite"))
			{
				type.SetType(FileTypeId::sprite);
			}
			if (ImGui::MenuItem("GT1G_13"))
			{
				type.SetType(FileTypeId::GT1G_13);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("NGS2"))
		{
			using namespace Databin::S2;
			if (ImGui::MenuItem("databin"))
			{
				type.SetType(FileTypeId::databin);
			}
			if (ImGui::MenuItem("type_00"))
			{
				type.SetType(FileTypeId::type_00);
			}
			if (ImGui::MenuItem("tdpack_00"))
			{
				type.SetType(FileTypeId::tdpack_00);
			}
			if (ImGui::MenuItem("LANG"))
			{
				type.SetType(FileTypeId::LANG_00);
			}
			if (ImGui::MenuItem("type_01"))
			{
				type.SetType(FileTypeId::type_01);
			}
			if (ImGui::MenuItem("type_02"))
			{
				type.SetType(FileTypeId::type_02);
			}
			if (ImGui::MenuItem("TDP4ACT"))
			{
				type.SetType(FileTypeId::TDP4ACT);
			}
			if (ImGui::MenuItem("TDP4CLD"))
			{
				type.SetType(FileTypeId::TDP4CLD);
			}
			if (ImGui::MenuItem("type_05"))
			{
				type.SetType(FileTypeId::type_05);
			}
			if (ImGui::MenuItem("type_06"))
			{
				type.SetType(FileTypeId::type_06);
			}
			if (ImGui::MenuItem("type_07"))
			{
				type.SetType(FileTypeId::type_07);
			}
			if (ImGui::MenuItem("TMC_08"))
			{
				type.SetType(FileTypeId::TMC_08);
			}
			if (ImGui::MenuItem("type_09"))
			{
				type.SetType(FileTypeId::type_09);
			}
			if (ImGui::MenuItem("type_0A"))
			{
				type.SetType(FileTypeId::type_0A);
			}
			if (ImGui::MenuItem("TMC_0B"))
			{
				type.SetType(FileTypeId::TMC_0B);
			}
			if (ImGui::MenuItem("type_0C"))
			{
				type.SetType(FileTypeId::type_0C);
			}
			if (ImGui::MenuItem("itm_dat2"))
			{
				type.SetType(FileTypeId::itm_dat2);
			}
			if (ImGui::MenuItem("type_0E"))
			{
				type.SetType(FileTypeId::type_0E);
			}
			if (ImGui::MenuItem("type_0F"))
			{
				type.SetType(FileTypeId::type_0F);
			}
			if (ImGui::MenuItem("type_10"))
			{
				type.SetType(FileTypeId::type_10);
			}
			if (ImGui::MenuItem("chr_dat"))
			{
				type.SetType(FileTypeId::chr_dat);
			}
			if (ImGui::MenuItem("rtm_dat"))
			{
				type.SetType(FileTypeId::rtm_dat);
			}
			if (ImGui::MenuItem("tdpack"))
			{
				type.SetType(FileTypeId::tdpack);
			}
			if (ImGui::MenuItem("TDP4SOB"))
			{
				type.SetType(FileTypeId::TDP4SOB);
			}
			if (ImGui::MenuItem("TDP4SOC"))
			{
				type.SetType(FileTypeId::TDP4SOC);
			}
			if (ImGui::MenuItem("sprpack"))
			{
				type.SetType(FileTypeId::sprpack);
			}
			if (ImGui::MenuItem("STAGEETC"))
			{
				type.SetType(FileTypeId::STAGEETC);
			}
			if (ImGui::MenuItem("TDP4STY"))
			{
				type.SetType(FileTypeId::TDP4STY);
			}
			if (ImGui::MenuItem("TNF"))
			{
				type.SetType(FileTypeId::TNF);
			}
			if (ImGui::MenuItem("type_1A"))
			{
				type.SetType(FileTypeId::type_1A);
			}
			if (ImGui::MenuItem("TMCL"))
			{
				type.SetType(FileTypeId::TMCL);
			}
			if (ImGui::MenuItem("XWSFILE"))
			{
				type.SetType(FileTypeId::XWSFILE_1C);
			}
			if (ImGui::MenuItem("tdpack_1C"))
			{
				type.SetType(FileTypeId::tdpack_1C);
			}
			if (ImGui::MenuItem("type_1D"))
			{
				type.SetType(FileTypeId::type_1D);
			}
			if (ImGui::MenuItem("type_1E"))
			{
				type.SetType(FileTypeId::type_1E);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("NG3RE"))
		{
			using namespace Databin::RE;
			if (ImGui::MenuItem("databin"))
			{
				type.SetType(FileTypeId::databin);
			}

			ImGui::EndMenu();
		}
	}

	//	The function drawing the menu bar of the application.
	static void OnImGuiMenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::BeginMenu("New..."))
				{
					FileType type = FileType();
					//OnImGuiNGMCFileTypeMenuItems(type);
					if (ImGui::BeginMenu("NGS"))
					{
						using namespace Databin::S1;
						if (ImGui::MenuItem("GT1G"))
						{
							type.SetType(FileTypeId::GT1G_07);
						}

						ImGui::EndMenu();
					}
					if (ImGui::BeginMenu("NGS2"))
					{
						using namespace Databin::S2;
						if (ImGui::MenuItem("LANG"))
						{
							type.SetType(FileTypeId::LANG_00);
						}

						ImGui::EndMenu();
					}

					ImGui::EndMenu();

					if (!type.IsUnknown())
					{
						switch (type.GetGame())
						{
						case SIGMA_1:
						{
							using namespace Databin::S1;
							switch (type.GetId())
							{
							case FileTypeId::GT1G_07:
							case FileTypeId::GT1G_13:
							{
								g_PopupBuildGT1G.Setup();
								g_PopupBuildGT1G.Open();
								break;
							}
							}
							break;
						}
						case SIGMA_2:
							using namespace Databin::S2;
							switch (type.GetId())
							{
							case FileTypeId::LANG_00:
							{
								std::vector<std::wstring> filePaths;
								if (OpenFileDialog(filePaths, false, false))
								{
									if (filePaths.size())
										ImportFileFromJSON(filePaths[0].c_str());
								}
							}
							}
						}
					}
				}

				if (ImGui::MenuItem("Open"))
				{
					std::vector<std::wstring> filePaths;
					if (OpenFileDialog(filePaths, false, false))
					{
						if (filePaths.size())
							fileManager.RegisterFile(filePaths[0].c_str());
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Options"))
			{
				ImGui::Separator();

				ImGui::TextUnformatted("GT1G:");
				if (ImGui::MenuItem("Render Color Map as raw data (color index)", NULL, &g_ReadColorMapAsLuminance))
				{
					previewer.Prepare();
				}

				ImGui::Separator();

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("About"))
			{
				g_PopupAbout.Setup();
				g_PopupAbout.Open();
			}

			ImGui::EndMenuBar();
		}
	}


	//	The actions possible in the context menu of files.
	static enum ContextMenuFileAction
	{
		None,
		Load,
		Unload,
		Save,
		Extract
	};

	//	The function drawing and handling the context menu in the Content Viewer window.
	static void OnImGuiNGMCFileContextMenu()
	{
		size_t selectionCount = fileManager.GetSelectionCount();
		if (selectionCount == 0)
		{

		}
		else
		{
			ContextMenuFileAction action = ContextMenuFileAction::None;

			if (selectionCount == 1)
			{
				File* p_File = fileManager.GetSelectedFile(0);
				FileType type = p_File->GetType();

				if (p_File->IsLoaded())
				{
					if (ImGui::Selectable("Unload"))
					{
						action = ContextMenuFileAction::Unload;
					}
				}
				else
				{
					if (ImGui::Selectable("Load"))
					{
						action = ContextMenuFileAction::Load;
					}
				}
				if (p_File->IsFileInMemory())
				{
					if (ImGui::Selectable("Save"))
					{
						action = ContextMenuFileAction::Save;
					}
				}

				switch (type.GetGame())
				{
				case SIGMA_1:
				{
					using namespace Databin::S1;
					switch (type.GetId())
					{
					case FileTypeId::databin:
					case FileTypeId::databinItem:
					case FileTypeId::GT1G_07:
					case FileTypeId::GT1G_13:
						if (ImGui::Selectable("Extract"))
						{
							action = ContextMenuFileAction::Extract;
						}
						break;
					}
					break;
				}
				case SIGMA_2:
				{
					using namespace Databin::S2;
					switch (type.GetId())
					{
					case FileTypeId::databin:
					case FileTypeId::databinItem:
					case FileTypeId::LANG_00:
						if (ImGui::Selectable("Extract"))
						{
							action = ContextMenuFileAction::Extract;
						}
						break;
					}
					break;
				}
				case RE_3:
				{
					using namespace Databin::RE;
					switch (type.GetId())
					{
					case FileTypeId::databin:
					case FileTypeId::databinItem:
						if (ImGui::Selectable("Extract"))
						{
							action = ContextMenuFileAction::Extract;
						}
						break;
					}
					break;
				}
				}

				if (ImGui::BeginMenu("Set type to..."))
				{
					OnImGuiNGMCFileTypeMenuItems(type);
					if (ImGui::MenuItem("DDS"))
					{
						type.SetType(Databin::General::FileTypeId::DDS);
					}

					p_File->SetType(type);

					ImGui::EndMenu();
				}
				if (ImGui::Selectable("Rename"))
				{
					g_PopupRenameFile.Setup(p_File);
					g_PopupRenameFile.Open();
				}

				ImGui::Separator();
				ImGui::TextUnformatted((std::string("Type: ") + type.GetTypeName() + std::format(" (0x{:02X})", (char)type.GetId())).c_str());
				ImGui::TextUnformatted((std::string("Size: ") + GetPrettySize(p_File->GetSize())).c_str());
				ImGui::Separator();
			}
			else if (selectionCount > 1)
			{
				if (ImGui::Selectable("Load"))
				{
					action = ContextMenuFileAction::Load;
				}
				if (ImGui::Selectable("Unload"))
				{
					action = ContextMenuFileAction::Unload;
				}
				if (ImGui::Selectable("Save"))
				{
					action = ContextMenuFileAction::Save;
				}

				ImGui::Separator();
				ImGui::TextUnformatted(std::format("{} items selected", fileManager.GetSelectionCount()).c_str());

				uintmax_t size = 0ULL;
				for (unsigned int i = 0; i < selectionCount; i++)
				{
					size += fileManager.GetSelectedFile(i)->GetSize();
				}
				ImGui::TextUnformatted((std::string("Size: ") + GetPrettySize(size)).c_str());
				ImGui::Separator();
			}

			switch (action)
			{
			case ContextMenuFileAction::Load:
			{
				for (unsigned int i = 0; i < selectionCount; i++)
				{
					fileManager.GetSelectedFile(i)->Load();

					if (fileManager.GetSelectedFile(i)->GetChildCount())
						fileManager.GetSelectedFile(i)->SetNodeOpenness(true);
				}
				break;
			}
			case ContextMenuFileAction::Unload:
			{
				for (unsigned int i = 0; i < selectionCount; i++)
				{
					fileManager.GetSelectedFile(i)->Unload();
				}
				break;
			}
			case ContextMenuFileAction::Save:
			{
				if (selectionCount == 1)
				{
					File* p_File = fileManager.GetSelectedFile(0);

					std::wstring savePath;
					std::string tempName = p_File->GetName();
					std::wstring defaultName = std::wstring(tempName.begin(), tempName.end());
					if (SaveFileDialog(savePath, defaultName))
					{
						p_File->Save(savePath.c_str());
					}
				}
				else
				{
					std::wstring savePath;
					if (OpenFolderDialog(savePath))
					{
						File* p_File;
						savePath += L"\\";
						for (unsigned int i = 0; i < selectionCount; i++)
						{
							p_File = fileManager.GetSelectedFile(i);
							std::string tempName = p_File->GetName();
							std::wstring saveName = std::wstring(tempName.begin(), tempName.end());
							p_File->Save((savePath + saveName).c_str());
						}
					}
				}
				break;
			}
			case ContextMenuFileAction::Extract:
			{
				std::wstring savePath;
				if (OpenFolderDialog(savePath))
				{
					if (savePath.length())
					{
						savePath += L"\\";
						for (unsigned int i = 0; i < selectionCount; i++)
						{
							File* p_File = fileManager.GetSelectedFile(i);

							p_File->Extract(savePath.c_str());
						}
					}
				}
				break;
			}
			}
		}
	}

	//	Whether a context menu is active.
	static bool IsContextMenu = false;

	//	Whether the context menu was activated when a CTRL key was held.
	static bool IsContextMenuCtrlClick = false;


	//	The ImGuiListClipper object responsible for clipping the contents of the Content Viewer to just the visible, greatly helping performance when thousands of files are registered.
	static ImGuiListClipper clipper;

	//	Whether the ImGuiListClipper object is still stepping, estimating how many objects will keep fitting in the visible space of the window.
	static bool IsStepping = true;

	//	The count of items exposed in the Content Viewer.
	static int listItemCount = 0;


	//	Whether the Content Window is focused.
	static bool IsContentWindowFocused = false;

	//	The function drawing and handling file objects in the Content Window.
	static void OnImGuiNGMCFile(File* p_File, bool isNotClipped)
	{
		unsigned int childCount = p_File->GetChildCount();

		//	Never clip nodes that are opened so that its children do not vanish when their parent is no longer on-screen
		if (!isNotClipped && p_File->IsNodeOpened())
		{
			isNotClipped = true;
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y));
		}

		bool opened = false;
		if (isNotClipped)
		{
			//	Get display name and unique identifier for ImGui
			const char* name = p_File->GetName();
			std::string identifier = std::string(name) + std::format("##{:X}", (uintptr_t)p_File);

			bool isSelected = fileManager.IsFileSelected(p_File);

			ImGuiTreeNodeFlags flags =
				ImGuiTreeNodeFlags_OpenOnArrow |
				ImGuiTreeNodeFlags_OpenOnDoubleClick |
				ImGuiTreeNodeFlags_SpanAvailWidth |
				ImGuiTreeNodeFlags_NavLeftJumpsBackHere;

			if (childCount == 0)
			{
				flags |= ImGuiTreeNodeFlags_Leaf;
			}
			if (isSelected)
			{
				flags |= ImGuiTreeNodeFlags_Selected;
			}


			if (p_File->IsNodeOpened())
			{
				ImGui::SetNextItemOpen(true, ImGuiCond_Always);
			}
			opened = ImGui::TreeNodeEx(identifier.c_str(), flags);


			//	Right-clicking
			if (ImGui::BeginPopupContextItem())
			{
				//	Releasing right-click enables the context menu
				IsContextMenu = true;
				if (!IsContextMenuCtrlClick)
				{
					if (!ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && !ImGui::IsKeyDown(ImGuiKey_RightCtrl))
					{
						fileManager.SelectFile(p_File, true);
					}
					else
					{
						IsContextMenuCtrlClick = true;
					}
				}
				OnImGuiNGMCFileContextMenu();

				childCount = p_File->GetChildCount();

				ImGui::EndPopup();
			}

			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
			{
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					//	Single left-click selects the file
					if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
					{
						//	Holding Ctrl keeps previous selection and toggles selection of this file
						if (isSelected)
							fileManager.DeselectFile(p_File);
						else
							fileManager.SelectFile(p_File);
					}
					else
						fileManager.SelectFile(p_File, true);
				}
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					//	Double left-click selects and loads the file
					fileManager.SelectFile(p_File, true);
					p_File->Load();
					childCount = p_File->GetChildCount();
				}
			}

			if (IsContentWindowFocused)
			{
				if (isSelected && (ImGui::IsKeyPressed(ImGuiKey_Enter, false) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false)))
				{
					//	Enter key-press loads the file
					p_File->Load();
					childCount = p_File->GetChildCount();
				}
			}

			if (childCount)
				p_File->SetNodeOpenness(opened);

			if (IsStepping && listItemCount + 1 == clipper.DisplayEnd)
			{
				IsStepping = clipper.Step();
			}
		}

		listItemCount++;

		if (opened)
		{
			for (unsigned int i = 0; i < childCount; i++)
			{
				OnImGuiNGMCFile(p_File->GetChild(i), clipper.DisplayStart - 1 <= listItemCount && listItemCount < clipper.DisplayEnd + 1);
			}

			if (isNotClipped)
				ImGui::TreePop();
		}
	}

	//	Whether the keys CTRL and A are held.
	static bool IsCtrlAHeld = false;

	//	The function drawing and handling the Content Window.
	static void OnImGuiContentWindow()
	{
		ImGui::SetNextWindowClass(&dockspaceWindowClass);
		ImGui::Begin("ContentViewer", NULL, dockspaceWindowFlags);

		IsContentWindowFocused = ImGui::IsWindowFocused();

		if (IsContentWindowFocused && !IsContextMenu)
		{
			//	If no context menu is active: check if Ctrl + A was held on previous frame
			if (IsCtrlAHeld)
			{
				//	If Ctrl + A was held on previous frame: check if it's still held
				if (!(ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) || !ImGui::IsKeyDown(ImGuiKey_A))
				{
					IsCtrlAHeld = false;
				}
			}
			else
			{
				//	If Ctrl + A wasn't held on previous frame: check for Ctrl + A
				if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) && ImGui::IsKeyDown(ImGuiKey_A))
				{
					//	If Ctrl + A is pressed, then select all files
					IsCtrlAHeld = true;
					fileManager.SelectAllFiles();
				}
			}
		}

		int fileIndex = 0;
		IsContextMenu = false;

		clipper.Begin(listItemCount);
		listItemCount = 0U;
		IsStepping = clipper.Step();
		for (unsigned int i = 0; i < fileManager.GetFileCount(); i++)
		{
			OnImGuiNGMCFile(fileManager.GetFile(i), clipper.DisplayStart - 1 <= listItemCount && listItemCount < clipper.DisplayEnd + 1);
		}
		clipper.End();

		IsContextMenuCtrlClick = IsContextMenu && IsContextMenuCtrlClick;

		ImGui::End();
	}

	//	The function drawing and handling the Utility Window (only useful during development for now).
	static void OnImGuiUtilityWindow(ImGuiIO& io)
	{
		ImGui::SetNextWindowClass(&dockspaceWindowClass);

		ImGui::Begin("Utility", NULL, dockspaceWindowFlags);

		ImGui::Checkbox("Demo Window", &show_demo_window);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

		//ImGui::Dummy(ImVec2(0.0f, 2.0f));
		//ImGui::Separator();
		//ImGui::Dummy(ImVec2(0.0f, 2.0f));
		//
		//ImGui::TextUnformatted(std::format("Popup count: {}", popups.size()).c_str());

		ImGui::End();
	}

	//	The function drawing and handling the Preview Window.
	static void OnImGuiPreviewWindow()
	{
		ImGui::SetNextWindowClass(&dockspaceWindowClass);

		ImGui::Begin("Preview", NULL, dockspaceWindowFlags);

		size_t selectionCount = fileManager.GetSelectionCount();
		if (selectionCount == 1)
		{
			previewer.SetFile(fileManager.GetSelectedFile(0));
		}
		else if (selectionCount == 0)
		{
			previewer.SetFile(nullptr);
		}

		previewer.OnRender();

		ImGui::End();
	}

	void OnImGuiRender(ImGuiIO& io)
	{
		//  Dockspace
		{
			static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
			// because it would be confusing to have two docking targets within each others.
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


			// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
				window_flags |= ImGuiWindowFlags_NoBackground;

			// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
			// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
			// all active windows docked into it will lose their parent and become undocked.
			// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
			// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace", nullptr, window_flags);
			ImGui::PopStyleVar();
			ImGui::PopStyleVar(2);

			OnImGuiMenuBar();

			// DockSpace
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

				static auto first_time = true;
				if (first_time)
				{
					first_time = false;

					ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
					ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
					ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

					// split the dockspace into 2 nodes -- DockBuilderSplitNode takes in the following args in the following order
					//   window ID to split, direction, fraction (between 0 and 1), the final two setting let's us choose which id we want (which ever one we DON'T set as NULL, will be returned by the function)
					//                                                              out_id_at_dir is the id of the node in the direction we specified earlier, out_id_at_opposite_dir is in the opposite direction
					auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);
					auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);

					// we now dock our windows into the docking node we made above
					ImGui::DockBuilderDockWindow("ContentViewer", dock_id_left); //	Node split out to the left
					ImGui::DockBuilderDockWindow("Utility", dock_id_down);		//	Node split out downwards, out of the Node split out to the right
					ImGui::DockBuilderDockWindow("Preview", dockspace_id);		//	Node filling up the rest
					ImGui::DockBuilderFinish(dockspace_id);
				}
			}
			ImGui::End();

			dockspaceWindowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		}

		OnImGuiContentWindow();

		//	Debug purposes
		//OnImGuiUtilityWindow(io);

		OnImGuiPreviewWindow();

		//  Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		{
			if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);
		}

		HandlePopups();
	}
}
