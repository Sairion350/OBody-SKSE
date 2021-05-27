#pragma once

namespace Papyrus
{
	using VM = RE::BSScript::IVirtualMachine;

	void GenActor(RE::StaticFunctionTag*, RE::Actor* act);
	
	void MorphFemale(RE::StaticFunctionTag*);
	void MorphMale(RE::StaticFunctionTag*);

	void SetSeedOffset(RE::StaticFunctionTag*, RE::BSFixedString a_offset);
	RE::BSFixedString GetSeedOffset(RE::StaticFunctionTag*);

	float GetMorphFactorMale(RE::StaticFunctionTag*);
	float GetMorphFactorFemale(RE::StaticFunctionTag*);

	void SetMorphFactorMale(RE::StaticFunctionTag*, float a_factor);
	void SetMorphFactorFemale(RE::StaticFunctionTag*, float a_factor);

	float GetMorphFactorBreasts(RE::StaticFunctionTag*);
	void SetMorphFactorBreasts(RE::StaticFunctionTag*, float a_factor);

	float GetMorphFactorMuscles(RE::StaticFunctionTag*);
	void SetMorphFactorMuscles(RE::StaticFunctionTag*, float a_factor);

	int32_t GetFemalesFound(RE::StaticFunctionTag*);
	int32_t GetMalesFound(RE::StaticFunctionTag*);
	void DeleteMorphs(RE::StaticFunctionTag*);
	void SetSkipEssential(RE::StaticFunctionTag*, bool a_value);
	bool GetSkipEssential(RE::StaticFunctionTag*);
	void SetSkipUnique(RE::StaticFunctionTag*, bool a_value);
	bool GetSkipUnique(RE::StaticFunctionTag*);

	bool TriFemaleExists(RE::StaticFunctionTag*);
	bool TriMaleExists(RE::StaticFunctionTag*);
	int32_t PreFilterActors(RE::StaticFunctionTag*);
	bool IsPrefiltered(RE::StaticFunctionTag*);
	void ChangeHair(RE::StaticFunctionTag*);

	RE::FormID GetRandomFemaleFormID(RE::StaticFunctionTag*);

	bool Bind(VM* a_vm);
}
