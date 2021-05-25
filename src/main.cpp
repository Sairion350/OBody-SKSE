#include "Version.h"
#include "Body/Morph.h"
#include "Papyrus.h"
#include "SKEE.h"

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kPostLoad:
		{
			SKEE::InterfaceExchangeMessage msg;
			auto intfc = SKSE::GetMessagingInterface();
			intfc->Dispatch(SKEE::InterfaceExchangeMessage::kExchangeInterface, (void*)&msg, sizeof(SKEE::InterfaceExchangeMessage*), "skee");
			if (msg.interfaceMap) {
				logger::critical("Couldn't get interface map!");
				return;
			}
			
			auto morphInt = static_cast<SKEE::IBodyMorphInterface*>(msg.interfaceMap->QueryInterface("BodyMorph"));
			if (!morphInt) {
				logger::critical("Couldn't get serialization MorphInterface!");
				return;
			}

			logger::info("BodyMorph Version {}", morphInt->GetVersion());
			auto morph = Body::Morph::GetSingleton();
			if (!morph->SetMorphInterface(morphInt))
				logger::info("BodyMorphInterace not provided");
		}
		break;
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= "OBody.log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);

	logger::info("OBody v{}"sv, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "OBody";
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		logger::critical("Unsupported runtime version {}"sv, ver.string());
		return false;
	}

	return true;
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);

	auto message = SKSE::GetMessagingInterface();
	if (!message->RegisterListener(MessageHandler))
		return false;

	auto papyrus = SKSE::GetPapyrusInterface();
	if (!papyrus->Register(Papyrus::Bind))
		return false;

	logger::info("OBody loaded"sv);

	return true;
}
