
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
	MidiMessage *message = new MidiMessage();
	message->channel = this->channel;
	message->message_type = this->message_type;
	message->NORC = this->norc;
	message->value = *this->value;
	return std::move(message);
}
void MidiHook::EXE(int MidiVal)
{
	(*obsControlFunction)(this, MidiVal);
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
	case ActionsClass::Actions::Set_Current_Scene:
		obsControlFunction = OBSController::SetCurrentScene;
		break;
	case ActionsClass::Actions::Reset_Scene_Item:
		obsControlFunction = OBSController::ResetSceneItem;
		break;
	case ActionsClass::Actions::Toggle_Mute:
		obsControlFunction = OBSController::ToggleMute;
		break;
	case ActionsClass::Actions::Do_Transition:
		obsControlFunction = OBSController::TransitionToProgram;
		break;
	case ActionsClass::Actions::Set_Current_Transition:
		obsControlFunction = OBSController::SetCurrentTransition;
		break;
	case ActionsClass::Actions::Set_Mute:
		obsControlFunction = OBSController::SetMute;
		break;
	case ActionsClass::Actions::Toggle_Start_Stop_Streaming:
		obsControlFunction = OBSController::StartStopStreaming;
		break;
	case ActionsClass::Actions::Set_Preview_Scene:
		obsControlFunction = OBSController::SetPreviewScene;
		break;
	case ActionsClass::Actions::Set_Current_Scene_Collection:
		obsControlFunction = OBSController::SetCurrentSceneCollection;
		break;
	case ActionsClass::Actions::Set_Transition_Duration:
		obsControlFunction = OBSController::SetTransitionDuration;
		break;
	case ActionsClass::Actions::Start_Streaming:
		obsControlFunction = OBSController::StartStreaming;
		break;
	case ActionsClass::Actions::Stop_Streaming:
		obsControlFunction = OBSController::StopStreaming;
		break;
	case ActionsClass::Actions::Start_Recording:
		obsControlFunction = OBSController::StartRecording;
		break;
	case ActionsClass::Actions::Stop_Recording:
		obsControlFunction = OBSController::StopRecording;
		break;
	case ActionsClass::Actions::Start_Replay_Buffer:
		obsControlFunction = OBSController::StartReplayBuffer;
		break;
	case ActionsClass::Actions::Stop_Replay_Buffer:
		obsControlFunction = OBSController::StopReplayBuffer;
		break;
	case ActionsClass::Actions::Set_Volume:
		obsControlFunction = OBSController::SetVolume;
		break;
	case ActionsClass::Actions::Take_Source_Screenshot:
		obsControlFunction = OBSController::TakeSourceScreenshot;
		break;
	case ActionsClass::Actions::Pause_Recording:
		obsControlFunction = OBSController::PauseRecording;
		break;
	case ActionsClass::Actions::Enable_Source_Filter:
		obsControlFunction = OBSController::EnableSourceFilter;
		break;
	case ActionsClass::Actions::Disable_Source_Filter:
		obsControlFunction = OBSController::DisableSourceFilter;
		break;
	case ActionsClass::Actions::Toggle_Start_Stop_Recording:
		obsControlFunction = OBSController::StartStopRecording;
		break;
	case ActionsClass::Actions::Toggle_Start_Stop_Replay_Buffer:
		obsControlFunction = OBSController::StartStopReplayBuffer;
		break;
	case ActionsClass::Actions::Resume_Recording:
		obsControlFunction = OBSController::ResumeRecording;
		break;
	case ActionsClass::Actions::Save_Replay_Buffer:
		obsControlFunction = OBSController::SaveReplayBuffer;
		break;
	case ActionsClass::Actions::Set_Current_Profile:
		obsControlFunction = OBSController::SetCurrentProfile;
		break;
	case ActionsClass::Actions::Toggle_Source_Filter:
		obsControlFunction = OBSController::ToggleSourceFilter;
		break;
	case ActionsClass::Actions::Set_Text_GDIPlus_Text:
		obsControlFunction = OBSController::SetTextGDIPlusText;
		break;
	case ActionsClass::Actions::Set_Browser_Source_URL:
		obsControlFunction = OBSController::SetBrowserSourceURL;
		break;
	case ActionsClass::Actions::Reload_Browser_Source:
		obsControlFunction = OBSController::ReloadBrowserSource;
		break;
	case ActionsClass::Actions::Set_Sync_Offset:
		obsControlFunction = OBSController::SetSyncOffset;
		break;
	case ActionsClass::Actions::Set_Source_Rotation:
		obsControlFunction = OBSController::SetSourceRotation;
		break;
	case ActionsClass::Actions::Set_Source_Position:
		obsControlFunction = OBSController::SetSourcePosition;
		break;
	case ActionsClass::Actions::Set_Gain_Filter:
		obsControlFunction = OBSController::SetGainFilter;
		break;
	case ActionsClass::Actions::Set_Opacity:
		obsControlFunction = OBSController::SetOpacity;
		break;
	case ActionsClass::Actions::Set_Source_Scale:
		obsControlFunction = OBSController::SetSourceScale;
		break;
	case ActionsClass::Actions::Move_T_Bar:
		obsControlFunction = OBSController::move_t_bar;
		break;
	case ActionsClass::Actions::Play_Pause_Media:
		obsControlFunction = OBSController::play_pause_media_source;
		break;
	case ActionsClass::Actions::Studio_Mode:
		obsControlFunction = OBSController::toggle_studio_mode;
		break;
	case ActionsClass::Actions::Reset_Stats:
		obsControlFunction = OBSController::reset_stats;
		break;
	case ActionsClass::Actions::Restart_Media:
		obsControlFunction = OBSController::restart_media;
		break;
	case ActionsClass::Actions::Stop_Media:
		obsControlFunction = OBSController::stop_media;
		break;
	case ActionsClass::Actions::Previous_Media:
		obsControlFunction = OBSController::prev_media;
		break;
	case ActionsClass::Actions::Next_Media:
		obsControlFunction = OBSController::next_media;
		break;
	case ActionsClass::Actions::Toggle_Source_Visibility:
		obsControlFunction = OBSController::ToggleSourceVisibility;
		break;
	case ActionsClass::Actions::Take_Screenshot:
		obsControlFunction = OBSController::TakeScreenshot;
		break;
	case ActionsClass::Actions::Disable_Preview:
		obsControlFunction = OBSController::DisablePreview;
		break;
	case ActionsClass::Actions::Enable_Preview:
		obsControlFunction = OBSController::EnablePreview;
		break;
	default:
		blog(LOG_DEBUG, "Action Does not exist");
		break;
	};
}
