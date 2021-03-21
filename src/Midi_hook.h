#pragma once
#include <QObject>
#include <optional>
#include "utils.h"
#include "Midi_message.h"
class Actions;
/*
 * Midi Hook Class
 */
class MidiMapping : public QObject{
	Q_OBJECT
public:
	MidiMapping();
	MidiMapping(const QString &json_string);

	MidiMessage *get_message_from_hook();
	QString GetData();
	void set_obs_action();
	void EXE();
	int channel = -1;     // midi channel
	QString message_type; // Message Type
	int norc = -1;        // Note or Control
	bool value_as_filter = false;
	std::optional<int> value;
	QString action;
	Actions *actions;
private:

	void get_channel(obs_data_t *data);
	void get_message(obs_data_t *data);
	void get_norc(obs_data_t *data);
	void get_value(obs_data_t *data);
	void get_action(obs_data_t *data);
};
