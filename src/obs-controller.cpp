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
#include "obs-controller.h"
#include "macro-helpers.h"
////////////////////
// BUTTON ACTIONS //
////////////////////
/*
 * Sets the currently active scene
 */
void OBSController::SetCurrentScene(MidiHook *hook, int midiVal)
{
	OBSSourceAutoRelease source = obs_get_source_by_name(hook->scene.toUtf8());
	obs_frontend_set_current_scene(source);
}
/**
 * Sets the scene in preview. Must be in Studio mode or will throw error
 */
void OBSController::SetPreviewScene(MidiHook *hook, int midiVal)
{
	if (!obs_frontend_preview_program_mode_active()) {
		blog(LOG_INFO, "Can Not Set Preview scene -- studio mode not enabled");
	}
	OBSScene scene = Utils::GetSceneFromNameOrCurrent(hook->scene);
	if (!scene) {
		blog(LOG_DEBUG, "specified scene doesn't exist");
	}
	obs_source_t *source = obs_scene_get_source(scene);
	obs_frontend_set_current_preview_scene(source);
}
void OBSController::DisablePreview(MidiHook *hook, int midiVal)
{
	obs_queue_task(
		OBS_TASK_UI,
		[](void *param) {
			if (obs_frontend_preview_enabled()) {
				obs_frontend_set_preview_enabled(false);
			}
			(void)param;
		},
		nullptr, true);
}
void OBSController::EnablePreview(MidiHook *hook, int midiVal){
	obs_queue_task(
		OBS_TASK_UI,
		[](void *param) {
			obs_frontend_set_preview_enabled(true);
			(void)param;
		},
		nullptr, true);
}
/**
 * Change the active scene collection.
 */
void OBSController::SetCurrentSceneCollection(MidiHook *hook, int midiVal)
{
	// TODO : Check if specified profile exists and if changing is allowed
	obs_frontend_set_current_scene_collection(hook->scene_collection.toUtf8());
}
/**
 * Reset a scene item.
 */
void OBSController::ResetSceneItem(MidiHook *hook, int midiVal)
{
	OBSScene scene = Utils::GetSceneFromNameOrCurrent(hook->scene);
	if (!scene) {
		throw("requested scene doesn't exist");
	}
	OBSSceneItemAutoRelease sceneItem = Utils::GetSceneItemFromName(scene, hook->source);
	if (!sceneItem) {
		throw("specified scene item doesn't exist");
	}
	OBSSourceAutoRelease sceneItemSource = obs_sceneitem_get_source(sceneItem);
	OBSDataAutoRelease settings = obs_source_get_settings(sceneItemSource);

	obs_source_update(sceneItemSource, settings);
}
/**
 * Transitions the currently previewed scene to the main output.
 */
/**
 * Transitions the currently previewed scene to the main output using specified transition.
 * transitionDuration is optional. (milliseconds)
 */
void OBSController::TransitionToProgram(MidiHook *hook, int midiVal)
{
	if (state::transitioning)
		return;
	state()._CurrentTransitionDuration = obs_frontend_get_transition_duration();
	obs_source_t *transition = obs_frontend_get_current_transition();
	QString scenename;
	/**
	 * If Transition from hook is not Current Transition, and if it is not an empty Value, then set current transition
	 */
	if ((hook->transition != "Current Transition") && !hook->transition.isEmpty() && !hook->transition.isNull()) {
		Utils::SetTransitionByName(hook->transition);
		state()._TransitionWasCalled = true;
	}
	if ((hook->scene != "Preview Scene") && !hook->scene.isEmpty() && !hook->scene.isNull()) {
		state()._TransitionWasCalled = true;
	}
	if (hook->scene == "Preview Scene") {
		obs_source_t *source = obs_frontend_get_current_scene();
		hook->scene = QString(obs_source_get_name(source));
		state()._TransitionWasCalled = true;
	}
	if (hook->int_override && *hook->int_override > 0) {
		obs_frontend_set_transition_duration(*hook->int_override);
		state()._TransitionWasCalled = true;
	}
	(obs_frontend_preview_program_mode_active()) ? obs_frontend_preview_program_trigger_transition() : SetCurrentScene(hook, midiVal);

	state()._CurrentTransition = QString(obs_source_get_name(transition));

	obs_source_release(transition);
}
/**
 * Set the active transition.
 */
void OBSController::SetCurrentTransition(MidiHook *hook, int midiVal)
{
	Utils::SetTransitionByName(hook->transition);
}
/**
 * Set the duration of the currently active transition
 */
void OBSController::SetTransitionDuration(MidiHook *hook, int midiVal)
{
	obs_frontend_set_transition_duration(*hook->duration);
}
void OBSController::SetSourceVisibility(MidiHook *hook, int midiVal)
{
	obs_sceneitem_set_visible(Utils::GetSceneItemFromName(Utils::GetSceneFromNameOrCurrent(hook->scene), hook->source), midiVal);
}
/**
 *
 * Toggles the source's visibility
 * seems to stop audio from playing as well
 *
 */
void OBSController::ToggleSourceVisibility(MidiHook *hook, int midiVal)
{
	auto scene = Utils::GetSceneItemFromName(Utils::GetSceneFromNameOrCurrent(hook->scene), hook->source);
	if (obs_sceneitem_visible(scene)) {
		obs_sceneitem_set_visible(scene, false);
	} else {
		obs_sceneitem_set_visible(scene, true);
	}
}
/**
 * Inverts the mute status of a specified source.
 */
void OBSController::ToggleMute(MidiHook *hook, int midiVal)
{
	if (hook->audio_source.isEmpty()) {
		throw("sourceName is empty");
	}
	obs_source *source = obs_get_source_by_name(hook->audio_source.toStdString().c_str());
	if (!source) {
		throw("sourceName not found");
	}
	obs_source_set_muted(source, !obs_source_muted(source));
}
/**
 * Sets the mute status of a specified source.
 */
void OBSController::SetMute(MidiHook *hook, int midiVal)
{
	if (hook->source.isEmpty()) {
		throw("sourceName is empty");
	}
	OBSSourceAutoRelease source = obs_get_source_by_name(hook->source.toUtf8());
	if (!source) {
		throw("specified source doesn't exist");
	}
	obs_source_set_muted(source, midiVal);
}
/**
 * Toggle streaming on or off.
 */
void OBSController::StartStopStreaming(MidiHook *hook, int midiVal)
{
	if (obs_frontend_streaming_active())
		obs_frontend_streaming_stop();
	else
		obs_frontend_streaming_start();
}
/**
 * Start streaming.
 */
void OBSController::StartStreaming(MidiHook *hook, int midiVal)
{
	if (!obs_frontend_streaming_active()) {
		obs_frontend_streaming_start();
	}
}
/**
 * Stop streaming.
 */
void OBSController::StopStreaming(MidiHook *hook, int midiVal)
{
	if (obs_frontend_streaming_active()) {
		obs_frontend_streaming_stop();
	}
}
/**
 * Toggle recording on or off.
 */
void OBSController::StartStopRecording(MidiHook *hook, int midiVal)
{
	(obs_frontend_recording_active() ? obs_frontend_recording_stop() : obs_frontend_recording_start());
}
/**
 * Start recording.
 */
void OBSController::StartRecording(MidiHook *hook, int midiVal)
{
	if (!obs_frontend_recording_active()) {
		obs_frontend_recording_start();
	}
}
/**
 * Stop recording.
 */
void OBSController::StopRecording(MidiHook *hook, int midiVal)
{
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_stop();
	}
}
/**
 * Pause the current recording.
 */
void OBSController::PauseRecording(MidiHook *hook, int midiVal)
{
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_pause(true);
	}
}
/**
 * Resume/unpause the current recording (if paused).
 */
void OBSController::ResumeRecording(MidiHook *hook, int midiVal)
{
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_pause(false);
	}
}
/**
 * Toggle the Replay Buffer on/off.
 */
void OBSController::StartStopReplayBuffer(MidiHook *hook, int midiVal)
{
	if (!Utils::ReplayBufferEnabled()) {
		Utils::alert_popup("replay buffer disabled in settings");
		return;
	}
	if (obs_frontend_replay_buffer_active()) {
		obs_frontend_replay_buffer_stop();
	} else {
		Utils::StartReplayBuffer();
	}
}
/**
 * Start recording into the Replay Buffer.
 * Will throw an error if "Save Replay Buffer" hotkey is not set in OBS' settings.
 * Setting this hotkey is mandatory, even when triggering saves only
 * through obs-midi.
 */
void OBSController::StartReplayBuffer(MidiHook *hook, int midiVal)
{
	if (!Utils::ReplayBufferEnabled()) {
		Utils::alert_popup("replay buffer disabled in settings");
		return;
	}
	if (!obs_frontend_replay_buffer_active()) {
		Utils::StartReplayBuffer();
	}
}
/**
 * Stop recording into the Replay Buffer.
 */
void OBSController::StopReplayBuffer(MidiHook *hook, int midiVal)
{
	if (!Utils::ReplayBufferEnabled()) {
		Utils::alert_popup("replay buffer disabled in settings");
		return;
	}
	if (obs_frontend_replay_buffer_active()) {
		obs_frontend_replay_buffer_stop();
	}
}
/**
 * Flush and save the contents of the Replay Buffer to disk. This is
 * basically the same as triggering the "Save Replay Buffer" hotkey.
 * Will return an `error` if the Replay Buffer is not active.
 */
void OBSController::SaveReplayBuffer(MidiHook *hook, int midiVal)
{
	if (!Utils::ReplayBufferEnabled()) {
		Utils::alert_popup("replay buffer disabled in settings");
		return;
	}
	if (!obs_frontend_replay_buffer_active()) {
		Utils::alert_popup("replay buffer not active");
		return;
	}
	OBSOutputAutoRelease replayOutput = obs_frontend_get_replay_buffer_output();
	calldata_t cd = {0};
	proc_handler_t *ph = obs_output_get_proc_handler(replayOutput);
	proc_handler_call(ph, "save", &cd);
	calldata_free(&cd);
}
void OBSController::SetCurrentProfile(MidiHook *hook, int midiVal)
{
	if (hook->profile.isEmpty()) {
		throw("profile name is empty");
	}
	// TODO : check if profile exists
	obs_frontend_set_current_profile(hook->profile.toUtf8());
}
void OBSController::SetTextGDIPlusText(MidiHook *hook, int midiVal) {}
void OBSController::SetBrowserSourceURL(MidiHook *hook, int midiVal)
{
	OBSSourceAutoRelease source = obs_get_source_by_name(hook->source.toUtf8());
	QString sourceId = obs_source_get_id(source);
	if (sourceId != "browser_source" && sourceId != "linuxbrowser-source") {
		return blog(LOG_DEBUG, "Not a browser Source");
	}
	OBSDataAutoRelease settings = obs_source_get_settings(source);
	obs_data_set_string(settings, "url", hook->string_override.toUtf8());
	obs_source_update(source, settings);
}
void OBSController::ReloadBrowserSource(MidiHook *hook, int midiVal)
{
	OBSSourceAutoRelease source = obs_get_source_by_name(hook->source.toUtf8());
	obs_properties_t *sourceProperties = obs_source_properties(source);
	obs_property_t *property = obs_properties_get(sourceProperties, "refreshnocache");
	obs_property_button_clicked(property, source); // This returns a boolean but we ignore it because the browser plugin always returns `false`.
	obs_properties_destroy(sourceProperties);
}
void OBSController::TakeScreenshot(MidiHook *hook, int midiVal)
{
	obs_frontend_take_screenshot();
}
void OBSController::TakeSourceScreenshot(MidiHook *hook, int midiVal)
{
	OBSSourceAutoRelease source = obs_get_source_by_name(hook->scene.toUtf8());
	obs_frontend_take_source_screenshot(source);
}
void OBSController::EnableSourceFilter(MidiHook *hook, int midiVal)
{
	OBSSourceAutoRelease source = obs_get_source_by_name(hook->source.toUtf8());
	OBSSourceAutoRelease filter = obs_source_get_filter_by_name(source, hook->filter.toUtf8());
	obs_source_set_enabled(filter, true);
}
void OBSController::DisableSourceFilter(MidiHook *hook, int midiVal)
{
	OBSSourceAutoRelease source = obs_get_source_by_name(hook->source.toUtf8());
	OBSSourceAutoRelease filter = obs_source_get_filter_by_name(source, hook->filter.toUtf8());
	obs_source_set_enabled(filter, false);
}
void OBSController::ToggleSourceFilter(MidiHook *hook, int midiVal)
{
	OBSSourceAutoRelease source = obs_get_source_by_name(hook->source.toUtf8());
	OBSSourceAutoRelease filter = obs_source_get_filter_by_name(source, hook->filter.toUtf8());
	if (obs_source_enabled(filter)) {
		obs_source_set_enabled(filter, false);
	} else {
		obs_source_set_enabled(filter, true);
	}
}
////////////////
// CC ACTIONS //
////////////////
void OBSController::SetVolume(MidiHook *hook, int midiVal)
{
	OBSSourceAutoRelease obsSource = obs_get_source_by_name(hook->audio_source.toUtf8());
	obs_source_set_volume(obsSource, pow(Utils::mapper(midiVal), 3.0));
}
/**
 * Set the audio sync offset of a specified source.
 */
void OBSController::SetSyncOffset(MidiHook *hook, int midiVal)
{
	OBSSourceAutoRelease source = obs_get_source_by_name(hook->source.toUtf8());
	obs_source_set_sync_offset(source, midiVal);
}
void OBSController::SetSourcePosition(MidiHook *hook, int midiVal) {}
void OBSController::SetSourceRotation(MidiHook *hook, int midiVal)
{
	obs_scene_t *scene = Utils::GetSceneFromNameOrCurrent(hook->scene);
	obs_sceneitem_t *item = Utils::GetSceneItemFromName(scene, hook->source);
	uint32_t current = obs_sceneitem_get_alignment(item);
	obs_sceneitem_set_alignment(item, OBS_ALIGN_CENTER);
	float rotation = Utils::map_to_range((hook->range_min) ? *hook->range_min : 0, (hook->range_max) ? *hook->range_max : 360, midiVal);
	obs_sceneitem_set_rot(item, rotation);
}
void OBSController::SetSourceScale(MidiHook *hook, int midiVal)
{
	obs_scene_t *scene = Utils::GetSceneFromNameOrCurrent(hook->scene);
	obs_sceneitem_t *item = Utils::GetSceneItemFromName(scene, hook->source);
	uint32_t current = obs_sceneitem_get_alignment(item);
	obs_sceneitem_set_alignment(item, OBS_ALIGN_CENTER);
	obs_sceneitem_set_bounds_type(item, obs_bounds_type::OBS_BOUNDS_NONE);
	vec2 *scale= new vec2();
	vec2_set(scale, Utils::map_to_range(0, (hook->range_min) ? *hook->range_min : 1, midiVal),
		 Utils::map_to_range(0, (hook->range_max) ? *hook->range_max : 1, midiVal));
	obs_sceneitem_set_scale(item, scale);
	delete (scale);
}
void OBSController::SetGainFilter(MidiHook *hook, int midiVal) {}
void OBSController::SetOpacity(MidiHook *hook, int midiVal) {}
void OBSController::move_t_bar(MidiHook *hook, int midiVal)
{
	if (obs_frontend_preview_program_mode_active()) {
		obs_frontend_set_tbar_position(Utils::t_bar_mapper(midiVal));
		obs_frontend_release_tbar();
	}
}
void OBSController::play_pause_media_source(MidiHook *hook, int midiVal)
{
	OBSSourceAutoRelease source = obs_get_source_by_name(hook->media_source.toUtf8());
	switch (obs_source_media_get_state(source)) {
	case obs_media_state::OBS_MEDIA_STATE_PAUSED:
		obs_source_media_play_pause(source, false);
		break;
	case obs_media_state::OBS_MEDIA_STATE_PLAYING:
		obs_source_media_play_pause(source, true);
		break;
	case obs_media_state::OBS_MEDIA_STATE_ENDED:
		obs_source_media_restart(source);
		break;
	case OBS_MEDIA_STATE_NONE:
		break;
	case OBS_MEDIA_STATE_OPENING:
		break;
	case OBS_MEDIA_STATE_BUFFERING:
		break;
	case OBS_MEDIA_STATE_STOPPED:
		break;
	case OBS_MEDIA_STATE_ERROR:
		break;
	}
}
// TODO:: Fix this
void OBSController::toggle_studio_mode(MidiHook *hook, int midiVal)
{
	obs_queue_task(
		OBS_TASK_UI,
		[](void *param) {
			obs_frontend_set_preview_program_mode(!obs_frontend_preview_program_mode_active());

			UNUSED_PARAMETER(param);
		},
		nullptr, true);
}
void OBSController::reset_stats(MidiHook *hook, int midiVal) {}
void OBSController::restart_media(MidiHook *hook, int midiVal)
{
	OBSSourceAutoRelease source = obs_get_source_by_name(hook->media_source.toUtf8());
	obs_source_media_restart(source);
}
void OBSController::play_media(MidiHook *hook, int midiVal)
{
	OBSSourceAutoRelease source = obs_get_source_by_name(hook->media_source.toUtf8());
	obs_source_media_play_pause(source, false);
}
void OBSController::stop_media(MidiHook *hook, int midiVal)
{
	OBSSourceAutoRelease source = obs_get_source_by_name(hook->media_source.toUtf8());
	obs_source_media_stop(source);
}
void OBSController::next_media(MidiHook *hook, int midiVal)
{
	OBSSourceAutoRelease source = obs_get_source_by_name(hook->media_source.toUtf8());
	obs_source_media_next(source);
}
void OBSController::prev_media(MidiHook *hook, int midiVal)
{
	OBSSourceAutoRelease source = obs_get_source_by_name(hook->media_source.toUtf8());
	obs_source_media_previous(source);
}
