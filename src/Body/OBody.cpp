#include "Body/OBody.h"

#include <string>
#include <vector>

using namespace std;

/*
	Database structure from largest to smallest:

	Database:
		- todo
	Bodyslide preset:
		- todo
		Type: Tuple < string, string, vector<float>, vector<pair<string, pair<float, float>>> >

		Values: Tuple < Name, Body, Scores, Slider set >
	Slider set:
		Type: vector<pair<string, pair<float, float>>>
		Represents: A list of all of the sliders in a bodyslide preset
		Values: vector<slider>
	Slider:
		Type: pair<string, pair<float, float>> slider
		Represents: A bodyslide slider
		Values: pair<Slider name, Slider values>
	Slider values:
		Type: pair<float, float>
		Represents: The min and max values of a individual bodyslide slider
		Values: pair<Minimum possible value, Maximum possible value>

*/

namespace Body{

	const fs::path root_path("Data\\CalienteTools\\BodySlide\\SliderPresets");
	const char* key = "OBody";

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

		logger::info("Has: {}", GetMorph(act, "obody_processed"));

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
		logger::info("A wild {}  appeared", act->GetName());

		auto docz = GetDocFromFile("Data\\CalienteTools\\BodySlide\\SliderPresets\\Teru Apex V2 3BBB.xml");

		GenerateFullBodyFromPreset(act, GeneratePresetFromDoc(docz));
	
		SetMorph(act, "obody_processed", 1.0f);
		
	}

	void OBody::GenerateFullBodyFromPreset(RE::Actor* act, tuple<string, string, vector<float>, vector<pair<string, pair<float, float>>>> BodyslidePreset){
		ApplyBodyslidePreset(act, BodyslidePreset);

		ApplyMorphs(act);
	}

	void OBody::SetMorph(RE::Actor* act, string MorphName, float value){
		morphInt->SetMorph(act->AsReference(), MorphName.c_str(), key, value);
	}

	float OBody::GetMorph(RE::Actor* act, string MorphName){
		return morphInt->GetMorph(act->AsReference(), MorphName.c_str(), key);
	}

	void OBody::SetMorphByWeight(RE::Actor* act, string MorphName, pair<float, float> values, float weight){
		float value = ((values.second - values.first) * weight) + values.first;
		SetMorph(act, MorphName, value);
	}

	void OBody::ApplySlider(RE::Actor* act, pair<string, pair<float, float>> slider, float weight){
		SetMorphByWeight(act, slider.first, slider.second, weight);
	}

	void OBody::ApplySliderSet(RE::Actor* act, vector<pair<string, pair<float, float>>> sliderSet){
		auto weight = GetWeight(act);
		for (auto i = sliderSet.begin(); i != sliderSet.end(); ++i)
        	ApplySlider(act, *i, weight);
	}

	void OBody::ApplyBodyslidePreset(RE::Actor* act, tuple<string, string, vector<float>, vector<pair<string, pair<float, float>>>> BodyslidePreset ){
		ApplySliderSet(act, get<3>(BodyslidePreset));
		//PrintPreset(BodyslidePreset);
	}

	tuple<string, string, vector<float>, vector<pair<string, pair<float, float>>>> OBody::GeneratePresetFromDoc(pugi::xml_document& doc){
		pugi::xml_node presetNode = doc.child("SliderPresets").child("Preset");

		string name = presetNode.attribute("name").value();
		string body = presetNode.attribute("set").value(); 
		vector<float> scores; // todo
		auto sliderset = GenerateSlidersetFromDoc(doc);


		return make_tuple(name, body, scores, sliderset);
	}

	vector<pair<string, pair<float, float>>> OBody::GenerateSlidersetFromDoc(pugi::xml_document& doc){
		vector<pair<string, pair<float, float>>> ret;

		pugi::xml_node tools = doc.child("SliderPresets").child("Preset");

    	for (pugi::xml_node_iterator it = tools.begin(); it != tools.end(); ++it)
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

	void OBody::AddSliderToSet(vector<pair<string, pair<float, float>>>& sliderset, pair<string, pair<float, float>> slider){
		if ( !sliderset.empty() &&( strcmp(sliderset.back().first.c_str(), slider.first.c_str()) == 0 )){
			// merge
			if ((sliderset.back().second.first == 0.0f) && (slider.second.first != 0.0f)){
				// fill in small
				sliderset.back().second.first = slider.second.first;
			} else if((sliderset.back().second.second == 0.0f) && (slider.second.second != 0.0f)){
				//fill in large
				sliderset.back().second.second = slider.second.second;
			}
		} else{
			sliderset.push_back(slider);
		}
		
		
	}

	pair<string, pair<float, float>> OBody::BuildSlider(string name, float min, float max){
		pair<float, float> set;
		set.first = min;
		set.second = max;

		pair<string, pair<float, float>> slider;
		slider.first = name;
		slider.second = set;

		return slider;
	}

	pair<string, pair<float, float>> OBody::BuildSlider(string name, float value){
		return BuildSlider(name, value, value);
	}

	void OBody::PrintPreset(tuple<string, string, vector<float>, vector<pair<string, pair<float, float>>>>& sliderset){
		logger::info(">Preset name: {}", get<0>(sliderset));
		logger::info(">Preset Body: {}", get<1>(sliderset));
		// todo scores
		PrintSliderSet(get<3>(sliderset));
	}

	void OBody::PrintSliderSet(vector<pair<string, pair<float, float>>>& sliderset){
		for (auto i = sliderset.begin(); i != sliderset.end(); ++i){
        	auto slider = (*i);

        	logger::info(">    Slider name: {}", slider.first);
        	logger::info(">        Small value: {}", slider.second.first);
        	logger::info(">        Large value: {}", slider.second.second);
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
   			if (StringContains(stringpath, "Outfit") || StringContains(stringpath, "outfit") || StringContains(stringpath, "OUTFIT") || !StringContains(stringpath, "xml")){
   				//
   				
   			} else{
   				
        		Files.push_back(stringpath);
        		logger::info(stringpath.c_str());
   			}
        		
        }

        return Files;

	}

	void OBody::GenerateDatabases(){
		GetFilesInBodyslideDir();
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