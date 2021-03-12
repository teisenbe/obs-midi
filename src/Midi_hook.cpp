
#include "Midi_hook.h"

#include "obs-controller.h"
MidiHook::MidiHook(){};
MidiHook::MidiHook(const QString &json_string)
{
	const auto data = obs_data_create_from_json(json_string.toStdString().c_str());
	channel = obs_data_get_int(data, "channel");
	message_type = obs_data_get_string(data, "message_type");
	norc = obs_data_get_int(data, "norc");
	action = obs_data_get_string(data, "action");
	scene = obs_data_get_string(data, "scene");
	source = obs_data_get_string(data, "source");
	filter = obs_data_get_string(data, "filter");
	transition = obs_data_get_string(data, "transition");
	item = obs_data_get_string(data, "item");
	audio_source = obs_data_get_string(data, "audio_source");
	media_source = obs_data_get_string(data, "media_source");
	duration.emplace(obs_data_get_int(data, "duration"));
	scene_collection = obs_data_get_string(data, "scene_collection");
	profile = obs_data_get_string(data, "profile");
	string_override = obs_data_get_string(data, "string_override");
	bool_override.emplace(obs_data_get_bool(data, "bool_override"));
	int_override.emplace(obs_data_get_int(data, "int_override"));
	int_override.emplace(obs_data_get_int(data, "range_min"));
	int_override.emplace(obs_data_get_int(data, "range_max"));
	value_as_filter = obs_data_get_bool(data, "value_as_filter");
	value.emplace(obs_data_get_int(data, "value"));
	setAction();
	obs_data_release(data);
}
MidiMessage *MidiHook::get_message_from_hook()
{
	auto *message = new MidiMessage();
	message->channel = this->channel;
	message->message_type = this->message_type;
	message->NORC = this->norc;
	message->value = *this->value;
	return std::move(message);
}
void MidiHook::EXE()
{
	(*obsControlFunction)(this);
}
QString MidiHook::GetData()
{
	blog(LOG_DEBUG, "MH::GetData");
	obs_data_t *data = obs_data_create();
	obs_data_set_int(data, "channel", channel);
	obs_data_set_string(data, "message_type", message_type.toStdString().c_str());
	obs_data_set_int(data, "norc", norc);
	obs_data_set_string(data, "action", action.toStdString().c_str());
	if (!scene.isEmpty())
		obs_data_set_string(data, "scene", scene.toStdString().c_str());
	if (!source.isEmpty())
		obs_data_set_string(data, "source", source.toStdString().c_str());
	if (!filter.isEmpty())
		obs_data_set_string(data, "filter", filter.toStdString().c_str());
	if (!transition.isEmpty())
		obs_data_set_string(data, "transition", transition.toStdString().c_str());
	if (!item.isEmpty())
		obs_data_set_string(data, "item", item.toStdString().c_str());
	if (!audio_source.isEmpty())
		obs_data_set_string(data, "audio_source", audio_source.toStdString().c_str());
	if (!media_source.isEmpty())
		obs_data_set_string(data, "media_source", media_source.toStdString().c_str());
	if (duration)
		obs_data_set_int(data, "duration", *duration);
	if (!scene_collection.isEmpty())
		obs_data_set_string(data, "scene_collection", scene_collection.toStdString().c_str());
	if (!profile.isEmpty())
		obs_data_set_string(data, "profile", profile.toStdString().c_str());
	if (!string_override.isEmpty())
		obs_data_set_string(data, "string_override", string_override.toStdString().c_str());
	if (bool_override)
		obs_data_set_bool(data, "bool_override", *bool_override);
	if (int_override)
		obs_data_set_int(data, "int_override", *int_override);
	if (range_min)
		obs_data_set_int(data, "range_min", *range_min);
	if (range_max)
		obs_data_set_int(data, "range_max", *range_max);
	if (value_as_filter)
		obs_data_set_int(data, "value", *value);
	obs_data_set_bool(data, "value_as_filter", value_as_filter);
	QString hook_data(obs_data_get_json(data));
	blog(LOG_DEBUG, "Midi Hook JSON = %s", hook_data.toStdString().c_str());
	obs_data_release(data);
	blog(LOG_DEBUG, "Midi Hook JSON post release = %s", hook_data.toStdString().c_str());
	return hook_data;
}
void MidiHook::setAction()
{
	switch (ActionsClass::string_to_action(Utils::untranslate(action))) {
	case ActionsClass::Set_Current_Scene:
		break;
	case ActionsClass::Reset_Scene_Item:
		obsControlFunction = ResetSceneItem;
		break;
	case ActionsClass::Toggle_Mute:
		obsControlFunction = ToggleMute;
		break;
	case ActionsClass::Do_Transition:
		obsControlFunction = TransitionToProgram;
		break;
	case ActionsClass::Set_Current_Transition:
		obsControlFunction = SetCurrentTransition;
		break;
	case ActionsClass::Set_Mute:
		obsControlFunction = SetMute;
		break;
	case ActionsClass::Toggle_Start_Stop_Streaming:
		obsControlFunction = StartStopStreaming;
		break;
	case ActionsClass::Set_Preview_Scene:
		obsControlFunction = SetPreviewScene;
		break;
	case ActionsClass::Set_Current_Scene_Collection:
		obsControlFunction = SetCurrentSceneCollection;
		break;
	case ActionsClass::Set_Transition_Duration:
		obsControlFunction = SetTransitionDuration;
		break;
	case ActionsClass::Start_Streaming:
		obsControlFunction = StartStreaming;
		break;
	case ActionsClass::Stop_Streaming:
		obsControlFunction = StopStreaming;
		break;
	case ActionsClass::Start_Recording:
		obsControlFunction = StartRecording;
		break;
	case ActionsClass::Stop_Recording:
		obsControlFunction = StopRecording;
		break;
	case ActionsClass::Start_Replay_Buffer:
		obsControlFunction = StartReplayBuffer;
		break;
	case ActionsClass::Stop_Replay_Buffer:
		obsControlFunction = StopReplayBuffer;
		break;
	case ActionsClass::Set_Volume:
		obsControlFunction = SetVolume;
		break;
	case ActionsClass::Take_Source_Screenshot:
		obsControlFunction = TakeSourceScreenshot;
		break;
	case ActionsClass::Pause_Recording:
		obsControlFunction = PauseRecording;
		break;
	case ActionsClass::Enable_Source_Filter:
		obsControlFunction = EnableSourceFilter;
		break;
	case ActionsClass::Disable_Source_Filter:
		obsControlFunction = DisableSourceFilter;
		break;
	case ActionsClass::Toggle_Start_Stop_Recording:
		obsControlFunction = StartStopRecording;
		break;
	case ActionsClass::Toggle_Start_Stop_Replay_Buffer:
		obsControlFunction = StartStopReplayBuffer;
		break;
	case ActionsClass::Resume_Recording:
		obsControlFunction = ResumeRecording;
		break;
	case ActionsClass::Save_Replay_Buffer:
		obsControlFunction = SaveReplayBuffer;
		break;
	case ActionsClass::Set_Current_Profile:
		obsControlFunction = SetCurrentProfile;
		break;
	case ActionsClass::Toggle_Source_Filter:
		obsControlFunction = ToggleSourceFilter;
		break;
	case ActionsClass::Set_Text_GDIPlus_Text:
		obsControlFunction = SetTextGDIPlusText;
		break;
	case ActionsClass::Set_Browser_Source_URL:
		obsControlFunction = SetBrowserSourceURL;
		break;
	case ActionsClass::Reload_Browser_Source:
		obsControlFunction = ReloadBrowserSource;
		break;
	case ActionsClass::Set_Sync_Offset:
		obsControlFunction = SetSyncOffset;
		break;
	case ActionsClass::Set_Source_Rotation:
		obsControlFunction = SetSourceRotation;
		break;
	case ActionsClass::Set_Source_Position:
		obsControlFunction = SetSourcePosition;
		break;
	case ActionsClass::Set_Gain_Filter:
		obsControlFunction = SetGainFilter;
		break;
	case ActionsClass::Set_Opacity:
		obsControlFunction = SetOpacity;
		break;
	case ActionsClass::Set_Source_Scale:
		obsControlFunction = SetSourceScale;
		break;
	case ActionsClass::Move_T_Bar:
		obsControlFunction = move_t_bar;
		break;
	case ActionsClass::Play_Pause_Media:
		obsControlFunction = play_pause_media_source;
		break;
	case ActionsClass::Studio_Mode:
		obsControlFunction = toggle_studio_mode;
		break;
	case ActionsClass::Reset_Stats:
		obsControlFunction = reset_stats;
		break;
	case ActionsClass::Restart_Media:
		obsControlFunction = restart_media;
		break;
	case ActionsClass::Stop_Media:
		obsControlFunction = stop_media;
		break;
	case ActionsClass::Previous_Media:
		obsControlFunction = prev_media;
		break;
	case ActionsClass::Next_Media:
		obsControlFunction = next_media;
		break;
	case ActionsClass::Toggle_Source_Visibility:
		obsControlFunction = ToggleSourceVisibility;
		break;
	case ActionsClass::Take_Screenshot:
		obsControlFunction = TakeScreenshot;
		break;
	case ActionsClass::Disable_Preview:
		obsControlFunction = DisablePreview;
		break;
	case ActionsClass::Enable_Preview:
		obsControlFunction = EnablePreview;
		break;
	case ActionsClass::Poke_filter:
		obsControlFunction = make_opacity_filter;

	default:
		blog(LOG_DEBUG, "Action Does not exist");
		break;
	};
}
