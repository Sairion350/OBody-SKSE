#include "Body/OBody.h"



using namespace std;


namespace Body{

	
	
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

	


	struct PresetDatabase FemalePresets;
	struct PresetDatabase MalePresets;

	struct BodypartScoreset BreastScores;
	struct BodypartScoreset ButtScores;


	float OBody::GetBodypartScore(struct BodypartScoreset& bodypartSet, struct SliderSet sliders, bool max){
		//PrintSliderSet(sliders);

		float ret = 1.0f;
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

	const fs::path root_path("Data\\CalienteTools\\BodySlide\\SliderPresets");

	OBody* OBody::GetInstance(){
			static OBody instance;
			return &instance;
		}

	void OBody::SetLoaded(bool a){
		GameLoaded = a;
	}

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
			logger::info("Removing clothe preset");
			RemoveClothePreset(act);
			ApplyMorphs(act);
		} else if(!HasActiveClothePreset(act) && !IsNaked(act)){
			logger::info("adding clothe preset");
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
			preset = GetRandomElementOfDatabase(FemalePresets);
		} else {
			preset = GetRandomElementOfDatabase(MalePresets);
		}

		GenerateFullBodyFromPreset(act, preset);
	
		SetMorph(act, "obody_processed", 1.0f, "OBody");
		
	}

	void OBody::GenerateFullBodyFromPreset(RE::Actor* act, struct BodyslidePreset preset){
		morphInt->ClearBodyMorphKeys(act, "OClothe");
		morphInt->ClearBodyMorphKeys(act, "OBody");

		ApplyBodyslidePreset(act, preset);

		logger::info("    Applying preset: {}", preset.name);

		

		if (!IsNaked(act)) {
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

	void OBody::SetMorph(RE::Actor* act, string MorphName, float value, string key){
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
		//PrintPreset(BodyslidePreset);
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
		struct PresetDatabase& base = FemalePresets;

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
		auto sliderset = GenerateSlidersetFromNode(node);
		vector<struct ScoreSet> scores; // todo

		struct ScoreSet breasts;
		breasts.name = "breasts";
		breasts.MinScore = GetBreastScore(sliderset, false);
		breasts.MaxScore = GetBreastScore(sliderset, true);

		struct ScoreSet butt;
		butt.name = "butt";
		butt.MinScore = GetButtScore(sliderset, false);
		butt.MaxScore = GetButtScore(sliderset, true);

		scores.push_back(breasts);
		scores.push_back(butt);

		struct BodyslidePreset ret;
		ret.name = name;
		ret.body = body;
		ret.scores = scores;
		ret.sliders = sliderset; 

		PrintPreset(ret);
		//PrintSliderSet(sliderset);


		return ret;

	}

	struct SliderSet OBody::GenerateSlidersetFromNode(pugi::xml_node& node){
		struct SliderSet ret; 


    	for (pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
    	{

    		//logger::info("Name: {}", it->name());

    		if ( strcmp(it->name(), "SetSlider") == 0 )
    		{

    			//logger::info((*it).attribute("name").value());

    			float min;
    			float max;

    			if ( strcmp((*it).attribute("size").value(), "big") == 0){
    				max = (float) atof( (*it).attribute("value").value() );	
    				max = max / 100;

    				min = 0.0f;

    				//logger::info("Big value: {}", values.second);

    			} else {
    				min = (float) atof( (*it).attribute("value").value() );	
    				min = min / 100;

    				max = 0.0f;
    			}

    			

    			AddSliderToSet(ret, BuildSlider((*it).attribute("name").value(), min, max ));

    		}

    

    	}


		return ret;
	}

	void OBody::AddSliderToSet(struct SliderSet& sliderset, struct Slider slider){
		if ( !sliderset.sliders.empty() &&( strcmp(sliderset.sliders.back().name.c_str(), slider.name.c_str()) == 0 )){
			// merge
			if ((sliderset.sliders.back().min == 0.0f) && (slider.min != 0.0f)){
				// fill in small
				sliderset.sliders.back().min = slider.min;
			} else if((sliderset.sliders.back().max == 0.0f) && (slider.max != 0.0f)){
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
		//PrintSliderSet(preset.sliders);
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
	}
	void OBody::ApplyMorphs(RE::Actor* act){
		morphInt->ApplyBodyMorphs(act->AsReference(), true);
	}

	float OBody::GetWeight(RE::Actor* act){
		float ret = act->GetActorBase()->GetWeight();
		ret = ret / 100;

		//logger::info("Weight: {}", ret);

		return ret;
	}

}