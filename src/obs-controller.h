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

/**
 * Actions
 */
void SetCurrentScene(MidiHook *hook);
void SetPreviewScene(MidiHook *hook);
void DisablePreview(MidiHook *hook);
void EnablePreview(MidiHook *hook);
void SetCurrentSceneCollection(MidiHook *hook);
void ResetSceneItem(MidiHook *hook);
void TransitionToProgram(MidiHook *hook);
void SetCurrentTransition(MidiHook *hook);
void SetTransitionDuration(MidiHook *hook);  // can also be used with cc
void SetSourceVisibility(MidiHook *hook);    // doesn't exist??
void ToggleSourceVisibility(MidiHook *hook); // doesn't exist?
void ToggleMute(MidiHook *hook);
void SetMute(MidiHook *hook);
void StartStopStreaming(MidiHook *hook);
void StartStreaming(MidiHook *hook);
void StopStreaming(MidiHook *hook);
void StartStopRecording(MidiHook *hook);
void StartRecording(MidiHook *hook);
void StopRecording(MidiHook *hook);
void PauseRecording(MidiHook *hook);
void ResumeRecording(MidiHook *hook);
void StartStopReplayBuffer(MidiHook *hook);
void StartReplayBuffer(MidiHook *hook);
void StopReplayBuffer(MidiHook *hook);
void SaveReplayBuffer(MidiHook *hook);
void SetCurrentProfile(MidiHook *hook);
void SetTextGDIPlusText(MidiHook *hook);
void SetBrowserSourceURL(MidiHook *hook);
void ReloadBrowserSource(MidiHook *hook);
void TakeScreenshot(MidiHook *hook);
void TakeSourceScreenshot(MidiHook *hook);
void EnableSourceFilter(MidiHook *hook);
void DisableSourceFilter(MidiHook *hook);
void ToggleSourceFilter(MidiHook *hook);

// CC ACTIONS
void SetVolume(MidiHook *hook);
void SetSyncOffset(MidiHook *hook);
void SetSourcePosition(MidiHook *hook);
void SetSourceRotation(MidiHook *hook);
void SetSourceScale(MidiHook *hook);
void SetGainFilter(MidiHook *hook);
void SetOpacity(MidiHook *hook);
void move_t_bar(MidiHook *hook);
void play_pause_media_source(MidiHook *hook);
void toggle_studio_mode(MidiHook *hook);
void reset_stats(MidiHook *hook);
void restart_media(MidiHook *hook);
void stop_media(MidiHook *hook);
void play_media(MidiHook *hook);
void next_media(MidiHook *hook);
void prev_media(MidiHook *hook);
