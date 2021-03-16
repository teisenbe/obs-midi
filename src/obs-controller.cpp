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
#include <thread>
#ifdef _WIN32
#include <Windows.h>
#define tsleep _sleep
#else
#include <unistd.h>
#define tsleep(x) usleep(x * 1000)
#endif
#include <util/platform.h>
////////////////////
// BUTTON ACTIONS //
////////////////////
/*
 * Sets the currently active scene
 */
void SetCurrentScene(MidiHook *hook)
{
	const OBSSourceAutoRelease source = obs_get_source_by_name(hook->scene.toUtf8());
	obs_frontend_set_current_scene(source);
}
/**
 * Sets the scene in preview. Must be in Studio mode or will throw error
 */
void SetPreviewScene(MidiHook *hook)
{
	if (!obs_frontend_preview_program_mode_active()) {
		blog(LOG_INFO, "Can Not Set Preview scene -- studio mode not enabled");
	}
	const OBSScene scene = Utils::GetSceneFromNameOrCurrent(hook->scene);
	if (!scene) {
		blog(LOG_DEBUG, "specified scene doesn't exist");
	}
	obs_source_t *source = obs_scene_get_source(scene);
	obs_frontend_set_current_preview_scene(source);
}
void DisablePreview(MidiHook *hook)
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
void EnablePreview(MidiHook *hook)
{
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
void SetCurrentSceneCollection(MidiHook *hook)
{
	// TODO : Check if specified profile exists and if changing is allowed
	obs_frontend_set_current_scene_collection(hook->scene_collection.toUtf8());
}
/**
 * Reset a scene item.
 */
void ResetSceneItem(MidiHook *hook)
{
	const OBSScene scene = Utils::GetSceneFromNameOrCurrent(hook->scene);
	if (!scene) {
		throw("requested scene doesn't exist");
	}
	const OBSSceneItemAutoRelease sceneItem = Utils::GetSceneItemFromName(scene, hook->source);
	if (!sceneItem) {
		throw("specified scene item doesn't exist");
	}
	const OBSSourceAutoRelease sceneItemSource = obs_sceneitem_get_source(sceneItem);
	const OBSDataAutoRelease settings = obs_source_get_settings(sceneItemSource);

	obs_source_update(sceneItemSource, settings);
}
/**
 * Transitions the currently previewed scene to the main output.
 */
/**
 * Transitions the currently previewed scene to the main output using specified transition.
 * transitionDuration is optional. (milliseconds)
 */
void TransitionToProgram(MidiHook *hook)
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
	(obs_frontend_preview_program_mode_active()) ? obs_frontend_preview_program_trigger_transition() : SetCurrentScene(hook);

	state()._CurrentTransition = QString(obs_source_get_name(transition));

	obs_source_release(transition);
}
/**
 * Set the active transition.
 */
void SetCurrentTransition(MidiHook *hook)
{
	Utils::SetTransitionByName(hook->transition);
}
/**
 * Set the duration of the currently active transition
 */
void SetTransitionDuration(MidiHook *hook)
{
	obs_frontend_set_transition_duration(*hook->duration);
}
void SetSourceVisibility(MidiHook *hook)
{
	obs_sceneitem_set_visible(Utils::GetSceneItemFromName(Utils::GetSceneFromNameOrCurrent(hook->scene), hook->source), *hook->value);
}
/**
 *
 * Toggles the source's visibility
 * seems to stop audio from playing as well
 *
 */
void ToggleSourceVisibility(MidiHook *hook)
{
	const auto scene = Utils::GetSceneItemFromName(Utils::GetSceneFromNameOrCurrent(hook->scene), hook->source);
	if (obs_sceneitem_visible(scene)) {
		obs_sceneitem_set_visible(scene, false);
	} else {
		obs_sceneitem_set_visible(scene, true);
	}
}
/**
 * Inverts the mute status of a specified source.
 */
void ToggleMute(MidiHook *hook)
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
void SetMute(MidiHook *hook)
{
	if (hook->source.isEmpty()) {
		throw("sourceName is empty");
	}
	const OBSSourceAutoRelease source = obs_get_source_by_name(hook->source.toUtf8());
	if (!source) {
		throw("specified source doesn't exist");
	}
	obs_source_set_muted(source, *hook->value);
}
/**
 * Toggle streaming on or off.
 */
void StartStopStreaming(MidiHook *hook)
{
	if (obs_frontend_streaming_active())
		obs_frontend_streaming_stop();
	else
		obs_frontend_streaming_start();
}
/**
 * Start streaming.
 */
void StartStreaming(MidiHook *hook)
{
	if (!obs_frontend_streaming_active()) {
		obs_frontend_streaming_start();
	}
}
/**
 * Stop streaming.
 */
void StopStreaming(MidiHook *hook)
{
	if (obs_frontend_streaming_active()) {
		obs_frontend_streaming_stop();
	}
}
/**
 * Toggle recording on or off.
 */
void StartStopRecording(MidiHook *hook)
{
	(obs_frontend_recording_active() ? obs_frontend_recording_stop() : obs_frontend_recording_start());
}
/**
 * Start recording.
 */
void StartRecording(MidiHook *hook)
{
	if (!obs_frontend_recording_active()) {
		obs_frontend_recording_start();
	}
}
/**
 * Stop recording.
 */
void StopRecording(MidiHook *hook)
{
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_stop();
	}
}
/**
 * Pause the current recording.
 */
void PauseRecording(MidiHook *hook)
{
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_pause(true);
	}
}
/**
 * Resume/unpause the current recording (if paused).
 */
void ResumeRecording(MidiHook *hook)
{
	if (obs_frontend_recording_active()) {
		obs_frontend_recording_pause(false);
	}
}
/**
 * Toggle the Replay Buffer on/off.
 */
void StartStopReplayBuffer(MidiHook *hook)
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
void StartReplayBuffer(MidiHook *hook)
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
void StopReplayBuffer(MidiHook *hook)
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
void SaveReplayBuffer(MidiHook *hook)
{
	if (!Utils::ReplayBufferEnabled()) {
		Utils::alert_popup("replay buffer disabled in settings");
		return;
	}
	if (!obs_frontend_replay_buffer_active()) {
		Utils::alert_popup("replay buffer not active");
		return;
	}
	const OBSOutputAutoRelease replayOutput = obs_frontend_get_replay_buffer_output();
	calldata_t cd = {0};
	proc_handler_t *ph = obs_output_get_proc_handler(replayOutput);
	proc_handler_call(ph, "save", &cd);
	calldata_free(&cd);
}
void SetCurrentProfile(MidiHook *hook)
{
	if (hook->profile.isEmpty()) {
		throw("profile name is empty");
	}
	// TODO : check if profile exists
	obs_frontend_set_current_profile(hook->profile.toUtf8());
}
void SetTextGDIPlusText(MidiHook *hook) {}
void SetBrowserSourceURL(MidiHook *hook)
{
	const OBSSourceAutoRelease source = obs_get_source_by_name(hook->source.toUtf8());
	const QString sourceId = obs_source_get_id(source);
	if (sourceId != "browser_source" && sourceId != "linuxbrowser-source") {
		return blog(LOG_DEBUG, "Not a browser Source");
	}
	const OBSDataAutoRelease settings = obs_source_get_settings(source);
	obs_data_set_string(settings, "url", hook->string_override.toUtf8());
	obs_source_update(source, settings);
}
void ReloadBrowserSource(MidiHook *hook)
{
	const OBSSourceAutoRelease source = obs_get_source_by_name(hook->source.toUtf8());
	obs_properties_t *sourceProperties = obs_source_properties(source);
	obs_property_t *property = obs_properties_get(sourceProperties, "refreshnocache");
	obs_property_button_clicked(property, source); // This returns a boolean but we ignore it because the browser plugin always returns `false`.
	obs_properties_destroy(sourceProperties);
}
void TakeScreenshot(MidiHook *hook)
{
	obs_frontend_take_screenshot();
}
void TakeSourceScreenshot(MidiHook *hook)
{
	const OBSSourceAutoRelease source = obs_get_source_by_name(hook->scene.toUtf8());
	obs_frontend_take_source_screenshot(source);
}
void EnableSourceFilter(MidiHook *hook)
{
	const OBSSourceAutoRelease source = obs_get_source_by_name(hook->source.toUtf8());
	const OBSSourceAutoRelease filter = obs_source_get_filter_by_name(source, hook->filter.toUtf8());
	obs_source_set_enabled(filter, true);
}
void DisableSourceFilter(MidiHook *hook)
{
	const OBSSourceAutoRelease source = obs_get_source_by_name(hook->source.toUtf8());
	const OBSSourceAutoRelease filter = obs_source_get_filter_by_name(source, hook->filter.toUtf8());
	obs_source_set_enabled(filter, false);
}
void ToggleSourceFilter(MidiHook *hook)
{
	const OBSSourceAutoRelease source = obs_get_source_by_name(hook->source.toUtf8());
	const OBSSourceAutoRelease filter = obs_source_get_filter_by_name(source, hook->filter.toUtf8());
	if (obs_source_enabled(filter)) {
		obs_source_set_enabled(filter, false);
	} else {
		obs_source_set_enabled(filter, true);
	}
}
void TriggerHotkeyByName(MidiHook *hook)
{
	obs_hotkey_t *obsHotkey = Utils::get_obs_hotkey_by_name(hook->hotkey);
	if (!obsHotkey) {
		blog(LOG_ERROR, "ERROR: Triggered hotkey <%s> was not found", hook->hotkey.toStdString().c_str());
		return;
	}
	obs_hotkey_trigger_routed_callback(obs_hotkey_get_id(obsHotkey), true);
}

////////////////
// CC ACTIONS //
////////////////
void SetVolume(MidiHook *hook)
{
	const OBSSourceAutoRelease obsSource = obs_get_source_by_name(hook->audio_source.toUtf8());
	obs_source_set_volume(obsSource, pow(Utils::mapper(*hook->value), 3.0));
}
/**
 * Set the audio sync offset of a specified source.
 */
void SetSyncOffset(MidiHook *hook)
{
	const OBSSourceAutoRelease source = obs_get_source_by_name(hook->source.toUtf8());
	obs_source_set_sync_offset(source, *hook->value);
}
void SetSourcePosition(MidiHook *hook) {}
void SetSourceRotation(MidiHook *hook)
{
	obs_scene_t *scene = Utils::GetSceneFromNameOrCurrent(hook->scene);
	obs_sceneitem_t *item = Utils::GetSceneItemFromName(scene, hook->source);
	uint32_t current = obs_sceneitem_get_alignment(item);
	obs_sceneitem_set_alignment(item, OBS_ALIGN_CENTER);
	const float rotation = Utils::map_to_range((hook->range_min) ? *hook->range_min : 0, (hook->range_max) ? *hook->range_max : 360, *hook->value);
	obs_sceneitem_set_rot(item, rotation);
}
void SetSourceScale(MidiHook *hook)
{
	obs_scene_t *scene = Utils::GetSceneFromNameOrCurrent(hook->scene);
	obs_sceneitem_t *item = Utils::GetSceneItemFromName(scene, hook->source);
	uint32_t current = obs_sceneitem_get_alignment(item);
	obs_sceneitem_set_alignment(item, OBS_ALIGN_CENTER);
	obs_sceneitem_set_bounds_type(item, obs_bounds_type::OBS_BOUNDS_NONE);
	vec2 *scale = new vec2();
	vec2_set(scale, Utils::map_to_range(0, (hook->range_min) ? *hook->range_min : 1, *hook->value),
		 Utils::map_to_range(0, (hook->range_max) ? *hook->range_max : 1, *hook->value));
	obs_sceneitem_set_scale(item, scale);
	delete (scale);
}
void SetGainFilter(MidiHook *hook) {}
void SetOpacity(MidiHook *hook) {}
void move_t_bar(MidiHook *hook)
{
	if (obs_frontend_preview_program_mode_active()) {
		obs_frontend_set_tbar_position(Utils::t_bar_mapper(*hook->value));
		obs_frontend_release_tbar();
	}
}
void play_pause_media_source(MidiHook *hook)
{
	const OBSSourceAutoRelease source = obs_get_source_by_name(hook->media_source.toUtf8());
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
void toggle_studio_mode(MidiHook *hook)
{
	obs_queue_task(
		OBS_TASK_UI,
		[](void *param) {
			obs_frontend_set_preview_program_mode(!obs_frontend_preview_program_mode_active());

			UNUSED_PARAMETER(param);
		},
		nullptr, true);
}
void reset_stats(MidiHook *hook) {}
void restart_media(MidiHook *hook)
{
	const OBSSourceAutoRelease source = obs_get_source_by_name(hook->media_source.toUtf8());
	obs_source_media_restart(source);
}
void play_media(MidiHook *hook)
{
	const OBSSourceAutoRelease source = obs_get_source_by_name(hook->media_source.toUtf8());
	obs_source_media_play_pause(source, false);
}
void stop_media(MidiHook *hook)
{
	const OBSSourceAutoRelease source = obs_get_source_by_name(hook->media_source.toUtf8());
	obs_source_media_stop(source);
}
void next_media(MidiHook *hook)
{
	const OBSSourceAutoRelease source = obs_get_source_by_name(hook->media_source.toUtf8());
	obs_source_media_next(source);
}
void prev_media(MidiHook *hook)
{
	const OBSSourceAutoRelease source = obs_get_source_by_name(hook->media_source.toUtf8());
	obs_source_media_previous(source);
}

float time_to_sleep(float duration)
{
	return duration / 2000;
}

void fade_in_scene_item(MidiHook *hook)
{
	try {
		std::thread th{[=]() {
			obs_data_t *data = obs_data_create();
			obs_data_set_double(data, "opacity", 0);
			OBSSourceAutoRelease filter = obs_source_create_private("color_filter", "ColorFilter", data);

			obs_scene_t *scene = Utils::GetSceneFromNameOrCurrent(hook->scene);
			obs_sceneitem_t *item = Utils::GetSceneItemFromName(scene, hook->source);
			obs_source_t *source = obs_sceneitem_get_source(item);
			float i = 0;
			float tts = time_to_sleep((hook->int_override) ? (float)*hook->int_override : 500.0f);
			obs_source_filter_add(source, filter);
			obs_sceneitem_set_visible(item, true);

			while (i <= 100) {

				obs_data_set_double(data, "opacity", i);
				obs_source_update(filter, data);
				i = i + 0.05f;

				tsleep((int)tts);
			}

			obs_source_filter_remove(source, filter);

			obs_data_release(data);
		}};
		th.detach();
	} catch (std::exception &e) {
		blog(LOG_DEBUG, "Fade error %s", e.what());
	}
}

void fade_out_scene_item(MidiHook *hook)
{
	try {

		std::thread th{[=]() {
			obs_data_t *data = obs_data_create();
			obs_data_set_double(data, "opacity", 100);
			OBSSourceAutoRelease filter = obs_source_create_private("color_filter", "ColorFilter", data);

			obs_scene_t *scene = Utils::GetSceneFromNameOrCurrent(hook->scene);
			obs_sceneitem_t *item = Utils::GetSceneItemFromName(scene, hook->source);
			obs_source_t *source = obs_sceneitem_get_source(item);
			float i = 100;
			float tts = time_to_sleep((hook->int_override) ? (float)*hook->int_override : 500.0f);
			obs_source_filter_add(source, filter);
			while (i >= 0) {
				obs_data_set_double(data, "opacity", i);
				obs_source_update(filter, data);
				i = i - 0.05f;
				tsleep(tts);
			}
			obs_sceneitem_set_visible(item, false);

			obs_source_filter_remove(source, filter);

			obs_data_release(data);
		}};
		th.detach();
	} catch (std::exception &e) {
		blog(LOG_DEBUG, "Fade error %s", e.what());
	}
}
void make_opacity_filter(MidiHook *hook)
{
	obs_scene_t *scene = Utils::GetSceneFromNameOrCurrent(hook->scene);
	obs_sceneitem_t *item = Utils::GetSceneItemFromName(scene, hook->source);
	(obs_sceneitem_visible(item)) ? fade_out_scene_item(hook) : fade_in_scene_item(hook);
}
