#include "Version.h"
#include "Body/Body.h"
#include "Body/Event.h"
#include "Papyrus/Papyrus.h"
#include "SKEE.h"

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kPostLoad:
		{
			SKEE::InterfaceExchangeMessage msg;
			auto intfc = SKSE::GetMessagingInterface();
			intfc->Dispatch(SKEE::InterfaceExchangeMessage::kExchangeInterface, (void*)&msg, sizeof(SKEE::InterfaceExchangeMessage*), "skee");
			if (!msg.interfaceMap) {
				logger::critical("Couldn't get interface map!");
				return;
			}

			auto morphInterface = static_cast<SKEE::IBodyMorphInterface*>(msg.interfaceMap->QueryInterface("BodyMorph"));
			if (!morphInterface) {
				logger::critical("Couldn't get serialization MorphInterface!");
				return;
			}

			logger::info("BodyMorph Version {}", morphInterface->GetVersion());
			auto obody = Body::OBody::GetInstance();
			if (!obody->SetMorphInterface(morphInterface))
				logger::info("BodyMorphInterace not provided");

			obody->setGameLoaded = false;
			obody->Generate();
			//obody->PrintDatabase();

			Event::Register();
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

	*path /= fmt::format(FMT_STRING("{}.log"), Version::PROJECT);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("OBody: [%^%l%$] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
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

	Papyrus::Bind();

	logger::info(FMT_STRING("{} loaded"), Version::PROJECT);

	return true;
}
