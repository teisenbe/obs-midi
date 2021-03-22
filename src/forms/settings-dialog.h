/*
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
#include <QtWidgets/QDialog>
#include <vector>

#include "ui_settings-dialog.h"
#include "../midi-agent.h"
#include "../version.h"
#include "../Midi_hook.h"
class PluginWindow : public QDialog {
	Q_OBJECT
public:
	explicit PluginWindow(QWidget *parent);
	~PluginWindow() override;
public slots:
	void ToggleShowHide();
private slots:
	void on_check_enabled_state_changed(int state) const;
	void on_device_select(const QString &curitem) const;
	void handle_midi_message(const MidiMessage &mess) const;
	void obs_actions_select(const QString &action) const;
	void set_edit_mode();
	void save_edit();

	void reset_to_defaults() const;
	void clear_actions_box(QLayout *layout)const;
	void set_all_cell_colors(int row) const;
	void tab_changed(int tab) const;
	void clear_table() const;
	void remove_hook(MidiMapping *hook) const;
	void delete_mapping() const;
	void edit_mapping();
	//static void set_cell_colors(const QColor &color, QTableWidgetItem *item);

private:
	Ui::PluginWindow *ui;
	bool hidedebugitems = true;
	bool loadingdevices = false;
	bool starting = true;
	bool listening = false;
	//void configure_table() const;
	void set_title_window();
	void connect_ui_signals() const;
	void setup_actions() const;
	//	void setCheck(bool check); // not implemented
	void load_devices();

	//static QStringList translatelist(QStringList list);
	
	void add_midi_device(const QString &Name) const;
	//void set_headers() const;
	void set_configure_title(const QString &title) const;
	void disconnect_midi_message_handler() const;
	void connect_midi_message_handler() const;
	bool map_exists() const;
	bool verify_mapping() const;
	void add_row_from_hook(const MidiMapping *hook) const;
	void load_table() const;
	void table_select(int selection);
	void midi_message_select(MidiMapping *hook);
private:
	int editrow = -1;
	bool editmode = false;
	bool switching = false;
	MidiMapping *edithook;
};
