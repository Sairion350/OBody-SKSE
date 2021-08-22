#pragma once

#include "Body/Body.h"

namespace PapyrusBody
{
	using VM = RE::BSScript::IVirtualMachine;

	void GenActor(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		auto obody = Body::OBody::GetInstance();
		obody->GenerateActorBody(a_actor);
	}

	void SetORefit(RE::StaticFunctionTag*, bool a_enabled)
	{
		auto obody = Body::OBody::GetInstance();
		obody->setRefit = a_enabled;
	}

	void SetNippleRand(RE::StaticFunctionTag*, bool a_enabled)
	{
		auto obody = Body::OBody::GetInstance();
		obody->setNippleRand = a_enabled;
	}

	void SetGenitalRand(RE::StaticFunctionTag*, bool a_enabled)
	{
		auto obody = Body::OBody::GetInstance();
		obody->setGenitalRand = a_enabled;
	}

	int GetFemaleDatabaseSize(RE::StaticFunctionTag*)
	{
		auto obody = Body::OBody::GetInstance();
		return static_cast<int>(obody->femalePresets.size());
	}

	int GetMaleDatabaseSize(RE::StaticFunctionTag*)
	{
		auto obody = Body::OBody::GetInstance();
		return static_cast<int>(obody->malePresets.size());
	}

	void RegisterForOBodyEvent(RE::StaticFunctionTag*, RE::TESQuest* a_quest)
	{
		Body::OnActorGenerated.Register(a_quest);
	}

	void RegisterForOBodyNakedEvent(RE::StaticFunctionTag*, RE::TESQuest* a_quest)
	{
		Body::OnActorNaked.Register(a_quest);
	}

	void ApplyPresetByFile(RE::StaticFunctionTag*, RE::Actor* a_actor, std::string a_path)
	{
		auto obody = Body::OBody::GetInstance();
		obody->GenerateBodyByFile(a_actor, a_path);
	}

	void ApplyPresetByName(RE::StaticFunctionTag*, RE::Actor* a_actor, std::string a_name)
	{
		auto obody = Body::OBody::GetInstance();
		obody->GenerateBodyByName(a_actor, a_name);
	}

	void AddClothesOverlay(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		auto obody = Body::OBody::GetInstance();
		obody->ApplyClothePreset(a_actor);
		obody->ApplyMorphs(a_actor);
	}

	auto GetAllPossiblePresets(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		std::vector<RE::BSFixedString> ret;
		auto obody = Body::OBody::GetInstance();
		if (obody->IsFemale(a_actor))
			for (auto& preset : obody->femalePresets)
				ret.push_back(preset.name);
		else
			for (auto& preset : obody->malePresets)
				ret.push_back(preset.name);

		return ret;
	}

	bool Bind(VM* a_vm)
	{
		const auto obj = "OBodyNative"sv;

		BIND(GenActor);
		BIND(ApplyPresetByFile);
		BIND(ApplyPresetByName);
		BIND(GetAllPossiblePresets);
		BIND(AddClothesOverlay);
		BIND(RegisterForOBodyEvent);
		BIND(RegisterForOBodyNakedEvent);
		BIND(GetFemaleDatabaseSize);
		BIND(GetMaleDatabaseSize);

		BIND(SetORefit);
		BIND(SetNippleRand);
		BIND(SetGenitalRand);

		return true;
	}
}
