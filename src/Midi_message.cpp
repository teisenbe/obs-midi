#include "Midi_message.h"
int MidiMessage::get_midi_note_or_control(const libremidi::message &mess)
{
	int bytetopullfrom = -1;
	switch (mess.get_message_type()) {
	case libremidi::message_type::INVALID:
		break;
	case libremidi::message_type::NOTE_OFF:
		bytetopullfrom = 1;
		break;
	case libremidi::message_type::NOTE_ON:
		bytetopullfrom = 1;
		break;
	case libremidi::message_type::PITCH_BEND:
		bytetopullfrom = 0;
		break;
	case libremidi::message_type::CONTROL_CHANGE:
		bytetopullfrom = 1;
		break;
	case libremidi::message_type::POLY_PRESSURE:
		break;
	case libremidi::message_type::PROGRAM_CHANGE:
		break;
	case libremidi::message_type::AFTERTOUCH:
		break;
	case libremidi::message_type::SYSTEM_EXCLUSIVE:
		break;
	case libremidi::message_type::TIME_CODE:
		break;
	case libremidi::message_type::SONG_POS_POINTER:
		break;
	case libremidi::message_type::SONG_SELECT:
		break;
	case libremidi::message_type::RESERVED1:
		break;
	case libremidi::message_type::RESERVED2:
		break;
	case libremidi::message_type::TUNE_REQUEST:
		break;
	case libremidi::message_type::EOX:
		break;
	case libremidi::message_type::TIME_CLOCK:
		break;
	case libremidi::message_type::RESERVED3:
		break;
	case libremidi::message_type::START:
		break;
	case libremidi::message_type::CONTINUE:
		break;
	case libremidi::message_type::STOP:
		break;
	case libremidi::message_type::RESERVED4:
		break;
	case libremidi::message_type::ACTIVE_SENSING:
		break;
	case libremidi::message_type::SYSTEM_RESET:
		break;
	}
	return mess[bytetopullfrom];
}
int MidiMessage::get_midi_value(const libremidi::message &mess)
{
	int bytetopullfrom = -1;
	switch (mess.get_message_type()) {
	case libremidi::message_type::INVALID:
		break;
	case libremidi::message_type::PITCH_BEND:
		bytetopullfrom = 2;
		break;
	case libremidi::message_type::CONTROL_CHANGE:
		bytetopullfrom = 2;
		break;
	case libremidi::message_type::NOTE_OFF:
		bytetopullfrom = 2;
		break;
	case libremidi::message_type::NOTE_ON:
		bytetopullfrom = 2;
		break;
	case libremidi::message_type::PROGRAM_CHANGE:
		bytetopullfrom = 1;
		break;
	case libremidi::message_type::POLY_PRESSURE:
		break;
	case libremidi::message_type::AFTERTOUCH:
		break;
	case libremidi::message_type::SYSTEM_EXCLUSIVE:
		break;
	case libremidi::message_type::TIME_CODE:
		break;
	case libremidi::message_type::SONG_POS_POINTER:
		break;
	case libremidi::message_type::SONG_SELECT:
		break;
	case libremidi::message_type::RESERVED1:
		break;
	case libremidi::message_type::RESERVED2:
		break;
	case libremidi::message_type::TUNE_REQUEST:
		break;
	case libremidi::message_type::EOX:
		break;
	case libremidi::message_type::TIME_CLOCK:
		break;
	case libremidi::message_type::RESERVED3:
		break;
	case libremidi::message_type::START:
		break;
	case libremidi::message_type::CONTINUE:
		break;
	case libremidi::message_type::STOP:
		break;
	case libremidi::message_type::RESERVED4:
		break;
	case libremidi::message_type::ACTIVE_SENSING:
		break;
	case libremidi::message_type::SYSTEM_RESET:
		break;
	}
	return mess[bytetopullfrom];
}
QString MidiMessage::mtype_to_string(libremidi::message_type mess)
{
	switch (mess) {
	case libremidi::message_type::INVALID:
		return "INVALID";
	// Standard Message
	case libremidi::message_type::NOTE_OFF:
		return "Note Off";
	case libremidi::message_type::NOTE_ON:
		return "Note On";
	case libremidi::message_type::POLY_PRESSURE:
		return "POLY_PRESSURE";
	case libremidi::message_type::CONTROL_CHANGE:
		return "Control Change";
	case libremidi::message_type::PROGRAM_CHANGE:
		return "Program Change";
	case libremidi::message_type::AFTERTOUCH:
		return "AFTERTOUCH";
	case libremidi::message_type::PITCH_BEND:
		return "Pitch Bend";
	// System Common Messages
	case libremidi::message_type::SYSTEM_EXCLUSIVE:
		return "SYSTEM_EXCLUSIVE";
	case libremidi::message_type::TIME_CODE:
		return "TIME_CODE";
	case libremidi::message_type::SONG_POS_POINTER:
		return "SONG_POS_POINTER";
	case libremidi::message_type::SONG_SELECT:
		return "SONG_SELECT";
	case libremidi::message_type::RESERVED1:
		return "RESERVED1";
	case libremidi::message_type::RESERVED2:
		return "RESERVED2";
	case libremidi::message_type::TUNE_REQUEST:
		return "TUNE_REQUEST";
	case libremidi::message_type::EOX:
		return "EOX";
	// System Realtime Messages
	case libremidi::message_type::TIME_CLOCK:
		return "TIME_CLOCK";
	case libremidi::message_type::RESERVED3:
		return "RESERVED3";
	case libremidi::message_type::START:
		return "START";
	case libremidi::message_type::CONTINUE:
		return "CONTINUE";
	case libremidi::message_type::STOP:
		return "STOP";
	case libremidi::message_type::RESERVED4:
		return "RESERVED4";
	case libremidi::message_type::ACTIVE_SENSING:
		return "ACTIVE_SENSING";
	case libremidi::message_type::SYSTEM_RESET:
		return "SYSTEM_RESET";
	}
	return "ERROR";
}
QString MidiMessage::get_midi_message_type(const libremidi::message &message)
{
	switch (message.get_message_type()) {
	case libremidi::message_type::CONTROL_CHANGE:
		return "Control Change";
	case libremidi::message_type::NOTE_OFF:
		return "Note Off";
	case libremidi::message_type::NOTE_ON:
		return "Note On";
	case libremidi::message_type::PROGRAM_CHANGE:
		return "Program Change";
	case libremidi::message_type::PITCH_BEND:
		return "Pitch Bend";
	case libremidi::message_type::INVALID:
		break;
	case libremidi::message_type::POLY_PRESSURE:
		break;
	case libremidi::message_type::AFTERTOUCH:
		break;
	case libremidi::message_type::SYSTEM_EXCLUSIVE:
		break;
	case libremidi::message_type::TIME_CODE:
		break;
	case libremidi::message_type::SONG_POS_POINTER:
		break;
	case libremidi::message_type::SONG_SELECT:
		break;
	case libremidi::message_type::RESERVED1:
		break;
	case libremidi::message_type::RESERVED2:
		break;
	case libremidi::message_type::TUNE_REQUEST:
		break;
	case libremidi::message_type::EOX:
		break;
	case libremidi::message_type::TIME_CLOCK:
		break;
	case libremidi::message_type::RESERVED3:
		break;
	case libremidi::message_type::START:
		break;
	case libremidi::message_type::CONTINUE:
		break;
	case libremidi::message_type::STOP:
		break;
	case libremidi::message_type::RESERVED4:
		break;
	case libremidi::message_type::ACTIVE_SENSING:
		break;
	case libremidi::message_type::SYSTEM_RESET:
		break;
	}
	return "Unknown Message Type";
}
