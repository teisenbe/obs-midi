#pragma once
#include <QObject>
#include <optional>
#include "utils.h"
#include "Midi_message.h"
/*
 * Midi Hook Class
 */
class MidiHook : public QObject {
	Q_OBJECT
public:
	MidiHook();
	MidiHook(const QString& json_string);
	MidiMessage* get_message_from_hook();
	QString GetData();

	int channel = -1;     // midi channel
	QString message_type; // Message Type
	int norc = -1;        // Note or Control
	QString action;
	QString scene;
	QString source;
	QString filter;
	QString transition;
	QString item;
	QString shortcut;
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
	/// <summary>
	/// Function pointer to execute action
	/// </summary>
	typedef void (*obsc)(MidiHook*);
	obsc obsControlFunction;
	void EXE();
	void setAction();
};
