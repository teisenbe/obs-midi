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
#if __has_include(<obs-frontend-api.h>)
#include <obs-frontend-api.h>
#else
#include <obs-frontend-api/obs-frontend-api.h>
#endif
#include <map>
#include <utility>

#include <QDialogButtonBox>

#include <obs-module.h>
#include <QObject>
#include "settings-dialog.h"
#include "ui_settings-dialog.h"
#include "../device-manager.h"
#include "../config.h"
#include "Macros.h"
#include <QListView>

PluginWindow::PluginWindow(QWidget *parent) : QDialog(parent, Qt::Dialog), ui(new Ui::PluginWindow)
{
	ui->setupUi(this);
	auto *macros = new Macros(ui);
	macros->setParent(this);
	// Set Window Title
	setup_actions();
	set_title_window();
	// configure_table();
	connect_ui_signals();
	ui->box_action->setAlignment((int)Alignment::Top_Center);
	ui->box_action->setFlat(true);
	ui->box_action->setPalette(ui->list_mapping->palette());
	//connect(ui->search_mapping, &QLineEdit::textChanged, devmodel, &QSortFilterProxyModel::setFilterWildcard);
	starting = false;
}

void PluginWindow::set_title_window()
{
	QString title;
	title.append(GIT_TAG);
	blog(LOG_DEBUG, "OBS-MIDI Version: %s", title.qtocs());
	title.prepend("OBS MIDI Settings:  ");
	this->setWindowTitle(title);
}
void PluginWindow::connect_ui_signals() const
{
	connect(ui->list_midi_dev, SIGNAL(currentTextChanged(QString)), this, SLOT(on_device_select(QString)));
	connect(ui->check_enabled, SIGNAL(stateChanged(int)), this, SLOT(on_check_enabled_state_changed(int)));
	connect(ui->bidirectional, SIGNAL(stateChanged(int)), this, SLOT(on_bid_enabled_state_changed(int)));
	// Connections for Configure Tab
	connect(ui->cb_obs_output_action, SIGNAL(currentTextChanged(QString)), this, SLOT(obs_actions_select(QString)));
	// connect(ui->table_mapping, SIGNAL(cellClicked(int, int)), this, SLOT(edit_mapping()));
	/**************Connections to mappper****************/
	connect(ui->btn_add, SIGNAL(clicked()), this, SLOT(add_new_mapping()));
	connect(ui->btn_reset, SIGNAL(clicked()), this, SLOT(reset_to_defaults()));
	connect(ui->btn_delete, SIGNAL(clicked()), this, SLOT(delete_mapping()));
	connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tab_changed(int)));
	connect(ui->outbox, SIGNAL(currentTextChanged(QString)), this, SLOT(select_output_device(QString)));
}
void PluginWindow::setup_actions() const
{
	ui->cb_obs_output_action->clear();
	ui->cb_obs_output_action->addItems(Utils::TranslateActions());
	ui->cb_obs_output_action->setCurrentIndex(1);
	ui->cb_obs_output_action->setCurrentIndex(0);
}
void PluginWindow::ToggleShowHide()
{
	if (!isVisible()) {
		load_devices();
		ui->tabWidget->setCurrentIndex(0);
		ui->list_midi_dev->setCurrentRow(0);
		setVisible(true);
	} else {
		setVisible(false);
		ui->btn_Listen_many->setChecked(false);
		ui->btn_Listen_one->setChecked(false);
		reset_to_defaults();
	}
}

void PluginWindow::load_devices()
{
	loadingdevices = true;
	const auto midiOutDevices = GetDeviceManager()->get_output_ports_list();
	const auto midiDevices = GetDeviceManager()->get_input_ports_list();
	this->ui->list_midi_dev->clear();
	if (midiDevices.size() == 0) {
		this->ui->list_midi_dev->addItem("No Devices Available");
		ui->tab_configure->setEnabled(false);
		ui->bidirectional->setEnabled(false);
		ui->check_enabled->setEnabled(false);
		this->ui->outbox->setEnabled(false);
		ui->tabWidget->setEnabled(false);
	} else if (midiDevices.size() > 0) {
		for (int i = 0; i < midiDevices.size(); i++) {
			this->ui->list_midi_dev->addItem(midiDevices.at(i));
		}
		ui->tab_configure->setEnabled(true);
		ui->bidirectional->setEnabled(true);
		ui->check_enabled->setEnabled(true);
		ui->tabWidget->setEnabled(true);
		this->ui->outbox->clear();
		this->ui->outbox->insertItems(0, midiOutDevices);
	}
	loadingdevices = false;
}
void PluginWindow::select_output_device(const QString &selectedDeviceName) const
{
	if (!loadingdevices) {
		const auto selectedDevice = ui->list_midi_dev->currentItem()->text().toStdString();
		auto device = GetDeviceManager()->get_midi_device(selectedDevice.c_str());
		device->set_midi_output_name(selectedDeviceName);
		GetConfig()->Save();
	}
}
void PluginWindow::on_check_enabled_state_changed(int state) const
{
	if (state == Qt::CheckState::Checked) {
		const auto selectedDeviceName = ui->list_midi_dev->currentItem()->text().toStdString();
		const auto selectedOutDeviceName = ui->outbox->currentText().toStdString();
		auto device = GetDeviceManager()->get_midi_device(selectedDeviceName.c_str());
		blog(LOG_INFO, "Item enabled: %s", selectedDeviceName.c_str());
		const int devicePort = GetDeviceManager()->get_input_port_number(selectedDeviceName.c_str());
		int deviceOutPort = GetDeviceManager()->get_output_port_number(selectedOutDeviceName.c_str());
		if (device == NULL) {
			device = (ui->bidirectional->isChecked()) ? GetDeviceManager()->register_midi_device(devicePort, deviceOutPort)
								  : GetDeviceManager()->register_midi_device(devicePort);
		}
		device->open_midi_input_port();
		device->open_midi_output_port();
		device->set_enabled(true);
		ui->bidirectional->setEnabled(true);
		ui->bidirectional->setChecked(device->isBidirectional());
		ui->outbox->setEnabled(device->isBidirectional());
		set_configure_title(QString::fromStdString(selectedDeviceName));
		connect_midi_message_handler();
	}
	GetConfig()->Save();
}
void PluginWindow::disconnect_midi_message_handler() const
{
	auto devicemanager = GetDeviceManager();
	auto devices = devicemanager->get_active_midi_devices();
	for (auto device : devices) {
		disconnect(device, SIGNAL(broadcast_midi_message(MidiMessage)), this, SLOT(handle_midi_message(MidiMessage)));
	}
}
void PluginWindow::connect_midi_message_handler() const
{
	/**
	 * Disconnects all midi agents from the ui message handler, to ensure only one device is connected to the UI at a time
	 */
	disconnect_midi_message_handler();

	auto devicemanager = GetDeviceManager();

	const auto MAdevice = devicemanager->get_midi_device(ui->list_midi_dev->currentItem()->text());
	connect(MAdevice, SIGNAL(broadcast_midi_message(MidiMessage)), this,
		SLOT(handle_midi_message(MidiMessage))); /// name, mtype, norc, channel
}
void PluginWindow::on_device_select(const QString &curitem) const
{
	if (!starting) {
		blog(LOG_DEBUG, "on_device_select %s", curitem.qtocs());
		auto devicemanager = GetDeviceManager();
		auto config = GetConfig();
		MidiAgent *MAdevice = devicemanager->get_midi_device(curitem);
		set_configure_title(curitem);
		// Pull info on if device is enabled, if so set true if not set false
		try {
			if (MAdevice != NULL && MAdevice->isEnabled()) {
				ui->check_enabled->setChecked(true);
				ui->outbox->setEnabled(true);
				ui->bidirectional->setEnabled(true);
				ui->bidirectional->setChecked(MAdevice->isBidirectional());

				if (MAdevice->isBidirectional()) {
					ui->outbox->setCurrentText(MAdevice->get_midi_output_name());
				}
				connect_midi_message_handler();
			} else {
				ui->check_enabled->setChecked(false);
				ui->outbox->setEnabled(false);
				ui->bidirectional->setEnabled(false);
			}
			/// HOOK up the Message Handler
			ui->mapping_lbl_device_name->setText(curitem);
		} catch (...) {
		}
	}
}
void PluginWindow::set_configure_title(const QString &title) const
{
	ui->tabWidget->setTabText(1, QString("Configure - ").append(title));
}
void PluginWindow::handle_midi_message(const MidiMessage &mess) const
{
	if (ui->tabWidget->currentIndex() != 1)
		return;

	if (ui->btn_Listen_one->isChecked() || ui->btn_Listen_many->isChecked()) {
		blog(1, "got midi message via gui, \n Device = %s \n MType = %s \n NORC : %i \n Channel: %i \n Value: %i", mess.device_name.qtocs(),
		     mess.message_type.qtocs(), mess.NORC, mess.channel, mess.value);
		ui->mapping_lbl_device_name->setText(mess.device_name);
		ui->sb_channel->setValue(mess.channel);
		ui->sb_norc->setValue(mess.NORC);
		ui->slider_value->setValue(mess.value);
		ui->cb_mtype->setCurrentText(mess.message_type);
		ui->btn_Listen_one->setChecked(false);
	}

}
void PluginWindow::on_bid_enabled_state_changed(int state) const
{
	auto device = GetDeviceManager()->get_midi_device(ui->list_midi_dev->currentItem()->text().qtocs());
	ui->outbox->setEnabled(state);
	device->set_bidirectional(state);
	Config().Save();
}
PluginWindow::~PluginWindow()
{
	delete ui;
}

void PluginWindow::reset_to_defaults() const
{
}
void PluginWindow::clear_actions_box(QLayout *layout) const
{
	if (layout) {
		QLayoutItem *item;
		while ((item = layout->takeAt(0))) {
			if (item->layout()) {
				clear_actions_box(item->layout());
				delete item->layout();
			}
			if (item->widget()) {
				delete item->widget();
			}
		}
		delete layout;
	}
}

void PluginWindow::obs_actions_select(const QString &action) const
{
	clear_actions_box(ui->box_action->layout());
	Actions *AC = Actions::make_action(Utils::untranslate(action));
	ui->box_action->setLayout(AC->set_widgets());
	ui->btn_reset->setEnabled(true);
	}/*
	if (!switching) {
		hide_all_pairs();
		switch (ActionsClass::string_to_action(Utils::untranslate(action))) {
		case ActionsClass::Actions::Set_Preview_Scene:
			show_pair(Pairs::Scene);
			break;
		case ActionsClass::Actions::Enable_Source_Filter:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			show_pair(Pairs::Filter);
			break;
		case ActionsClass::Actions::Disable_Source_Filter:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			show_pair(Pairs::Filter);
			break;
		case ActionsClass::Actions::Set_Gain_Filter:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			show_pair(Pairs::Filter);
			break;
		case ActionsClass::Actions::Toggle_Source_Filter:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			show_pair(Pairs::Filter);
			break;
		case ActionsClass::Actions::Reset_Scene_Item:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			break;
		case ActionsClass::Actions::Set_Scene_Item_Render:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			show_pair(Pairs::Item);
			break;
		case ActionsClass::Actions::Set_Scene_Item_Position:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Item);
			break;
		case ActionsClass::Actions::Set_Scene_Item_Transform:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Item);
			break;
		case ActionsClass::Actions::Set_Scene_Item_Crop:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Item);
			break;
		case ActionsClass::Actions::Set_Scene_Transition_Override:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Transition);
			break;
		case ActionsClass::Actions::Set_Current_Transition:
			show_pair(Pairs::Transition);
			break;
		case ActionsClass::Actions::Set_Source_Filter_Visibility:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			show_pair(Pairs::Filter);
			break;
		case ActionsClass::Actions::Take_Source_Screenshot:
			show_pair(Pairs::Scene);
			break;

		case ActionsClass::Actions::Toggle_Source_Visibility:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			break;
		case ActionsClass::Actions::Reload_Browser_Source:
			show_pair(Pairs::Source);
			ui->cb_obs_output_source->clear();
			ui->cb_obs_output_source->addItems(Utils::get_browser_sources());
			break;
		case ActionsClass::Actions::Set_Source_Rotation:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			show_pair(Pairs::Range);
			set_min_max_range_defaults(0, 360);
			set_range_text("Min", "Max");
			break;
		case ActionsClass::Actions::Set_Source_Scale:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			show_pair(Pairs::Range);
			set_min_max_range_defaults(10, 10);
			set_range_text("Max X", "Max Y");
			break;
		case ActionsClass::Actions::Toggle_Fade_Source:
			show_pair(Pairs::Source);
			show_pair(Pairs::Scene);
			show_pair(Pairs::Integer);
			ui->check_int_override->setChecked(true);
			ui->sb_int_override->setValue(500);
			ui->label_Int_override->setText("Duration * ");
			ui->sb_int_override->setSuffix(" ms");
			break;
		case ActionsClass::Actions::Trigger_Hotkey_By_Name:
			show_pair(Pairs::Hotkey);
			break;
		default:
			hide_all_pairs();
			break;
		}
	}
}
*/
void PluginWindow::set_edit_mode() {}
void PluginWindow::save_edit() {}

bool PluginWindow::map_exists() const
{
	auto devicemanager = GetDeviceManager();
	const auto hooks = devicemanager->get_midi_hooks(ui->mapping_lbl_device_name->text());
	return false;
}


void PluginWindow::set_all_cell_colors(const int row) const
{
	const QColor midi_color(0, 170, 255);
	const QColor action_color(170, 0, 255);

	/*for (auto col = 0; col <= ui->table_mapping->columnCount(); col++) {
		auto *const rc = ui->table_mapping->item(row, col);
		(col < 3) ? set_cell_colors(midi_color, rc) : set_cell_colors(action_color, rc);
	}*/
}

void PluginWindow::tab_changed(const int tab) const
{
	reset_to_defaults();
	if (tab == 1) {
		ui->cb_obs_output_action->setCurrentIndex(1);
		ui->cb_obs_output_action->setCurrentIndex(0);
		ui->mapping_lbl_device_name->setText(ui->list_midi_dev->currentItem()->text());
		Utils::build_hotkey_map();
	}

	// this->ui->table_mapping->resizeColumnsToContents();
}
void PluginWindow::clear_table() const
{
	// ui->table_mapping->clearContents();
	// set_headers();
	// ui->table_mapping->setRowCount(0);
} /*
void PluginWindow::insert_mapping_table_help_text() const
{
	ui->list_mapping->clear();

	ui->list_mapping->addItem("No Current Actions, To Add an action:");
	ui->list_mapping->addItem("");
	ui->list_mapping->addItem("1.) Click Listen one or Listen Many.");
	ui->list_mapping->addItem("2.) Toggle a button, fader, or knob on your MIDI device.");
	ui->list_mapping->addItem("3.) Choose an action.");
	ui->list_mapping->addItem("4.) Select options");
	ui->list_mapping->addItem("5.) Click  'Add Mapping'");
}
*/
void PluginWindow::remove_hook(MidiMapping *hook) const
{
	GetDeviceManager()->get_midi_device(ui->mapping_lbl_device_name->text())->remove_MidiMapping(hook);
	GetConfig()->Save();
}
void PluginWindow::delete_mapping() const
{
	/*if (ui->table_mapping->rowCount() <= 0)
		return;

	auto row = ui->table_mapping->selectedItems().at(0)->row();
	blog(LOG_DEBUG, "selected row to delete %i", row);
	const auto hooks = GetDeviceManager()->get_midi_device(ui->mapping_lbl_device_name->text())->GetMidiMappings();
	for (auto *hook : hooks) {
		if ((hook->channel == ui->sb_channel->value()) && (hook->norc == ui->sb_norc->value()) && (hook->message_type == ui->cb_mtype->currentText())) {
			if (hook->value_as_filter) {
				if (hook->value == ui->slider_value->value()) {
					remove_hook(hook);
					ui->table_mapping->removeRow(row);
					ui->table_mapping->clearSelection();
				}
			} else {
				remove_hook(hook);
				ui->table_mapping->removeRow(row);
				ui->table_mapping->clearSelection();
			}
		}
	}*/
	reset_to_defaults();
}
void PluginWindow::edit_mapping()
{
	/**if (ui->table_mapping->rowCount() != 0) {
		editmode = true;

		ui->btn_add->setText("Save Edits");
		ui->btn_reset->setEnabled(true);
		const auto dv = GetDeviceManager().get()->get_midi_hooks(ui->mapping_lbl_device_name->text());
		blog(LOG_DEBUG, "hook numners: name %s = %i", ui->mapping_lbl_device_name->text().qtocs(), dv.count());
		const auto selected_items = ui->table_mapping->selectedItems();
		const auto row = selected_items.at(0)->row();
		editrow = row;
		blog(LOG_DEBUG, "hook in row= %i", row);
		// rebuild midi
		ui->sb_channel->setValue(selected_items.at(0)->text().toInt());
		ui->cb_mtype->setCurrentText(selected_items.at(1)->text());
		ui->sb_norc->setValue(selected_items.at(2)->text().toInt());
		ui->check_use_value->setChecked(dv.at(row)->value_as_filter);
		ui->slider_value->setValue((dv.at(row)->value_as_filter) ? *dv.at(row)->value : 0);
		// rebuild actions
		ui->cb_obs_output_action->setCurrentText(selected_items.at(3)->text());
		ui->cb_obs_output_scene->setCurrentText(selected_items.at(4)->text());
		ui->cb_obs_output_source->setCurrentText(selected_items.at(5)->text());
		ui->cb_obs_output_filter->setCurrentText(selected_items.at(6)->text());
		ui->cb_obs_output_transition->setCurrentText(selected_items.at(7)->text());
		ui->cb_obs_output_item->setCurrentText(selected_items.at(8)->text());
		ui->cb_obs_output_audio_source->setCurrentText(selected_items.at(9)->text());
		ui->cb_obs_output_media_source->setCurrentText(selected_items.at(10)->text());
		const bool check = (selected_items.at(11)->text().toInt() > 0) ? true : false;
		ui->check_int_override->setChecked(check);
		ui->sb_int_override->setValue(selected_items.at(11)->text().toInt());
		ui->cb_obs_output_hotkey->setCurrentText(selected_items.at(14)->text());
		ui->btn_delete->setEnabled(true);
		edithook = find_existing_hook();
	}*/
}

bool PluginWindow::verify_mapping() const
{
	return true;
}
