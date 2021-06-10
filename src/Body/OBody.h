#pragma once

#include "SKEE.h"

using namespace std;


namespace Body
{
	

	class OBody
	{
		using VM = RE::BSScript::IVirtualMachine;
	public:

		static OBody* GetInstance();

		bool GameLoaded;

		void SetLoaded(bool a);
		float GetBreastScore(struct SliderSet sliders, bool max);

		vector<string> GetFilesInBodyslideDir();
		void GenerateDatabases();
		bool StringContains(string& str, const char* testcase);

		void GenBodyByFile(RE::Actor* act, string path);

		struct SliderSet GenerateClotheSliders(RE::Actor* act);
		struct Slider BuildDerivativeSlider(RE::Actor* act, string morph, float target);

		void GenerateActorBody(RE::Actor* act);
		void SetMorph(RE::Actor* act, string MorphName, float value, string key);
		void SetMorphByWeight(RE::Actor* act, struct Slider slider, float weight, string key);
		void ApplyMorphs(RE::Actor* act);
		bool SetMorphInterface(SKEE::IBodyMorphInterface* a_morphInt);
		void ApplySlider(RE::Actor* act, struct Slider slid, float weight, string key);
		float GetWeight(RE::Actor* act);
		void ApplySliderSet(RE::Actor* act, struct SliderSet sliderset, string key);
		void ApplyBodyslidePreset(RE::Actor* act, struct BodyslidePreset preset );

		pugi::xml_document GetDocFromFile(string pathToFile);
		struct SliderSet GenerateSlidersetFromDoc(pugi::xml_document& doc);

		void AddSliderToSet(struct SliderSet& sliderset, struct Slider slider);
		struct Slider BuildSlider(string name, float min, float max);
		struct Slider BuildSlider(string name, float value);
		vector<struct BodyslidePreset> GeneratePresetsFromDoc(pugi::xml_document& doc);
		void ProcessActor(RE::Actor* act);
		float GetMorph(RE::Actor* act, string MorphName);
		void GenerateFullBodyFromPreset(RE::Actor* act, struct BodyslidePreset preset);
		struct BodyslidePreset GenerateSinglePresetFromFile(string file);
		struct BodyslidePreset GeneratePresetFromNode(pugi::xml_node node);

		void AddPresetToDatabase(struct PresetDatabase& database, struct BodyslidePreset preset);
		struct BodyslidePreset GetRandomElementOfDatabase(struct PresetDatabase& database);
		bool IsFemalePreset(struct BodyslidePreset& preset);
		struct SliderSet GenerateSlidersetFromNode(pugi::xml_node& node);

		void AddPresetToDatabase(struct PresetDatabase& database, vector<struct BodyslidePreset> presets);
		vector<struct BodyslidePreset> GeneratePresetsFromFile(string file);

		void PrintSliderSet(struct SliderSet set);
		void PrintPreset(struct BodyslidePreset preset);

		void ApplyClothePreset(RE::Actor* act);
		void ProcessActorEquipEvent(RE::Actor* act, bool RemovingBodyArmor);

		bool IsProcessed(RE::Actor* act);

		bool IsClothedSet(string set);
		bool IsFemale(RE::Actor* act);

		bool IsNaked(RE::Actor* act);

		bool HasActiveClothePreset(RE::Actor* act);
		vector<RE::BSFixedString> GetPresets(RE::Actor* act);
		void RemoveClothePreset(RE::Actor* act);
		struct BodyslidePreset GetPresetByName(struct PresetDatabase& database, string name);
		void GenBodyByName(RE::Actor* act, string PresetName);
		int GetSliderScore(struct BodypartScoreset& scoreset, string slidername);

		float GetBodypartScore(struct BodypartScoreset& bodypartSet, struct SliderSet sliders, bool max);
		float GetButtScore(struct SliderSet sliders, bool max);
		float GetWaistScore(struct SliderSet sliders, bool max);
		void RegisterQuestForEvent(RE::TESQuest* quest);


	private:
		OBody() = default;
		OBody(const OBody&) = delete;
		OBody(OBody&&) = delete;
		~OBody() = default;

		SKEE::IBodyMorphInterface* morphInt;




	};
}
