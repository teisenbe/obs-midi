#include "obs-controller.h"
#include "Midi_hook.h"

MidiMapping::MidiMapping(){};
MidiMapping::MidiMapping(const QString &json_string)
{
	const auto data = obs_data_create_from_json(json_string.qtocs());
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
	set_obs_action(data);
	obs_data_release(data);
}
MidiMessage *MidiMapping::get_message_from_hook()
{
	auto *message = new MidiMessage();
	message->channel = this->channel;
	message->message_type = this->message_type;
	message->NORC = this->norc;
	message->value = *this->value;
	return std::move(message);
}
void MidiMapping::get_scene(obs_data_t *data)
{
	if (scene.isEmpty() || scene.isNull())
		return;
	obs_data_set_string(data, "scene", scene.qtocs());
}
void MidiMapping::get_source(obs_data_t *data)
{
	if (source.isEmpty())
		return;
	obs_data_set_string(data, "source", source.qtocs());
}
void MidiMapping::get_filter(obs_data_t *data)
{
	if (filter.isEmpty())
		return;
	obs_data_set_string(data, "filter", filter.qtocs());
}
void MidiMapping::get_transition(obs_data_t *data)
{
	if (transition.isEmpty())
		return;
	obs_data_set_string(data, "transition", transition.qtocs());
}
void MidiMapping::get_item(obs_data_t *data)
{
	if (item.isEmpty())
		return;
	obs_data_set_string(data, "item", item.qtocs());
}
void MidiMapping::get_hotkey(obs_data_t *data)
{
	if (hotkey.isEmpty())
		return;
	obs_data_set_string(data, "hotkey", hotkey.qtocs());
}
void MidiMapping::get_audio_source(obs_data_t *data)
{
	if (audio_source.isEmpty())
		return;
	obs_data_set_string(data, "audio_source", audio_source.qtocs());
}
void MidiMapping::get_media_source(obs_data_t *data)
{
	if (media_source.isEmpty())
		return;
	obs_data_set_string(data, "media_source", media_source.qtocs());
}
void MidiMapping::get_duration(obs_data_t *data)
{
	if (duration)
		obs_data_set_int(data, "duration", *duration);
}
void MidiMapping::get_scene_collection(obs_data_t *data)
{
	if (scene_collection.isEmpty())
		return;
	obs_data_set_string(data, "scene_collection", scene_collection.qtocs());
}
void MidiMapping::get_profile(obs_data_t *data)
{
	if (profile.isEmpty())
		return;
	obs_data_set_string(data, "profile", profile.qtocs());
}
void MidiMapping::get_string_override(obs_data_t *data)
{
	if (string_override.isEmpty())
		return;
	obs_data_set_string(data, "string_override", string_override.qtocs());
}
void MidiMapping::get_bool_override(obs_data_t *data)
{
	if (bool_override)
		obs_data_set_bool(data, "bool_override", *bool_override);
}
void MidiMapping::get_int_override(obs_data_t *data)
{
	if (int_override)
		obs_data_set_int(data, "int_override", *int_override);
}
void MidiMapping::get_range_min(obs_data_t *data)
{
	if (range_min)
		obs_data_set_int(data, "range_min", *range_min);
}
void MidiMapping::get_range_max(obs_data_t *data)
{
	if (range_max)
		obs_data_set_int(data, "range_max", *range_max);
}
void MidiMapping::get_value(obs_data_t *data)
{
	obs_data_set_bool(data, "value_as_filter", value_as_filter);
	if (value_as_filter)
		obs_data_set_int(data, "value", *value);
}
void MidiMapping::get_channel(obs_data_t *data)
{
	obs_data_set_int(data, "channel", channel);
}

void MidiMapping::get_message(obs_data_t *data)
{
	obs_data_set_string(data, "message_type", message_type.qtocs());
}

void MidiMapping::get_norc(obs_data_t *data)
{
	obs_data_set_int(data, "norc", norc);
}

void MidiMapping::get_action(obs_data_t *data)
{
	obs_data_set_string(data, "action", action.qtocs());
}

QString MidiMapping::GetData()
{
	obs_data_t *data = obs_data_create();
	get_channel(data);
	get_message(data);
	get_norc(data);
	get_action(data);
	get_scene(data);
	get_source(data);
	get_filter(data);
	get_transition(data);
	get_item(data);
	get_hotkey(data);
	get_audio_source(data);
	get_media_source(data);
	get_duration(data);
	get_scene_collection(data);
	get_profile(data);
	get_string_override(data);
	get_bool_override(data);
	get_int_override(data);
	get_range_min(data);
	get_range_max(data);
	get_value(data);
	QString hook_data(obs_data_get_json(data));
	obs_data_release(data);
	return hook_data;
}
void MidiMapping::set_obs_action(obs_data_t *data)
{
	if (action.isEmpty() || action.isNull())
		return;
	Actions AC(this);
	actions = AC.make_action(action, this,data);
}
void MidiMapping::EXE()
{
	actions->execute();
}
