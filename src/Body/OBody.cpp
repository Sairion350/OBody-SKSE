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
		vector<float> scores;
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


	struct PresetDatabase FemalePresets;
	struct PresetDatabase MalePresets;

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

		if (GetMorph(act, "obody_processed") == 1.0f){
			// ignore, already set
			logger::info("Skipping: {}", act->GetName());
		} else{
			logger::info("Processing: {}", act->GetName());

			GenerateActorBody(act);

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
		ApplyBodyslidePreset(act, preset);

		logger::info("    Applying preset: {}", preset.name);

		auto clotheSet = GenerateClotheSliders(act);

		//PrintSliderSet(clotheSet);
		ApplySliderSet(act, clotheSet, "OClothe");

		ApplyMorphs(act);


		SetMorph(act, "obody_processed", 1.0f, "OBody");
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
		AddSliderToSet(set, BuildSlider("BreastCleavage", 0.2f, 0.4f));
		// push up smaller breasts more
		AddSliderToSet(set, BuildSlider("BreastGravity2", -0.1f, -0.05f));
		// Make top of breast rise higher
		AddSliderToSet(set, BuildSlider("BreastTopSlope", -0.3f, -0.4f));
		// push breasts together
		AddSliderToSet(set, BuildSlider("BreastsTogether", 0.2f, 0.25f));

		// butt 
		// remove butt impressions
		AddSliderToSet(set, BuildDerivativeSlider(act, "ButtDimples", 0.0f));
		AddSliderToSet(set, BuildDerivativeSlider(act, "ButtUnderFold", 0.0f));

		// Torso
		// remove definition on clavical bone
		AddSliderToSet(set, BuildDerivativeSlider(act, "Clavicle_v2", 0.0f));
		// Make navel sink in more? Change?
		AddSliderToSet(set, BuildDerivativeSlider(act, "NavelEven", 0.0f));


		// hip
		// remove defintion on hip bone
		AddSliderToSet(set, BuildDerivativeSlider(act, "HipCarved", 0.0f));

		// nipple
		// sublte change to tip shape
		AddSliderToSet(set, BuildDerivativeSlider(act, "NippleDip", 0.0f));
		//flatten areola 
		AddSliderToSet(set, BuildDerivativeSlider(act, "NipplePuffy_v2", 0.0f));
		AddSliderToSet(set, BuildSlider("NipBGone", 0.6f));
		// push nipples together
		AddSliderToSet(set, BuildSlider("NippleDistance", 0.05f, 0.08f));
		// Lift large breasts up
		AddSliderToSet(set, BuildSlider("NippleDown", 0.0f, -0.1f));
		// Less pointy nipple
		AddSliderToSet(set, BuildSlider("NipplePerkManga", -0.15f));


		return set;
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
		vector<float> scores; // todo
		auto sliderset = GenerateSlidersetFromNode(node);

		struct BodyslidePreset ret;
		ret.name = name;
		ret.body = body;
		ret.scores = scores;
		ret.sliders = sliderset; 

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
		// todo scores
		PrintSliderSet(preset.sliders);
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
		return !( StringContains(preset.body, "HIMBO") || StringContains(preset.body, "himbo") || StringContains(preset.body, "Himbo") );
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
		return (StringContains(set, "Outfit") || StringContains(set, "outfit") || StringContains(set, "OUTFIT") || StringContains(set, "Clothed") || StringContains(set, "CLOTHED") || StringContains(set, "clothed"));
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