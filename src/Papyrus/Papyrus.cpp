#include "Papyrus/Papyrus.h"

#define BIND(a_method, ...) a_vm->RegisterFunction(#a_method##sv, obj, a_method __VA_OPT__(, ) __VA_ARGS__)
#include "Papyrus/PapyrusBody.h"
#undef BIND

namespace Papyrus
{
	void Bind()
	{
		auto papyrus = SKSE::GetPapyrusInterface();
		if (papyrus) {
			papyrus->Register(PapyrusBody::Bind);
		}
	}
}
