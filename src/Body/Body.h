#pragma once

#include "SKEE.h"

namespace Body
{
	inline SKSE::RegistrationSet<RE::Actor*> OnActorGenerated("OnActorGenerated"sv);
	inline SKSE::RegistrationSet<RE::Actor*> OnActorNaked("OnActorNaked"sv);

	enum class BodyType : int32_t
	{
		CBBE,
		UNP
	};

	struct RaceStat
	{
		std::string name;
		std::string bodypart;
		int32_t value{ -1 };
	};

	struct Score
	{
		std::string name;
		float min{ 0 };	 // 100 = large?
		float max{ 0 };
	};

	class Slider
	{
	public:
		Slider() = default;
		Slider(std::string a_name, float a_val) :
			name(std::move(a_name)), min(a_val), max(a_val) {}
		Slider(std::string a_name, float a_min, float a_max) :
			name(std::move(a_name)), min(a_min), max(a_max) {}
		~Slider() = default;

		std::string name;
		float min{ 0 };
		float max{ 0 };
	};

	using SliderSet = std::unordered_map<std::string, Slider>;
	using ScoreSet = std::vector<Score>;

	class Preset
	{
	public:
		Preset() = default;
		Preset(std::string a_name) :
			name(std::move(a_name)) {}
		Preset(std::string a_name, std::string a_body) :
			name(std::move(a_name)), body(std::move(a_body)) {}
		~Preset() = default;

		std::string name;
		std::string body;
		SliderSet sliders;
		ScoreSet scores;
	};

	using PresetSet = std::vector<Preset>;
	using RaceStatSet = std::vector<RaceStat>;
	using PartScoreSet = std::unordered_map<std::string, int32_t>;

	class OBody
	{
	public:
		OBody();
		static OBody* GetInstance();

		bool SetMorphInterface(SKEE::IBodyMorphInterface* a_morphInterface);
		void SetMorph(RE::Actor* a_actor, const char* a_morphName, const char* a_key, float a_value);
		float GetMorph(RE::Actor* a_actor, const char* a_morphName);
		void ApplyMorphs(RE::Actor* a_actor);

		void ProcessActor(RE::Actor* a_actor);
		void ProcessActorEquipEvent(RE::Actor* a_actor, bool a_removingArmor);

		void Generate();
		void GenerateRaceStatDB();
		std::optional<Preset> GeneratePreset(pugi::xml_node& a_node);
		PresetSet GeneratePresetsByFile(std::string a_path);
		void GenerateActorBody(RE::Actor* a_actor);
		void GenerateBodyByFile(RE::Actor* a_actor, std::string a_path);
		void GenerateBodyByName(RE::Actor* a_actor, std::string a_name);
		void GenerateBodyByPreset(RE::Actor* a_actor, Preset& a_preset);
		SliderSet GenerateRandomNippleSliders();
		SliderSet GenerateRandomGenitalSliders();
		SliderSet GenerateClotheSliders(RE::Actor* a_actor);

		Preset GetPresetByName(PresetSet& a_presetSet, std::string a_name);
		Preset GetRandomPreset(PresetSet& a_presetSet);
		PresetSet SortPresetSetByRaceStat(PresetSet& a_presetSet, RaceStat& a_stat);
		PresetSet SortPresetSetByBodypart(PresetSet& a_presetSet, std::string a_bodypart);

		Score GetScoreByName(Preset& a_preset, std::string a_name);
		float GetPartScore(PartScoreSet& a_scoreSet, SliderSet& a_sliders, bool a_max);
		float GetScoreByWeight(Score& a_score, float a_weight);
		void SaveScoreToActor(RE::Actor* a_actor, Score& a_score, float a_weight);

		RaceStat GetCorrespondingRaceStat(RE::Actor* a_actor);

		SliderSet SliderSetFromNode(pugi::xml_node& a_node, BodyType a_body);
		void AddSliderToSet(SliderSet& a_sliderSet, Slider a_slider, bool a_inverted = false);

		void ApplySlider(RE::Actor* a_actor, Slider& a_slider, const char* a_key, float a_weight);
		void ApplySliderSet(RE::Actor* a_actor, SliderSet& a_sliders, const char* a_key);
		void ApplyPreset(RE::Actor* a_actor, Preset& a_preset);
		void ApplyClothePreset(RE::Actor* a_actor);
		void RemoveClothePreset(RE::Actor* a_actor);

		Slider DeriveSlider(RE::Actor* a_actor, const char* a_morph, float a_target);

		BodyType GetBodyType(std::string a_body);
		float GetWeight(RE::Actor* a_actor);

		bool IsClothedSet(std::string& a_set);
		bool IsClotheActive(RE::Actor* a_actor);
		bool IsNaked(RE::Actor* a_actor);
		bool IsFemale(RE::Actor* a_actor);
		bool IsFemalePreset(Preset& a_preset);
		bool IsProcessed(RE::Actor* a_actor);

		void PrintSliderSet(SliderSet& a_sliderSet);
		void PrintPreset(Preset& a_preset);
		void PrintDatabase();

	public:
		bool setGameLoaded{ false };
		bool setUseRaceStats{ true };
		bool setRefit{ true };
		bool setNippleRand{ true };
		bool setGenitalRand{ true };

		std::vector<std::string> defaultSliders;

		RaceStatSet raceStats;

		PresetSet femalePresets;
		PresetSet malePresets;

		PartScoreSet breastScores;
		PartScoreSet buttScores;
		PartScoreSet waistScores;

	private:
		OBody(const OBody&) = delete;
		OBody(OBody&&) = delete;
		~OBody() = default;

		SKEE::IBodyMorphInterface* morphInterface;
	};
}
