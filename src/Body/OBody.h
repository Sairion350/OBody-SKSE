#pragma once

#include "SKEE.h"


namespace Body
{
	class OBody
	{
		using VM = RE::BSScript::IVirtualMachine;
	public:

		static OBody* GetInstance();

		void GenerateActorBody(RE::Actor* act);

	private:
		OBody() = default;
		OBody(const OBody&) = delete;
		OBody(OBody&&) = delete;
		~OBody() = default;

		SKEE::IBodyMorphInterface* morphInt;
	};
}
