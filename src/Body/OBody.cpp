#include "Body/OBody.h"



using namespace std;

inline SKSE::RegistrationSet<RE::Actor*> OnActorGenerated("OnActorGenerated"sv);


namespace Body{

	vector<string> oneDefaultSliders;
	
	struct SliderSet
	{
		vector<struct Slider> sliders;
	};
	struct BodyslidePreset
	{
		string name;
		string body;
		vector<struct ScoreSet> scores;
		struct SliderSet sliders;
	};
	struct Slider
	{
		string name;
		float min;
		float max;
	};
	struct PresetDatabase
	{
		vector<struct BodyslidePreset> presets;
	};
	struct ScoreSet
	{
		string name;
		float MinScore; // 100 = large?
		float MaxScore;
	};
	struct BodysliderSliderScore
	{
		string name;
		int score;
	};
	struct BodypartScoreset
	{
		vector<struct BodysliderSliderScore> scores;
	};
	struct RaceStat
	{
		string name;
		string bodypart;
		int value;
	};
	struct RaceStatDatabase
	{
		vector<struct RaceStat> races;
	};

	
	struct RaceStatDatabase RaceStats;

	struct PresetDatabase FemalePresets;
	struct PresetDatabase MalePresets;

	struct BodypartScoreset BreastScores;
	struct BodypartScoreset ButtScores;
	struct BodypartScoreset WaistScores;



	bool UseRaceStats = true;


	float OBody::GetBodypartScore(struct BodypartScoreset& bodypartSet, struct SliderSet sliders, bool max){
		//PrintSliderSet(sliders);

		float ret = 0.0f;
		for (auto i = sliders.sliders.begin(); i != sliders.sliders.end(); ++i){
			struct Slider slider = (*i);

			float mult;
			string name = slider.name;
			if (max){
				mult = slider.max;
			} else {
				mult = slider.min;
			}

			int val = GetSliderScore(bodypartSet, slider.name);

			//logger::info("Trying: {}", slider.name);

			ret = ret + (((float) val) * mult);
		}

		//logger::info("Final score: {}", ret);
		return ret;
	}

	float OBody::GetBreastScore(struct SliderSet sliders, bool max){
		return GetBodypartScore(BreastScores, sliders, max);
	}

	float OBody::GetButtScore(struct SliderSet sliders, bool max){
		return GetBodypartScore(ButtScores, sliders, max);
	}

	float OBody::GetWaistScore(struct SliderSet sliders, bool max){
		return GetBodypartScore(WaistScores, sliders, max);
	}

	int OBody::GetSliderScore(struct BodypartScoreset& scoreset, string slidername){
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

	int OBody::GetFemaleDatabaseSize(){
		return (int)FemalePresets.presets.size();
	}
	int OBody::GetMaleDatabaseSize(){
		return (int)MalePresets.presets.size();
	}

	const fs::path root_path("Data\\CalienteTools\\BodySlide\\SliderPresets");

	bool set_ORefit = true;
	bool set_NippleRand = true;
	bool set_GenitalRand = true;

	OBody* OBody::GetInstance(){
			static OBody instance;
			return &instance;
		}

	void OBody::SetLoaded(bool a){
		GameLoaded = a;
	}

	void OBody::SetORefit(bool a){
		set_ORefit = a;
	}

	void OBody::SetNippleRand(bool a){
		set_NippleRand = a;
	}

	void OBody::SetGenitalRand(bool a){
		set_GenitalRand = a;
	}

	struct PresetDatabase OBody::SortPresetDatabaseByRaceStat(struct PresetDatabase& database, struct RaceStat stat){
		return SortPresetDatabaseByBodypart(database, stat.bodypart);
	}

	struct PresetDatabase OBody::SortPresetDatabaseByBodypart(struct PresetDatabase& database, string bodypart){
		struct PresetDatabase ret;
		auto arr = database.presets;

		int n = (int) arr.size();
 
        // One by one move boundary of unsorted subarray
        for (int i = 0; i < n-1; i++)
        {
            // Find the minimum element in unsorted array
            int min_idx = i;
            for (int j = i+1; j < n; j++){
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


	struct ScoreSet OBody::GetScoresetFromPresetByName(struct BodyslidePreset& preset, string scorename){

		

		for (auto i = preset.scores.begin(); i != preset.scores.end(); ++i){
			auto score = (*i);

			if (strcmp(score.name.c_str(), scorename.c_str()) == 0){
				return score;
			}
		}

		struct ScoreSet blank;

		return blank;
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

	void OBody::ProcessActor(RE::Actor* act){
		///morphInt->EvaluateBodyMorphs(act);

		if (!GameLoaded){
			logger::info("Game not fully loaded, skipping actor");
			return;
		}

		//logger::info("Has: {}", GetMorph(act, "obody_processed"));
		//if (morphInt->HasBodyMorphKey(act, key)){

		if (IsProcessed(act)){
			// ignore, already set
			logger::info("Skipping: {}", act->GetName());
		} else{
			logger::info("Processing: {}", act->GetName());

			GenerateActorBody(act);

		}
		

	}

	bool OBody::IsProcessed(RE::Actor* act){
		return GetMorph(act, "obody_processed") == 1.0f;
	}

	void OBody::ProcessActorEquipEvent(RE::Actor* act, bool RemovingBodyArmor){	
		if (!IsProcessed(act)){
			return;
		}
	

		if (HasActiveClothePreset(act) && (IsNaked(act) || RemovingBodyArmor)){
			//logger::info("Removing clothe preset");
			RemoveClothePreset(act);
			ApplyMorphs(act);
		} else if(!HasActiveClothePreset(act) && !IsNaked(act) && set_ORefit){
			//logger::info("adding clothe preset");
			ApplyClothePreset(act);
			ApplyMorphs(act);
		}
	}

	bool OBody::SetMorphInterface(SKEE::IBodyMorphInterface* a_morphInt)
	{
		return a_morphInt->GetVersion() ? this->morphInt = a_morphInt : false;
	}

	void OBody::GenerateActorBody(RE::Actor* act){
		struct BodyslidePreset preset;


		if (IsFemale(act)){
			if (GetFemaleDatabaseSize() < 1){
				SetMorph(act, "obody_processed", 1.0f, "OBody");
				OnActorGenerated.SendEvent(act);
				return;
			}
			if (RaceStats.races.size() > 0){
				// user is using the XML race feature
				auto RaceStat = GetCorrespondingRaceStat(act);
				if (RaceStat.value > -1){
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



				} else{
					preset = GetRandomElementOfDatabase(FemalePresets);
				}
				

			} else {
				preset = GetRandomElementOfDatabase(FemalePresets);
			}
			
		} else {
			if (GetMaleDatabaseSize() < 1){
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

	void OBody::GenerateFullBodyFromPreset(RE::Actor* act, struct BodyslidePreset preset){
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

		if (IsFemale(act)){
			// random nipples
			if (set_NippleRand){
				ApplySliderSet(act, GenerateRandomNippleSliders(), "OBody");
			}

			// random vagina
			if (set_GenitalRand){
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

	bool OBody::HasActiveClothePreset(RE::Actor* act){
		return morphInt->HasBodyMorphKey(act, "OClothe");
	}

	void OBody::RemoveClothePreset(RE::Actor* act){
		morphInt->ClearBodyMorphKeys(act, "OClothe");
	}

	void OBody::ApplyClothePreset(RE::Actor* act){
		auto clotheSet = GenerateClotheSliders(act);

		//PrintSliderSet(clotheSet);

		ApplySliderSet(act, clotheSet, "OClothe");
	}

	bool OBody::IsNaked(RE::Actor* act){
		auto* changes = act->GetInventoryChanges();
		auto* const armor = changes->GetArmorInSlot(32);

		
		if (armor){
			return false;
		} else {
			return true;
		}

	}

	struct RaceStat OBody::GetCorrespondingRaceStat(RE::Actor* act){
		struct RaceStat ret;
		string RaceName = act->GetActorBase()->GetRace()->GetName();

		for (auto i = RaceStats.races.begin(); i != RaceStats.races.end(); ++i){
			auto stat = (*i);

			if (strcmp(stat.name.c_str(), RaceName.c_str()) == 0){
				return stat;
			}
		}

		return ret;
	}

	void OBody::SetMorph(RE::Actor* act, string MorphName, float value, string key){
		//logger::info(">> Setting morph: {}", MorphName);
		//logger::info(">>  With morph value: {}", value);
		morphInt->SetMorph(act->AsReference(), MorphName.c_str(), key.c_str(), value);
	}

	float OBody::GetMorph(RE::Actor* act, string MorphName){

		return morphInt->GetMorph(act->AsReference(), MorphName.c_str(), "OBody");
	}

	void OBody::SetMorphByWeight(RE::Actor* act, struct Slider slider, float weight, string key){
		float value = ((slider.max - slider.min) * weight) + slider.min;
		SetMorph(act, slider.name, value, key);
	}

	void OBody::ApplySlider(RE::Actor* act, struct Slider slid, float weight, string key){
		SetMorphByWeight(act, slid, weight, key);
	}

	void OBody::ApplySliderSet(RE::Actor* act, struct SliderSet sliderset, string key){
		auto weight = GetWeight(act);
		for (auto i = sliderset.sliders.begin(); i != sliderset.sliders.end(); ++i)
        	ApplySlider(act, *i, weight, key);
	}

	void OBody::ApplyBodyslidePreset(RE::Actor* act, struct BodyslidePreset preset ){
		ApplySliderSet(act, preset.sliders, "OBody");
		//PrintPreset(preset);
	}

	struct SliderSet OBody::GenerateRandomNippleSliders(){
		struct SliderSet set;

		if (ChanceRoll(15)){
			AddSliderToSet(set, BuildSlider("AreolaSize", RandomFloat(-1.0f, 0.0f) ));
		} else{
			AddSliderToSet(set, BuildSlider("AreolaSize", RandomFloat(0.0f, 1.0f) ));
		}

		if (ChanceRoll(75)){
			AddSliderToSet(set, BuildSlider("AreolaPull_v2", RandomFloat(-0.25f, 1.0f) ));
		}
		
		if (ChanceRoll(15)){
			AddSliderToSet(set, BuildSlider("NippleLength", RandomFloat(0.2f, 0.3f) ));
		} else{
			AddSliderToSet(set, BuildSlider("NippleLength", RandomFloat(0.0f, 0.1f) ));
		}

		AddSliderToSet(set, BuildSlider("NippleManga", RandomFloat(-0.3f, 0.8f) ));

		if (ChanceRoll(25)){
			AddSliderToSet(set, BuildSlider("NipplePerkManga", RandomFloat(-0.3f, 1.2f) ));
		}

		if (ChanceRoll(15)){
			AddSliderToSet(set, BuildSlider("NipBGone", RandomFloat(0.6f, 1.0f) ));
		}

		AddSliderToSet(set, BuildSlider("NippleSize", RandomFloat(-0.5f, 0.3f) ));

		AddSliderToSet(set, BuildSlider("NippleDip", RandomFloat(0.0f, 1.0f) ));

		AddSliderToSet(set, BuildSlider("NippleCrease_v2", RandomFloat(-0.4f, 1.0f) ));

		if (ChanceRoll(6)){
			AddSliderToSet(set, BuildSlider("NipplePuffy_v2", RandomFloat(0.4f, 0.7f) ));
		}

		if (ChanceRoll(35)){
			AddSliderToSet(set, BuildSlider("NippleThicc_v2", RandomFloat(0.0f, 0.9f) ));
		}

		if (ChanceRoll(2)){
			if (ChanceRoll(50)){
				AddSliderToSet(set, BuildSlider("NippleInvert_v2", 1.0f ));
			} else {
				AddSliderToSet(set, BuildSlider("NippleInvert_v2", RandomFloat(0.65f, 0.8f) ));
			}
		}


		return set;
	}

	struct SliderSet OBody::GenerateRandomGenitalSliders(){
		struct SliderSet set;

		if (ChanceRoll(20)){
			// innie
			AddSliderToSet(set, BuildSlider("Innieoutie", RandomFloat(0.95f, 1.1f) ));

			if (ChanceRoll(50)){
				AddSliderToSet(set, BuildSlider("Labiapuffyness", RandomFloat(0.75f, 1.25f) ));
			}

			if (ChanceRoll(40)){
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

		} else if (ChanceRoll(75)){
			//average
			AddSliderToSet(set, BuildSlider("Innieoutie", RandomFloat(0.4f, 0.75f) ));

			if (ChanceRoll(40)){
				AddSliderToSet(set, BuildSlider("Labiapuffyness", RandomFloat(0.50f, 1.00f) ));
			}

			if (ChanceRoll(30)){
				AddSliderToSet(set, BuildSlider("LabiaMorePuffyness_v2", RandomFloat(0.0f, 0.75f) ));
			}

			AddSliderToSet(set, BuildSlider("Labiaprotrude", RandomFloat(0.0f, 0.5f) ));
			AddSliderToSet(set, BuildSlider("Labiaprotrude2", RandomFloat(0.0f, 0.75f) ));
			AddSliderToSet(set, BuildSlider("Labiaprotrudeback", RandomFloat(0.0f, 1.0f) ));

			if (ChanceRoll(50)){
				AddSliderToSet(set, BuildSlider("Labiaspread", RandomFloat(0.0f, 1.0f) ));
				AddSliderToSet(set, BuildSlider("LabiaCrumpled_v2", RandomFloat(0.0f, 0.7f) ));
				
				if (ChanceRoll(60)){
					AddSliderToSet(set, BuildSlider("LabiaBulgogi_v2", RandomFloat(0.0f, 0.1f) ));
				}
			} else {
				AddSliderToSet(set, BuildSlider("Labiaspread", 0.0f ));
				AddSliderToSet(set, BuildSlider("LabiaCrumpled_v2", RandomFloat(0.0f, 0.2f) ));
				
				if (ChanceRoll(45)){
					AddSliderToSet(set, BuildSlider("LabiaBulgogi_v2", RandomFloat(0.0f, 0.3f) ));
				}
			}
			



			AddSliderToSet(set, BuildSlider("LabiaNeat_v2", 0.0f ));

			AddSliderToSet(set, BuildSlider("VaginaHole", RandomFloat(-0.2f, 0.40f) ));

			AddSliderToSet(set, BuildSlider("Clit", RandomFloat(-0.2f, 0.25f) ));


		} else{
			//outie

			AddSliderToSet(set, BuildSlider("Innieoutie", RandomFloat(-0.25f, 0.30f) ));


			if (ChanceRoll(30)){
				AddSliderToSet(set, BuildSlider("Labiapuffyness", RandomFloat(0.20f, 0.50f) ));
			}

			if (ChanceRoll(10)){
				AddSliderToSet(set, BuildSlider("LabiaMorePuffyness_v2", RandomFloat(0.0f, 0.35f) ));
			}

			AddSliderToSet(set, BuildSlider("Labiaprotrude", RandomFloat(0.0f, 1.0f) ));
			AddSliderToSet(set, BuildSlider("Labiaprotrude2", RandomFloat(0.0f, 1.0f) ));
			AddSliderToSet(set, BuildSlider("Labiaprotrudeback", RandomFloat(0.0f, 1.0f) ));

			AddSliderToSet(set, BuildSlider("Labiaspread", RandomFloat(0.0f, 1.0f) ));

			AddSliderToSet(set, BuildSlider("LabiaCrumpled_v2", RandomFloat(0.0f, 1.0f) ));

			AddSliderToSet(set, BuildSlider("LabiaBulgogi_v2", RandomFloat(0.0f, 1.0f) ));

			if (ChanceRoll(40)){
				AddSliderToSet(set, BuildSlider("LabiaNeat_v2", RandomFloat(0.0f, 0.25f) ));
			}
			

			AddSliderToSet(set, BuildSlider("VaginaHole", RandomFloat(0.0f, 1.0f) ));

			AddSliderToSet(set, BuildSlider("Clit", RandomFloat(-0.4f, 0.25f) ));
		}



		AddSliderToSet(set, BuildSlider("Vaginasize", RandomFloat(0.0f, 1.0f) ));
		AddSliderToSet(set, BuildSlider("ClitSwell_v2", RandomFloat(-0.3f, 1.1f) ));
		AddSliderToSet(set, BuildSlider("Cutepuffyness", RandomFloat(0.0f, 1.0f) ));

		AddSliderToSet(set, BuildSlider("LabiaTightUp", RandomFloat(0.0f, 1.0f) ));

		if (ChanceRoll(60)){
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

	
	float OBody::RandomFloat(float a, float b) {
		// non-inclusive
    	float random = ((float) rand()) / (float) RAND_MAX;
    	float diff = b - a;
    	float r = random * diff;
    	return a + r;
	}

	int OBody::RandomInt(int a, int b) {
		// non-inclusive
    	float random = ((float) rand()) / (float) RAND_MAX;
    	int diff = b - a;
    	int r = (int) (random * diff);
    	return a + r;
	}

	bool OBody::ChanceRoll(int chance) {
		float roll = RandomFloat(0.0f, 99.0f);

		if (roll <= (float) chance){
			return true;
		}
		return false;
	}

	struct SliderSet OBody::GenerateClotheSliders(RE::Actor* act){
		struct SliderSet set;

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


	vector<RE::BSFixedString> OBody::GetPresets(RE::Actor* act){
		vector<RE::BSFixedString> ret;
		struct PresetDatabase base = FemalePresets;

		if (!IsFemale(act)){
			base = MalePresets;
		}

		for (auto i = base.presets.begin(); i != base.presets.end(); ++i){
        	auto preset = (*i);
        	ret.push_back(preset.name);

        }

        return ret;
	}

	struct BodyslidePreset OBody::GetPresetByName(struct PresetDatabase& database, string name){
		for (auto i = database.presets.begin(); i != database.presets.end(); ++i){
        	auto preset = (*i);
        	if (strcmp(preset.name.c_str(), name.c_str()) == 0){
        		return preset;
        	}

        }

        return database.presets[0];

	}

	void OBody::GenBodyByName(RE::Actor* act, string PresetName){
		// do not send this an invalid name, you have been warned
		struct BodyslidePreset preset;

		if (IsFemale(act)){
			preset = GetPresetByName(FemalePresets, PresetName);
		} else {
			preset = GetPresetByName(MalePresets, PresetName);
		}

		//if (preset != null){
			GenerateFullBodyFromPreset(act, preset);
		//}
	}

	struct BodyslidePreset OBody::GenerateSinglePresetFromFile(string file){
		return GeneratePresetsFromFile(file)[0];
	}

	void OBody::GenBodyByFile(RE::Actor* act, string path){
		logger::info("Path: {}", path);
		GenerateFullBodyFromPreset(act, GenerateSinglePresetFromFile(path));
	}	

	vector<struct BodyslidePreset> OBody::GeneratePresetsFromFile(string file){
		auto doc = GetDocFromFile(file);
		return GeneratePresetsFromDoc(doc);
	}

	vector<struct BodyslidePreset> OBody::GeneratePresetsFromDoc(pugi::xml_document& doc){
		pugi::xml_node presetNode = doc.child("SliderPresets");

		vector<struct BodyslidePreset> ret;

		for (pugi::xml_node_iterator it = presetNode.begin(); it != presetNode.end(); ++it){
			ret.push_back(GeneratePresetFromNode((*it)));
		}

		return ret;

		
	}

	struct BodyslidePreset OBody::GeneratePresetFromNode(pugi::xml_node node){
		string name = node.attribute("name").value();
		string body = node.attribute("set").value(); 
		auto sliderset = GenerateSlidersetFromNode(node, GetBodyType(body));
		vector<struct ScoreSet> scores; 

		struct ScoreSet breasts;
		breasts.name = "breasts";
		breasts.MinScore = GetBreastScore(sliderset, false);
		breasts.MaxScore = GetBreastScore(sliderset, true);

		struct ScoreSet butt;
		butt.name = "butt";
		butt.MinScore = GetButtScore(sliderset, false);
		butt.MaxScore = GetButtScore(sliderset, true);

		struct ScoreSet waist;
		waist.name = "waist";
		waist.MinScore = GetWaistScore(sliderset, false);
		waist.MaxScore = GetWaistScore(sliderset, true);

		scores.push_back(breasts);
		scores.push_back(butt);
		scores.push_back(waist);

		struct BodyslidePreset ret;
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
	struct SliderSet OBody::GenerateSlidersetFromNode(pugi::xml_node& node, int body){
		//logger::info("Body type: {}", body);
		struct SliderSet ret; 


    	for (pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
    	{

    		//logger::info("Name: {}", it->name());

    		if ( strcmp(it->name(), "SetSlider") == 0 )
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

    			if ( strcmp((*it).attribute("size").value(), "big") == 0){
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

	bool OBody::SliderSetContainsSlider(struct SliderSet& set, string slidername){
		for (auto i = set.sliders.begin(); i != set.sliders.end(); ++i){
			auto slider = (*i);

			if (strcmp(slider.name.c_str(), slidername.c_str()) == 0){
				return true;
			}
		}

		return false;
	}

	int OBody::GetBodyType(string body){
		if (StringContains(body, "unp") || StringContains(body, "UNP")|| StringContains(body, "Unp") || StringContains(body, "coco") || StringContains(body, "COCO") || StringContains(body, "Coco")){
			return 1; // unp/coco
		} else{
			return 0;
		}
	}
	
	void OBody::AddSliderToSet(struct SliderSet& sliderset, struct Slider slider){
		AddSliderToSet(sliderset, slider, false);
	}	

	void OBody::AddSliderToSet(struct SliderSet& sliderset, struct Slider slider, bool inverted){
//		float val;
//		if (inverted){
//			val = 1.0f;
//		} else{
		inverted;
		float val = 0.0f;
//		}

		if ( !sliderset.sliders.empty() &&( strcmp(sliderset.sliders.back().name.c_str(), slider.name.c_str()) == 0 )){
			// merge
			if ((sliderset.sliders.back().min == val) && (slider.min != val)){
				// fill in small
				sliderset.sliders.back().min = slider.min;
			} else if((sliderset.sliders.back().max == val) && (slider.max != val)){
				//fill in large
				sliderset.sliders.back().max = slider.max;
			}
		} else{
			sliderset.sliders.push_back(slider);
		}
		
		
	}

	struct Slider OBody::BuildSlider(string name, float min, float max){
		struct Slider ret = {name, min, max};

		return ret;
	}

	struct Slider OBody::BuildSlider(string name, float value){
		return BuildSlider(name, value, value);
	}

	struct Slider OBody::BuildDerivativeSlider(RE::Actor* act, string morph, float target){
		return BuildSlider(morph, target - GetMorph(act, morph));
	}

	
	void OBody::PrintPreset(struct BodyslidePreset preset){
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

	void OBody::PrintDatabase(struct PresetDatabase& database){
		logger::info("Printing database");
		for (auto i = database.presets.begin(); i != database.presets.end(); ++i){
			auto preset = (*i);
			PrintPreset(preset);
		}
	}

	float OBody::GetScoreByWeight(struct ScoreSet& score, float& weight){
		return ((score.MaxScore - score.MinScore) * weight) + score.MinScore;
	}

	void OBody::SaveScoreToActor(RE::Actor* act, struct ScoreSet& score, float& weight){
		float val = GetScoreByWeight(score, weight);

		SetMorph(act, "obody_score_" + score.name, val, "OBody");
	}

	void OBody::PrintSliderSet(struct SliderSet set){
		for (auto i = set.sliders.begin(); i != set.sliders.end(); ++i){
        	auto slider = (*i);

        	logger::info(">    Slider name: {}", slider.name);
        	logger::info(">        Small value: {}", slider.min);
        	logger::info(">        Large value: {}", slider.max);
        }
        	
	}
	

	pugi::xml_document OBody::GetDocFromFile(string pathToFile){
		pugi::xml_document doc;
		doc.load_file(pathToFile.c_str());
		//pugi::xml_parse_result result = doc.load_file(pathToFile.c_str());
		//logger::info("Loaded: {}", result);

		return doc;
	}

	void OBody::BuildRaceStatDB(){
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

	struct RaceStat OBody::GenerateRaceStatFromNode(pugi::xml_node node){
		struct RaceStat ret;

		ret.name = node.attribute("name").value();
		ret.bodypart = node.attribute("bodypart").value();
		ret.value = atoi( node.attribute("value").value() );

		//logger::info("Name: {}", name);

		return ret;
	}

	vector<string> OBody::GetFilesInBodyslideDir(){
		vector<string> Files;
		std::string path = "Data\\CalienteTools\\BodySlide\\SliderPresets\\";
   		for (const auto & entry : fs::directory_iterator(path)){
   			string stringpath = entry.path().string();
   			if (IsClothedSet(stringpath) || !StringContains(stringpath, "xml")){
   				//
   				
   			} else{
   				
        		Files.push_back(stringpath);
        		//logger::info(stringpath.c_str());
   			}
        		
        }

        return Files;

	}

	void OBody::GenerateDatabases(){
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

		vector<string> files = GetFilesInBodyslideDir();

		vector<struct BodyslidePreset> presets;
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

	bool OBody::IsFemalePreset(struct BodyslidePreset& preset){
		return !( StringContains(preset.body, "HIMBO") || StringContains(preset.body, "himbo") || StringContains(preset.body, "Himbo") || StringContains(preset.body, "Talos") || StringContains(preset.body, "talos") || StringContains(preset.body, "TALOS") );
	}

	struct BodyslidePreset OBody::GetRandomElementOfDatabase(struct PresetDatabase& database){
  		random_device seed ;
   		// generator 
  		std::mt19937 engine( seed( ) ) ;
   		// number distribution
   		uniform_int_distribution<int> choose( 0 , (int)database.presets.size( ) - 1 ) ;
   		
   		return database.presets[ choose( engine ) ];
	}

	bool OBody::IsClothedSet(string set){
		return (StringContains(set, "Outfit") || StringContains(set, "outfit") || StringContains(set, "OUTFIT") || StringContains(set, "Cloth") || StringContains(set, "CLOTH") || StringContains(set, "cloth"));
	}

	bool OBody::IsFemale(RE::Actor* act){
		return act->GetActorBase()->GetSex() == 1;
	}

	void OBody::AddPresetToDatabase(struct PresetDatabase& database, struct BodyslidePreset preset){
		if (IsClothedSet(preset.name)) {
			// do not add
		} else {
			database.presets.push_back(preset);
			logger::info("Adding preset: {}", preset.name);
		}
		

	}

	void OBody::AddPresetToDatabase(struct PresetDatabase& database, vector<struct BodyslidePreset> presets){
		for (auto i = presets.begin(); i != presets.end(); ++i)
        	AddPresetToDatabase(database, *i);
	}

	bool OBody::StringContains(string& str, const char* testcase){
		string s = testcase;
		return (str).find(s) != string::npos;

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
	void OBody::ApplyMorphs(RE::Actor* act){
		morphInt->ApplyBodyMorphs(act->AsReference(), true);
		morphInt->UpdateModelWeight(act->AsReference(), false);
	}


	void OBody::RegisterQuestForEvent(RE::TESQuest* quest)
	{

		OnActorGenerated.Register(quest);
	}

	float OBody::GetWeight(RE::Actor* act){
		float ret = act->GetActorBase()->GetWeight();
		ret = ret / 100;

		//logger::info("Weight: {}", ret);

		return ret;
	}

}