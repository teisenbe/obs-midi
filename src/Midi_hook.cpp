#include"obs-controller.h"
#include "Midi_hook.h"

MidiHook::MidiHook(){};
MidiHook::MidiHook(const QString &json_string)
{
	const auto data = obs_data_create_from_json(json_string.toStdString().c_str());
	channel = obs_data_get_int(data, "channel");
	message_type = obs_data_get_string(data, "message_type");
	norc = obs_data_get_int(data, "norc");
	action = obs_data_get_string(data, "action");
	scene = obs_data_get_string(data, "scene");
	source = obs_data_get_string(data, "source");
	filter = obs_data_get_string(data, "filter");
	transition = obs_data_get_string(data, "transition");
	item = obs_data_get_string(data, "item");
        hotkey = obs_data_get_string(data, "hotkey");
	audio_source = obs_data_get_string(data, "audio_source");
	media_source = obs_data_get_string(data, "media_source");
	duration.emplace(obs_data_get_int(data, "duration"));
	scene_collection = obs_data_get_string(data, "scene_collection");
	profile = obs_data_get_string(data, "profile");
	string_override = obs_data_get_string(data, "string_override");
	bool_override.emplace(obs_data_get_bool(data, "bool_override"));
	int_override.emplace(obs_data_get_int(data, "int_override"));
	int_override.emplace(obs_data_get_int(data, "range_min"));
	int_override.emplace(obs_data_get_int(data, "range_max"));
	value_as_filter = obs_data_get_bool(data, "value_as_filter");
	value.emplace(obs_data_get_int(data, "value"));
	setAction();
	obs_data_release(data);
}
MidiMessage *MidiHook::get_message_from_hook()
{
	auto *message = new MidiMessage();
	message->channel = this->channel;
	message->message_type = this->message_type;
	message->NORC = this->norc;
	message->value = *this->value;
	return std::move(message);
}

QString MidiHook::GetData()
{
	blog(LOG_DEBUG, "MH::GetData");
	obs_data_t *data = obs_data_create();
	obs_data_set_int(data, "channel", channel);
	obs_data_set_string(data, "message_type", message_type.toStdString().c_str());
	obs_data_set_int(data, "norc", norc);
	obs_data_set_string(data, "action", action.toStdString().c_str());
	if (!scene.isEmpty())
		obs_data_set_string(data, "scene", scene.toStdString().c_str());
	if (!source.isEmpty())
		obs_data_set_string(data, "source", source.toStdString().c_str());
	if (!filter.isEmpty())
		obs_data_set_string(data, "filter", filter.toStdString().c_str());
	if (!transition.isEmpty())
		obs_data_set_string(data, "transition", transition.toStdString().c_str());
	if (!item.isEmpty())
		obs_data_set_string(data, "item", item.toStdString().c_str());
	if (!hotkey.isEmpty())
		obs_data_set_string(data, "hotkey", hotkey.toStdString().c_str());
	if (!audio_source.isEmpty())
		obs_data_set_string(data, "audio_source", audio_source.toStdString().c_str());
	if (!media_source.isEmpty())
		obs_data_set_string(data, "media_source", media_source.toStdString().c_str());
	if (duration)
		obs_data_set_int(data, "duration", *duration);
	if (!scene_collection.isEmpty())
		obs_data_set_string(data, "scene_collection", scene_collection.toStdString().c_str());
	if (!profile.isEmpty())
		obs_data_set_string(data, "profile", profile.toStdString().c_str());
	if (!string_override.isEmpty())
		obs_data_set_string(data, "string_override", string_override.toStdString().c_str());
	if (bool_override)
		obs_data_set_bool(data, "bool_override", *bool_override);
	if (int_override)
		obs_data_set_int(data, "int_override", *int_override);
	if (range_min)
		obs_data_set_int(data, "range_min", *range_min);
	if (range_max)
		obs_data_set_int(data, "range_max", *range_max);
	if (value_as_filter)
		obs_data_set_int(data, "value", *value);
	obs_data_set_bool(data, "value_as_filter", value_as_filter);
	QString hook_data(obs_data_get_json(data));
	blog(LOG_DEBUG, "Midi Hook JSON = %s", hook_data.toStdString().c_str());
	obs_data_release(data);
	blog(LOG_DEBUG, "Midi Hook JSON post release = %s", hook_data.toStdString().c_str());
	return hook_data;
}
void MidiHook::setAction()
{
	if (action.isEmpty() || action.isNull())
		return;
	Actions AC(this);
	actions = AC.get_action(action, this);
}
void MidiHook::EXE()
{
	actions->execute();
}
void MidiHook::setHotkey(obs_hotkey_t *hotkey)
{
	if (!hotkey) {
		hotkeyInstance = NULL;
		return;
	}
	hotkeyInstance = hotkey;
	this->hotkey = QString(obs_hotkey_get_name(hotkey));
}

void MidiHook::initHotkey()
{
	if (hotkey.isEmpty()) {
		this->hotkey = nullptr;
		return;
	}
	obs_hotkey_t *obsHotkey = Utils::FindHotkeyByName(hotkey);
	if (obsHotkey) {
		this->hotkeyInstance = obsHotkey;
	}
}

obs_hotkey_t *MidiHook::getHotkey() const
{
	if (!hotkeyInstance) {
		blog(LOG_ERROR, "ERROR: Stored hotkey %s not found", hotkey.toStdString().c_str());
		return nullptr;
	}
	return hotkeyInstance;
}
