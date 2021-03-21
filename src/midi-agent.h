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
#include <vector>
#include <functional>
#include <map>
#include <string>
#include <iostream>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>

#include <libremidi/libremidi.hpp>
#if __has_include(<obs-frontend-api.h>)
#include <obs-frontend-api.h>
#else
#include <obs-frontend-api/obs-frontend-api.h>
#endif
#include "rpc/RpcEvent.h"
#include "utils.h"
#include "obs-controller.h"
#include <qabstractitemmodel.h>

class MidiAgent : public QObject
{
	Q_OBJECT
public:
	MidiAgent(const int &in_port, std::optional<int> out_port = std::nullopt);
	MidiAgent(const char *data);
	
	~MidiAgent();
	bool is_device_attached(const char *idata);
	void Load(const char *data);
	// Open Actions
	void open_midi_input_port();
	// Close Actions
	void close_midi_input_port();
	const QString &get_midi_input_name() const;
	void set_input_port(int port);
	int GetPort() const;
	bool isEnabled() const;
	bool isConnected() const;
	void set_enabled(const bool &state);
	static void HandleInput(const libremidi::message &message, void *userData);
	static void HandleError(const libremidi::midi_error &error, const std::string_view &error_message, void *userData);
	static void HandleError(const libremidi::driver_error &error_type, const std::string_view &error_message, void *userData);
	void set_callbacks();
	QVector<MidiMapping *> GetMidiMappings() const;
	void set_midi_hooks(QVector<MidiMapping *>);
	void exe_midi_hook_if_exists(MidiMessage *message);
	void add_MidiMapping(MidiMapping *hook);
	void remove_MidiMapping(MidiMapping *hook);
	void edit_midi_hook(MidiMapping *old_hook, MidiMapping *new_hook);
	void clear_MidiMappings();
	QString GetData();
	void set_current_volumes();
	void startup();
	QVector<MidiMapping *> midiHooks;
signals:
	void broadcast_midi_message(const MidiMessage &);
	void do_obs_action(MidiMapping *, int);

private:
	bool loading = true;
	libremidi::midi_in midiin;
	QString midi_input_name;
	bool sending{};
	int input_port = -1;
	bool enabled = false;
	bool connected = false;
	bool closing = false;
	
};
