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
#include <qpropertyanimation.h>
#include <QTime>
#include <QObject>

class Actions : public QObject {
	Q_OBJECT
public:
	Actions(){};
	Actions(MidiHook *_hook);
	virtual void execute(){};
	static Actions *get_action(QString action, MidiHook* h);

protected:
	MidiHook *hook;

private:
	static void make_map();
	inline static QMap<QString, Actions *> _action_map;
};
class SetCurrentScene : public Actions {

public:
	SetCurrentScene(){};
	void execute() override;
};
/**
 * Actions
 */

class SetPreviewScene : public Actions {
public:
	SetPreviewScene(){};
	void execute() override;
};
class DisablePreview : public Actions {
public:
	DisablePreview(){};
	void execute() override;
};
class EnablePreview : public Actions {
public:
	EnablePreview(){};
	void execute() override;
};
class SetCurrentSceneCollection : public Actions {
public:
	SetCurrentSceneCollection(){};
	void execute() override;
};
class ResetSceneItem : public Actions {
public:
	ResetSceneItem(){};
	void execute() override;
};
class TransitionToProgram : public Actions {
public:
	TransitionToProgram(){};
	void execute() override;
};
class SetCurrentTransition : public Actions {
public:
	SetCurrentTransition(){};
	void execute() override;
};
class SetTransitionDuration : public Actions {
public:
	SetTransitionDuration(){};
	void execute() override;
}; // can also be used with cc
class SetSourceVisibility : public Actions {
public:
	SetSourceVisibility(){};
	void execute() override;
}; // doesn't exist??
class ToggleSourceVisibility : public Actions {
public:
	ToggleSourceVisibility(){};
	void execute() override;
}; // doesn't exist?
class ToggleMute : public Actions {
public:
	ToggleMute(){};
	void execute() override;
};
class SetMute : public Actions {
public:
	SetMute(){};
	void execute() override;
};
class StartStopStreaming : public Actions {
public:
	StartStopStreaming(){};
	void execute() override;
};
class StartStreaming : public Actions {
public:
	StartStreaming(){};
	void execute() override;
};
class StopStreaming : public Actions {
public:
	StopStreaming(){};
	void execute() override;
};
class StartStopRecording : public Actions {
public:
	StartStopRecording(){};
	void execute() override;
};
class StartRecording : public Actions {
public:
	StartRecording(){};
	void execute() override;
};
class StopRecording : public Actions {
public:
	StopRecording(){};
	void execute() override;
};
class PauseRecording : public Actions {
public:
	PauseRecording(){};
	void execute() override;
};
class ResumeRecording : public Actions {
public:
	ResumeRecording(){};
	void execute() override;
};
class StartStopReplayBuffer : public Actions {
public:
	StartStopReplayBuffer(){};
	void execute() override;
};
class StartReplayBuffer : public Actions {
public:
	StartReplayBuffer(){};
	void execute() override;
};
class StopReplayBuffer : public Actions {
public:
	StopReplayBuffer(){};
	void execute() override;
};
class SaveReplayBuffer : public Actions {
public:
	SaveReplayBuffer(){};
	void execute() override;
};
class SetCurrentProfile : public Actions {
public:
	SetCurrentProfile(){};
	void execute() override;
};
class SetTextGDIPlusText : public Actions {
public:
	SetTextGDIPlusText(){};
	void execute() override;
};
class SetBrowserSourceURL : public Actions {
public:
	SetBrowserSourceURL(){};
	void execute() override;
};
class ReloadBrowserSource : public Actions {
public:
	ReloadBrowserSource(){};
	void execute() override;
};
class TakeScreenshot : public Actions {
public:
	TakeScreenshot(){};
	void execute() override;
};
class TakeSourceScreenshot : public Actions {
public:
	TakeSourceScreenshot(){};
	void execute() override;
};
class EnableSourceFilter : public Actions {
public:
	EnableSourceFilter(){};
	void execute() override;
};
class DisableSourceFilter : public Actions {
public:
	DisableSourceFilter(){};
	void execute() override;
};
class ToggleSourceFilter : public Actions {
public:
	ToggleSourceFilter(){};
	void execute() override;
};
class TriggerHotkey : public Actions {
public:
	TriggerHotkey(){};
	void execute() override;
};

// CC ACTIONS
class SetVolume : public Actions {
public:
	SetVolume(){};
	void execute() override;
};
class SetSyncOffset : public Actions {
public:
	SetSyncOffset(){};
	void execute() override;
};
class SetSourcePosition : public Actions {
public:
	SetSourcePosition(){};
	void execute() override;
};
class SetSourceRotation : public Actions {
public:
	SetSourceRotation(){};
	void execute() override;
};
class SetSourceScale : public Actions {
public:
	SetSourceScale(){};
	void execute() override;
};
class SetGainFilter : public Actions {
public:
	SetGainFilter(){};
	void execute() override;
};
class SetOpacity : public Actions {
public:
	SetOpacity(){};
	void execute() override;
};
class move_t_bar : public Actions {
public:
	move_t_bar(){};
	void execute() override;
};
class play_pause_media_source : public Actions {
public:
	play_pause_media_source(){};
	void execute() override;
};
class toggle_studio_mode : public Actions {
public:
	toggle_studio_mode(){};
	void execute() override;
};
class reset_stats : public Actions {
public:
	reset_stats(){};
	void execute() override;
};
class restart_media : public Actions {
public:
	restart_media(){};
	void execute() override;
};
class stop_media : public Actions {
public:
	stop_media(){};
	void execute() override;
};
class play_media : public Actions {
public:
	play_media(){};
	void execute() override;
};
class next_media : public Actions {
public:
	next_media(){};
	void execute() override;
};
class prev_media : public Actions {
public:
	prev_media(){};
	void execute() override;
};
class make_opacity_filter : public Actions {
public:
	make_opacity_filter(){};
	void execute() override;
};
int time_to_sleep(int duration);
