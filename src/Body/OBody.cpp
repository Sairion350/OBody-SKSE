#include "Body/OBody.h"

namespace Body 
{
	inline SKSE::RegistrationSet<RE::Actor*> OnActorGenerated("OnActorGenerated"sv);

	const fs::path root_path("Data\\CalienteTools\\BodySlide\\SliderPresets");

	float OBody::GetBodypartScore(BodypartScoreSet& bodypartSet, SliderSet sliders, bool max)
	{
		//PrintSliderSet(sliders);

		float ret = 0.0f;
		for (auto i = sliders.sliders.begin(); i != sliders.sliders.end(); ++i){
			struct Slider slider = (*i);

			float mult;
			std::string name = slider.name;
			if (max){
				mult = slider.max;
			} else {
				mult = slider.min;
			}

			int val = GetSliderScore(bodypartSet, slider.name);

			//logger::info("Trying: {}", slider.name);

			ret += static_cast<float>(val) * mult;
		}

		//logger::info("Final score: {}", ret);
		return ret;
	}

	float OBody::GetBreastScore(SliderSet sliders, bool max)
	{
		return GetBodypartScore(BreastScores, sliders, max);
	}

	float OBody::GetButtScore(SliderSet sliders, bool max)
	{
		return GetBodypartScore(ButtScores, sliders, max);
	}

	float OBody::GetWaistScore(SliderSet sliders, bool max)
	{
		return GetBodypartScore(WaistScores, sliders, max);
	}

	int OBody::GetSliderScore(BodypartScoreSet& scoreset, std::string slidername)
	{
		for (auto i = scoreset.scores.begin(); i != scoreset.scores.end(); ++i){
			auto slider = (*i);

			if (strcmp(slider.name.c_str(), slidername.c_str()) == 0){
			//	logger::info("Name: {}", slider.name);
			//	logger::info("Score: {}", slider.score);
				return slider.score;
			}
		}

		return 0;
	}

	int OBody::GetFemaleDatabaseSize()
	{
		return static_cast<int>(FemalePresets.presets.size());
	}

	int OBody::GetMaleDatabaseSize()
	{
		return static_cast<int>(MalePresets.presets.size());
	}

	OBody* OBody::GetInstance()
	{
		static OBody instance;
		return &instance;
	}

	void OBody::SetLoaded(bool a)
	{
		GameLoaded = a;
	}

	void OBody::SetORefit(bool a) 
	{
		set_ORefit = a;
	}

	void OBody::SetNippleRand(bool a)
	{
		set_NippleRand = a;
	}

	void OBody::SetGenitalRand(bool a)
	{
		set_GenitalRand = a;
	}

	PresetDatabase OBody::SortPresetDatabaseByRaceStat(PresetDatabase& database, RaceStat stat)
	{
		return SortPresetDatabaseByBodypart(database, stat.bodypart);
	}

	PresetDatabase OBody::SortPresetDatabaseByBodypart(PresetDatabase& database, std::string bodypart)
	{
		PresetDatabase ret;
		auto arr = database.presets;
		int n = (int) arr.size();
 
        // One by one move boundary of unsorted subarray
        for (int i = 0; i < n-1; i++) {
            // Find the minimum element in unsorted array
            int min_idx = i;
            for (int j = i+1; j < n; j++) {
            	auto j_scoreset = GetScoresetFromPresetByName(arr[j], bodypart);
            	auto minidx_scoreset = GetScoresetFromPresetByName(arr[min_idx], bodypart);

            	auto j_val = ((j_scoreset.MinScore + j_scoreset.MaxScore) / 2);
            	auto minidx_val = ((minidx_scoreset.MinScore + minidx_scoreset.MaxScore) / 2);
                if (j_val < minidx_val)
                    min_idx = j;
 			}
            // Swap the found minimum element with the first
            // element
            auto temp = arr[min_idx];
            arr[min_idx] = arr[i];
            arr[i] = temp;
        }

        ret.presets = arr;
        return ret;
	}

	ScoreSet OBody::GetScoresetFromPresetByName(BodyslidePreset& preset, std::string scorename)
	{
		for (auto i = preset.scores.begin(); i != preset.scores.end(); ++i) {
			auto score = (*i);

			if (strcmp(score.name.c_str(), scorename.c_str()) == 0){
				return score;
			}
		}

		return {};
	}

/*
	static bool OBody::comparePresetByScore(const BodyslidePreset &a, const BodyslidePreset &b, string scorename){
		auto aVal = GetScoresetFromPresetByName(a, scorename);
		auto bVal = GetScoresetFromPresetByName(b, scorename);

		return ((aVal.MinScore + aVal.MaxScore) / 2) < ((bVal.MinScore + bVal.MaxScore) / 2);
	}

	static bool OBody::comparePresetByBreastScore(const BodyslidePreset &a, const BodyslidePreset &b){
		return comparePresetByScore(a, b, "breasts")
	}

	static bool OBody::comparePresetByButtScore(const BodyslidePreset &a, const BodyslidePreset &b){
		return comparePresetByScore(a, b, "butt")
	}

	static bool OBody::comparePresetByWaistScore(const BodyslidePreset &a, const BodyslidePreset &b){
		return comparePresetByScore(a, b, "waist")
	}
*/

	void OBody::ProcessActor(RE::Actor* act)
	{
		///morphInt->EvaluateBodyMorphs(act);

		if (!GameLoaded) {
			logger::info("Game not fully loaded, skipping actor");
			return;
		}

		//logger::info("Has: {}", GetMorph(act, "obody_processed"));
		//if (morphInt->HasBodyMorphKey(act, key)){

		if (IsProcessed(act)) {
			// ignore, already set
			logger::info("Skipping: {}", act->GetName());
		} else {
			logger::info("Processing: {}", act->GetName());
			GenerateActorBody(act);
		}
	}

	bool OBody::IsProcessed(RE::Actor* act)
	{
		return GetMorph(act, "obody_processed") == 1.0f;
	}

	void OBody::ProcessActorEquipEvent(RE::Actor* act, bool RemovingBodyArmor)
	{	
		if (!IsProcessed(act)) {
			return;
		}
	
		if (HasActiveClothePreset(act) && (IsNaked(act) || RemovingBodyArmor)) {
			//logger::info("Removing clothe preset");
			RemoveClothePreset(act);
			ApplyMorphs(act);
		} else if(!HasActiveClothePreset(act) && !IsNaked(act) && set_ORefit) {
			//logger::info("adding clothe preset");
			ApplyClothePreset(act);
			ApplyMorphs(act);
		}
	}

	bool OBody::SetMorphInterface(SKEE::IBodyMorphInterface* a_morphInt)
	{
		return a_morphInt->GetVersion() ? this->morphInt = a_morphInt : false;
	}

	void OBody::GenerateActorBody(RE::Actor* act)
	{
		BodyslidePreset preset;
		if (IsFemale(act)) {
			if (GetFemaleDatabaseSize() < 1) {
				SetMorph(act, "obody_processed", 1.0f, "OBody");
				OnActorGenerated.SendEvent(act);
				return;
			}
			if (RaceStats.races.size() > 0) {
				// user is using the XML race feature
				auto RaceStat = GetCorrespondingRaceStat(act);
				if (RaceStat.value > -1) {
					auto sortedDB = SortPresetDatabaseByRaceStat(FemalePresets, RaceStat);

					int dbSize = (int)sortedDB.presets.size();

					int StartingPoint = (int) ((dbSize - 1) * (( (float)RaceStat.value ) / 10.0f));

					int range = (dbSize) / 3;
					int min = StartingPoint - range;
					int max = StartingPoint + range;

					if (min < 0)
						min = 0;

					if (max > dbSize - 1)
						max = dbSize - 1;

					int finalPreset = RandomInt(min, max);
					preset = sortedDB.presets[finalPreset];
				} else {
					preset = GetRandomElementOfDatabase(FemalePresets);
				}
			} else {
				preset = GetRandomElementOfDatabase(FemalePresets);
			}
		} else {
			if (GetMaleDatabaseSize() < 1) {
				SetMorph(act, "obody_processed", 1.0f, "OBody");
				OnActorGenerated.SendEvent(act);
				return;
			}
			preset = GetRandomElementOfDatabase(MalePresets);
		}

		GenerateFullBodyFromPreset(act, preset);
		SetMorph(act, "obody_processed", 1.0f, "OBody");

		OnActorGenerated.SendEvent(act);
	}

	void OBody::GenerateFullBodyFromPreset(RE::Actor* act, BodyslidePreset preset)
	{
		//morphInt->ClearBodyMorphKeys(act, "OClothe");
		//morphInt->ClearBodyMorphKeys(act, "OBody");
		morphInt->ClearMorphs(act);

		auto weight = GetWeight(act);

		ApplyBodyslidePreset(act, preset);

		logger::info("    Applying preset: {}", preset.name);

		for (auto i = preset.scores.begin(); i != preset.scores.end(); ++i){
			auto score = (*i);
			SaveScoreToActor(act, score, weight);
		}

		if (IsFemale(act)) {
			// random nipples
			if (set_NippleRand) {
				ApplySliderSet(act, GenerateRandomNippleSliders(), "OBody");
			}

			// random vagina
			if (set_GenitalRand) {
				ApplySliderSet(act, GenerateRandomGenitalSliders(), "OBody");
			}
		}

		if (!IsNaked(act) && set_ORefit) {
			logger::info("Not naked, adding cloth preset");
			ApplyClothePreset(act);
		}

		ApplyMorphs(act);
		SetMorph(act, "obody_processed", 1.0f, "OBody");
	}

	bool OBody::HasActiveClothePreset(RE::Actor* act)
	{
		return morphInt->HasBodyMorphKey(act, "OClothe");
	}

	void OBody::RemoveClothePreset(RE::Actor* act)
	{
		morphInt->ClearBodyMorphKeys(act, "OClothe");
	}

	void OBody::ApplyClothePreset(RE::Actor* act)
	{
		auto clotheSet = GenerateClotheSliders(act);

		//PrintSliderSet(clotheSet);
		ApplySliderSet(act, clotheSet, "OClothe");
	}

	bool OBody::IsNaked(RE::Actor* act)
	{
		auto changes = act->GetInventoryChanges();
		auto armor = changes->GetArmorInSlot(32);
		return armor ? true : false;
	}

	RaceStat OBody::GetCorrespondingRaceStat(RE::Actor* act)
	{
		RaceStat ret;
		std::string RaceName = act->GetActorBase()->GetRace()->GetName();

		for (auto i = RaceStats.races.begin(); i != RaceStats.races.end(); ++i){
			auto stat = (*i);

			if (strcmp(stat.name.c_str(), RaceName.c_str()) == 0){
				return stat;
			}
		}

		return ret;
	}

	void OBody::SetMorph(RE::Actor* act, std::string MorphName, float value, std::string key)
	{
		//logger::info(">> Setting morph: {}", MorphName);
		//logger::info(">>  With morph value: {}", value);
		morphInt->SetMorph(act, MorphName.c_str(), key.c_str(), value);
	}

	float OBody::GetMorph(RE::Actor* act, std::string MorphName)
	{
		return morphInt->GetMorph(act, MorphName.c_str(), "OBody");
	}

	void OBody::SetMorphByWeight(RE::Actor* act, Slider slider, float weight, std::string key)
	{
		float value = ((slider.max - slider.min) * weight) + slider.min;
		SetMorph(act, slider.name, value, key);
	}

	void OBody::ApplySlider(RE::Actor* act, struct Slider slid, float weight, std::string key)
	{
		SetMorphByWeight(act, slid, weight, key);
	}

	void OBody::ApplySliderSet(RE::Actor* act, SliderSet sliderset, std::string key)
	{
		auto weight = GetWeight(act);
		for (auto i = sliderset.sliders.begin(); i != sliderset.sliders.end(); ++i)
        	ApplySlider(act, *i, weight, key);
	}

	void OBody::ApplyBodyslidePreset(RE::Actor* act, BodyslidePreset preset)
	{
		ApplySliderSet(act, preset.sliders, "OBody");
		//PrintPreset(preset);
	}

	SliderSet OBody::GenerateRandomNippleSliders()
	{
		SliderSet set;

		if (ChanceRoll(15)) {
			AddSliderToSet(set, BuildSlider("AreolaSize", RandomFloat(-1.0f, 0.0f) ));
		} else{
			AddSliderToSet(set, BuildSlider("AreolaSize", RandomFloat(0.0f, 1.0f) ));
		}

		if (ChanceRoll(75)) {
			AddSliderToSet(set, BuildSlider("AreolaPull_v2", RandomFloat(-0.25f, 1.0f) ));
		}
		
		if (ChanceRoll(15)) {
			AddSliderToSet(set, BuildSlider("NippleLength", RandomFloat(0.2f, 0.3f) ));
		} else{
			AddSliderToSet(set, BuildSlider("NippleLength", RandomFloat(0.0f, 0.1f) ));
		}

		AddSliderToSet(set, BuildSlider("NippleManga", RandomFloat(-0.3f, 0.8f) ));

		if (ChanceRoll(25)) {
			AddSliderToSet(set, BuildSlider("NipplePerkManga", RandomFloat(-0.3f, 1.2f) ));
		}

		if (ChanceRoll(15)) {
			AddSliderToSet(set, BuildSlider("NipBGone", RandomFloat(0.6f, 1.0f) ));
		}

		AddSliderToSet(set, BuildSlider("NippleSize", RandomFloat(-0.5f, 0.3f) ));

		AddSliderToSet(set, BuildSlider("NippleDip", RandomFloat(0.0f, 1.0f) ));

		AddSliderToSet(set, BuildSlider("NippleCrease_v2", RandomFloat(-0.4f, 1.0f) ));

		if (ChanceRoll(6)) {
			AddSliderToSet(set, BuildSlider("NipplePuffy_v2", RandomFloat(0.4f, 0.7f) ));
		}

		if (ChanceRoll(35)) {
			AddSliderToSet(set, BuildSlider("NippleThicc_v2", RandomFloat(0.0f, 0.9f) ));
		}

		if (ChanceRoll(2)) {
			if (ChanceRoll(50)) {
				AddSliderToSet(set, BuildSlider("NippleInvert_v2", 1.0f ));
			} else {
				AddSliderToSet(set, BuildSlider("NippleInvert_v2", RandomFloat(0.65f, 0.8f) ));
			}
		}

		return set;
	}

	SliderSet OBody::GenerateRandomGenitalSliders()
	{
		SliderSet set;

		if (ChanceRoll(20)) {
			// innie
			AddSliderToSet(set, BuildSlider("Innieoutie", RandomFloat(0.95f, 1.1f) ));

			if (ChanceRoll(50)) {
				AddSliderToSet(set, BuildSlider("Labiapuffyness", RandomFloat(0.75f, 1.25f) ));
			}

			if (ChanceRoll(40)) {
				AddSliderToSet(set, BuildSlider("LabiaMorePuffyness_v2", RandomFloat(0.0f, 1.0f) ));
			}

			AddSliderToSet(set, BuildSlider("Labiaprotrude", RandomFloat(0.0f, 0.5f) ));
			AddSliderToSet(set, BuildSlider("Labiaprotrude2", RandomFloat(0.0f, 0.1f) ));
			AddSliderToSet(set, BuildSlider("Labiaprotrudeback", RandomFloat(0.0f, 0.1f) ));

			AddSliderToSet(set, BuildSlider("Labiaspread", 0.0f ));

			AddSliderToSet(set, BuildSlider("LabiaCrumpled_v2", RandomFloat(0.0f, 0.3f) ));

			AddSliderToSet(set, BuildSlider("LabiaBulgogi_v2", 0.0f ));
			AddSliderToSet(set, BuildSlider("LabiaNeat_v2", 0.0f ));

			AddSliderToSet(set, BuildSlider("VaginaHole", RandomFloat(-0.2f, 0.05f) ));

			AddSliderToSet(set, BuildSlider("Clit", RandomFloat(-0.4f, 0.25f) ));

		} else if (ChanceRoll(75)) {
			//average
			AddSliderToSet(set, BuildSlider("Innieoutie", RandomFloat(0.4f, 0.75f) ));

			if (ChanceRoll(40)) {
				AddSliderToSet(set, BuildSlider("Labiapuffyness", RandomFloat(0.50f, 1.00f) ));
			}

			if (ChanceRoll(30)) {
				AddSliderToSet(set, BuildSlider("LabiaMorePuffyness_v2", RandomFloat(0.0f, 0.75f) ));
			}

			AddSliderToSet(set, BuildSlider("Labiaprotrude", RandomFloat(0.0f, 0.5f) ));
			AddSliderToSet(set, BuildSlider("Labiaprotrude2", RandomFloat(0.0f, 0.75f) ));
			AddSliderToSet(set, BuildSlider("Labiaprotrudeback", RandomFloat(0.0f, 1.0f) ));

			if (ChanceRoll(50)) {
				AddSliderToSet(set, BuildSlider("Labiaspread", RandomFloat(0.0f, 1.0f) ));
				AddSliderToSet(set, BuildSlider("LabiaCrumpled_v2", RandomFloat(0.0f, 0.7f) ));
				
				if (ChanceRoll(60)) {
					AddSliderToSet(set, BuildSlider("LabiaBulgogi_v2", RandomFloat(0.0f, 0.1f) ));
				}
			} else {
				AddSliderToSet(set, BuildSlider("Labiaspread", 0.0f ));
				AddSliderToSet(set, BuildSlider("LabiaCrumpled_v2", RandomFloat(0.0f, 0.2f) ));
				
				if (ChanceRoll(45)) {
					AddSliderToSet(set, BuildSlider("LabiaBulgogi_v2", RandomFloat(0.0f, 0.3f) ));
				}
			}

			AddSliderToSet(set, BuildSlider("LabiaNeat_v2", 0.0f ));

			AddSliderToSet(set, BuildSlider("VaginaHole", RandomFloat(-0.2f, 0.40f) ));

			AddSliderToSet(set, BuildSlider("Clit", RandomFloat(-0.2f, 0.25f) ));
		} else{
			//outie

			AddSliderToSet(set, BuildSlider("Innieoutie", RandomFloat(-0.25f, 0.30f) ));

			if (ChanceRoll(30)) {
				AddSliderToSet(set, BuildSlider("Labiapuffyness", RandomFloat(0.20f, 0.50f) ));
			}

			if (ChanceRoll(10)) {
				AddSliderToSet(set, BuildSlider("LabiaMorePuffyness_v2", RandomFloat(0.0f, 0.35f) ));
			}

			AddSliderToSet(set, BuildSlider("Labiaprotrude", RandomFloat(0.0f, 1.0f) ));
			AddSliderToSet(set, BuildSlider("Labiaprotrude2", RandomFloat(0.0f, 1.0f) ));
			AddSliderToSet(set, BuildSlider("Labiaprotrudeback", RandomFloat(0.0f, 1.0f) ));

			AddSliderToSet(set, BuildSlider("Labiaspread", RandomFloat(0.0f, 1.0f) ));

			AddSliderToSet(set, BuildSlider("LabiaCrumpled_v2", RandomFloat(0.0f, 1.0f) ));

			AddSliderToSet(set, BuildSlider("LabiaBulgogi_v2", RandomFloat(0.0f, 1.0f) ));

			if (ChanceRoll(40)) {
				AddSliderToSet(set, BuildSlider("LabiaNeat_v2", RandomFloat(0.0f, 0.25f) ));
			}
			
			AddSliderToSet(set, BuildSlider("VaginaHole", RandomFloat(0.0f, 1.0f) ));

			AddSliderToSet(set, BuildSlider("Clit", RandomFloat(-0.4f, 0.25f) ));
		}

		AddSliderToSet(set, BuildSlider("Vaginasize", RandomFloat(0.0f, 1.0f) ));
		AddSliderToSet(set, BuildSlider("ClitSwell_v2", RandomFloat(-0.3f, 1.1f) ));
		AddSliderToSet(set, BuildSlider("Cutepuffyness", RandomFloat(0.0f, 1.0f) ));

		AddSliderToSet(set, BuildSlider("LabiaTightUp", RandomFloat(0.0f, 1.0f) ));

		if (ChanceRoll(60)) {
			AddSliderToSet(set, BuildSlider("CBPC", RandomFloat(-0.25f, 0.25f) ));
		} else {
			AddSliderToSet(set, BuildSlider("CBPC", RandomFloat(0.6f, 1.0f) ));
		}

		AddSliderToSet(set, BuildSlider("AnalPosition_v2", RandomFloat(0.0f, 1.0f) ));
		AddSliderToSet(set, BuildSlider("AnalTexPos_v2", RandomFloat(0.0f, 1.0f) ));
		AddSliderToSet(set, BuildSlider("AnalTexPosRe_v2", RandomFloat(0.0f, 1.0f) ));

		AddSliderToSet(set, BuildSlider("AnalLoose_v2", -0.1f ));

		return set;
	}
	
	float OBody::RandomFloat(float a, float b)
	{
		// non-inclusive
    	float random = ((float) rand()) / (float) RAND_MAX;
    	float diff = b - a;
    	float r = random * diff;
    	return a + r;
	}

	int OBody::RandomInt(int a, int b)
	{
		// non-inclusive
    	float random = ((float) rand()) / (float) RAND_MAX;
    	int diff = b - a;
    	int r = (int) (random * diff);
    	return a + r;
	}

	bool OBody::ChanceRoll(int chance)
	{
		float roll = RandomFloat(0.0f, 99.0f);
		return roll <= static_cast<float>(chance);
	}

	SliderSet OBody::GenerateClotheSliders(RE::Actor* act)
	{
		SliderSet set;

		// breasts
		// make area on sides behind breasts not sink in
		AddSliderToSet(set, BuildDerivativeSlider(act, "BreastSideShape", 0.0f));
		// make area under breasts not sink in
		AddSliderToSet(set, BuildDerivativeSlider(act, "BreastUnderDepth", 0.0f));
		// push breasts together
		AddSliderToSet(set, BuildDerivativeSlider(act, "BreastCleavage", 1.0f));
		// push up smaller breasts more
		AddSliderToSet(set, BuildSlider("BreastGravity2", -0.1f, -0.05f));
		// Make top of breast rise higher
		AddSliderToSet(set, BuildSlider("BreastTopSlope", -0.2f, -0.35f));
		// push breasts together
		AddSliderToSet(set, BuildSlider("BreastsTogether", 0.3f, 0.35f));
		// push breasts up
		//AddSliderToSet(set, BuildSlider("PushUp", 0.6f, 0.4f));
		// Shrink breasts slightly
		AddSliderToSet(set, BuildSlider("Breasts", -0.05f));
		// Move breasts up on body slightly
		AddSliderToSet(set, BuildSlider("BreastHeight", 0.15f));
		// Fix "sock" chest
		//AddSliderToSet(set, BuildSlider("BreastsConverage_v2", 0.00f, 0.35f));

		// butt 
		// remove butt impressions
		AddSliderToSet(set, BuildDerivativeSlider(act, "ButtDimples", 0.0f));
		AddSliderToSet(set, BuildDerivativeSlider(act, "ButtUnderFold", 0.0f));
		// shrink ass slightly
		AddSliderToSet(set, BuildSlider("AppleCheeks", -0.05f));
		AddSliderToSet(set, BuildSlider("Butt", -0.05f));

		// Torso
		// remove definition on clavical bone
		AddSliderToSet(set, BuildDerivativeSlider(act, "Clavicle_v2", 0.0f));
		// Push out navel
		AddSliderToSet(set, BuildDerivativeSlider(act, "NavelEven", 1.0f));

		// hip
		// remove defintion on hip bone
		AddSliderToSet(set, BuildDerivativeSlider(act, "HipCarved", 0.0f));

		// nipple
		// sublte change to tip shape
		AddSliderToSet(set, BuildDerivativeSlider(act, "NippleDip", 0.0f));
		AddSliderToSet(set, BuildDerivativeSlider(act, "NippleTip", 0.0f));
		//flatten areola 
		AddSliderToSet(set, BuildDerivativeSlider(act, "NipplePuffy_v2", 0.0f));
		// shrink areola
		AddSliderToSet(set, BuildDerivativeSlider(act, "AreolaSize", -0.3f));
		// flatten nipple
		AddSliderToSet(set, BuildDerivativeSlider(act, "NipBGone", 1.0f));
		AddSliderToSet(set, BuildDerivativeSlider(act, "NippleManga", -0.75f));
		// push nipples together
		AddSliderToSet(set, BuildSlider("NippleDistance", 0.05f, 0.08f));
		// Lift large breasts up
		AddSliderToSet(set, BuildSlider("NippleDown", 0.0f, -0.1f));
		// Flatten nipple + areola 
		AddSliderToSet(set, BuildDerivativeSlider(act, "NipplePerkManga", -0.25f));
		// Flatten nipple
		AddSliderToSet(set, BuildDerivativeSlider(act, "NipplePerkiness", 0.0f));

		return set;
	}

	std::vector<RE::BSFixedString> OBody::GetPresets(RE::Actor* act)
	{
		std::vector<RE::BSFixedString> ret;
		PresetDatabase base = FemalePresets;

		if (!IsFemale(act)) {
			base = MalePresets;
		}

		for (auto i = base.presets.begin(); i != base.presets.end(); ++i) {
        	auto preset = (*i);
        	ret.push_back(preset.name);
        }

        return ret;
	}

	BodyslidePreset OBody::GetPresetByName(PresetDatabase& database, std::string name)
	{
		for (auto i = database.presets.begin(); i != database.presets.end(); ++i) {
        	auto preset = (*i);
        	if (strcmp(preset.name.c_str(), name.c_str()) == 0) {
        		return preset;
        	}
        }

        return database.presets[0];
	}

	void OBody::GenBodyByName(RE::Actor* act, std::string PresetName)
	{
		// do not send this an invalid name, you have been warned
		BodyslidePreset preset;

		if (IsFemale(act)){
			preset = GetPresetByName(FemalePresets, PresetName);
		} else {
			preset = GetPresetByName(MalePresets, PresetName);
		}

		//if (preset != null){
			GenerateFullBodyFromPreset(act, preset);
		//}
	}

	BodyslidePreset OBody::GenerateSinglePresetFromFile(std::string file)
	{
		return GeneratePresetsFromFile(file)[0];
	}

	void OBody::GenBodyByFile(RE::Actor* act, std::string path)
	{
		logger::info("Path: {}", path);
		GenerateFullBodyFromPreset(act, GenerateSinglePresetFromFile(path));
	}	

	std::vector<BodyslidePreset> OBody::GeneratePresetsFromFile(std::string file)
	{
		auto doc = GetDocFromFile(file);
		return GeneratePresetsFromDoc(doc);
	}

	std::vector<BodyslidePreset> OBody::GeneratePresetsFromDoc(pugi::xml_document& doc)
	{
		pugi::xml_node presetNode = doc.child("SliderPresets");

		std::vector<BodyslidePreset> ret;
		for (pugi::xml_node_iterator it = presetNode.begin(); it != presetNode.end(); ++it){
			ret.push_back(GeneratePresetFromNode((*it)));
		}

		return ret;
	}

	BodyslidePreset OBody::GeneratePresetFromNode(pugi::xml_node node)
	{
		std::string name = node.attribute("name").value();
		std::string body = node.attribute("set").value(); 
		auto sliderset = GenerateSlidersetFromNode(node, GetBodyType(body));
		std::vector<ScoreSet> scores; 

		ScoreSet breasts;
		breasts.name = "breasts";
		breasts.MinScore = GetBreastScore(sliderset, false);
		breasts.MaxScore = GetBreastScore(sliderset, true);

		ScoreSet butt;
		butt.name = "butt";
		butt.MinScore = GetButtScore(sliderset, false);
		butt.MaxScore = GetButtScore(sliderset, true);

		ScoreSet waist;
		waist.name = "waist";
		waist.MinScore = GetWaistScore(sliderset, false);
		waist.MaxScore = GetWaistScore(sliderset, true);

		scores.push_back(breasts);
		scores.push_back(butt);
		scores.push_back(waist);

		BodyslidePreset ret;
		ret.name = name;
		ret.body = body;
		ret.scores = scores;
		ret.sliders = sliderset; 

		//PrintPreset(ret);
		//PrintSliderSet(sliderset);

		return ret;
	}

	// 0 - cbbe
	// 1 - unp
	SliderSet OBody::GenerateSlidersetFromNode(pugi::xml_node& node, int body)
	{
		//logger::info("Body type: {}", body);
		SliderSet ret; 
    	for (pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it) {
    		//logger::info("Name: {}", it->name());

    		if (strcmp(it->name(), "SetSlider") == 0 )
    		{
    			//logger::info((*it).attribute("name").value());

    			bool inverted = false; 
    			if ((body == 1)){
    				if (find(oneDefaultSliders.begin(), oneDefaultSliders.end(), (*it).attribute("name").value()) != oneDefaultSliders.end()){
    					inverted = true;
    				}
    			}
    					
    			float min;
    			float max;

    			if (strcmp((*it).attribute("size").value(), "big") == 0) {
    				max = (float) atof( (*it).attribute("value").value() );	
    				max = max / 100;

    				if (inverted){
    					max = 1.0f - max;
    				}

    				min = 0.0f;

    				//logger::info("Big value: {}", values.second);
    			} else {
    				min = (float) atof( (*it).attribute("value").value() );	
    				min = min / 100;

    				if (inverted){
    					min = 1.0f - min;
    				}

    				max = 0.0f;
    			}

    			AddSliderToSet(ret, BuildSlider((*it).attribute("name").value(), min, max ), inverted);
    		}
    	}

    	/*
    	if (body == 1){
    		for (auto i = oneDefaultSliders.begin(); i != oneDefaultSliders.end(); ++i){
    			string slidername = (*i);

    			if (!SliderSetContainsSlider(ret, slidername)){
    				AddSliderToSet(ret, BuildSlider(slidername, 0.0f), true);
    			}
    		}

    		// oneDefaultSliders
    	}
    	*/

		return ret;
	}

	bool OBody::SliderSetContainsSlider(SliderSet& set, std::string slidername)
	{
		for (auto i = set.sliders.begin(); i != set.sliders.end(); ++i) {
			auto slider = (*i);

			if (strcmp(slider.name.c_str(), slidername.c_str()) == 0) {
				return true;
			}
		}

		return false;
	}

	int OBody::GetBodyType(std::string body)
	{
		if (StringContains(body, "unp") || StringContains(body, "UNP") || StringContains(body, "Unp") || StringContains(body, "coco") || StringContains(body, "COCO") || StringContains(body, "Coco")) {
			return 1;  // unp/coco
		}

		return 0;
	}
	
	void OBody::AddSliderToSet(SliderSet& sliderset, Slider slider)
	{
		AddSliderToSet(sliderset, slider, false);
	}	

	void OBody::AddSliderToSet(SliderSet& sliderset, Slider slider, [[maybe_unused]] bool inverted)
	{
		float val = 0;
//		if (inverted) {
//			val = 1.0f;
//		}

		if ( !sliderset.sliders.empty() &&( strcmp(sliderset.sliders.back().name.c_str(), slider.name.c_str()) == 0 )) {
			// merge
			if ((sliderset.sliders.back().min == val) && (slider.min != val)) {
				// fill in small
				sliderset.sliders.back().min = slider.min;
			} else if((sliderset.sliders.back().max == val) && (slider.max != val)) {
				//fill in large
				sliderset.sliders.back().max = slider.max;
			}
		} else {
			sliderset.sliders.push_back(slider);
		}
	}

	Slider OBody::BuildSlider(std::string name, float min, float max)
	{
		return Slider{ name, min, max };
	}

	Slider OBody::BuildSlider(std::string name, float value)
	{
		return BuildSlider(name, value, value);
	}

	Slider OBody::BuildDerivativeSlider(RE::Actor* act, std::string morph, float target)
	{
		return BuildSlider(morph, target - GetMorph(act, morph));
	}
	
	void OBody::PrintPreset(BodyslidePreset preset)
	{
		logger::info(">Preset name: {}", preset.name);
		logger::info(">Preset Body: {}", preset.body);

		for (auto i = preset.scores.begin(); i != preset.scores.end(); ++i){
			auto score = (*i);
			logger::info("> Score: {}", score.name);

			logger::info(">  Minimum score: {}", score.MinScore);
			logger::info(">  Maximum score: {}", score.MaxScore);

		}

		PrintSliderSet(preset.sliders);
	}

	void OBody::PrintDatabase(PresetDatabase& database)
	{
		logger::info("Printing database");
		for (auto i = database.presets.begin(); i != database.presets.end(); ++i){
			auto preset = (*i);
			PrintPreset(preset);
		}
	}

	float OBody::GetScoreByWeight(ScoreSet& score, float& weight)
	{
		return ((score.MaxScore - score.MinScore) * weight) + score.MinScore;
	}

	void OBody::SaveScoreToActor(RE::Actor* act, ScoreSet& score, float& weight)
	{
		float val = GetScoreByWeight(score, weight);
		SetMorph(act, "obody_score_" + score.name, val, "OBody");
	}

	void OBody::PrintSliderSet(SliderSet set)
	{
		for (auto i = set.sliders.begin(); i != set.sliders.end(); ++i){
        	auto slider = (*i);

        	logger::info(">    Slider name: {}", slider.name);
        	logger::info(">        Small value: {}", slider.min);
        	logger::info(">        Large value: {}", slider.max);
        }	
	}
	

	pugi::xml_document OBody::GetDocFromFile(std::string pathToFile)
	{
		pugi::xml_document doc;
		doc.load_file(pathToFile.c_str());
		//pugi::xml_parse_result result = doc.load_file(pathToFile.c_str());
		//logger::info("Loaded: {}", result);

		return doc;
	}

	void OBody::BuildRaceStatDB()
	{
		auto doc = GetDocFromFile("Data\\obody.xml");

		pugi::xml_node SettingNode = doc.child("Enable");

		int val = atoi(SettingNode.attribute("value").value() );
		if (val != 1){
			logger::info("Racesettings turned off, exiting");
			return;
		}

		pugi::xml_node raceNode = doc.child("Races");

		for (pugi::xml_node_iterator it = raceNode.begin(); it != raceNode.end(); ++it){
			RaceStats.races.push_back(GenerateRaceStatFromNode(*it));
		}
	}

	RaceStat OBody::GenerateRaceStatFromNode(pugi::xml_node node)
	{
		RaceStat ret;

		ret.name = node.attribute("name").value();
		ret.bodypart = node.attribute("bodypart").value();
		ret.value = std::atoi(node.attribute("value").value());

		//logger::info("Name: {}", name);

		return ret;
	}

	std::vector<std::string> OBody::GetFilesInBodyslideDir()
	{
		std::vector<std::string> files;
		std::string path = "Data\\CalienteTools\\BodySlide\\SliderPresets\\";
   		for (const auto & entry : fs::directory_iterator(path)) {
			std::string stringpath = entry.path().string();
   			if (IsClothedSet(stringpath) || !StringContains(stringpath, "xml")) {
   				//
   			} else{
				files.push_back(stringpath);
        		//logger::info(stringpath.c_str());
   			}	
        }

        return files;
	}

	void OBody::GenerateDatabases()
	{
		// Generate scores

		oneDefaultSliders.push_back("Breasts");
		oneDefaultSliders.push_back("BreastsSmall");
		oneDefaultSliders.push_back("NippleDistance");
		oneDefaultSliders.push_back("NippleSize");
		oneDefaultSliders.push_back("ButtCrack");
		oneDefaultSliders.push_back("Butt");
		oneDefaultSliders.push_back("ButtSmall");
		oneDefaultSliders.push_back("Legs");
		oneDefaultSliders.push_back("Arms");
		oneDefaultSliders.push_back("ShoulderWidth");

		// breasts (cbbe)
        BreastScores.scores.push_back({"DoubleMelon", 30});
        BreastScores.scores.push_back({"BreastsFantasy", 28});
        BreastScores.scores.push_back({"Breasts", 25});
        BreastScores.scores.push_back({"BreastsNewSH", 45});

        BreastScores.scores.push_back({"BreastFlatness", -30});
        BreastScores.scores.push_back({"BreastFlatness2", -30});

        BreastScores.scores.push_back({"BreastsSmall", -15});
        BreastScores.scores.push_back({"BreastsSmall2", -15});
        BreastScores.scores.push_back({"BreastsGone", -25});

        BreastScores.scores.push_back({"BreastPerkiness", -10});

        BreastScores.scores.push_back({"OldBaseShape", 33});
        BreastScores.scores.push_back({"7B Upper", 50});
        BreastScores.scores.push_back({"VanillaSSEHi", 20});
        BreastScores.scores.push_back({"VanillaSSELo", -10});

        // butt (cbbe)
        ButtScores.scores.push_back({"AppleCheeks", 30});
        ButtScores.scores.push_back({"BigButt", 20});
        ButtScores.scores.push_back({"ChubbyButt", 20});
        ButtScores.scores.push_back({"Butt", 15});
        ButtScores.scores.push_back({"ButtSaggy_v2", 5});
        ButtScores.scores.push_back({"ButtShape2", 5});
        ButtScores.scores.push_back({"RoundAss", 7});
        BreastScores.scores.push_back({"7B Lower", 15});

        ButtScores.scores.push_back({"ButtSmall", -5});
        ButtScores.scores.push_back({"ButtPressed_v2", -10});

        // waist (cbbe)
        WaistScores.scores.push_back({"BigTorso", 20});
        WaistScores.scores.push_back({"ChubbyWaist", 30});
        WaistScores.scores.push_back({"Waist", 12});
        WaistScores.scores.push_back({"WideWaistLine", 15});

        WaistScores.scores.push_back({"MuscleMoreAbs_v2", 15});

        WaistScores.scores.push_back({"Belly", 2});
        WaistScores.scores.push_back({"BigBelly", 10});

        WaistScores.scores.push_back({"PregnancyBelly", 50});

        WaistScores.scores.push_back({"7B Lower", 25});
        WaistScores.scores.push_back({"Vanilla SSE High", 25});

		std::vector<std::string> files = GetFilesInBodyslideDir();

		std::vector<BodyslidePreset> presets;
		for (auto i = files.begin(); i != files.end(); ++i){
			presets = GeneratePresetsFromFile(*i);

			if (IsFemalePreset(presets[0])){
				AddPresetToDatabase(FemalePresets, presets);
			} else{
				AddPresetToDatabase(MalePresets, presets);
			}
        }

        BuildRaceStatDB();

        logger::info("Female presets loaded: {}", FemalePresets.presets.size());
        logger::info("Male presets loaded: {}", MalePresets.presets.size());
	}

	bool OBody::IsFemalePreset(BodyslidePreset& preset) {
		return !( StringContains(preset.body, "HIMBO") || StringContains(preset.body, "himbo") || StringContains(preset.body, "Himbo") || StringContains(preset.body, "Talos") || StringContains(preset.body, "talos") || StringContains(preset.body, "TALOS") );
	}

	BodyslidePreset OBody::GetRandomElementOfDatabase(PresetDatabase& database)
	{
		std::random_device seed;
   		// generator 
  		std::mt19937 engine( seed( ) ) ;
   		// number distribution
		auto size = static_cast<int>(database.presets.size());
		std::uniform_int_distribution<int> choose(0, size - 1);
   		return database.presets[ choose( engine ) ];
	}

	bool OBody::IsClothedSet(std::string set)
	{
		return (StringContains(set, "Outfit") || StringContains(set, "outfit") || StringContains(set, "OUTFIT") || StringContains(set, "Cloth") || StringContains(set, "CLOTH") || StringContains(set, "cloth"));
	}

	bool OBody::IsFemale(RE::Actor* act)
	{
		return act->GetActorBase()->GetSex() == 1;
	}

	void OBody::AddPresetToDatabase(PresetDatabase& database, BodyslidePreset preset)
	{
		if (IsClothedSet(preset.name)) {
			// do not add
		} else {
			database.presets.push_back(preset);
			logger::info("Adding preset: {}", preset.name);
		}
	}

	void OBody::AddPresetToDatabase(PresetDatabase& database, std::vector<BodyslidePreset> presets)
	{
		for (auto i = presets.begin(); i != presets.end(); ++i)
        	AddPresetToDatabase(database, *i);
	}

	bool OBody::StringContains(std::string& str, const char* testcase)
	{
		std::string s = testcase;
		return (str).find(s) != std::string::npos;

		/*
		auto text = str;
		auto subtext = string(testcase);

		transform(text.begin(), text.end(), text.begin(),
			[](unsigned char c) { return tolower(c); });

		transform(subtext.begin(), subtext.end(), subtext.begin(),
			[](unsigned char c) { return tolower(c); });

		auto it = search(text.begin(), text.end(), boyer_moore_searcher(subtext.begin(), subtext.end()));
		return it != text.end();
		*/
	}

	void OBody::ApplyMorphs(RE::Actor* act)
	{
		morphInt->ApplyBodyMorphs(act->AsReference(), true);
		morphInt->UpdateModelWeight(act->AsReference(), false);
	}

	void OBody::RegisterQuestForEvent(RE::TESQuest* quest)
	{
		OnActorGenerated.Register(quest);
	}

	float OBody::GetWeight(RE::Actor* act)
	{
		float ret = act->GetActorBase()->GetWeight();
		ret = ret / 100;

		//logger::info("Weight: {}", ret);
		return ret;
	}
}
