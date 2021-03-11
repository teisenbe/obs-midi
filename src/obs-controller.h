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
#include "Midi_hook.h"

namespace OBSController {

	/**
	 * Actions
	 */
	void SetCurrentScene(MidiHook *hook, int midiVal);
	void SetPreviewScene(MidiHook *hook, int midiVal);
	void DisablePreview(MidiHook *hook, int midiVal);
	void EnablePreview(MidiHook *hook, int midiVal);
	void SetCurrentSceneCollection(MidiHook *hook, int midiVal);
	void ResetSceneItem(MidiHook *hook, int midiVal);
	void TransitionToProgram(MidiHook *hook, int midiVal);
	void SetCurrentTransition(MidiHook *hook, int midiVal);
	void SetTransitionDuration(MidiHook *hook, int midiVal);  // can also be used with cc
	void SetSourceVisibility(MidiHook *hook, int midiVal);    // doesn't exist??
	void ToggleSourceVisibility(MidiHook *hook, int midiVal); // doesn't exist?
	void ToggleMute(MidiHook *hook, int midiVal);
	void SetMute(MidiHook *hook, int midiVal);
	void StartStopStreaming(MidiHook *hook, int midiVal);
	void StartStreaming(MidiHook *hook, int midiVal);
	void StopStreaming(MidiHook *hook, int midiVal);
	void StartStopRecording(MidiHook *hook, int midiVal);
	void StartRecording(MidiHook *hook, int midiVal);
	void StopRecording(MidiHook *hook, int midiVal);
	void PauseRecording(MidiHook *hook, int midiVal);
	void ResumeRecording(MidiHook *hook, int midiVal);
	void StartStopReplayBuffer(MidiHook *hook, int midiVal);
	void StartReplayBuffer(MidiHook *hook, int midiVal);
	void StopReplayBuffer(MidiHook *hook, int midiVal);
	void SaveReplayBuffer(MidiHook *hook, int midiVal);
	void SetCurrentProfile(MidiHook *hook, int midiVal);
	void SetTextGDIPlusText(MidiHook *hook, int midiVal);
	void SetBrowserSourceURL(MidiHook *hook, int midiVal);
	void ReloadBrowserSource(MidiHook *hook, int midiVal);
	void TakeScreenshot(MidiHook *hook, int midiVal);
	void TakeSourceScreenshot(MidiHook *hook, int midiVal);
	void EnableSourceFilter(MidiHook *hook, int midiVal);
	void DisableSourceFilter(MidiHook *hook, int midiVal);
	void ToggleSourceFilter(MidiHook *hook, int midiVal);

	// CC ACTIONS
	void SetVolume(MidiHook *hook, int midiVal);
	void SetSyncOffset(MidiHook *hook, int midiVal);
	void SetSourcePosition(MidiHook *hook, int midiVal);
	void SetSourceRotation(MidiHook *hook, int midiVal);
	void SetSourceScale(MidiHook *hook, int midiVal);
	void SetGainFilter(MidiHook *hook, int midiVal);
	void SetOpacity(MidiHook *hook, int midiVal);
	void move_t_bar(MidiHook *hook, int midiVal);
	void play_pause_media_source(MidiHook *hook, int midiVal);
	void toggle_studio_mode(MidiHook *hook, int midiVal);
	void reset_stats(MidiHook *hook, int midiVal);
	void restart_media(MidiHook *hook, int midiVal);
	void stop_media(MidiHook *hook, int midiVal);
	void play_media(MidiHook *hook, int midiVal);
	void next_media(MidiHook *hook, int midiVal);
	void prev_media(MidiHook *hook, int midiVal);
};
