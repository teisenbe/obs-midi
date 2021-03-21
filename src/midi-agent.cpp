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
#include <functional>
#include <string>
#include <utility>
#include <QtCore/QTime>
#include "utils.h"
#include "midi-agent.h"
#include "obs-midi.h"
#include "events.h"
#include "config.h"
#include "device-manager.h"
#include "macro-helpers.h"
using namespace std;
////////////////
// MIDI AGENT //
////////////////

/// <summary>
/// Creates a new Midi Agent from input and output ports
/// </summary>
/// <param name="in_port">Input Port number of MIDI Device</param>
/// <param name="out_port">Output Port number of MIDI Device</param>
MidiAgent::MidiAgent(const int &in_port, std::optional<int> out_port)
{
	set_input_port(in_port);
	this->setParent(GetDeviceManager().get());
	set_callbacks();
}
/// <summary>
/// Creates a Midi Agent from saved devices
/// </summary>
/// <param name="midiData"></param>
MidiAgent::MidiAgent(const char *midiData)
{
	// Sets the parent of this instance of MidiAgent to Device Manager
	this->setParent(GetDeviceManager().get());
	// Sets the Midi Callback function
	this->Load(midiData);
	if (is_device_attached(midiData)) {
		set_callbacks();
		if (enabled)
			open_midi_input_port();
	}
}

/// <summary>
///  Sets the callbacks for
/// * Events
/// * MIDI messages
/// * MIDI device errors
/// </summary>
void MidiAgent::set_callbacks()
{
	midiin.set_callback([this](const auto &message) { HandleInput(message, this); });
	midiin.set_error_callback([this](const auto &error_type, const auto &error_message) { HandleError(error_type, error_message, this); });
}
/// <summary>
/// MidiAgent Deconstructor
/// </summary>
MidiAgent::~MidiAgent()
{
	this->disconnect();
	clear_MidiMappings();
	midiin.cancel_callback();
}
/// <summary>
/// Checks wether a device is attached and in the device list;
/// </summary>
/// <param name="incoming_data"></param>
/// <returns></returns>
bool MidiAgent::is_device_attached(const char *incoming_data)
{
	obs_data_t *data = obs_data_create_from_json(incoming_data);
	const int minput_port = DeviceManager().get_input_port_number(obs_data_get_string(data, "name"));
	obs_data_release(data);
	return (minput_port != -1);
}
/// <summary>
/// Loads information from OBS data. (recalled from Config)
/// This will not enable the MidiAgent or open the port. (and shouldn't)
/// </summary>
/// <param name="incoming_data"></param>
void MidiAgent::Load(const char *incoming_data)
{
	obs_data_t *data = obs_data_create_from_json(incoming_data);
	obs_data_set_default_bool(data, "enabled", false);
	obs_data_set_default_bool(data, "bidirectional", false);
	midi_input_name = QString(obs_data_get_string(data, "name"));
	input_port = DeviceManager().get_input_port_number(midi_input_name);
	enabled = obs_data_get_bool(data, "enabled");
	obs_data_array_t *hooksData = obs_data_get_array(data, "hooks");
	const size_t hooksCount = obs_data_array_count(hooksData);
	for (size_t i = 0; i < hooksCount; i++) {
		obs_data_t *hookData = obs_data_array_item(hooksData, i);
		auto *mh = new MidiMapping(obs_data_get_json(hookData));
		add_MidiMapping(std::move(mh));
		obs_data_release(hookData);
	}
	obs_data_array_release(hooksData);
	obs_data_release(data);
}
/// <summary>
/// Sets the input port number and name
/// </summary>
/// <param name="port"></param>
void MidiAgent::set_input_port(const int port)
{
	input_port = port;
	midi_input_name = QString::fromStdString(midiin.get_port_name(port));
}
/// <summary>
/// Opens MIDI input port
/// </summary>
void MidiAgent::open_midi_input_port()
{
	if (!midiin.is_port_open()) {
		try {
			midiin.open_port(input_port);
		} catch (const libremidi::midi_exception &error) {
			blog(LOG_DEBUG, "Midi Error %s", error.what());
		} catch (const libremidi::driver_error &error) {
			blog(LOG_DEBUG, "Midi Driver Error %s", error.what());
		} catch (const libremidi::system_error &error) {
			blog(LOG_DEBUG, "Midi system Error %s", error.what());
		}
		blog(LOG_INFO, "MIDI device connected In: [%d] %s", input_port, midi_input_name.toStdString().c_str());
	}
}

/// <summary>
/// Closes the connection to the Midi Input Port
/// *Does not cancel callback*
/// </summary>
void MidiAgent::close_midi_input_port()
{
	if (midiin.is_port_open()) {
		midiin.close_port();
	}
}

/// <summary>
///
/// </summary>
/// <returns></returns>
const QString &MidiAgent::get_midi_input_name() const
{
	return midi_input_name;
}
/// <summary>
///
/// </summary>
/// <returns></returns>
int MidiAgent::GetPort() const
{
	return input_port;
}
/// <summary>
///
/// </summary>
/// <returns></returns>
bool MidiAgent::isEnabled() const
{
	return enabled;
}
/// <summary>
///
/// </summary>
/// <returns></returns>
bool MidiAgent::isConnected() const
{
	return connected;
}
/// <summary>
/// Midi input callback.
/// Extend input handling functionality in the OBSController Class.
/// For OBS action triggers, edit the funcMap instead.
/// </summary>
/// <param name="message"></param>
/// <param name="userData"></param>
void MidiAgent::HandleInput(const libremidi::message &message, void *userData)
{
	auto *self = static_cast<MidiAgent *>(userData);
	if (!self->enabled) {
		return;
	}
	/*************Get Message parts***********/
	self->sending = true;
	auto *x = new MidiMessage();
	x->set_message(message);
	/***** Send Messages to emit function *****/
	x->device_name = self->get_midi_input_name();
	emit self->broadcast_midi_message((MidiMessage)*x);
	/** check if hook exists for this note or cc norc and launch it **/
	self->exe_midi_hook_if_exists(x);
	delete x;
}
/// <summary>
/// Callback function to handle midi errors
/// </summary>
/// <param name="error_type"></param>
/// <param name="error_message"></param>
/// <param name="userData"></param>
void MidiAgent::HandleError(const libremidi::midi_error &error_type, const std::string_view &error_message, void *userData)
{
	blog(LOG_ERROR, "Midi Error: %s", error_message.data());
	UNUSED_PARAMETER(error_type);
	UNUSED_PARAMETER(userData);
}

/// <summary>
/// Returns a QVector containing all Midi Hooks for this device
/// </summary>
/// <returns>QVector<MidiMapping*></returns>
QVector<MidiMapping *> MidiAgent::GetMidiMappings() const
{
	return midiHooks;
}
/// <summary>
/// Executes a MidiMapping* if Message Type, NORC and Channel are found
/// </summary>
/// <param name="message">MidiMessage</param>
/// <returns>MidiMapping*</returns>
void MidiAgent::exe_midi_hook_if_exists(MidiMessage *message)
{
	for (auto midiHook : this->midiHooks) {
		if (midiHook->message_type == message->message_type && midiHook->norc == message->NORC && midiHook->channel == message->channel) {
			if (midiHook->value_as_filter) {
				if (message->value == *midiHook->value)
					midiHook->EXE();
			} else {
				midiHook->value.emplace(message->value);
				midiHook->EXE();
			}
		}
	}
}
void MidiAgent::add_MidiMapping(MidiMapping *hook)
{
	// Add a new MidiMapping
	midiHooks.push_back(hook);
}
/// <summary>
/// Sets wether or not this Midi Agent is enabled
/// </summary>
/// <param name="state">Enable State</param>
void MidiAgent::set_enabled(const bool &state)
{
	this->enabled = state;
	if (state)
		open_midi_input_port();
	else
		close_midi_input_port();
	GetConfig().get()->Save();
}
/// <summary>
/// Replaces current MidiMappings
/// </summary>
/// <param name="mh">Midi Hooks </param>
void MidiAgent::set_midi_hooks(QVector<MidiMapping *> mh)
{
	midiHooks = std::move(mh);
}
/// <summary>
/// Remove a midi hook
/// *This does not remove from config unless saved afterwards*
/// </summary>
/// <param name="hook"></param>
void MidiAgent::remove_MidiMapping(MidiMapping *hook)
{
	// Remove a MidiMapping
	if (midiHooks.contains(hook)) {
		midiHooks.removeOne(hook);
		delete (hook);
	}
}
void MidiAgent::edit_midi_hook(MidiMapping *old_hook, MidiMapping *new_hook)
{
	remove_MidiMapping(old_hook);
	add_MidiMapping(new_hook);
}
/// <summary>
/// Clears all the MidiMappings for this device.
/// *This does not delete hooks from config unless saved afterwards*
/// </summary>
void MidiAgent::clear_MidiMappings()
{
	for (auto hook:midiHooks) {
		delete hook;
	}
	midiHooks.clear();
}
/// <summary>
/// Get this MidiAgent state as OBS Data. (includes midi hooks)
/// *This is needed to Serialize the state in the config.*
/// https://obsproject.com/docs/reference-settings.html
/// </summary>
/// <returns>QString (OBSData Json string)</returns>
QString MidiAgent::GetData()
{
	blog(LOG_DEBUG, "MA::GetData");
	obs_data_t *data = obs_data_create();
	obs_data_set_string(data, "name", midi_input_name.toStdString().c_str());
	obs_data_set_bool(data, "enabled", enabled);
	obs_data_array_t *arrayData = obs_data_array_create();
	for (auto midiHook : midiHooks) {
		obs_data_t *hookData = obs_data_create_from_json(midiHook->GetData().toStdString().c_str());
		obs_data_array_push_back(arrayData, hookData);
		obs_data_release(hookData);
	}
	obs_data_set_array(data, "hooks", arrayData);
	QString return_data(obs_data_get_json(data));
	obs_data_array_release(arrayData);
	obs_data_release(data);
	return return_data;
}

