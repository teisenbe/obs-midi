#pragma once
#include <QObject>
#include <optional>
#include "utils.h"
#include "Midi_message.h"
class Actions;
/*
 * Midi Hook Class
 */
class MidiHook : public QObject {
	Q_OBJECT
public:
	MidiHook();
	MidiHook(const QString &json_string);
	MidiMessage *get_message_from_hook();
	QString GetData();
	void set_obs_action();
	void EXE();
	int channel = -1;     // midi channel
	QString message_type; // Message Type
	int norc = -1;        // Note or Control
	QString action;
	QString scene;
	QString source;
	QString filter;
	QString transition;
	QString item;
	QString hotkey;
	QString audio_source;
	QString media_source;
	std::optional<int> duration;
	QString scene_collection;
	QString profile;
	QString string_override;
	std::optional<bool> bool_override;
	std::optional<int> int_override;
	std::optional<int> range_min;
	std::optional<int> range_max;
	bool value_as_filter = false;
	std::optional<int> value;
	Actions *actions;

private:
	/// <summary>
	/// class pointer to execute action
	/// </summary>
	

	//get Midi data from hook data
	void get_channel(obs_data_t *data);
	void get_message(obs_data_t *data);
	void get_norc(obs_data_t *data);
	void get_value(obs_data_t *data);
	// get sction data from hook data
	void get_action(obs_data_t *data);
	void get_scene(obs_data_t *data);
	void get_source(obs_data_t *data);
	void get_filter(obs_data_t *data);
	void get_transition(obs_data_t *data);
	void get_item(obs_data_t *data);
	void get_hotkey(obs_data_t *data);
	void get_audio_source(obs_data_t *data);
	void get_media_source(obs_data_t *data);
	void get_duration(obs_data_t *data);
	void get_scene_collection(obs_data_t *data);
	void get_profile(obs_data_t *data);
	void get_string_override(obs_data_t *data);
	void get_bool_override(obs_data_t *data);
	void get_int_override(obs_data_t *data);
	void get_range_min(obs_data_t *data);
	void get_range_max(obs_data_t *data);
};
