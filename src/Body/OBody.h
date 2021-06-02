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
		vector<string> GetFilesInBodyslideDir();
		void GenerateDatabases();
		bool StringContains(string& str, const char* testcase);

		void GenerateActorBody(RE::Actor* act);
		void SetMorph(RE::Actor* act, string MorphName, float value);
		void ApplyMorphs(RE::Actor* act);
		bool SetMorphInterface(SKEE::IBodyMorphInterface* a_morphInt);
		void SetMorphByWeight(RE::Actor* act, string MorphName, pair<float, float> values, float weight);
		float GetWeight(RE::Actor* act);
		void ApplySlider(RE::Actor* act, pair<string, pair<float, float>> slider, float weight);
		void ApplySliderSet(RE::Actor* act, vector<pair<string, pair<float, float>>> sliderSet);
		void ApplyBodyslidePreset(RE::Actor* act, tuple<string, string, vector<float>, vector<pair<string, pair<float, float>>>> BodyslidePreset);

		pugi::xml_document GetDocFromFile(string pathToFile);
		vector<pair<string, pair<float, float>>> GenerateSlidersetFromDoc(pugi::xml_document& doc);

		void AddSliderToSet(vector<pair<string, pair<float, float>>>& sliderset, pair<string, pair<float, float>> slider);
		void PrintSliderSet(vector<pair<string, pair<float, float>>>& sliderset);
		pair<string, pair<float, float>> BuildSlider(string name, float min, float max);
		pair<string, pair<float, float>> BuildSlider(string name, float value);
		tuple<string, string, vector<float>, vector<pair<string, pair<float, float>>>> GeneratePresetFromDoc(pugi::xml_document& doc);
		void PrintPreset(tuple<string, string, vector<float>, vector<pair<string, pair<float, float>>>>& sliderset);
		void ProcessActor(RE::Actor* act);
		float GetMorph(RE::Actor* act, string MorphName);
		void GenerateFullBodyFromPreset(RE::Actor* act, tuple<string, string, vector<float>, vector<pair<string, pair<float, float>>>> BodyslidePreset);


	private:
		OBody() = default;
		OBody(const OBody&) = delete;
		OBody(OBody&&) = delete;
		~OBody() = default;

		SKEE::IBodyMorphInterface* morphInt;

	};
}
