#include "Papyrus.h"

#include "Body/Actors.h"
#include "Body/Morph.h"
#include "Body/MorphProcedural.h"
#include "Body/OBody.h"

namespace Papyrus
{
	int32_t femalesFound = 0;
	int32_t malesFound = 0;


	void GenActor(RE::StaticFunctionTag*, RE::Actor* act)
	{
		auto OBodyinstance = Body::OBody::GetInstance();
		OBodyinstance->GenerateActorBody(act);
	}

	void ApplyPresetByFile(RE::StaticFunctionTag*, RE::Actor* act, RE::BSFixedString path)
	{
		auto OBodyinstance = Body::OBody::GetInstance();
		OBodyinstance->GenBodyByFile(act, (std::string) path );
	}

	void ApplyPresetByName(RE::StaticFunctionTag*, RE::Actor* act, RE::BSFixedString name)
	{
		auto OBodyinstance = Body::OBody::GetInstance();
		OBodyinstance->GenBodyByName(act, (std::string) name );
	}

	void AddClothesOverlay(RE::StaticFunctionTag*, RE::Actor* act)
	{
		auto OBodyinstance = Body::OBody::GetInstance();
		OBodyinstance->ApplyClothePreset(act);
		OBodyinstance->ApplyMorphs(act);
	}

	auto GetAllPossiblePresets(RE::StaticFunctionTag*, RE::Actor* act){
		auto OBodyinstance = Body::OBody::GetInstance();
		return OBodyinstance->GetPresets(act);
	}

	RE::FormID GetRandomFemaleFormID(RE::StaticFunctionTag*)
	{
		auto actors = Body::Actors::GetSingleton();
		return actors->GetRandomFemaleFormID();
	}

	int32_t PreFilterActors(RE::StaticFunctionTag*)
	{
		logger::info("Prefiltering");
		auto actors = Body::Actors::GetSingleton();
		int32_t AmountActorsPrefilterd = actors->PrefilterActors();
		logger::info("Prefiltering done");
		return AmountActorsPrefilterd;
	}

	bool IsPrefiltered(RE::StaticFunctionTag*)
	{
		auto actors = Body::Actors::GetSingleton();
		return actors->IsPrefiltered();
	}

	void SetSkipEssential(RE::StaticFunctionTag*, bool a_skipEssential)
	{
		auto morph = Body::Morph::GetSingleton();
		morph->SetSkipEssential(a_skipEssential);
	}

	bool GetSkipEssential(RE::StaticFunctionTag*)
	{
		auto morph = Body::Morph::GetSingleton();
		return morph->GetSkipEssential();
	}

	void SetSkipUnique(RE::StaticFunctionTag*, bool a_skipUnique)
	{
		auto morph = Body::Morph::GetSingleton();
		morph->SetSkipUnique(a_skipUnique);
	}

	bool GetSkipUnique(RE::StaticFunctionTag*)
	{
		auto morph = Body::Morph::GetSingleton();
		return morph->GetSkipUnique();
	}

	void SetSeedOffset(RE::StaticFunctionTag*, RE::BSFixedString a_offset)
	{
		int32_t seedOffset = std::strtol(a_offset.c_str(), nullptr, 10);
		auto morphProcedural = Body::MorphProcedural::GetSingleton();
		morphProcedural->SetSeedOffset(seedOffset);
	}

	RE::BSFixedString GetSeedOffset(RE::StaticFunctionTag*)
	{
		auto morphProcedural = Body::MorphProcedural::GetSingleton();
		std::string s = std::to_string(morphProcedural->GetSeedOffset());
		return s.c_str();
	}

	int32_t GetFemalesFound(RE::StaticFunctionTag*)
	{
		return femalesFound;
	}

	int32_t GetMalesFound(RE::StaticFunctionTag*)
	{
		return malesFound;
	}

	bool TriFemaleExists(RE::StaticFunctionTag*)
	{
		logger::info("Checking TriFemale");
		auto morph = Body::Morph::GetSingleton();
		return morph->TriFemaleExists();
	}

	bool TriMaleExists(RE::StaticFunctionTag*)
	{
		auto morph = Body::Morph::GetSingleton();
		return morph->TriMaleExists();
	}

	void MorphMale(RE::StaticFunctionTag*)
	{
		auto morph = Body::Morph::GetSingleton();
		malesFound = morph->MorphMale();
	}

	void MorphFemale(RE::StaticFunctionTag*)
	{
		logger::info("Morphing Female");
		auto morph = Body::Morph::GetSingleton();
		femalesFound = morph->MorphFemale();
		logger::info("Morphing Female done");
	}

	void DeleteMorphs(RE::StaticFunctionTag*)
	{
		auto morph = Body::Morph::GetSingleton();
		morph->DeleteMorphs();
	}

	void SetMorphFactorFemale(RE::StaticFunctionTag*, float a_factor)
	{
		auto morphProcedural = Body::MorphProcedural::GetSingleton();
		morphProcedural->SetMorphFactorFemale(a_factor);
	}

	float GetMorphFactorFemale(RE::StaticFunctionTag*)
	{
		auto morphProcedural = Body::MorphProcedural::GetSingleton();
		return morphProcedural->GetMorphFactorFemale();
	}

	void SetMorphFactorMale(RE::StaticFunctionTag*, float a_factor)
	{
		auto morphProcedural = Body::MorphProcedural::GetSingleton();
		morphProcedural->SetMorphFactorMale(a_factor);
	}

	float GetMorphFactorMale(RE::StaticFunctionTag*)
	{
		auto morphProcedural = Body::MorphProcedural::GetSingleton();
		return morphProcedural->GetMorphFactorMale();
	}

	void SetMorphFactorBreasts(RE::StaticFunctionTag*, float a_factor)
	{
		auto morphProcedural = Body::MorphProcedural::GetSingleton();
		morphProcedural->SetMorphFactorBreasts(a_factor);
	}

	float GetMorphFactorBreasts(RE::StaticFunctionTag*)
	{
		auto morphProcedural = Body::MorphProcedural::GetSingleton();
		return morphProcedural->GetMorphFactorBreasts();
	}

	void SetMorphFactorMuscles(RE::StaticFunctionTag*, float a_factor)
	{
		auto morphProcedural = Body::MorphProcedural::GetSingleton();
		morphProcedural->SetMorphFactorMuscles(a_factor);
	}

	float GetMorphFactorMuscles(RE::StaticFunctionTag*)
	{
		auto morphProcedural = Body::MorphProcedural::GetSingleton();
		return morphProcedural->GetMorphFactorMuscles();
	}

	#define BIND(a_method, ...) a_vm->RegisterFunction(#a_method##sv, obj, a_method __VA_OPT__(, ) __VA_ARGS__)

	bool Bind(VM* a_vm)
	{
		if (!a_vm) {
			logger::critical("Virtual Machine not found!");
			return false;
		}

		const auto obj = "OBodyNative"sv;

		BIND(GenActor);
		BIND(ApplyPresetByFile);
		BIND(ApplyPresetByName);
		BIND(GetAllPossiblePresets);
		BIND(AddClothesOverlay);

		BIND(IsPrefiltered);
		BIND(PreFilterActors);
		BIND(MorphMale);
		BIND(MorphFemale);
		BIND(SetSeedOffset);
		BIND(GetSeedOffset);

		BIND(SetMorphFactorFemale);
		BIND(GetMorphFactorFemale);
		BIND(SetMorphFactorMale);
		BIND(GetMorphFactorMale);
		BIND(SetMorphFactorBreasts);
		BIND(GetMorphFactorBreasts);
		BIND(SetMorphFactorMuscles);
		BIND(GetMorphFactorMuscles);

		BIND(GetFemalesFound);
		BIND(GetMalesFound);
		BIND(DeleteMorphs);
		BIND(SetSkipEssential);
		BIND(GetSkipEssential);
		BIND(SetSkipUnique);
		BIND(GetSkipUnique);
		BIND(TriFemaleExists);
		BIND(TriMaleExists);

		BIND(GetRandomFemaleFormID);

		return true;
	}

	#undef BIND
}
