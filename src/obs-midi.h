#pragma once
#include <memory>

#include <QObject>
#include <optional>

#include <obs.hpp>


void ___data_item_dummy_addref(obs_data_item_t *);
void ___data_item_release(obs_data_item_t *);
using OBSDataItemAutoRelease = OBSRef<obs_data_item_t *, ___data_item_dummy_addref, ___data_item_release>;

class Events;
class Config;
class DeviceManager;
class PluginWindow;
typedef std::shared_ptr<Events> eventsPtr;
typedef std::shared_ptr<Config> ConfigPtr;
typedef std::shared_ptr<DeviceManager> DeviceManagerPtr;
ConfigPtr GetConfig();
DeviceManagerPtr GetDeviceManager();
eventsPtr GetEventsSystem();
static PluginWindow *plugin_window;
#define OBS_MIDI_VERSION "0.1"
#define blog(level, msg, ...) blog(level, "[obs-midi] " msg, ##__VA_ARGS__)
