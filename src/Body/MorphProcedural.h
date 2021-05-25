#pragma once

namespace Body
{
	class MorphProcedural
	{
	public:
		static MorphProcedural* GetSingleton();

		void SetFormID(int32_t a_formID);

		float GetWeight();
		void SetWeight(float a_weight);

		float GetMusclepower();
		void SetMusclepower(float a_muscle);

		float GetFitness();
		void SetFitness(float a_fitness);

		void SetGravityOffset(float a_offset);
		float GetGravityOffset();

		void SetSeed(int32_t a_seed);
		void SetSeedOffset(int32_t a_offset);
		int32_t GetSeedOffset();

		void SetMorphFactor(float a_factor);

		float GetMorphFactorFemale();
		float GetMorphFactorMale();

		void SetMorphFactorMale(float a_factor);
		void SetMorphFactorFemale(float a_factor);

		float GetMorphFactorBreasts();
		void SetMorphFactorBreasts(float a_factor);

		float GetMorphFactorMuscles();
		void SetMorphFactorMuscles(float a_factor);

		float GetRandomFloat(float a_min, float a_max);
		float GetRandomFloatNormalByWeight(float a_min, float a_max);
		float GetRandomFloatNormalByMusclePower(float a_min, float a_max);

		float GetMorphValueSamSamuel();
		float GetMorphValueSamSamson();
		float GetMorphValueForearmSize();
		float GetMorphValueChubbyArms();
		float GetMorphValueShoulderWidth();
		float GetMorphValueArms();
		float GetMorphValueShoulderSmooth();
		float GetMorphValueShoulderTweak();

		float GetMorphValueBelly();
		float GetMorphValueTummyTuck();
		float GetMorphValueBigBelly();
		float GetMorphValueHipBone();
		float GetMorphValueHips();
		float GetMorphValueHipForward();
		float GetMorphValueHipUpperWidth();

		float GetMorphValueNippleLength();
		float GetMorphValueNipplePerkiness();
		float GetMorphValueNipplePerkManga();
		float GetMorphValueNippleTipManga();
		float GetMorphValueNippleDistance();
		float GetMorphValueNippleSize();
		float GetMorphValueAreolaSize();
		float GetMorphValueNippleDown();

		float GetMorphValueMuscleAbs();
		float GetMorphValueMuscleArms();
		float GetMorphValueMuscleButt();
		float GetMorphValueMuscleLegs();
		float GetMorphValueMusclePecs();

		float GetMorphValueBigButt();
		float GetMorphValueChubbyButt();
		float GetMorphValueButt();
		float GetMorphValueButtClassic();
		float GetMorphValueButtShape2();
		float GetMorphValueButtCrack();
		float GetMorphValueAppleCheeks();
		float GetMorphValueRoundAss();
		float GetMorphValueCrotchBack();
		float GetMorphValueGroin();

		float GetMorphValueChubbyLegs();
		float GetMorphValueLegs();
		float GetMorphValueThighs();
		float GetMorphValueKneeShape();
		float GetMorphValueKneeHeight();
		float GetMorphValueCalfSize();
		float GetMorphValueCalfSmooth();
		float GetMorphValueLegShapeClassic();

		float GetMorphValueBigTorso();
		float GetMorphValueChestDepth();
		float GetMorphValueChestWidth();
		float GetMorphValueChubbyWaist();
		float GetMorphValueWaist();
		float GetMorphValueSternumDepth();
		float GetMorphValueSternumHeight();
		float GetMorphValueWaistHeight();
		float GetMorphValueWaistLine();

		float GetMorphValueBreasts();
		float GetMorphValueBreastsNewSH();
		float GetMorphValueBreastsFantasy();
		float GetMorphValueBreastCenter();
		float GetMorphValueBreastCenterBig();
		float GetMorphValueBreastCleavage();
		float GetMorphValueBreastFlatness2();
		float GetMorphValueBreastFlatness();
		float GetMorphValueBreastGravity2();
		float GetMorphValueBreastHeight();
		float GetMorphValueBreastsTogether();
		float GetMorphValueBreastTopSlope();
		float GetMorphValuePushUp();
		float GetMorphValueBreastPerkiness();
		float GetMorphValueBreastWidth();

	private:
		MorphProcedural() = default;
		MorphProcedural(const MorphProcedural&) = delete;
		MorphProcedural(MorphProcedural&&) = delete;
		~MorphProcedural() = default;

		int32_t formID{ 0 };
		float weight{ 0 };
		float musclepower{ 0 };
		float morphfactor{ 50 };
		float morphfactorFemale{ 100 };
		float morphfactorMale{ 100 };
		float morphfactorBreasts{ 100 };
		float morphfactorMuscles{ 100 };
		float musclePower{ 0 };
		float fitness{ 0 };
		float gravityOffset{ 0 };
		int32_t seedOffset{ 0 };
	};
}
