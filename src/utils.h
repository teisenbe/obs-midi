/*
obs-midi
Copyright (C) 2020-2021	Chris Yarger <cpyarger@gmail.com>
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

#include <QtCore/QString>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QLayout>
#include <QtWidgets/QListWidget>

#include <iostream>
#include <obs.hpp>
#include "obs-midi.h"

typedef void (*PauseRecordingFunction)(bool);
typedef bool (*RecordingPausedFunction)();
enum class Pairs { Scene, Source, Item, Transition, Audio, Media, Filter, String, Integer, Boolean, Range, Hotkey };
enum class Alignment {
	Top_Left = OBS_ALIGN_LEFT | OBS_ALIGN_TOP,
	Top_Center = OBS_ALIGN_TOP | OBS_ALIGN_CENTER,
	Top_Right = OBS_ALIGN_TOP | OBS_ALIGN_RIGHT,
	Center_Left = OBS_ALIGN_CENTER | OBS_ALIGN_LEFT,
	Center = OBS_ALIGN_CENTER,
	Center_Right = OBS_ALIGN_CENTER | OBS_ALIGN_RIGHT,
	Bottom_Left = OBS_ALIGN_BOTTOM | OBS_ALIGN_LEFT,
	Bottom_Center = OBS_ALIGN_BOTTOM | OBS_ALIGN_CENTER,
	Bottom_Right = OBS_ALIGN_BOTTOM | OBS_ALIGN_RIGHT
};

enum class Speed { Slow, Medium, Fast };
class ActionsClass : public QObject {
	Q_OBJECT
public:
	enum Actions {
		Toggle_Fade_Source,
		Disable_Preview,
		Disable_Source_Filter,
		Do_Transition,
		Enable_Preview,
		Enable_Source_Filter,
		Move_T_Bar,
		Next_Media,
		Pause_Recording,
		Play_Pause_Media,
		Previous_Media,
		Reload_Browser_Source,
		Reset_Scene_Item,
		Reset_Stats,
		Restart_Media,
		Resume_Recording,
		Save_Replay_Buffer,
		Scrub_Media,
		Set_Audio_Monitor_Type,
		Set_Browser_Source_URL,
		Set_Current_Profile,
		Set_Current_Scene,
		Set_Current_Scene_Collection,
		Set_Current_Transition,
		Set_Gain_Filter,
		Set_Media_Time,
		Set_Mute,
		Set_Opacity,
		Set_Preview_Scene,
		Set_Scene_Item_Crop,
		Set_Scene_Item_Position,
		Set_Scene_Item_Render,
		Set_Scene_Item_Transform,
		Set_Scene_Transition_Override,
		Set_Source_Filter_Visibility,
		Set_Source_Name,
		Set_Source_Position,
		Set_Source_Rotation,
		Set_Source_Scale,
		Set_Source_Settings,
		Set_Sync_Offset,
		Set_Text_GDIPlus_Text,
		Set_Transition_Duration,
		Set_Volume,
		Start_Recording,
		Start_Replay_Buffer,
		Start_Streaming,
		Stop_Media,
		Stop_Recording,
		Stop_Replay_Buffer,
		Stop_Streaming,
		Studio_Mode,
		Take_Screenshot,
		Take_Source_Screenshot,
		Toggle_Mute,
		Toggle_Source_Filter,
		Toggle_Source_Visibility,
		Toggle_Start_Stop_Recording,
		Toggle_Start_Stop_Replay_Buffer,
		Toggle_Start_Stop_Streaming,
		Trigger_Hotkey_By_Name,
		Unpause_Recording
	};
	Q_ENUM(Actions)
	
	static QString action_to_string(const Actions &enumval);
	static Actions string_to_action(const QString &string);
	static float fade_value();
};
typedef void (*PauseRecordingFunction)(bool);
typedef bool (*RecordingPausedFunction)();
namespace Utils {
class OBSActionsWidget;
float mapper(int x);
float map_to_range(int min, int max, int input);
int mapper2(double x);
int t_bar_mapper(int x);
bool is_number(const QString &s);
bool isJSon(const QString &val);
QStringList GetMediaSourceNames();
QStringList GetAudioSourceNames();
QString nsToTimestamp(uint64_t ns);
obs_data_array_t *StringListToArray(char **strings, const char *key);
QString GetSceneItems(obs_source_t *source);
QStringList GetSceneItemsBySource(obs_source_t *source);
obs_data_t *GetSceneItemData(obs_sceneitem_t *item);
QString GetSourceArray();
QString GetSceneArray(const QString &name = "");
// These functions support nested lookup into groups
obs_sceneitem_t *GetSceneItemFromName(obs_scene_t *scene, const QString &name);
obs_sceneitem_t *GetSceneItemFromId(obs_scene_t *scene, int64_t id);
obs_sceneitem_t *GetSceneItemFromItem(obs_scene_t *scene, obs_data_t *item);
obs_sceneitem_t *GetSceneItemFromRequestField(obs_scene_t *scene, obs_data_item_t *dataItem);
obs_scene_t *GetSceneFromNameOrCurrent(const QString &sceneName);
obs_data_t *GetSceneItemPropertiesData(obs_sceneitem_t *item);
obs_data_t *GetSourceFilterInfo(obs_source_t *filter, bool includeSettings);
obs_data_array_t *GetSourceFiltersList(obs_source_t *source, bool includeSettings);
bool IsValidAlignment(uint32_t alignment);
obs_data_array_t *GetScenes();
QString GetSceneData(obs_source_t *source);
// TODO contribute a proper frontend API method for this to OBS and remove this hack
int GetTransitionDuration(obs_source_t *transition);
obs_source_t *GetTransitionFromName(const QString &transitionName);
bool SetTransitionByName(const QString &transitionName);
obs_data_t *GetTransitionData(obs_source_t *transition);
QString OBSVersionString();
const char *GetRecordingFolder();
bool SetRecordingFolder(const char *path);
QString ParseDataToQueryString(obs_data_t *data);
obs_hotkey_t *FindHotkeyByName(const QString &name);
QStringList GetHotkeysList();
bool ReplayBufferEnabled();
void StartReplayBuffer();
bool IsRPHotkeySet();
const char *GetFilenameFormatting();
bool SetFilenameFormatting(const char *filenameFormatting);
bool inrange(int low, int high, int x);
QStringList GetTransitionsList();
QStringList GetSceneItemsList(const QString &scene);
bool inrange(int low, int high, int x);
QSpinBox *GetTransitionDurationControl();
QStringList TranslateActions();
QStringList get_scene_names();
QStringList get_source_names(const QString &scene);
QStringList get_filter_names(const QString &Source);
QStringList get_transition_names();
QString untranslate(const QString &tstring);
QStringList get_browser_sources();
const QList<ActionsClass::Actions> AllActions_raw = {
	ActionsClass::Actions::Do_Transition,
	ActionsClass::Actions::Set_Volume,
	ActionsClass::Actions::Toggle_Mute,
	ActionsClass::Actions::Set_Preview_Scene,
	ActionsClass::Actions::Disable_Source_Filter,
	ActionsClass::Actions::Enable_Source_Filter,
	ActionsClass::Actions::Move_T_Bar,
	ActionsClass::Actions::Next_Media,
	ActionsClass::Actions::Pause_Recording,
	ActionsClass::Actions::Play_Pause_Media,
	ActionsClass::Actions::Previous_Media,
	ActionsClass::Actions::Reload_Browser_Source,
	ActionsClass::Actions::Reset_Scene_Item,
	ActionsClass::Actions::Restart_Media,
	ActionsClass::Actions::Resume_Recording,
	ActionsClass::Actions::Save_Replay_Buffer,
	ActionsClass::Actions::Set_Current_Transition,
	ActionsClass::Actions::Set_Scene_Transition_Override,
	ActionsClass::Actions::Start_Recording,
	ActionsClass::Actions::Start_Replay_Buffer,
	ActionsClass::Actions::Start_Streaming,
	ActionsClass::Actions::Stop_Media,
	ActionsClass::Actions::Stop_Recording,
	ActionsClass::Actions::Stop_Replay_Buffer,
	ActionsClass::Actions::Stop_Streaming,
	ActionsClass::Actions::Studio_Mode,
	ActionsClass::Actions::Take_Screenshot,
	ActionsClass::Actions::Take_Source_Screenshot,
	ActionsClass::Actions::Toggle_Source_Filter,
	ActionsClass::Actions::Toggle_Source_Visibility,
	ActionsClass::Actions::Toggle_Start_Stop_Recording,
	ActionsClass::Actions::Toggle_Start_Stop_Replay_Buffer,
	ActionsClass::Actions::Toggle_Start_Stop_Streaming,
	ActionsClass::Actions::Unpause_Recording,
	ActionsClass::Actions::Set_Source_Rotation,
	ActionsClass::Actions::Set_Source_Scale,
	ActionsClass::Actions::Trigger_Hotkey_By_Name,
	ActionsClass::Toggle_Fade_Source,
};
const QList<ActionsClass::Actions> not_ready_actions{
	ActionsClass::Actions::Set_Current_Scene_Collection,
	ActionsClass::Actions::Reset_Stats,
	ActionsClass::Actions::Set_Current_Profile,
	ActionsClass::Actions::Set_Source_Position,
	ActionsClass::Actions::Set_Source_Filter_Visibility,
	ActionsClass::Actions::Set_Source_Name,
	ActionsClass::Actions::Set_Transition_Duration,
	ActionsClass::Actions::Set_Gain_Filter,
	ActionsClass::Actions::Set_Media_Time,
	ActionsClass::Actions::Set_Source_Settings,
	ActionsClass::Actions::Set_Sync_Offset,
	ActionsClass::Actions::Set_Mute,
	ActionsClass::Actions::Scrub_Media,
	ActionsClass::Actions::Set_Audio_Monitor_Type,
	ActionsClass::Actions::Set_Scene_Item_Crop,
	ActionsClass::Actions::Set_Scene_Item_Position,
	ActionsClass::Actions::Set_Scene_Item_Render,
	ActionsClass::Actions::Set_Scene_Item_Transform,
	ActionsClass::Actions::Set_Text_GDIPlus_Text,
	ActionsClass::Actions::Set_Opacity,
	ActionsClass::Actions::Set_Browser_Source_URL,
};
void alert_popup(const QString &message);
QString translate_action(ActionsClass::Actions action);

static QMap<QString, QString> hotkey_map;
void build_hotkey_map();
QString get_hotkey_key(QString value);
QString get_hotkey_value(QString key);


};
