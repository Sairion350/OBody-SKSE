#include "Body/Actors.h"

namespace Body
{
	Actors* Actors::GetSingleton()
	{
		static Actors singleton;
		return &singleton;
	}

	int32_t Actors::PrefilterActors()
	{
		logger::info("Prefiltering Actors Start");

		int count = 0;
		auto player = RE::PlayerCharacter::GetSingleton();
		bool isCreature;

		m_males.clear();
		m_females.clear();

		auto allForms = RE::TESForm::GetAllForms();
		auto& formIDs = *allForms.first;
		for (auto elem : formIDs) {
			if ((int)elem.second->GetFormType() == 62) {
				RE::Actor* myActor = RE::TESForm::LookupByID<RE::Actor>(elem.first);
				if (myActor) {
					if (myActor->GetRace()) {
						if (myActor->GetActorBase()) {
							//	if (myActor->GetActorBase()->defaultOutfit) {
							if (myActor->GetActorBase()->factions.size() > 0) {
								if (myActor->GetActorBase()->factions.begin()) {
									if (myActor->GetActorBase()->factions.begin()->faction) {
										isCreature = false;
										for (auto faction : myActor->GetActorBase()->factions) {
											if (faction.faction->GetFormID() == 19) {
												isCreature = true;
											} else {
												// ?
											}
										}
										if (!isCreature) {
											if (myActor->GetActorBase()->voiceType) {
												auto vt = myActor->GetActorBase()->voiceType->GetFormEditorID();
												std::string voicename = vt;
												int count_voices = 0;
												if (voicename.find("Female") != std::string::npos) {
													count_voices++;
												}
												if (voicename.find("Male") != std::string::npos) {
													count_voices++;
												}
												if (count_voices > 0) {
													if (!myActor->IsChild()) {
														if (myActor == player) {
															continue;
														}
														count++;
														if (myActor->GetActorBase()->IsFemale()) {
															m_females.push_back(RE::TESForm::LookupByID<RE::Actor>(elem.first));

														} else {
															m_males.push_back(RE::TESForm::LookupByID<RE::Actor>(elem.first));
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

		m_prefiltered = true;
		logger::info("Prefiltering Actors End");
		return count;
	}

	bool Actors::IsPrefiltered() { return m_prefiltered; }

	std::vector<RE::Actor*> Actors::GetFemales() { return m_females; }
	std::vector<RE::Actor*> Actors::GetMales() { return m_males; }

	
	RE::FormID Actors::GetRandomFemaleFormID()
	{
		auto females = GetFemales();
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> d(0, static_cast<int>(females.size()));
		int pos = d(gen);
		auto formid = females[pos]->GetFormID();
		females.erase(females.begin() + pos);
		return formid;
	}
}
