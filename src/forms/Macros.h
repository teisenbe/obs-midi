#pragma once

#include <qobject.h>
#include "ui_settings-dialog.h"
#include "../utils.h"
#include "src/Midi_hook.h"
typedef void (*Macro_Action)(MidiHook *, int);
enum class repeat { none, one, many, infinite };
class Macro : public QObject {
	Q_OBJECT
public:
	QString name;
	Macro_Action macro_action;
	void set_repeat(const repeat r) { this->repeat_ = r; }
	void set_delay(const int d) { this->delay_ = d; }
	void set_iterations(const int i) { this->repeat_iterations_ = i; }

private:
	int delay_ = 500;
	repeat repeat_ = repeat::none;
	int repeat_iterations_ = 0;
};
class Macros : public QObject {
	Q_OBJECT
public:
	Macros(Ui::PluginWindow *pw);
	~Macros();
	void setup_connections();
	void setup_actions();
	void show_pair(Pairs pair);
	void hide_pair(Pairs pair);
	void hide_all_pairs();
public slots:
	void slot_action_changed(const QString &action);

private:
	Ui::PluginWindow *ui;
};
