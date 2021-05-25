#include "Body/Morph.h"
#include "Body/MorphProcedural.h"
#include "Body/Actors.h"

namespace Body
{
	Morph* Morph::GetSingleton()
	{
		static Morph singleton;
		return &singleton;
	}

	bool Morph::SetMorphInterface(SKEE::IBodyMorphInterface* a_morphInt)
	{
		return a_morphInt->GetVersion() ? this->morphInt = a_morphInt : false;
	}

	void Morph::SetSkipEssential(bool a_val)
	{
		this->skipEssential = a_val;
	}

	bool Morph::GetSkipEssential()
	{
		return skipEssential;
	}

	void Morph::SetSkipUnique(bool a_val)
	{
		this->skipUnique = a_val;
	}

	bool Morph::GetSkipUnique()
	{
		return skipUnique;
	}

	float Morph::CalculateMusclepower(RE::Actor* a_actor)
	{
		float oneHanded = a_actor->GetActorValue(RE::ActorValue::kOneHanded);
		if (oneHanded < 15)
			oneHanded = 15;
		else if (oneHanded > 30)
			oneHanded = 30;

		float smithing = a_actor->GetActorValue(RE::ActorValue::kSmithing);
		if (smithing < 15)
			smithing = 15;
		else if (smithing > 30)
			smithing = 30;

		float block = a_actor->GetActorValue(RE::ActorValue::kBlock);
		if (block < 15)
			block = 15;
		else if (block > 30)
			block = 30;

		float unarmedDamage = a_actor->GetActorValue(RE::ActorValue::kUnarmedDamage);
		if (unarmedDamage < 15)
			unarmedDamage = 15;
		else if (unarmedDamage > 30)
			unarmedDamage = 30;

		return (((oneHanded + smithing + block + unarmedDamage) - 60.0f) /
				(120.0f - 60.0f) * 1.8f);
	}

	float Morph::CalculateFitness(RE::Actor* a_actor)
	{
		float sneak = a_actor->GetActorValue(RE::ActorValue::kSneak);
		if (sneak < 15)
			sneak = 15;
		else if (sneak > 30)
			sneak = 30;

		float pickpocket = a_actor->GetActorValue(RE::ActorValue::kPickpocket);
		if (pickpocket < 15)
			pickpocket = 15;
		else if (pickpocket > 30)
			pickpocket = 30;

		float lockpicking = a_actor->GetActorValue(RE::ActorValue::kLockpicking);
		if (lockpicking < 15)
			lockpicking = 15;
		else if (lockpicking > 30)
			lockpicking = 30;

		float lightArmor = a_actor->GetActorValue(RE::ActorValue::kLightArmor);
		if (lightArmor < 15)
			lightArmor = 15;
		else if (lightArmor > 30)
			lightArmor = 30;

		return ((sneak + pickpocket + lockpicking + lightArmor) - 60.0f) /
			   (120.0f - 60.0f);
	}

	bool Morph::TriFemaleExists()
	{
		bool isTri = fs::exists(
			fs::current_path() /
			fs::path("Data\\meshes\\actors\\character\\character assets\\femalebody.tri").string().c_str());
		if (isTri)
			logger::info("femalebody.tri found");
		else
			logger::info("femalebody.tri NOT found");

		return isTri;
	}

	bool Morph::TriMaleExists()
	{
		bool isTri = fs::exists(
			fs::current_path() /
			fs::path("Data\\meshes\\actors\\character\\character assets\\malebody.tri").string().c_str());
		if (isTri)
			logger::info("malebody.tri found");
		else
			logger::info("malebody.tri NOT found");

		return isTri;
	}

	int32_t Morph::MorphMale()
	{
		auto actors = Actors::GetSingleton();
		return Morph::MorphActors(actors->GetMales(), false);
	}

	int32_t Morph::MorphFemale()
	{
		auto actors = Actors::GetSingleton();
		return Morph::MorphActors(actors->GetFemales(), true);
	}

	int32_t Morph::MorphActors(std::vector<RE::Actor*> a_actors, bool a_female)
	{
		int32_t actorsFound = 0;
		for (auto& actor : a_actors) {
			if (this->GetSkipUnique()) {
				if (actor->GetActorBase()->IsUnique()) {
					continue;
				}
			}

			if (this->GetSkipEssential()) {
				if (actor->GetActorBase()->IsEssential()) {
					continue;
				}
			}

			if (a_female) {
				MorphActorCBBE(actor);
				actorsFound++;
			} else {
				MorphActorSMMB(actor);
				actorsFound++;
			}
		}
		return actorsFound;
	}

	void Morph::DeleteMorphs()
	{
		auto allForms = RE::TESForm::GetAllForms();
		auto& formIDs = *allForms.first;
		for (auto elem : formIDs) {
			auto myActor = RE::TESForm::LookupByID<RE::Actor>(elem.first);
			if (myActor && !myActor->IsChild()) {
				DeleteMorph(myActor);
			}
		}
	}

	void Morph::DeleteMorph(RE::Actor* a_actor)
	{
		auto refr = a_actor->AsReference();
		morphInt->ClearBodyMorphKeys(refr, "OBody");
		morphInt->ApplyBodyMorphs(refr, false);
		morphInt->UpdateModelWeight(refr, true);
	}

	void Morph::MorphActorSMMB(RE::Actor* a_actor)
	{
		auto morphProcedural = MorphProcedural::GetSingleton();
		auto refr = a_actor->AsReference();
		//_MESSAGENNL("Got ObjectRefence,");
		morphProcedural->SetSeed(a_actor->GetFormID());
		//_MESSAGENNL("Seed set,");
		morphProcedural->SetFitness(CalculateFitness(a_actor));
		//_MESSAGENNL("Fitness set,");
		morphProcedural->SetMusclepower(CalculateMusclepower(a_actor));
		//_MESSAGENNL("Musclepower set,");
		morphProcedural->SetMorphFactor(morphProcedural->GetMorphFactorMale());
		//_MESSAGENNL("Morphfactor set,");
		morphProcedural->SetWeight(a_actor->GetActorBase()->GetWeight());
		//_MESSAGENNL("Weight set,");
		morphInt->SetMorph(refr, "SAM Samuel", "OBody", morphProcedural->GetMorphValueSamSamuel());
		//_MESSAGENNL("Morph Samuel,");
		morphInt->SetMorph(refr, "SAM Samson", "OBody", morphProcedural->GetMorphValueSamSamson());
		//_MESSAGENNL("Morph Samson,");
		morphInt->ApplyBodyMorphs(refr, true);
		//_MESSAGENNL("Morphs applied,");
		// bodyMorphInterface->UpdateModelWeight((TESObjectREFR*)refr, true);
		//_MESSAGENNL("Modelweight updated,");
	}

	void Morph::MorphActorCBBE(RE::Actor* a_actor)
	{
		auto morphProcedural = MorphProcedural::GetSingleton();
		auto refr = a_actor->AsReference();
		//_MESSAGE("Got ObjectRefence,");
		morphProcedural->SetSeed(a_actor->GetFormID());
		//_MESSAGENNL("Seed set,");
		morphProcedural->SetMusclepower(CalculateMusclepower(a_actor));
		//_MESSAGENNL("Musclepower set,");
		morphProcedural->SetMorphFactor(morphProcedural->GetMorphFactorFemale());
		//_MESSAGENNL("Morphfactor set,");
		auto race = a_actor->GetRace();
		if (race) {
			//_MESSAGENNL("Got Race,");
			auto name = race->GetName();
			if (name) {
				//_MESSAGENNL("Got Racename,");
				std::string raceName = name;
				size_t found = raceName.find("Old");

				if (found != std::string::npos) {
					//_MESSAGENNL("Old found,");
					morphProcedural->SetGravityOffset(
						morphProcedural->GetRandomFloat(0.5, 1));
					//_MESSAGENNL("Gravity set,");
				} else {
					//_MESSAGENNL("Old not found,");
					morphProcedural->SetGravityOffset(0);
					//_MESSAGENNL("Gravity set to 0,");
				}
			}
		}

		auto actorBase = a_actor->GetActorBase();
		//_MESSAGENNL("Got weight,");
		if (actorBase) {
			//_MESSAGENNL("Got Actorbase,");
			auto baseWeight = actorBase->GetWeight();
			if (baseWeight == 0) {
				//_MESSAGENNL("Weight was 0,");
				morphProcedural->SetWeight(morphProcedural->GetRandomFloat(0, 20));
				//_MESSAGENNL("Changed Weight,");
			} else {
				morphProcedural->SetWeight(baseWeight);
				//_MESSAGENNL("Set Weight,");
			}
		} else {
			morphProcedural->SetWeight(morphProcedural->GetRandomFloat(0, 100));
			//_MESSAGENNL("Set Weight Random,");
		}
		//_MESSAGENNL("Begin Morphing Values,");
		morphInt->SetMorph(refr, "ForearmSize", "OBody", morphProcedural->GetMorphValueForearmSize());
		morphInt->SetMorph(refr, "ChubbyArms", "OBody", morphProcedural->GetMorphValueChubbyArms());
		morphInt->SetMorph(refr, "ShoulderWidth", "OBody", morphProcedural->GetMorphValueShoulderWidth());
		morphInt->SetMorph(refr, "Arms", "OBody", morphProcedural->GetMorphValueArms());
		morphInt->SetMorph(refr, "ShoulderSmooth", "OBody", morphProcedural->GetMorphValueShoulderSmooth());
		morphInt->SetMorph(refr, "ShoulderTweak", "OBody", morphProcedural->GetMorphValueShoulderTweak());

		morphInt->SetMorph(refr, "Belly", "OBody", morphProcedural->GetMorphValueBelly());
		morphInt->SetMorph(refr, "TummyTuck", "OBody", morphProcedural->GetMorphValueTummyTuck());
		morphInt->SetMorph(refr, "BigBelly", "OBody", morphProcedural->GetMorphValueBigBelly());

		morphInt->SetMorph(refr, "HipBone", "OBody", morphProcedural->GetMorphValueHipBone());
		morphInt->SetMorph(refr, "Hips", "OBody", morphProcedural->GetMorphValueHips());
		morphInt->SetMorph(refr, "HipForward", "OBody", morphProcedural->GetMorphValueHipForward());
		morphInt->SetMorph(refr, "HipUpperWidth", "OBody", morphProcedural->GetMorphValueHipUpperWidth());

		morphInt->SetMorph(refr, "NippleLength", "OBody", morphProcedural->GetMorphValueNippleLength());
		morphInt->SetMorph(refr, "NipplePerkiness", "OBody", morphProcedural->GetMorphValueNipplePerkiness());
		morphInt->SetMorph(refr, "NipplePerkManga", "OBody", morphProcedural->GetMorphValueNipplePerkManga());
		morphInt->SetMorph(refr, "NippleTipManga", "OBody", morphProcedural->GetMorphValueNippleTipManga());
		morphInt->SetMorph(refr, "NippleDistance", "OBody", morphProcedural->GetMorphValueNippleDistance());
		morphInt->SetMorph(refr, "NippleSize", "OBody", morphProcedural->GetMorphValueNippleSize());
		morphInt->SetMorph(refr, "AreolaSize", "OBody", morphProcedural->GetMorphValueAreolaSize());
		morphInt->SetMorph(refr, "NippleDown", "OBody", morphProcedural->GetMorphValueNippleDown());

		morphInt->SetMorph(refr, "MuscleAbs", "OBody", morphProcedural->GetMorphValueMuscleAbs());
		morphInt->SetMorph(refr, "MuscleArms", "OBody", morphProcedural->GetMorphValueMuscleArms());
		morphInt->SetMorph(refr, "MuscleButt", "OBody", morphProcedural->GetMorphValueMuscleButt());
		morphInt->SetMorph(refr, "MuscleLegs", "OBody", morphProcedural->GetMorphValueMuscleLegs());
		morphInt->SetMorph(refr, "MusclePecs", "OBody", morphProcedural->GetMorphValueMusclePecs());

		morphInt->SetMorph(refr, "BigButt", "OBody", morphProcedural->GetMorphValueBigButt());
		morphInt->SetMorph(refr, "ChubbyButt", "OBody", morphProcedural->GetMorphValueChubbyButt());
		morphInt->SetMorph(refr, "Butt", "OBody", morphProcedural->GetMorphValueButt());

		morphInt->SetMorph(refr, "ButtClassic", "OBody", morphProcedural->GetMorphValueButtClassic());
		morphInt->SetMorph(refr, "ButtShape2", "OBody", morphProcedural->GetMorphValueButtShape2());
		morphInt->SetMorph(refr, "ButtCrack", "OBody", morphProcedural->GetMorphValueButtCrack());
		morphInt->SetMorph(refr, "AppleCheeks", "OBody", morphProcedural->GetMorphValueAppleCheeks());
		morphInt->SetMorph(refr, "RoundAss", "OBody", morphProcedural->GetMorphValueRoundAss());
		morphInt->SetMorph(refr, "CrotchBack", "OBody", morphProcedural->GetMorphValueCrotchBack());
		morphInt->SetMorph(refr, "Groin", "OBody", morphProcedural->GetMorphValueGroin());

		morphInt->SetMorph(refr, "ChubbyLegs", "OBody", morphProcedural->GetMorphValueChubbyLegs());
		morphInt->SetMorph(refr, "Legs", "OBody", morphProcedural->GetMorphValueLegs());
		morphInt->SetMorph(refr, "Thighs", "OBody", morphProcedural->GetMorphValueThighs());

		morphInt->SetMorph(refr, "KneeShape", "OBody", morphProcedural->GetMorphValueKneeShape());
		morphInt->SetMorph(refr, "KneeHeight", "OBody", morphProcedural->GetMorphValueKneeHeight());
		morphInt->SetMorph(refr, "CalfSize", "OBody", morphProcedural->GetMorphValueCalfSize());
		morphInt->SetMorph(refr, "CalfSmooth", "OBody", morphProcedural->GetMorphValueCalfSmooth());
		morphInt->SetMorph(refr, "LegShapeClassic", "OBody", morphProcedural->GetMorphValueLegShapeClassic());

		morphInt->SetMorph(refr, "BigTorso", "OBody", morphProcedural->GetMorphValueBigTorso());
		morphInt->SetMorph(refr, "ChestDepth", "OBody", morphProcedural->GetMorphValueChestDepth());
		morphInt->SetMorph(refr, "ChestWidth", "OBody", morphProcedural->GetMorphValueChestWidth());
		morphInt->SetMorph(refr, "ChubbyWaist", "OBody", morphProcedural->GetMorphValueChubbyWaist());
		morphInt->SetMorph(refr, "Waist", "OBody", morphProcedural->GetMorphValueWaist());
		morphInt->SetMorph(refr, "SternumDepth", "OBody", morphProcedural->GetMorphValueSternumDepth());
		morphInt->SetMorph(refr, "SternumHeight", "OBody", morphProcedural->GetMorphValueSternumHeight());
		morphInt->SetMorph(refr, "WaistHeight", "OBody", morphProcedural->GetMorphValueWaistHeight());
		morphInt->SetMorph(refr, "WaistLine", "OBody", morphProcedural->GetMorphValueWaistLine());

		morphInt->SetMorph(refr, "Breasts", "OBody", morphProcedural->GetMorphValueBreasts());
		morphInt->SetMorph(refr, "BreastsNewSH", "OBody", morphProcedural->GetMorphValueBreastsNewSH());
		morphInt->SetMorph(refr, "BreastCleavage", "OBody", morphProcedural->GetMorphValueBreastCleavage());
		morphInt->SetMorph(refr, "BreastsFantasy", "OBody", morphProcedural->GetMorphValueBreastsFantasy());
		morphInt->SetMorph(refr, "BreastCenter", "OBody", morphProcedural->GetMorphValueBreastCenter());
		morphInt->SetMorph(refr, "BreastCenterBig", "OBody", morphProcedural->GetMorphValueBreastCenterBig());
		morphInt->SetMorph(refr, "BreastFlatness2", "OBody", morphProcedural->GetMorphValueBreastFlatness2());
		morphInt->SetMorph(refr, "BreastFlatness", "OBody", morphProcedural->GetMorphValueBreastFlatness());
		morphInt->SetMorph(refr, "BreastGravity2", "OBody", morphProcedural->GetMorphValueBreastGravity2());
		morphInt->SetMorph(refr, "BreastHeight", "OBody", morphProcedural->GetMorphValueBreastHeight());
		morphInt->SetMorph(refr, "BreastsTogether", "OBody", morphProcedural->GetMorphValueBreastsTogether());
		morphInt->SetMorph(refr, "BreastTopSlope", "OBody", morphProcedural->GetMorphValueBreastTopSlope());
		// bodyMorphInterface->SetMorph(refr, "PushUp", "",
		// morphProcedural->GetMorphValuePushUp());
		morphInt->SetMorph(refr, "BreastPerkiness", "OBody", morphProcedural->GetMorphValueBreastPerkiness());
		morphInt->SetMorph(refr, "BreastWidth", "OBody", morphProcedural->GetMorphValueBreastWidth());
		//_MESSAGENNL("End Morphing Values,");
		morphInt->ApplyBodyMorphs(refr, true);
		//_MESSAGENNL("Bodymorphs applied,");
		// bodyMorphInterface->UpdateModelWeight((TESObjectREFR*)refr, true);
		//_MESSAGENNL("Modelweight updated,");
		//_MESSAGE("Actors %s %f, %f ,%f", myActor->GetName(),
		// morphProcedural->GetMorphValueBreasts(),
		// morphProcedural->GetMorphValueBigTorso(),
		// morphProcedural->GetMorphValueForearmSize());
	}

	void Morph::MorphActorBHUNP(RE::Actor* a_actor)
	{
		auto morphProcedural = MorphProcedural::GetSingleton();
		auto refr = a_actor->AsReference();
		//_MESSAGENNL("Got ObjectRefence,");
		morphProcedural->SetSeed(a_actor->GetFormID());
		//_MESSAGENNL("Seed set,");
		morphProcedural->SetMusclepower(CalculateMusclepower(a_actor));
		//_MESSAGENNL("Musclepower set,");;
		morphProcedural->SetMorphFactor(morphProcedural->GetMorphFactorFemale());
		//_MESSAGENNL("Morphfactor set,");
		auto race = a_actor->GetRace();
		if (race) {
			//_MESSAGENNL("Got Race,");
			auto name = race->GetName();
			if (name) {
				//_MESSAGENNL("Got Racename,");
				std::string raceName = name;
				size_t found = raceName.find("Old");

				if (found != std::string::npos) {
					//_MESSAGENNL("Old found,");
					morphProcedural->SetGravityOffset(
						morphProcedural->GetRandomFloat(0.5, 1));
					//_MESSAGENNL("Gravity set,");
				} else {
					//_MESSAGENNL("Old not found,");
					morphProcedural->SetGravityOffset(0);
					//_MESSAGENNL("Gravity set to 0,");
				}
			}
		}

		auto actorBase = a_actor->GetActorBase();
		//_MESSAGENNL("Got weight,");
		if (actorBase) {
			//_MESSAGENNL("Got Actorbase,");
			auto baseWeight = actorBase->GetWeight();
			if (baseWeight == 0) {
				//_MESSAGENNL("Weight was 0,");
				morphProcedural->SetWeight(morphProcedural->GetRandomFloat(0, 20));
				//_MESSAGENNL("Changed Weight,");
			} else {
				morphProcedural->SetWeight(baseWeight);
				//_MESSAGENNL("Set Weight,");
			}
		} else {
			morphProcedural->SetWeight(morphProcedural->GetRandomFloat(0, 100));
			//_MESSAGENNL("Set Weight Random,");
		}
		morphInt->SetMorph(refr, "ForearmSize", "OBody", morphProcedural->GetMorphValueForearmSize());
		morphInt->SetMorph(refr, "ChubbyArms", "OBody", morphProcedural->GetMorphValueChubbyArms());
		morphInt->SetMorph(refr, "ShoulderWidth", "OBody", morphProcedural->GetMorphValueShoulderWidth());
		morphInt->SetMorph(refr, "Arms", "OBody", morphProcedural->GetMorphValueArms());
		morphInt->SetMorph(refr, "ShoulderSmooth", "OBody", morphProcedural->GetMorphValueShoulderSmooth());
		morphInt->SetMorph(refr, "ShoulderTweak", "OBody", morphProcedural->GetMorphValueShoulderTweak());

		morphInt->SetMorph(refr, "Belly", "OBody", morphProcedural->GetMorphValueBelly());
		morphInt->SetMorph(refr, "TummyTuck", "OBody", morphProcedural->GetMorphValueTummyTuck());
		morphInt->SetMorph(refr, "BigBelly", "OBody", morphProcedural->GetMorphValueBigBelly());

		morphInt->SetMorph(refr, "HipBone", "OBody", morphProcedural->GetMorphValueHipBone());
		morphInt->SetMorph(refr, "Hips", "OBody", morphProcedural->GetMorphValueHips());
		morphInt->SetMorph(refr, "HipForward", "OBody", morphProcedural->GetMorphValueHipForward());
		morphInt->SetMorph(refr, "HipUpperWidth", "OBody", morphProcedural->GetMorphValueHipUpperWidth());

		morphInt->SetMorph(refr, "NippleLength", "OBody", morphProcedural->GetMorphValueNippleLength());
		morphInt->SetMorph(refr, "NipplePerkiness", "OBody", morphProcedural->GetMorphValueNipplePerkiness());
		morphInt->SetMorph(refr, "NipplePerkManga", "OBody", morphProcedural->GetMorphValueNipplePerkManga());
		morphInt->SetMorph(refr, "NippleTipManga", "OBody", morphProcedural->GetMorphValueNippleTipManga());
		morphInt->SetMorph(refr, "NippleDistance", "OBody", morphProcedural->GetMorphValueNippleDistance());
		morphInt->SetMorph(refr, "NippleSize", "OBody", morphProcedural->GetMorphValueNippleSize());
		morphInt->SetMorph(refr, "AreolaSize", "OBody", morphProcedural->GetMorphValueAreolaSize());
		morphInt->SetMorph(refr, "NippleDown", "OBody", morphProcedural->GetMorphValueNippleDown());

		morphInt->SetMorph(refr, "MuscleAbs", "OBody", morphProcedural->GetMorphValueMuscleAbs());
		morphInt->SetMorph(refr, "MuscleArms", "OBody", morphProcedural->GetMorphValueMuscleArms());
		morphInt->SetMorph(refr, "MuscleButt", "OBody", morphProcedural->GetMorphValueMuscleButt());
		morphInt->SetMorph(refr, "MuscleLegs", "OBody", morphProcedural->GetMorphValueMuscleLegs());
		morphInt->SetMorph(refr, "MusclePecs", "OBody", morphProcedural->GetMorphValueMusclePecs());

		morphInt->SetMorph(refr, "BigButt", "OBody", morphProcedural->GetMorphValueBigButt());
		morphInt->SetMorph(refr, "ChubbyButt", "OBody", morphProcedural->GetMorphValueChubbyButt());
		morphInt->SetMorph(refr, "Butt", "OBody", morphProcedural->GetMorphValueButt());

		morphInt->SetMorph(refr, "ButtClassic", "OBody", morphProcedural->GetMorphValueButtClassic());
		morphInt->SetMorph(refr, "ButtShape2", "OBody", morphProcedural->GetMorphValueButtShape2());
		morphInt->SetMorph(refr, "ButtCrack", "OBody", morphProcedural->GetMorphValueButtCrack());
		morphInt->SetMorph(refr, "AppleCheeks", "OBody", morphProcedural->GetMorphValueAppleCheeks());
		morphInt->SetMorph(refr, "RoundAss", "OBody", morphProcedural->GetMorphValueRoundAss());
		morphInt->SetMorph(refr, "CrotchBack", "OBody", morphProcedural->GetMorphValueCrotchBack());
		morphInt->SetMorph(refr, "Groin", "OBody", morphProcedural->GetMorphValueGroin());

		morphInt->SetMorph(refr, "ChubbyLegs", "OBody", morphProcedural->GetMorphValueChubbyLegs());
		morphInt->SetMorph(refr, "Legs", "OBody", morphProcedural->GetMorphValueLegs());
		morphInt->SetMorph(refr, "Thighs", "OBody", morphProcedural->GetMorphValueThighs());

		morphInt->SetMorph(refr, "KneeShape", "OBody", morphProcedural->GetMorphValueKneeShape());
		morphInt->SetMorph(refr, "KneeHeight", "OBody", morphProcedural->GetMorphValueKneeHeight());
		morphInt->SetMorph(refr, "CalfSize", "OBody", morphProcedural->GetMorphValueCalfSize());
		morphInt->SetMorph(refr, "CalfSmooth", "OBody", morphProcedural->GetMorphValueCalfSmooth());
		morphInt->SetMorph(refr, "LegShapeClassic", "OBody", morphProcedural->GetMorphValueLegShapeClassic());

		morphInt->SetMorph(refr, "BigTorso", "OBody", morphProcedural->GetMorphValueBigTorso());
		morphInt->SetMorph(refr, "ChestDepth", "OBody", morphProcedural->GetMorphValueChestDepth());
		morphInt->SetMorph(refr, "ChestWidth", "OBody", morphProcedural->GetMorphValueChestWidth());
		morphInt->SetMorph(refr, "ChubbyWaist", "OBody", morphProcedural->GetMorphValueChubbyWaist());
		morphInt->SetMorph(refr, "Waist", "OBody", morphProcedural->GetMorphValueWaist());
		morphInt->SetMorph(refr, "SternumDepth", "OBody", morphProcedural->GetMorphValueSternumDepth());
		morphInt->SetMorph(refr, "SternumHeight", "OBody", morphProcedural->GetMorphValueSternumHeight());
		morphInt->SetMorph(refr, "WaistHeight", "OBody", morphProcedural->GetMorphValueWaistHeight());
		morphInt->SetMorph(refr, "WaistLine", "OBody", morphProcedural->GetMorphValueWaistLine());

		morphInt->SetMorph(refr, "Breasts", "OBody", morphProcedural->GetMorphValueBreasts() * -1);
		morphInt->SetMorph(refr, "BreastsNewSH", "OBody", morphProcedural->GetMorphValueBreastsNewSH());
		morphInt->SetMorph(refr, "BreastCleavage", "OBody", morphProcedural->GetMorphValueBreastCleavage());
		morphInt->SetMorph(refr, "BreastsFantasy", "OBody", morphProcedural->GetMorphValueBreastsFantasy());
		morphInt->SetMorph(refr, "BreastCenter", "OBody", morphProcedural->GetMorphValueBreastCenter());
		morphInt->SetMorph(refr, "BreastCenterBig", "OBody", morphProcedural->GetMorphValueBreastCenterBig());
		morphInt->SetMorph(refr, "BreastFlatness2", "OBody", morphProcedural->GetMorphValueBreastFlatness2());
		morphInt->SetMorph(refr, "BreastFlatness", "OBody", morphProcedural->GetMorphValueBreastFlatness());
		morphInt->SetMorph(refr, "BreastGravity2", "OBody", morphProcedural->GetMorphValueBreastGravity2());
		morphInt->SetMorph(refr, "BreastHeight", "OBody", morphProcedural->GetMorphValueBreastHeight());
		morphInt->SetMorph(refr, "BreastsTogether", "OBody", morphProcedural->GetMorphValueBreastsTogether());
		morphInt->SetMorph(refr, "BreastTopSlope", "OBody", morphProcedural->GetMorphValueBreastTopSlope());
		// morphInt->SetMorph(refr, "PushUp", "", morphProcedural->GetMorphValuePushUp());
		morphInt->SetMorph(refr, "BreastPerkiness", "OBody", morphProcedural->GetMorphValueBreastPerkiness());
		morphInt->SetMorph(refr, "BreastWidth", "OBody", morphProcedural->GetMorphValueBreastWidth());
		//_MESSAGENNL("End Morphing Values,");
		morphInt->ApplyBodyMorphs(refr, true);
		//_MESSAGENNL("Bodymorphs applied,");
		// morphInt->UpdateModelWeight((TESObjectREFR*)refr, true);
		//_MESSAGENNL("Modelweight updated,");
		//_MESSAGE("Actors %s %f, %f ,%f", myActor->GetName(),
		// morphProcedural->GetMorphValueBreasts(),
		// morphProcedural->GetMorphValueBigTorso(),
		// morphProcedural->GetMorphValueForearmSize());
	}
}
