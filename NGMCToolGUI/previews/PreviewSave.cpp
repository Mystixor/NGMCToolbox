#include "stdafx.h"

#include "PreviewSave.h"

namespace NGMC
{
	/*SubPreviewSave::SubPreviewSave(File& file)
		: m_File(file), m_IsSetup(false)
	{
	}

	bool SubPreviewSave::IsSetup() const
	{
		return m_IsSetup;
	}

	bool SubPreviewSave::Prepare()
	{
		return m_IsSetup;
	}*/

	class PreviewSystemSaveS1 final : public BasePreview
	{
	public:
		PreviewSystemSaveS1(File& file)
			: BasePreview(file)
		{
		}

		void OnRender() override
		{
			ImGui::PushID(this);
			{
				if (ImGui::BeginTable("tablePreviewerSaveData", 2, Preview::tableFlags))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("x");
					ImGui::TableSetColumnIndex(1); ImGui::Text("y");

					ImGui::EndTable();
				}
			}
			ImGui::PopID();
		}
	};
	
	class PreviewGameSaveS1 final : public BasePreview
	{
	public:
		PreviewGameSaveS1(File& file)
			: BasePreview(file)
		{
		}

		void OnRender() override
		{
			ImGui::PushID(this);
			{
				if (ImGui::BeginTable("tablePreviewerSaveData", 2, Preview::tableFlags))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("x");
					ImGui::TableSetColumnIndex(1); ImGui::Text("y");

					ImGui::EndTable();
				}
			}
			ImGui::PopID();
		}
	};
	
	class PreviewSystemSaveS2 final : public BasePreview
	{
	public:
		PreviewSystemSaveS2(File& file)
			: BasePreview(file)
		{
		}

		void OnRender() override
		{
			ImGui::PushID(this);
			{
				if (ImGui::BeginTable("tablePreviewerSaveData", 2, Preview::tableFlags))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("x");
					ImGui::TableSetColumnIndex(1); ImGui::Text("y");

					ImGui::EndTable();
				}
			}
			ImGui::PopID();
		}
	};
	
	class PreviewStorySaveS2 final : public BasePreview
	{
	public:
		PreviewStorySaveS2(File& file)
			: BasePreview(file)
		{
		}

		void OnRender() override
		{
			ImGui::PushID(this);
			{
				if (ImGui::BeginTable("tablePreviewerSaveData", 2, Preview::tableFlags))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("x");
					ImGui::TableSetColumnIndex(1); ImGui::Text("y");

					ImGui::EndTable();
				}
			}
			ImGui::PopID();
		}
	};
	
	class PreviewSystemSave2B final : public BasePreview
	{
	public:
		PreviewSystemSave2B(File& file)
			: BasePreview(file),
			m_Completion({}), m_IsChapterChallengeUnlock(false), m_IsStoryMasterNinjaUnlock(false),
			m_TagMissionUnlocks({}), m_TagMissionWeaponUnlocks({}), m_TagMissionNinpoUnlocks({}), m_IsTagMissionHowlingCannonUnlock(false)
		{
		}

		void OnRender() override
		{
			ImGui::PushID(this);
			{
				if (ImGui::BeginTable("tablePreviewerSaveData", 2, Preview::tableFlags))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Completion");
					ImGui::TableSetColumnIndex(1);
					if (ImGui::BeginTable("tablePreviewerSaveDataCompletion", 2, Preview::tableFlags))
					{
						unsigned char heroCompletion = 0;
						unsigned char acolyteCompletion = 0;
						unsigned char warriorCompletion = 0;
						unsigned char mentorCompletion = 0;
						unsigned char masterNinjaCompletion = 0;
						for (int i = 0; i < 17; i++)
						{
							heroCompletion += *(uint32_t*)&m_Completion.Hero >> i & 1;
							acolyteCompletion += *(uint32_t*)&m_Completion.Acolyte >> i & 1;
							warriorCompletion += *(uint32_t*)&m_Completion.Warrior >> i & 1;
							mentorCompletion += *(uint32_t*)&m_Completion.Mentor >> i & 1;
							masterNinjaCompletion += *(uint32_t*)&m_Completion.MasterNinja >> i & 1;
						}
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Hero Mode");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("Chapter {:d}", heroCompletion).c_str());
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Acolyte");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("Chapter {:d}", acolyteCompletion).c_str());
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Warrior");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("Chapter {:d}", warriorCompletion).c_str());
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Mentor");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("Chapter {:d}", mentorCompletion).c_str());
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Master Ninja");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("Chapter {:d}", masterNinjaCompletion).c_str());

						ImGui::EndTable();
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Chapter Challenge unlocked");
					ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_IsChapterChallengeUnlock).c_str());
					
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Master Ninja (Story Mode) unlocked");
					ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_IsStoryMasterNinjaUnlock).c_str());
					
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Tag Missions unlocked");
					ImGui::TableSetColumnIndex(1);
					if (ImGui::TreeNodeEx("Acolyte", ImGuiTreeNodeFlags_SpanAvailWidth))
					{
						if (ImGui::BeginTable("tablePreviewerSaveDataTagMissionUnlocks", 2, Preview::tableFlags))
						{
							for (int i = 0; i < 10; i++)
							{
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0); ImGui::Text(std::format("Mission {:02d}", i + 1).c_str());
								ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_TagMissionUnlocks[0 + i]).c_str());
							}
							ImGui::EndTable();
						}
						ImGui::TreePop();
					}
					if (ImGui::TreeNodeEx("Warrior", ImGuiTreeNodeFlags_SpanAvailWidth))
					{
						if (ImGui::BeginTable("tablePreviewerSaveDataTagMissionUnlocks", 2, Preview::tableFlags))
						{
							for (int i = 0; i < 10; i++)
							{
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0); ImGui::Text(std::format("Mission {:02d}", i + 1).c_str());
								ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_TagMissionUnlocks[10 + i]).c_str());
							}
							ImGui::EndTable();
						}
						ImGui::TreePop();
					}
					if (ImGui::TreeNodeEx("Mentor", ImGuiTreeNodeFlags_SpanAvailWidth))
					{
						if (ImGui::BeginTable("tablePreviewerSaveDataTagMissionUnlocks", 2, Preview::tableFlags))
						{
							for (int i = 0; i < 5; i++)
							{
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0); ImGui::Text(std::format("Mission {:02d}", i + 1).c_str());
								ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_TagMissionUnlocks[20 + i]).c_str());
							}
							ImGui::EndTable();
						}
						ImGui::TreePop();
					}
					if (ImGui::TreeNodeEx("Master Ninja", ImGuiTreeNodeFlags_SpanAvailWidth))
					{
						if (ImGui::BeginTable("tablePreviewerSaveDataTagMissionUnlocks", 2, Preview::tableFlags))
						{
							for (int i = 0; i < 5; i++)
							{
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0); ImGui::Text(std::format("Mission {:02d}", i + 1).c_str());
								ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_TagMissionUnlocks[30 + i]).c_str());
							}
							ImGui::EndTable();
						}
						ImGui::TreePop();
					}
					if (ImGui::TreeNodeEx("Ultimate Ninja", ImGuiTreeNodeFlags_SpanAvailWidth))
					{
						if (ImGui::BeginTable("tablePreviewerSaveDataTagMissionUnlocks", 2, Preview::tableFlags))
						{
							for (int i = 0; i < 5; i++)
							{
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0); ImGui::Text(std::format("Mission {:02d}", i + 1).c_str());
								ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_TagMissionUnlocks[40 + i]).c_str());
							}
							ImGui::EndTable();
						}
						ImGui::TreePop();
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Weapons (Tag Missions) unlocked");
					ImGui::TableSetColumnIndex(1);
					if (ImGui::BeginTable("tablePreviewerSaveDataTagMissionWeaponUnlocks", 2, Preview::tableFlags))
					{
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Enma's Fang");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_TagMissionWeaponUnlocks[1] && m_TagMissionWeaponUnlocks[10]).c_str());
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Vigoorian Flail");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_TagMissionWeaponUnlocks[2]).c_str());
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Tonfa");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_TagMissionWeaponUnlocks[3]).c_str());
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Eclipse Scythe");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_TagMissionWeaponUnlocks[4]).c_str());
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Blade of the Archfiend");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_TagMissionWeaponUnlocks[6]).c_str());
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Kusari-Gama");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_TagMissionWeaponUnlocks[7]).c_str());
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Falcon's Talons");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_TagMissionWeaponUnlocks[8]).c_str());
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Lunar Staff");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_TagMissionWeaponUnlocks[9]).c_str());
						ImGui::EndTable();
					}
					
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Ninpo (Tag Missions) unlocked");
					ImGui::TableSetColumnIndex(1);
					if (ImGui::BeginTable("tablePreviewerSaveDataTagMissionNinpoUnlocks", 2, Preview::tableFlags))
					{
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Wind Blades");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_TagMissionNinpoUnlocks[0]).c_str());
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Flame Phoenix");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_TagMissionNinpoUnlocks[1]).c_str());
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Piercing Void");
						ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_TagMissionNinpoUnlocks[2]).c_str());
						ImGui::EndTable();
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Howling Cannon (Tag Missions) unlocked");
					ImGui::TableSetColumnIndex(1); ImGui::Text(std::format("{}", m_IsTagMissionHowlingCannonUnlock).c_str());

					ImGui::EndTable();
				}
			}
			ImGui::PopID();
		}

	private:
		struct Completion
		{
			struct Difficulty
			{
				bool Chapter01 : 1;
				bool Chapter02 : 1;
				bool Chapter03 : 1;
				bool Chapter04 : 1;
				bool Chapter05 : 1;
				bool Chapter06 : 1;
				bool Chapter07 : 1;
				bool Chapter08 : 1;
				bool Chapter09 : 1;
				bool Chapter10 : 1;
				bool Chapter11 : 1;
				bool Chapter12 : 1;
				bool Chapter13 : 1;
				bool Chapter14 : 1;
				bool Chapter15 : 1;
				bool Chapter16 : 1;
				bool Chapter17 : 1;
				bool Chapter18 : 1;
				bool Chapter19 : 1;
				bool Chapter20 : 1;
				bool Chapter21 : 1;
				bool Chapter22 : 1;
				bool Chapter23 : 1;
				bool Chapter24 : 1;
				bool Chapter25 : 1;
				bool Chapter26 : 1;
				bool Chapter27 : 1;
				bool Chapter28 : 1;
				bool Chapter29 : 1;
				bool Chapter30 : 1;
				bool Chapter31 : 1;
				bool Chapter32 : 1;
			};

			Difficulty Hero;
			Difficulty Acolyte;
			Difficulty Warrior;
			Difficulty Mentor;
			Difficulty MasterNinja;
		};

		Completion m_Completion;

		bool m_IsChapterChallengeUnlock;

		bool m_IsStoryMasterNinjaUnlock;

		bool m_TagMissionUnlocks[45];

		bool m_TagMissionWeaponUnlocks[11];

		bool m_TagMissionNinpoUnlocks[3];

		bool m_IsTagMissionHowlingCannonUnlock;

		bool Prepare() override
		{
			DataReader reader;
			if (reader.Init(m_File))
			{
				reader.Seek(0xC17, MemoryBuffer::beg);
				reader.ReadValue(m_Completion);

				reader.Seek(0x21C8, MemoryBuffer::beg);
				reader.ReadValue(m_IsChapterChallengeUnlock);

				reader.Seek(0x21CE, MemoryBuffer::beg);
				reader.ReadValue(m_IsStoryMasterNinjaUnlock);

				reader.Seek(0x21D3, MemoryBuffer::beg);
				reader.ReadValue(m_TagMissionUnlocks);

				reader.Seek(0x2206, MemoryBuffer::beg);
				reader.ReadValue(m_TagMissionWeaponUnlocks);

				reader.Seek(0x2212, MemoryBuffer::beg);
				reader.ReadValue(m_TagMissionNinpoUnlocks);

				reader.Seek(0x2216, MemoryBuffer::beg);
				reader.ReadValue(m_IsTagMissionHowlingCannonUnlock);

				m_IsSetup = true;
			}
			return m_IsSetup;
		}
	};
	
	class PreviewStorySave2B final : public BasePreview
	{
	public:
		PreviewStorySave2B(File& file)
			: BasePreview(file)
		{
		}

		void OnRender() override
		{
			ImGui::PushID(this);
			{
				if (ImGui::BeginTable("tablePreviewerSaveData", 2, Preview::tableFlags))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("x");
					ImGui::TableSetColumnIndex(1); ImGui::Text("y");

					ImGui::EndTable();
				}
			}
			ImGui::PopID();
		}
	};
	
	class PreviewSystemSave3RE final : public BasePreview
	{
	public:
		PreviewSystemSave3RE(File& file)
			: BasePreview(file)
		{
		}

		void OnRender() override
		{
			ImGui::PushID(this);
			{
				if (ImGui::BeginTable("tablePreviewerSaveData", 2, Preview::tableFlags))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("x");
					ImGui::TableSetColumnIndex(1); ImGui::Text("y");

					ImGui::EndTable();
				}
			}
			ImGui::PopID();
		}
	};


	PreviewSave::PreviewSave(File& file)
		: BasePreview(file),
		m_SaveType(SaveFile::Type::unknown), mp_SubPreview(nullptr)
	{
	}

	PreviewSave::~PreviewSave()
	{
		if (m_IsSetup)
		{
			delete mp_SubPreview;
		}
	}

	void PreviewSave::OnRender()
	{
		ImGui::PushID(this);
		{
			ImGui::Dummy({});

			if (ImGui::BeginTable("tablePreviewerSave", 2, Preview::tableFlags))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Type");
				ImGui::TableSetColumnIndex(1);
				switch (m_SaveType)
				{
				case SaveFile::Type::SYSTEMSAVE_S1:
				case SaveFile::Type::SYSTEMSAVE_S2:
				case SaveFile::Type::SYSTEMSAVE_2B:
				case SaveFile::Type::SYSTEMSAVE_3RE:
					ImGui::Text("System Save"); break;
				case SaveFile::Type::GAMESAVE_S1:
					ImGui::Text("Game Save"); break;
				case SaveFile::Type::STORYSAVE_S2:
				case SaveFile::Type::STORYSAVE_2B:
					ImGui::Text("Story Save"); break;
				case SaveFile::Type::unknown:
					ImGui::Text("Unknown"); break;
				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Data");
				ImGui::TableSetColumnIndex(1); mp_SubPreview->OnRender();

				ImGui::EndTable();
			}
		}
		ImGui::PopID();
	}

	bool PreviewSave::Prepare()
	{
		{
			LoaderSave loader(m_File);

			m_SaveType = loader.DetermineSaveType();

			switch (m_SaveType)
			{
			case SaveFile::Type::SYSTEMSAVE_S1:
				mp_SubPreview = new PreviewSystemSaveS1(m_File);
				break;
			case SaveFile::Type::GAMESAVE_S1:
				mp_SubPreview = new PreviewGameSaveS1(m_File);
				break;
			case SaveFile::Type::SYSTEMSAVE_S2:
				mp_SubPreview = new PreviewSystemSaveS2(m_File);
				break;
			case SaveFile::Type::STORYSAVE_S2:
				mp_SubPreview = new PreviewStorySaveS2(m_File);
				break;
			case SaveFile::Type::SYSTEMSAVE_2B:
				mp_SubPreview = new PreviewSystemSave2B(m_File);
				break;
			case SaveFile::Type::STORYSAVE_2B:
				mp_SubPreview = new PreviewStorySave2B(m_File);
				break;
			case SaveFile::Type::SYSTEMSAVE_3RE:
				mp_SubPreview = new PreviewSystemSave3RE(m_File);
				break;
			case SaveFile::Type::unknown:
			default:
				mp_SubPreview = nullptr;
				break;
			}
		}

		if (mp_SubPreview)
		{
			mp_SubPreview->Prepare();
			m_IsSetup = true;
		}
		else
		{
			m_IsSetup = false;
		}

		return m_IsSetup;
	}
}