#include "Body/Body.h"
#include "Body/Default.h"

#include "STL.h"

namespace Body 
{
	const fs::path root_path("Data\\CalienteTools\\BodySlide\\SliderPresets");

	OBody::OBody() :
		breastScores(DefaultBreastScores),
		buttScores(DefaultButtScores),
		waistScores(DefaultWaistScores)
	{}

	OBody* OBody::GetInstance()
	{
		static OBody instance;
		return std::addressof(instance);
	}

	bool OBody::SetMorphInterface(SKEE::IBodyMorphInterface* a_morphInterface)
	{
		return a_morphInterface->GetVersion() ? morphInterface = a_morphInterface : false;
	}

	void OBody::SetMorph(RE::Actor* a_actor, const char* a_morphName, const char* a_key, float a_value)
	{
		morphInterface->SetMorph(a_actor, a_morphName, a_key, a_value);
	}

	float OBody::GetMorph(RE::Actor* a_actor, const char* a_morphName)
	{
		return morphInterface->GetMorph(a_actor, a_morphName, "OBody");
	}

	void OBody::ApplyMorphs(RE::Actor* a_actor)
	{
		morphInterface->ApplyBodyMorphs(a_actor, true);
		morphInterface->UpdateModelWeight(a_actor, false);
	}

	void OBody::ProcessActor(RE::Actor* a_actor)
	{
		if (!setGameLoaded)
			return;

		if (!IsProcessed(a_actor)) {
			logger::info("Processing: {}", a_actor->GetName());
			GenerateActorBody(a_actor);
		}
	}

	void OBody::ProcessActorEquipEvent(RE::Actor* a_actor, bool a_removingArmor)
	{
		if (!IsProcessed(a_actor))
			return;


		bool naked = IsNaked(a_actor);
		bool clotheActive = IsClotheActive(a_actor);

		if (!(naked) && (a_removingArmor)){
			// Fires when removing their armor
			OnActorNaked.SendEvent(a_actor);
		}

		if (clotheActive && (naked || a_removingArmor)){
			RemoveClothePreset(a_actor);
		}else if (!clotheActive && !naked && setRefit){
			ApplyClothePreset(a_actor);
		}

		ApplyMorphs(a_actor);
	}

	void OBody::Generate()
	{ 
		std::vector<std::string> files;
		stl::files(root_path, files, ".xml");

		for (auto& entry : files) {
			if (IsClothedSet(entry))
				continue;

			pugi::xml_document doc;
			auto result = doc.load_file(entry.c_str());
			if (!result) {
				logger::warn("load failed: {} [{}]", entry, result.description());
				continue;
			}

			auto presets = doc.child("SliderPresets");
			for (auto& node : presets) {
				auto preset = GeneratePreset(node);
				if (!preset)
					continue;

				if (IsFemalePreset(*preset))
					femalePresets.push_back(*preset);
				else
					malePresets.push_back(*preset);
			}
		}

		logger::info("Female presets: {}", femalePresets.size());
		logger::info("Male presets: {}", malePresets.size());
	}

	void OBody::GenerateRaceStatDB()
	{
		pugi::xml_document doc;
		auto result = doc.load_file("Data\\SKSE\\Plugins\\OBody.xml");
		if (!result) {
			logger::warn("Settings failed: {}", result.description());
			return;
		}

		auto enableNode = doc.child("Enable");
		bool enable = enableNode.attribute("Value").as_bool();
		if (!enable)
			return;

		auto racesNode = doc.child("Races");
		for (auto& race : racesNode) {
			std::string name = race.attribute("name").as_string("");
			if (name != "")
				continue;

			auto addStat = [&](std::string a_name, const char* a_statName) {
				auto val = race.attribute(a_statName).as_int(-1);
				if (val > -1) {
					RaceStat stat{ name, a_statName, val };
					raceStats.push_back(std::move(stat));
				}
			};

			addStat(name, "breasts");
			addStat(name, "butt");
			addStat(name, "waist");
		}
	}

	Preset OBody::GetPresetByName(PresetSet& a_presetSet, std::string a_name)
	{
		logger::info("Looking for preset: {}", a_name);

		for (auto& preset : a_presetSet) {
			if (stl::cmp(preset.name, a_name))
				return preset;
		}

		auto preset = a_presetSet.front();
		logger::info("Preset not found");
		logger::info("Choosing default: {}", preset.name);
		return preset;
	}

	Preset OBody::GetRandomPreset(PresetSet& a_presetSet)
	{
		std::random_device seed;
		// generator
		std::mt19937 engine(seed());
		// number distribution
		auto size = static_cast<int>(a_presetSet.size());
		std::uniform_int_distribution<int> choose(0, size - 1);
		return a_presetSet[choose(engine)];
	}

	PresetSet OBody::SortPresetSetByRaceStat(PresetSet& a_presetSet, RaceStat& a_stat)
	{
		return SortPresetSetByBodypart(a_presetSet, a_stat.bodypart);
	}

	PresetSet OBody::SortPresetSetByBodypart(PresetSet& a_presetSet, std::string a_bodypart)
	{
		PresetSet ret = a_presetSet;

		// Note: This can be improved
		// One by one move boundary of unsorted subarray
		int n = static_cast<int>(ret.size());
		for (int i = 0; i < n - 1; i++) {
			// Find the minimum element in unsorted array
			int min_idx = i;
			for (int j = i + 1; j < n; j++) {
				auto j_scoreset = GetScoreByName(ret[j], a_bodypart);
				auto minidx_scoreset = GetScoreByName(ret[min_idx], a_bodypart);

				auto j_val = ((j_scoreset.min + j_scoreset.max) / 2);
				auto minidx_val = ((minidx_scoreset.min + minidx_scoreset.max) / 2);
				if (j_val < minidx_val)
					min_idx = j;
			}
			// Swap the found minimum element with the first
			// element
			auto temp = ret[min_idx];
			ret[min_idx] = ret[i];
			ret[i] = temp;
		}

		return ret;
	}


	Score OBody::GetScoreByName(Preset& a_preset, std::string a_name)
	{
		for (auto& score : a_preset.scores) {
			if (stl::cmp(score.name, a_name))
				return score;
		}

		return {};
	}

	float OBody::GetPartScore(PartScoreSet& a_scoreSet, SliderSet& a_sliderSet, bool a_max = false)
	{
		float ret = 0;
		for (auto& [name, slider] : a_sliderSet) {
			auto it = a_scoreSet.find(name);
			if (it != a_scoreSet.end()) {
				float mult = a_max ? slider.max : slider.min;
				ret += static_cast<float>(it->second) * mult;
			}
		}

		return ret;
	}

	float OBody::GetScoreByWeight(Score& a_score, float a_weight)
	{
		return ((a_score.max - a_score.min) * a_weight) + a_score.min;
	}

	void OBody::SaveScoreToActor(RE::Actor* a_actor, Score& a_score, float a_weight)
	{
		float val = GetScoreByWeight(a_score, a_weight);
		auto name = "obody_score_" + a_score.name;
		SetMorph(a_actor, name.c_str(), "OBody", val);
	}

	std::optional<Preset> OBody::GeneratePreset(pugi::xml_node& a_node)
	{
		std::string name = a_node.attribute("name").value();
		if (IsClothedSet(name))
			return std::nullopt;

		std::string body = a_node.attribute("set").value();
		auto sliderSet = SliderSetFromNode(a_node, GetBodyType(body));
		
		Score breasts{ "breasts" };
		breasts.min = GetPartScore(breastScores, sliderSet);
		breasts.max = GetPartScore(breastScores, sliderSet, true);

		Score butt{ "butt" };
		butt.min = GetPartScore(buttScores, sliderSet);
		butt.max = GetPartScore(buttScores, sliderSet, true);

		Score waist{ "waist" };
		waist.min = GetPartScore(waistScores, sliderSet);
		waist.max = GetPartScore(waistScores, sliderSet, true);

		std::vector<Score> scores;
		scores.push_back(std::move(breasts));
		scores.push_back(std::move(butt));
		scores.push_back(std::move(waist));

		Preset preset{ name, body };
		preset.scores = std::move(scores);
		preset.sliders = std::move(sliderSet);
		return preset;
	}

	PresetSet OBody::GeneratePresetsByFile(std::string a_path)
	{
		pugi::xml_document doc;
		auto result = doc.load_file(a_path.c_str());
		if (!result) {
			logger::warn("load failed: {} [{}]", a_path, result.description());
			return {};
		}

		PresetSet set;
		auto presets = doc.child("SliderPresets");
		for (auto& node : presets) {
			auto preset = GeneratePreset(node);
			if (preset)
				set.push_back(std::move(*preset));
		}

		return set;
	}

	void OBody::GenerateActorBody(RE::Actor* a_actor)
	{
		Preset preset;
		bool female = IsFemale(a_actor);
		if ((female && femalePresets.size() < 1) || !female && malePresets.size() < 1) {
			SetMorph(a_actor, "obody_processed", "OBody", 1.0f);
			OnActorGenerated.SendEvent(a_actor);
			return;
		}

		if (female) {
			if (raceStats.size() > 0) {
				auto stat = GetCorrespondingRaceStat(a_actor);
				if (stat.value > -1) {
					auto sortedDB = SortPresetSetByRaceStat(femalePresets, stat);
					int dbSize = static_cast<int>(sortedDB.size());
					int start = (dbSize - 1) * static_cast<int>((static_cast<float>(stat.value) / 10.0f));
					int range = dbSize / 3;
					int min = start - range;
					int max = start + range;

					if (min < 0)
						min = 0;

					if (max > dbSize - 1)
						max = dbSize - 1;

					int finalPreset = stl::random(min, max);
					preset = sortedDB[finalPreset];
				} else {
					preset = GetRandomPreset(femalePresets);
				}
			} else {
				preset = GetRandomPreset(femalePresets);
			}
		} else {
			preset = GetRandomPreset(malePresets);
		}

		GenerateBodyByPreset(a_actor, preset);
		SetMorph(a_actor, "obody_processed", "OBody", 1.0f);
		OnActorGenerated.SendEvent(a_actor);
	}

	void OBody::GenerateBodyByFile(RE::Actor* a_actor, std::string a_path)
	{
		auto set = GeneratePresetsByFile(a_path);
		GenerateBodyByPreset(a_actor, set.front());
	}

	void OBody::GenerateBodyByName(RE::Actor* a_actor, std::string a_name)
	{
		// do not send this an invalid name, you have been warned
		Preset preset;
		if (IsFemale(a_actor))
			preset = GetPresetByName(femalePresets, a_name);
		else
			preset = GetPresetByName(malePresets, a_name);

		GenerateBodyByPreset(a_actor, preset);
	}

	void OBody::GenerateBodyByPreset(RE::Actor* a_actor, Preset& a_preset)
	{
		//morphInterface->ClearBodyMorphKeys(a_actor, "OClothe");
		//morphInterface->ClearBodyMorphKeys(a_actor, "OBody");
		morphInterface->ClearMorphs(a_actor);
		ApplyPreset(a_actor, a_preset);

		logger::info("Applying preset: {}", a_preset.name);
		for (auto& score : a_preset.scores) {
			float weight = GetWeight(a_actor);
			SaveScoreToActor(a_actor, score, weight);
		}

		if (IsFemale(a_actor)) {
			if (setNippleRand) {
				SliderSet set = GenerateRandomNippleSliders();
				ApplySliderSet(a_actor, set, "OBody");
			}

			if (setGenitalRand) {
				SliderSet set = GenerateRandomGenitalSliders();
				ApplySliderSet(a_actor, set, "OBody");
			}
		}

		if (!IsNaked(a_actor)) {
			if (setRefit){
				logger::info("Not naked, adding cloth preset");
				ApplyClothePreset(a_actor);
			}
		}else {
			OnActorNaked.SendEvent(a_actor);
		}

		ApplyMorphs(a_actor);
		SetMorph(a_actor, "obody_processed", "OBody", 1.0f);
	}

	SliderSet OBody::GenerateRandomNippleSliders()
	{
		SliderSet set;

		if (stl::chance(15))
			AddSliderToSet(set, Slider{ "AreolaSize", stl::random(-1.0f, 0.0f) });
		else
			AddSliderToSet(set, Slider{ "AreolaSize", stl::random(0.0f, 1.0f) });

		if (stl::chance(75))
			AddSliderToSet(set, Slider{ "AreolaPull_v2", stl::random(-0.25f, 1.0f) });

		if (stl::chance(15))
			AddSliderToSet(set, Slider{ "NippleLength", stl::random(0.2f, 0.3f) });
		else
			AddSliderToSet(set, Slider{ "NippleLength", stl::random(0.0f, 0.1f) });

		AddSliderToSet(set, Slider{ "NippleManga", stl::random(-0.3f, 0.8f) });

		if (stl::chance(25))
			AddSliderToSet(set, Slider{ "NipplePerkManga", stl::random(-0.3f, 1.2f) });

		if (stl::chance(15))
			AddSliderToSet(set, Slider{ "NipBGone", stl::random(0.6f, 1.0f) });

		AddSliderToSet(set, Slider{ "NippleSize", stl::random(-0.5f, 0.3f) });
		AddSliderToSet(set, Slider{ "NippleDip", stl::random(0.0f, 1.0f) });
		AddSliderToSet(set, Slider{ "NippleCrease_v2", stl::random(-0.4f, 1.0f) });

		if (stl::chance(6))
			AddSliderToSet(set, Slider{ "NipplePuffy_v2", stl::random(0.4f, 0.7f) });

		if (stl::chance(35))
			AddSliderToSet(set, Slider{ "NippleThicc_v2", stl::random(0.0f, 0.9f) });

		if (stl::chance(2)) {
			if (stl::chance(50))
				AddSliderToSet(set, Slider{ "NippleInvert_v2", 1.0f });
			else
				AddSliderToSet(set, Slider{ "NippleInvert_v2", stl::random(0.65f, 0.8f) });
		}

		return set;
	}

	SliderSet OBody::GenerateRandomGenitalSliders()
	{
		SliderSet set;

		if (stl::chance(20)) {
			// innie
			AddSliderToSet(set, Slider{ "Innieoutie", stl::random(0.95f, 1.1f) });

			if (stl::chance(50))
				AddSliderToSet(set, Slider{ "Labiapuffyness", stl::random(0.75f, 1.25f) });

			if (stl::chance(40))
				AddSliderToSet(set, Slider{ "LabiaMorePuffyness_v2", stl::random(0.0f, 1.0f) });

			AddSliderToSet(set, Slider{ "Labiaprotrude", stl::random(0.0f, 0.5f) });
			AddSliderToSet(set, Slider{ "Labiaprotrude2", stl::random(0.0f, 0.1f) });
			AddSliderToSet(set, Slider{ "Labiaprotrudeback", stl::random(0.0f, 0.1f) });
			AddSliderToSet(set, Slider{ "Labiaspread", 0.0f });
			AddSliderToSet(set, Slider{ "LabiaCrumpled_v2", stl::random(0.0f, 0.3f) });
			AddSliderToSet(set, Slider{ "LabiaBulgogi_v2", 0.0f });
			AddSliderToSet(set, Slider{ "LabiaNeat_v2", 0.0f });
			AddSliderToSet(set, Slider{ "VaginaHole", stl::random(-0.2f, 0.05f) });
			AddSliderToSet(set, Slider{ "Clit", stl::random(-0.4f, 0.25f) });
		} else if (stl::chance(75)) {
			// average
			AddSliderToSet(set, Slider{ "Innieoutie", stl::random(0.4f, 0.75f) });

			if (stl::chance(40))
				AddSliderToSet(set, Slider{ "Labiapuffyness", stl::random(0.5f, 1.0f) });

			if (stl::chance(30))
				AddSliderToSet(set, Slider{ "LabiaMorePuffyness_v2", stl::random(0.0f, 0.75f) });

			AddSliderToSet(set, Slider{ "Labiaprotrude", stl::random(0.0f, 0.5f) });
			AddSliderToSet(set, Slider{ "Labiaprotrude2", stl::random(0.0f, 0.75f) });
			AddSliderToSet(set, Slider{ "Labiaprotrudeback", stl::random(0.0f, 1.0f) });

			if (stl::chance(50)) {
				AddSliderToSet(set, Slider{ "Labiaspread", stl::random(0.0f, 1.0f) });
				AddSliderToSet(set, Slider{ "LabiaCrumpled_v2", stl::random(0.0f, 0.7f) });

				if (stl::chance(60))
					AddSliderToSet(set, Slider{ "LabiaBulgogi_v2", stl::random(0.0f, 0.1f) });
			} else {
				AddSliderToSet(set, Slider{ "Labiaspread", 0.0f });
				AddSliderToSet(set, Slider{ "LabiaCrumpled_v2", stl::random(0.0f, 0.2f) });

				if (stl::chance(45))
					AddSliderToSet(set, Slider{ "LabiaBulgogi_v2", stl::random(0.0f, 0.3f) });			
			}

			AddSliderToSet(set, Slider{ "LabiaNeat_v2", 0.0f });
			AddSliderToSet(set, Slider{ "VaginaHole", stl::random(-0.2f, 0.40f) });
			AddSliderToSet(set, Slider{ "Clit", stl::random(-0.2f, 0.25f) });
		} else {
			//outie
			AddSliderToSet(set, Slider{ "Innieoutie", stl::random(-0.25f, 0.30f) });

			if (stl::chance(30))
				AddSliderToSet(set, Slider{ "Labiapuffyness", stl::random(0.20f, 0.50f) });

			if (stl::chance(10))
				AddSliderToSet(set, Slider{ "LabiaMorePuffyness_v2", stl::random(0.0f, 0.35f) });

			AddSliderToSet(set, Slider{ "Labiaprotrude", stl::random(0.0f, 1.0f) });
			AddSliderToSet(set, Slider{ "Labiaprotrude2", stl::random(0.0f, 1.0f) });
			AddSliderToSet(set, Slider{ "Labiaprotrudeback", stl::random(0.0f, 1.0f) });
			AddSliderToSet(set, Slider{ "Labiaspread", stl::random(0.0f, 1.0f) });
			AddSliderToSet(set, Slider{ "LabiaCrumpled_v2", stl::random(0.0f, 1.0f) });
			AddSliderToSet(set, Slider{ "LabiaBulgogi_v2", stl::random(0.0f, 1.0f) });

			if (stl::chance(40))
				AddSliderToSet(set, Slider{ "LabiaNeat_v2", stl::random(0.0f, 0.25f) });

			AddSliderToSet(set, Slider{ "VaginaHole", stl::random(0.0f, 1.0f) });
			AddSliderToSet(set, Slider{ "Clit", stl::random(-0.4f, 0.25f) });
		}

		AddSliderToSet(set, Slider{ "Vaginasize", stl::random(0.0f, 1.0f) });
		AddSliderToSet(set, Slider{ "ClitSwell_v2", stl::random(-0.3f, 1.1f) });
		AddSliderToSet(set, Slider{ "Cutepuffyness", stl::random(0.0f, 1.0f) });
		AddSliderToSet(set, Slider{ "LabiaTightUp", stl::random(0.0f, 1.0f) });

		if (stl::chance(60))
			AddSliderToSet(set, Slider{ "CBPC", stl::random(-0.25f, 0.25f) });
		else
			AddSliderToSet(set, Slider{ "CBPC", stl::random(0.6f, 1.0f) });

		AddSliderToSet(set, Slider{ "AnalPosition_v2", stl::random(0.0f, 1.0f) });
		AddSliderToSet(set, Slider{ "AnalTexPos_v2", stl::random(0.0f, 1.0f) });
		AddSliderToSet(set, Slider{ "AnalTexPosRe_v2", stl::random(0.0f, 1.0f) });
		AddSliderToSet(set, Slider{ "AnalLoose_v2", -0.1f });

		return set;
	}

	SliderSet OBody::GenerateClotheSliders(RE::Actor* a_actor)
	{
		SliderSet set;
		// breasts
		// make area on sides behind breasts not sink in
		AddSliderToSet(set, DeriveSlider(a_actor, "BreastSideShape", 0.0f));
		// make area under breasts not sink in
		AddSliderToSet(set, DeriveSlider(a_actor, "BreastUnderDepth", 0.0f));
		// push breasts together
		AddSliderToSet(set, DeriveSlider(a_actor, "BreastCleavage", 1.0f));
		// push up smaller breasts more
		AddSliderToSet(set, Slider{ "BreastGravity2", -0.1f, -0.05f });
		// Make top of breast rise higher
		AddSliderToSet(set, Slider{ "BreastTopSlope", -0.2f, -0.35f });
		// push breasts together
		AddSliderToSet(set, Slider{ "BreastsTogether", 0.3f, 0.35f });
		// push breasts up
		//AddSliderToSet(set, Slider{ "PushUp", 0.6f, 0.4f });
		// Shrink breasts slightly
		AddSliderToSet(set, Slider{ "Breasts", -0.05f });
		// Move breasts up on body slightly
		AddSliderToSet(set, Slider{ "BreastHeight", 0.15f });
		// Fix "sock" chest
		//AddSliderToSet(set, Slider{ "BreastsConverage_v2", 0.00f, 0.35f });

		// butt
		// remove butt impressions
		AddSliderToSet(set, DeriveSlider(a_actor, "ButtDimples", 0.0f));
		AddSliderToSet(set, DeriveSlider(a_actor, "ButtUnderFold", 0.0f));
		// shrink ass slightly
		AddSliderToSet(set, Slider{ "AppleCheeks", -0.05f });
		AddSliderToSet(set, Slider{ "Butt", -0.05f });

		// Torso
		// remove definition on clavical bone
		AddSliderToSet(set, DeriveSlider(a_actor, "Clavicle_v2", 0.0f));
		// Push out navel
		AddSliderToSet(set, DeriveSlider(a_actor, "NavelEven", 1.0f));

		// hip
		// remove defintion on hip bone
		AddSliderToSet(set, DeriveSlider(a_actor, "HipCarved", 0.0f));

		// nipple
		// sublte change to tip shape
		AddSliderToSet(set, DeriveSlider(a_actor, "NippleDip", 0.0f));
		AddSliderToSet(set, DeriveSlider(a_actor, "NippleTip", 0.0f));
		//flatten areola
		AddSliderToSet(set, DeriveSlider(a_actor, "NipplePuffy_v2", 0.0f));
		// shrink areola
		AddSliderToSet(set, DeriveSlider(a_actor, "AreolaSize", -0.3f));
		// flatten nipple
		AddSliderToSet(set, DeriveSlider(a_actor, "NipBGone", 1.0f));
		AddSliderToSet(set, DeriveSlider(a_actor, "NippleManga", -0.75f));
		// push nipples together
		AddSliderToSet(set, Slider{ "NippleDistance", 0.05f, 0.08f });
		// Lift large breasts up
		AddSliderToSet(set, Slider{ "NippleDown", 0.0f, -0.1f });
		// Flatten nipple + areola
		AddSliderToSet(set, DeriveSlider(a_actor, "NipplePerkManga", -0.25f));
		// Flatten nipple
		AddSliderToSet(set, DeriveSlider(a_actor, "NipplePerkiness", 0.0f));

		return set;
	}

	RaceStat OBody::GetCorrespondingRaceStat(RE::Actor* a_actor)
	{
		auto raceName = a_actor->GetActorBase()->GetRace()->GetName();
		for (auto& stat : raceStats)
			if (stl::cmp(stat.name, raceName))
				return stat;

		return {};
	}

	SliderSet OBody::SliderSetFromNode(pugi::xml_node& a_node, BodyType a_body)
	{
		SliderSet ret;
		for (auto& node : a_node) {
			if (!stl::cmp(node.name(), "SetSlider"))
				continue;

			std::string name = node.attribute("name").value();

			bool inverted = false;
			if (a_body == BodyType::UNP) {
				if (DefaultSliders.contains(name))
					inverted = true;
			}

			float min{ 0 }, max{ 0 };
			float val = node.attribute("value").as_float() / 100.0f;
			auto size = node.attribute("size").value();
			if (stl::cmp(size, "big"))
				max = inverted ? 1.0f - val : val;
			else
				min = inverted ? 1.0f - val : val;

			Slider slider{ name, min, max };
			AddSliderToSet(ret, slider, inverted);
		}

		return ret;
	}

	void OBody::AddSliderToSet(SliderSet& a_sliderSet, Slider a_slider, [[maybe_unused]] bool a_inverted)
	{
		float val = 0;
		auto it = a_sliderSet.find(a_slider.name);
		if (it != a_sliderSet.end()) {
			auto& current = it->second;
			if ((current.min == val) && (a_slider.min != val))
				current.min = a_slider.min;
			if ((current.max == val) && (a_slider.max != val))
				current.max = a_slider.max;
		} else {
			a_sliderSet[a_slider.name] = std::move(a_slider);
		}
	}

	void OBody::ApplySlider(RE::Actor* a_actor, Slider& a_slider, const char* a_key, float a_weight)
	{
		float val = ((a_slider.max - a_slider.min) * a_weight) + a_slider.min;
		SetMorph(a_actor, a_slider.name.c_str(), a_key, val);
	}

	void OBody::ApplySliderSet(RE::Actor* a_actor, SliderSet& a_sliders, const char* a_key)
	{
		float weight = GetWeight(a_actor);
		for (auto& [name, slider] : a_sliders)
			ApplySlider(a_actor, slider, a_key, weight);
	}

	void OBody::ApplyPreset(RE::Actor* a_actor, Preset& a_preset)
	{
		ApplySliderSet(a_actor, a_preset.sliders, "OBody");
		PrintPreset(a_preset);
	}

	void OBody::ApplyClothePreset(RE::Actor* a_actor)
	{
		auto set = GenerateClotheSliders(a_actor);
		ApplySliderSet(a_actor, set, "OClothe");
	}

	void OBody::RemoveClothePreset(RE::Actor* a_actor)
	{
		morphInterface->ClearBodyMorphKeys(a_actor, "OClothe");
	}

	Slider OBody::DeriveSlider(RE::Actor* a_actor, const char* a_morph, float a_target)
	{
		return Slider{ a_morph, a_target - GetMorph(a_actor, a_morph) };
	}

	BodyType OBody::GetBodyType(std::string a_body)
	{
		std::vector<std::string> unp{ "unp", "coco" };
		return stl::contains(a_body, unp) ? BodyType::UNP : BodyType::CBBE;
	}

	float OBody::GetWeight(RE::Actor* a_actor)
	{
		return a_actor->GetActorBase()->GetWeight() / 100.0f;
	}

	bool OBody::IsClothedSet(std::string& a_set)
	{
		std::vector<std::string> clothed{ "outfit", "cloth" };
		return stl::contains(a_set, clothed);
	}

	bool OBody::IsClotheActive(RE::Actor* a_actor)
	{
		return morphInterface->HasBodyMorphKey(a_actor, "OClothe");
	}

	bool OBody::IsNaked(RE::Actor* a_actor)
	{
		auto changes = a_actor->GetInventoryChanges();
		auto armor = changes->GetArmorInSlot(32);
		return armor ? false : true;
	}

	bool OBody::IsFemale(RE::Actor* a_actor)
	{
		return a_actor->GetActorBase()->GetSex() == 1;
	}

	bool OBody::IsFemalePreset(Preset& a_preset)
	{
		std::vector<std::string> body{ "himbo", "talos" };
		return !stl::contains(a_preset.body, body);
	}

	bool OBody::IsProcessed(RE::Actor* a_actor)
	{
		return GetMorph(a_actor, "obody_processed") == 1.0f;
	}

	void OBody::PrintSliderSet(SliderSet& a_sliderSet)
	{
		logger::info(" > Sliders:");
		for (auto& [name, slider] : a_sliderSet) {
			logger::info("   > {}: [Small: {}] [Big: {}]", name, slider.min, slider.max);
		}
	}

	void OBody::PrintPreset(Preset& a_preset)
	{
		logger::info("[{}]", a_preset.name);
		logger::info(" > Body: {}", a_preset.body);

		logger::info(" > Scores:");
		for (auto& score : a_preset.scores)
			logger::info("   > {} : [Min: {}] [Max: {}]", score.name, score.min, score.max);

		PrintSliderSet(a_preset.sliders);
	}

	void OBody::PrintDatabase()
	{
		logger::info("Printing female presets");
		for (auto& preset : femalePresets) {
			PrintPreset(preset);
		}

		logger::info("Printing male presets");
		for (auto& preset : malePresets) {
			PrintPreset(preset);
		}
	}
}
