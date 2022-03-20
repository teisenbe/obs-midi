#pragma once
#include <QObject>
#include "utils.h"
#include "libremidi/include/libremidi/message.hpp"
/*Midi Message Structure*/
typedef struct MidiMessage {
public:
	MidiMessage() = default;
	void set_message(const libremidi::message &message)
	{
		this->channel = message.get_channel();
		this->message_type = get_midi_message_type(message);
		this->NORC = get_midi_note_or_control(message);
		this->value = get_midi_value(message);
	}
	QString device_name;
	QString message_type = "none";
	int channel = 0;
	int NORC = 0;
	int value = 0;
	inline bool isNote() const { return (message_type == "Note On" || message_type == "Note Off"); };
	MidiMessage get() const { return (MidiMessage) * this; }
	static QString get_midi_message_type(const libremidi::message &message);
	static QString mtype_to_string(libremidi::message_type);
	static int get_midi_note_or_control(const libremidi::message &mess);
	static int get_midi_value(const libremidi::message &mess);

} MidiMessage;
Q_DECLARE_METATYPE(MidiMessage);
