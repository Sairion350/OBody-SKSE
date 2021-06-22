#pragma once

#include "Body/OBody.h"

namespace PapyrusBody
{
	using VM = RE::BSScript::IVirtualMachine;

	int32_t femalesFound = 0;
	int32_t malesFound = 0;

	void GenActor(RE::StaticFunctionTag*, RE::Actor* act)
	{
		auto obody = Body::OBody::GetInstance();
		obody->GenerateActorBody(act);
	}

	void SetORefit(RE::StaticFunctionTag*, bool enabled)
	{
		auto obody = Body::OBody::GetInstance();
		return obody->SetORefit(enabled);
	}

	void SetNippleRand(RE::StaticFunctionTag*, bool enabled)
	{
		auto obody = Body::OBody::GetInstance();
		return obody->SetNippleRand(enabled);
	}

	void SetGenitalRand(RE::StaticFunctionTag*, bool enabled)
	{
		auto obody = Body::OBody::GetInstance();
		return obody->SetGenitalRand(enabled);
	}

	int GetFemaleDatabaseSize(RE::StaticFunctionTag*)
	{
		auto obody = Body::OBody::GetInstance();
		return obody->GetFemaleDatabaseSize();
	}

	int GetMaleDatabaseSize(RE::StaticFunctionTag*)
	{
		auto obody = Body::OBody::GetInstance();
		return obody->GetMaleDatabaseSize();
	}

	void RegisterForOBodyEvent(RE::StaticFunctionTag*, RE::TESQuest* quest)
	{
		auto obody = Body::OBody::GetInstance();
		obody->RegisterQuestForEvent(quest);
	}

	void ApplyPresetByFile(RE::StaticFunctionTag*, RE::Actor* act, RE::BSFixedString path)
	{
		auto obody = Body::OBody::GetInstance();
		obody->GenBodyByFile(act, (std::string)path);
	}

	void ApplyPresetByName(RE::StaticFunctionTag*, RE::Actor* act, RE::BSFixedString name)
	{
		auto obody = Body::OBody::GetInstance();
		obody->GenBodyByName(act, (std::string)name);
	}

	void AddClothesOverlay(RE::StaticFunctionTag*, RE::Actor* act)
	{
		auto obody = Body::OBody::GetInstance();
		obody->ApplyClothePreset(act);
		obody->ApplyMorphs(act);
	}

	auto GetAllPossiblePresets(RE::StaticFunctionTag*, RE::Actor* act)
	{
		auto obody = Body::OBody::GetInstance();
		return obody->GetPresets(act);
	}

	int32_t GetFemalesFound(RE::StaticFunctionTag*)
	{
		return femalesFound;
	}

	int32_t GetMalesFound(RE::StaticFunctionTag*)
	{
		return malesFound;
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
		BIND(GetFemaleDatabaseSize);
		BIND(GetMaleDatabaseSize);

		BIND(SetORefit);
		BIND(SetNippleRand);
		BIND(SetGenitalRand);

		BIND(GetFemalesFound);
		BIND(GetMalesFound);

		return true;
	}
}
