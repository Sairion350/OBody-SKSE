#pragma once

#include "SKEE.h"

namespace Body
{
	struct Slider
	{
		std::string name;
		float min;
		float max;
	};

	struct SliderSet
	{
		std::vector<Slider> sliders;
	};

	struct ScoreSet
	{
		std::string name;
		float MinScore;	 // 100 = large?
		float MaxScore;
	};

	struct BodyslidePreset
	{
		std::string name;
		std::string body;
		SliderSet sliders;
		std::vector<ScoreSet> scores;
	};

	struct PresetDatabase
	{
		std::vector<BodyslidePreset> presets;
	};

	struct BodysliderSliderScore
	{
		std::string name;
		int score;
	};

	struct BodypartScoreSet
	{
		std::vector<BodysliderSliderScore> scores;
	};

	struct RaceStat
	{
		std::string name;
		std::string bodypart;
		int value;
	};

	struct RaceStatDatabase
	{
		std::vector<RaceStat> races;
	};

	class OBody
	{
	public:
		static OBody* GetInstance();

		void SetLoaded(bool a);
		float GetBreastScore(SliderSet sliders, bool max);

		std::vector<std::string> GetFilesInBodyslideDir();
		void GenerateDatabases();
		bool StringContains(std::string& str, const char* testcase);

		void GenBodyByFile(RE::Actor* act, std::string path);

		SliderSet GenerateClotheSliders(RE::Actor* act);
		Slider BuildDerivativeSlider(RE::Actor* act, std::string morph, float target);

		void GenerateActorBody(RE::Actor* act);
		void SetMorph(RE::Actor* act, std::string MorphName, float value, std::string key);
		void SetMorphByWeight(RE::Actor* act, Slider slider, float weight, std::string key);
		void ApplyMorphs(RE::Actor* act);
		bool SetMorphInterface(SKEE::IBodyMorphInterface* a_morphInt);
		void ApplySlider(RE::Actor* act, Slider slid, float weight, std::string key);
		float GetWeight(RE::Actor* act);
		void ApplySliderSet(RE::Actor* act, SliderSet sliderset, std::string key);
		void ApplyBodyslidePreset(RE::Actor* act, BodyslidePreset preset );

		pugi::xml_document GetDocFromFile(std::string pathToFile);
		SliderSet GenerateSliderSetFromDoc(pugi::xml_document& doc);

		void AddSliderToSet(SliderSet& sliderset, Slider slider);
		Slider BuildSlider(std::string name, float min, float max);
		Slider BuildSlider(std::string name, float value);
		std::vector<BodyslidePreset> GeneratePresetsFromDoc(pugi::xml_document& doc);
		void ProcessActor(RE::Actor* act);
		float GetMorph(RE::Actor* act, std::string MorphName);
		void GenerateFullBodyFromPreset(RE::Actor* act, BodyslidePreset preset);
		BodyslidePreset GenerateSinglePresetFromFile(std::string file);
		BodyslidePreset GeneratePresetFromNode(pugi::xml_node node);

		void AddPresetToDatabase(PresetDatabase& database, BodyslidePreset preset);
		BodyslidePreset GetRandomElementOfDatabase(PresetDatabase& database);
		bool IsFemalePreset(BodyslidePreset& preset);
		SliderSet GenerateSlidersetFromNode(pugi::xml_node& node, int body);
		int GetBodyType(std::string body);
		void AddSliderToSet(SliderSet& sliderset, Slider slider, bool inverted);

		void AddPresetToDatabase(PresetDatabase& database, std::vector<BodyslidePreset> presets);
		std::vector<BodyslidePreset> GeneratePresetsFromFile(std::string file);

		void PrintSliderSet(SliderSet set);
		void PrintPreset(BodyslidePreset preset);

		void ApplyClothePreset(RE::Actor* act);
		void ProcessActorEquipEvent(RE::Actor* act, bool RemovingBodyArmor);

		bool IsProcessed(RE::Actor* act);

		bool IsClothedSet(std::string set);
		bool IsFemale(RE::Actor* act);

		bool IsNaked(RE::Actor* act);

		bool HasActiveClothePreset(RE::Actor* act);
		std::vector<RE::BSFixedString> GetPresets(RE::Actor* act);
		bool SliderSetContainsSlider(SliderSet& set, std::string slidername);
		void RemoveClothePreset(RE::Actor* act);
		BodyslidePreset GetPresetByName(PresetDatabase& database, std::string name);
		void GenBodyByName(RE::Actor* act, std::string PresetName);
		int GetSliderScore(BodypartScoreSet& scoreset, std::string slidername);

		float GetBodypartScore(BodypartScoreSet& bodypartSet, SliderSet sliders, bool max);
		float GetButtScore(SliderSet sliders, bool max);
		float GetWaistScore(SliderSet sliders, bool max);
		void RegisterQuestForEvent(RE::TESQuest* quest);
		float GetScoreByWeight(ScoreSet& score, float& weight);
		void SaveScoreToActor(RE::Actor* act, struct ScoreSet& score, float& weight);
		float RandomFloat(float a, float b);
		SliderSet GenerateRandomNippleSliders();
		bool ChanceRoll(int chance);
		SliderSet GenerateRandomGenitalSliders();

		void BuildRaceStatDB();
		RaceStat GenerateRaceStatFromNode(pugi::xml_node node);
		PresetDatabase SortPresetDatabaseByRaceStat(PresetDatabase& database, RaceStat stat);
		bool ComparePresetByBreastScore(const BodyslidePreset &a, const BodyslidePreset &b);
		bool ComparePresetByButtScore(const BodyslidePreset &a, const BodyslidePreset &b);
		bool ComparePresetByWaistScore(const BodyslidePreset &a, const BodyslidePreset &b);
		bool ComparePresetByScore(const BodyslidePreset& a, const BodyslidePreset& b, std::string scorename);
		ScoreSet GetScoresetFromPresetByName(BodyslidePreset& preset, std::string scorename);
		PresetDatabase SortPresetDatabaseByBodypart(PresetDatabase& database, std::string bodypart);
		void PrintDatabase(PresetDatabase& database);
		int RandomInt(int a, int b);
		int GetFemaleDatabaseSize();
		int GetMaleDatabaseSize();

		void SetORefit(bool a);
		void SetNippleRand(bool a);
		void SetGenitalRand(bool a);

		RaceStat GetCorrespondingRaceStat(RE::Actor* act);

	public:
		bool GameLoaded;
		bool UseRaceStats = true;

		bool set_ORefit = true;
		bool set_NippleRand = true;
		bool set_GenitalRand = true;

		std::vector<std::string> oneDefaultSliders;

		RaceStatDatabase RaceStats;

		PresetDatabase FemalePresets;
		PresetDatabase MalePresets;

		BodypartScoreSet BreastScores;
		BodypartScoreSet ButtScores;
		BodypartScoreSet WaistScores;

	private:
		OBody() = default;
		OBody(const OBody&) = delete;
		OBody(OBody&&) = delete;
		~OBody() = default;

		SKEE::IBodyMorphInterface* morphInt;
	};
}
