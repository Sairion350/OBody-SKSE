#pragma once

namespace Body
{
	class Actors
	{
	public:
		static Actors* GetSingleton();

		int32_t PrefilterActors();
		RE::FormID GetRandomFemaleFormID();
		bool IsPrefiltered();

		std::vector<RE::Actor*> GetFemales();
		std::vector<RE::Actor*> GetMales();

	private:
		Actors() = default;
		Actors(const Actors&) = delete;
		Actors(Actors&&) = delete;
		~Actors() = default;

		std::vector<RE::Actor*> m_females;
		std::vector<RE::Actor*> m_males;

		bool m_prefiltered{ false };
	};
}
