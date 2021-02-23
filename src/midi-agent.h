/*
obs-midi
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/
#pragma once
#if __has_include(<obs-frontend-api.h>)
#include <obs-frontend-api.h>
#include "rtmidi17/rtmidi17.hpp"
#else
#include <obs-frontend-api/obs-frontend-api.h>
#include "rtmidi17/rtmidi17.hpp"
#endif
#include <QtCore/QString>
#include <QtCore/QSharedPointer>
#include <vector>
#include <QObject>
#include <functional>
#include <map>
#include <string>
#include <iostream>
#include "rpc/RpcEvent.h"
#include "utils.h"
#include "obs-controller.h"
class MidiAgent : public QObject {
	Q_OBJECT
public:
	MidiAgent(const int &in_port, const int &out_port);
	MidiAgent(obs_data_t *data);
	~MidiAgent();
	void Load(obs_data_t *data);
	// Open Actions
	void open_midi_input_port();
	void open_midi_output_port();
	//Close Actions
	void close_both_midi_ports();
	void close_midi_output_port();
	void close_midi_input_port();
	const QString &get_midi_input_name();
	const QString &get_midi_output_name();
	void set_input_port(const int port);
	void set_output_port(const int port);
	void set_midi_output_name(const QString &oname);
	int GetPort();
	bool isEnabled();
	bool isConnected();
	bool isBidirectional();
	bool set_bidirectional(const bool &state);
	void set_enabled(const bool &state);
	static void HandleInput(const rtmidi::message &message, void *userData);
	static void HandleError(const rtmidi::midi_error &error,
				const std::string_view &error_message,
				void *userData);
	void set_callbacks();
	QVector<MidiHook *> GetMidiHooks();
	void set_midi_hooks(QVector<MidiHook *>);
	void add_MidiHook(MidiHook *hook);
	void remove_MidiHook(MidiHook *hook);
	void clear_MidiHooks();
	obs_data_t *GetData();
public slots:
	void handle_obs_event(const QString &eventType, const QString &eventData);
signals:
	void broadcast_midi_message(const MidiMessage &);
	void do_obs_action(MidiHook *, int);

private:
	void send_message_to_midi_device(const MidiMessage& message);
	rtmidi::midi_in midiin;
	rtmidi::midi_out midiout;
	QString midi_input_name;
	QString midi_output_name;
	bool sending;
	int input_port = -1;
	int output_port = -1;
	int lastscenebtn;
	int last_preview_scene_norc;
	bool enabled = false;
	bool connected = false;
	bool bidirectional = false;
	MidiHook *get_midi_hook_if_exists(MidiMessage *message);
	bool closing = false;
	QVector<MidiHook *> midiHooks;
};
