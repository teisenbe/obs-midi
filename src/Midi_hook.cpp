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
	value_as_filter = obs_data_get_bool(data, "value_as_filter");
	value.emplace(obs_data_get_int(data, "value"));
	set_obs_action();
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
	get_value(data);
	QString hook_data(obs_data_get_json(data));
	obs_data_release(data);
	return hook_data;
}
void MidiMapping::set_obs_action()
{
	if (action.isEmpty() || action.isNull())
		return;
	Actions AC(this);
	actions = AC.make_action(action, this);
}
void MidiMapping::EXE()
{
	actions->execute();
}
