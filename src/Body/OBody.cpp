#include "Body/OBody.h"



namespace Body{
	OBody* OBody::GetInstance(){
			static OBody instance;
			return &instance;
		}

	void OBody::GenerateActorBody(RE::Actor* act){
		logger::info("A wild {} jkghjkghas appeared", act->GetName());
	}

}