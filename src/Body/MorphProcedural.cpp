#include "Body/MorphProcedural.h"

namespace Body
{
	MorphProcedural* MorphProcedural::GetSingleton()
	{
		static MorphProcedural singleton;
		return &singleton;
	}

	void MorphProcedural::SetSeedOffset(int32_t a_seedOffset)
	{
		this->seedOffset = a_seedOffset;
	}

	int32_t MorphProcedural::GetSeedOffset()
	{
		return seedOffset;
	}

	void MorphProcedural::SetSeed(int32_t a_seed)
	{
		srand(a_seed + seedOffset);
	}

	float MorphProcedural::GetRandomFloat(float a_min, float a_max)
	{
		return (a_min + static_cast<float>(rand()) /
							(static_cast<float>(RAND_MAX / (a_max - a_min))));
	}

	float MorphProcedural::GetRandomFloatNormalByWeight(float a_min, float a_max)
	{
		int samples = static_cast<int>(std::ceilf(GetWeight() / 10.f));
		std::random_device rd;
		std::mt19937 gen(rd());
		// std::normal_distribution<float> d(min, max);
		std::uniform_real_distribution<float> d(a_min, a_max);
		float sample = 0;
		for (int i = 0; i < samples; ++i) {
			sample = sample + d(gen);
		}
		float ret = (sample * (morphfactor / 100)) - fitness;
		if (ret < 0)
			ret = 0;
		return (ret);
	}

	float MorphProcedural::GetRandomFloatNormalByMusclePower(float a_min, float a_max)
	{
		int samples = static_cast<int>(std::ceilf(GetMusclepower() * 10.f));
		std::random_device rd;
		std::mt19937 gen(rd());
		// std::normal_distribution<float> d(min, max);
		std::uniform_real_distribution<float> d(a_min, a_max);
		float sample = 0;
		for (int i = 0; i < samples; ++i) {
			sample = sample + d(gen);
		}
		return (sample * ((morphfactor / 100.f) * (morphfactorMuscles / 100.f)));
	}

	void MorphProcedural::SetGravityOffset(float a_offset)
	{
		this->gravityOffset = a_offset;
	}

	float MorphProcedural::GetGravityOffset()
	{
		return gravityOffset;
	}

	float MorphProcedural::GetWeight()
	{
		return weight;
	}

	float MorphProcedural::GetMusclepower()
	{
		return musclepower;
	}

	void MorphProcedural::SetMusclepower(float a_musclePower)
	{
		this->musclepower = a_musclePower;
	}

	float MorphProcedural::GetFitness()
	{
		return this->fitness;
	}

	void MorphProcedural::SetFitness(float a_fitness)
	{
		this->fitness = a_fitness;
	}

	void MorphProcedural::SetWeight(float a_weight)
	{
		this->weight = a_weight;
	}

	float MorphProcedural::GetMorphFactorMale()
	{
		if (morphfactorMale < 50)
			morphfactorMale = 50;
		return morphfactorMale;
	}

	float MorphProcedural::GetMorphFactorFemale()
	{
		if (morphfactorFemale < 50)
			morphfactorFemale = 50;
		return morphfactorFemale;
	}

	void MorphProcedural::SetMorphFactor(float a_morphFactor)
	{
		this->morphfactor = a_morphFactor;
	}

	void MorphProcedural::SetMorphFactorFemale(float a_morphFactor)
	{
		this->morphfactorFemale = a_morphFactor;
	}

	void MorphProcedural::SetMorphFactorMale(float a_morphFactor)
	{
		this->morphfactorMale = a_morphFactor;
	}

	float MorphProcedural::GetMorphFactorBreasts()
	{
		return morphfactorBreasts;
	}

	void MorphProcedural::SetMorphFactorBreasts(float a_morphFactor)
	{
		this->morphfactorBreasts = a_morphFactor;
	}

	float MorphProcedural::GetMorphFactorMuscles()
	{
		return morphfactorMuscles;
	}

	void MorphProcedural::SetMorphFactorMuscles(float a_morphFactor)
	{
		this->morphfactorBreasts = a_morphFactor;
	}

	void MorphProcedural::SetFormID(int32_t a_formID)
	{
		this->formID = a_formID;
	}

	float MorphProcedural::GetMorphValueSamSamuel()
	{
		return GetRandomFloatNormalByWeight(0.f, 0.1f);
	}

	float MorphProcedural::GetMorphValueSamSamson()
	{
		return GetRandomFloatNormalByMusclePower(0.0f, 0.1f);
	}

	float MorphProcedural::GetMorphValueForearmSize()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.1f);
	}

	float MorphProcedural::GetMorphValueChubbyArms()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.1f);
	}
	float MorphProcedural::GetMorphValueShoulderWidth()
	{
		return GetRandomFloatNormalByWeight(-0.1f, 0.0f);
	}
	float MorphProcedural::GetMorphValueArms()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueShoulderSmooth()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueShoulderTweak()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueBelly()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueTummyTuck()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueBigBelly()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.1f);
	}

	float MorphProcedural::GetMorphValueHipBone()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.055f);
	}

	float MorphProcedural::GetMorphValueHips()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.055f);
	}

	float MorphProcedural::GetMorphValueHipForward()
	{
		return GetRandomFloat(-0.7f, 0.7f);
	}

	float MorphProcedural::GetMorphValueHipUpperWidth()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueNippleLength()
	{
		return GetRandomFloat(0.0f, 0.3f);
	}

	float MorphProcedural::GetMorphValueNipplePerkiness()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueNipplePerkManga()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueNippleTipManga()
	{
		return GetRandomFloat(-0.5f, 0.5f);
	}

	float MorphProcedural::GetMorphValueNippleDistance()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueNippleSize()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueAreolaSize()
	{
		return GetRandomFloat(-1.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueNippleDown()
	{
		return GetRandomFloat(0.0f, 0.7f);
	}

	float MorphProcedural::GetMorphValueMuscleAbs()
	{
		return GetRandomFloat(0.0f, musclePower);
	}

	float MorphProcedural::GetMorphValueMuscleArms()
	{
		return GetRandomFloat(0.0f, musclePower);
	}

	float MorphProcedural::GetMorphValueMuscleButt()
	{
		return GetRandomFloat(0.0f, musclePower);
	}

	float MorphProcedural::GetMorphValueMuscleLegs()
	{
		return GetRandomFloat(0.0f, musclePower);
	}

	float MorphProcedural::GetMorphValueMusclePecs()
	{
		return GetRandomFloat(0.0f, musclePower);
	}

	float MorphProcedural::GetMorphValueBigButt()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.075f);
	}

	float MorphProcedural::GetMorphValueChubbyButt()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.075f);
	}

	float MorphProcedural::GetMorphValueButt()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.075f);
	}

	float MorphProcedural::GetMorphValueButtClassic()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueButtShape2()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueButtCrack()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueAppleCheeks()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueRoundAss()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueCrotchBack()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueGroin()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueChubbyLegs()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.1f);
	}

	float MorphProcedural::GetMorphValueLegs()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.1f);
	}

	float MorphProcedural::GetMorphValueThighs()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.1f);
	}

	float MorphProcedural::GetMorphValueKneeShape()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueKneeHeight()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueCalfSize()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueCalfSmooth()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueLegShapeClassic()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueBigTorso()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.07f);
	}

	float MorphProcedural::GetMorphValueChestDepth()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.05f);
	}

	float MorphProcedural::GetMorphValueChestWidth()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.07f);
	}

	float MorphProcedural::GetMorphValueChubbyWaist()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.07f);
	}

	float MorphProcedural::GetMorphValueWaist()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueSternumDepth()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueSternumHeight()
	{
		return GetRandomFloat(0.0f, 0.4f);
	}

	float MorphProcedural::GetMorphValueWaistHeight()
	{
		return GetRandomFloat(0.0f, 0.5f);
	}

	float MorphProcedural::GetMorphValueWaistLine()
	{
		return GetRandomFloat(-0.5f, 0.5f);
	}

	float MorphProcedural::GetMorphValueBreasts()
	{
		return (GetRandomFloatNormalByWeight(0.0f, 0.04f) + GetRandomFloat(0.0f, 0.75f)) * (morphfactorBreasts / 100.f);
	}

	float MorphProcedural::GetMorphValueBreastsNewSH()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.08f);
	}

	float MorphProcedural::GetMorphValueBreastCleavage()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.10f);
	}

	float MorphProcedural::GetMorphValueBreastsFantasy()
	{
		return GetRandomFloat(0.0f, 0.4f);
	}

	float MorphProcedural::GetMorphValueBreastCenter()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}
	float MorphProcedural::GetMorphValueBreastCenterBig()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueBreastFlatness2()
	{
		if (weight < 25.0) {
			return GetRandomFloat(0.0f, 0.2f);
		} else {
			return 0;
		}
	}

	float MorphProcedural::GetMorphValueBreastFlatness()
	{
		if (weight < 25.0) {
			return GetRandomFloat(0.0f, 0.2f);
		} else {
			return 0;
		}
	}

	float MorphProcedural::GetMorphValueBreastGravity2()
	{
		return (GetRandomFloatNormalByWeight(0.0f, 0.07f) + gravityOffset);
	}

	float MorphProcedural::GetMorphValueBreastHeight()
	{
		return GetRandomFloatNormalByWeight(0.0f, 0.075f);
	}

	float MorphProcedural::GetMorphValueBreastsTogether()
	{
		return GetRandomFloatNormalByWeight(0.03f, 0.1f);
	}

	float MorphProcedural::GetMorphValueBreastTopSlope()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValuePushUp()
	{
		return 0.f;
	}

	float MorphProcedural::GetMorphValueBreastPerkiness()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}

	float MorphProcedural::GetMorphValueBreastWidth()
	{
		return GetRandomFloat(0.0f, 1.0f);
	}
}
