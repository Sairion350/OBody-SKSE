#pragma once

#include "SKEE.h"

namespace Body
{
	class Morph
	{
		using VM = RE::BSScript::IVirtualMachine;
	public:
		static Morph* GetSingleton();

		void PrintMorphs(RE::Actor* a_actor);
		int32_t MorphFemale();
		int32_t MorphMale();
		int32_t MorphActors(std::vector<RE::Actor*>, bool a_female);
		bool SetMorphInterface(SKEE::IBodyMorphInterface* a_morphInt);

		void MorphActorCBBE(RE::Actor* a_actor);
		void MorphActorBHUNP(RE::Actor* a_actor);
		void MorphActorSMMB(RE::Actor* a_actor);
		void DeleteMorph(RE::Actor* a_actor);

		float CalculateFitness(RE::Actor* a_actor);
		float CalculateMusclepower(RE::Actor* a_actor);

		void DeleteMorphs();

		bool TriFemaleExists();
		bool TriMaleExists();

		void SetSkipEssential(bool a_val);
		bool GetSkipEssential();
		void SetSkipUnique(bool a_val);
		bool GetSkipUnique();

	private:
		Morph() = default;
		Morph(const Morph&) = delete;
		Morph(Morph&&) = delete;
		~Morph() = default;

		bool skipEssential = false;
		bool skipUnique = false;

		SKEE::IBodyMorphInterface* morphInt;
	};
}
