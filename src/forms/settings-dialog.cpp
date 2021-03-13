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
#include "ui_settings-dialog.h"
#include "settings-dialog.h"
#include "../device-manager.h"
#include "../config.h"
#include "Macros.h"

PluginWindow::PluginWindow(QWidget *parent) : QDialog(parent, Qt::Dialog), ui(new Ui::PluginWindow)
{
	ui->setupUi(this);
	auto *macros = new Macros(ui);
	macros->setParent(this);
	// Set Window Title
	setup_actions();
	set_title_window();
	configure_table();
	hide_all_pairs();
	connect_ui_signals();

	starting = false;
}
void PluginWindow::configure_table() const
{
	ui->table_mapping->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	ui->table_mapping->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	ui->table_mapping->setSortingEnabled(true);
}
void PluginWindow::set_title_window()
{
	QString title;
	title.append(GIT_TAG);
	blog(LOG_DEBUG, "OBS-MIDI Version: %s", title.toStdString().c_str());
	title.prepend("OBS MIDI Settings:  ");
	this->setWindowTitle(title);
}
void PluginWindow::connect_ui_signals() const
{
	connect(ui->list_midi_dev, SIGNAL(currentTextChanged(QString)), this, SLOT(on_device_select(QString)));
	connect(ui->check_enabled, SIGNAL(stateChanged(int)), this, SLOT(on_check_enabled_state_changed(int)));
	connect(ui->bidirectional, SIGNAL(stateChanged(int)), this, SLOT(on_bid_enabled_state_changed(int)));
	// Connections for Configure Tab
	connect(ui->cb_obs_output_scene, SIGNAL(currentTextChanged(QString)), this, SLOT(get_sources(QString)));
	connect(ui->cb_obs_output_action, SIGNAL(currentTextChanged(QString)), this, SLOT(obs_actions_select(QString)));
	connect(ui->cb_obs_output_source, SIGNAL(currentTextChanged(QString)), this, SLOT(on_source_change(QString)));
	connect(ui->cb_obs_output_scene, SIGNAL(currentTextChanged(QString)), this, SLOT(on_scene_change(QString)));
	connect(ui->table_mapping, SIGNAL(cellClicked(int, int)), this, SLOT(edit_mapping()));
	/**************Connections to mappper****************/
	connect(ui->btn_add, SIGNAL(clicked()), this, SLOT(add_new_mapping()));
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
		hide_all_pairs();
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
		blog(LOG_DEBUG, "on_device_select %s", curitem.toStdString().c_str());
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
		blog(1, "got midi message via gui, \n Device = %s \n MType = %s \n NORC : %i \n Channel: %i \n Value: %i",
		     mess.device_name.toStdString().c_str(), mess.message_type.toStdString().c_str(), mess.NORC, mess.channel, mess.value);
		ui->mapping_lbl_device_name->setText(mess.device_name);
		ui->sb_channel->setValue(mess.channel);
		ui->sb_norc->setValue(mess.NORC);
		ui->slider_value->setValue(mess.value);
		ui->cb_mtype->setCurrentText(mess.message_type);
		ui->btn_Listen_one->setChecked(false);
	}
	if (find_mapping_location(mess) != -1) {
		ui->table_mapping->selectRow(find_mapping_location(mess));
	}
}
void PluginWindow::on_bid_enabled_state_changed(int state) const
{
	auto device = GetDeviceManager()->get_midi_device(ui->list_midi_dev->currentItem()->text().toStdString().c_str());
	ui->outbox->setEnabled(state);
	device->set_bidirectional(state);
}
PluginWindow::~PluginWindow()
{
	delete ui;
}
void PluginWindow::add_midi_device(const QString &name) const
{
	blog(LOG_DEBUG, "Adding Midi Device %s", name.toStdString().c_str());

	// don't delete it, because the table takes ownership of the items
	auto *device_name = new QTableWidgetItem();
	auto *device_enabled = new QTableWidgetItem();
	auto *device_status = new QTableWidgetItem();
	auto *feedback_enabled = new QTableWidgetItem();
	auto *feedback_name = new QTableWidgetItem();
	auto *feedback_status = new QTableWidgetItem();
	const int rowcount = this->ui->table_mapping->rowCount();
	this->ui->table_mapping->insertRow(rowcount);
	device_name->setText(name);
	device_enabled->setCheckState(Qt::Unchecked);
	device_status->setText(QString("Disconnected"));
	// device_status->setForeground("grey");
	feedback_name->setText("");
	feedback_enabled->setCheckState(Qt::Unchecked);
	feedback_status->setText(QString("unset"));
	this->ui->table_mapping->setItem(rowcount, 0, device_name);
	this->ui->table_mapping->setItem(rowcount, 1, device_enabled);
	this->ui->table_mapping->setItem(rowcount, 2, device_status);
	this->ui->table_mapping->setItem(rowcount, 3, feedback_enabled);
	this->ui->table_mapping->setItem(rowcount, 4, feedback_name);
	this->ui->table_mapping->setItem(rowcount, 5, feedback_status);
}
void PluginWindow::set_headers() const
{
	ui->table_mapping->setHorizontalHeaderLabels({"Channel", "Message Type", "Note or Control", "Action", "Scene", "Source", "Filter", "Transition", "Item",
						      "Audio Source", "Media Source", "Duration"});
	const QColor midicolor("#00aaff");
	const QColor actioncolor("#aa00ff");
	for (int i = 0; i <= 11; i++) {
		(i <= 2) ? ui->table_mapping->horizontalHeaderItem(i)->setForeground(midicolor)
			 : ui->table_mapping->horizontalHeaderItem(i)->setForeground(actioncolor);
	}
}
void PluginWindow::show_pair(Pairs Pair) const
{
	switch (Pair) {
	case Pairs::Scene:
		ui->label_obs_output_scene->show();
		ui->cb_obs_output_scene->show();
		ui->cb_obs_output_scene->addItems(Utils::get_scene_names());
		ui->w_scene->show();
		break;
	case Pairs::Source:
		ui->label_obs_output_source->show();
		ui->cb_obs_output_source->show();
		ui->cb_obs_output_source->addItems(Utils::get_source_names(ui->cb_obs_output_scene->currentText()));
		ui->w_source->show();
		break;
	case Pairs::Filter:
		ui->label_obs_output_filter->show();
		ui->cb_obs_output_filter->show();
		ui->cb_obs_output_filter->addItems(Utils::get_filter_names(ui->cb_obs_output_source->currentText()));
		ui->w_filter->show();
		break;
	case Pairs::Transition:
		ui->label_obs_output_transition->show();
		ui->cb_obs_output_transition->show();
		ui->w_transition->show();
		ui->cb_obs_output_transition->addItems(Utils::GetTransitionsList());
		break;
	case Pairs::Item:
		ui->label_obs_output_item->show();
		ui->cb_obs_output_item->show();
		ui->cb_obs_output_item->addItems(Utils::GetSceneItemsList(ui->cb_obs_output_scene->currentText()));
		ui->w_item->show();
		break;
	case Pairs::Hotkey:
		
		ui->label_obs_output_hotkey->show();
		ui->cb_obs_output_hotkey->show();
		ui->cb_obs_output_hotkey->addItems(translatelist(Utils::GetHotkeysList()));
		ui->w_hotkey->show();
		break;
	case Pairs::Audio:
		ui->cb_obs_output_audio_source->clear();
		ui->cb_obs_output_audio_source->addItems(Utils::GetAudioSourceNames());
		ui->label_obs_output_audio_source->show();
		ui->cb_obs_output_audio_source->show();
		ui->w_audio->show();
		break;
	case Pairs::Media:
		ui->cb_obs_output_media_source->clear();
		ui->cb_obs_output_media_source->addItems(Utils::GetMediaSourceNames());
		ui->label_obs_output_media_source->show();
		ui->cb_obs_output_media_source->show();
		ui->w_media->show();
		break;
	case Pairs::String:
		break;
	case Pairs::Boolean:
		break;
	case Pairs::Integer:
		ui->sb_int_override->show();
		ui->label_Int_override->show();
		ui->check_int_override->show();
		ui->check_int_override->setChecked(false);
		break;
	case Pairs::Range:
		ui->sb_min->show();
		ui->label_min->show();
		ui->sb_max->show();
		ui->label_max->show();
		break;
	}
}

void PluginWindow::hide_pair(Pairs Pair) const
{
	switch (Pair) {
	case Pairs::Scene:
		ui->label_obs_output_scene->hide();
		ui->cb_obs_output_scene->hide();
		ui->cb_obs_output_scene->clear();
		ui->w_scene->hide();
		blog(LOG_DEBUG, "Hide Scene");
		break;
	case Pairs::Source:
		ui->label_obs_output_source->hide();
		ui->cb_obs_output_source->hide();
		ui->cb_obs_output_source->clear();
		ui->w_source->hide();
		blog(LOG_DEBUG, "Hide Source");
		break;
	case Pairs::Filter:
		ui->label_obs_output_filter->hide();
		ui->cb_obs_output_filter->hide();
		ui->cb_obs_output_filter->clear();
		ui->w_filter->hide();
		blog(LOG_DEBUG, "Hide Filter");
		break;
	case Pairs::Transition:
		ui->label_obs_output_transition->hide();
		ui->cb_obs_output_transition->hide();
		ui->cb_obs_output_transition->clear();
		ui->w_transition->hide();
		ui->check_int_override->hide();
		blog(LOG_DEBUG, "Hide Transition");
		break;
	case Pairs::Item:
		ui->label_obs_output_item->hide();
		ui->cb_obs_output_item->hide();
		ui->cb_obs_output_item->clear();
		ui->w_item->hide();
		blog(LOG_DEBUG, "Hide Item");
		break;
	case Pairs::Hotkey:
		ui->label_obs_output_hotkey->hide();
		ui->cb_obs_output_hotkey->hide();
		ui->cb_obs_output_hotkey->clear();
		ui->w_hotkey->hide();
		blog(LOG_DEBUG, "Hide Hotkey");
		break;
	case Pairs::Audio:
		ui->label_obs_output_audio_source->hide();
		ui->cb_obs_output_audio_source->hide();
		ui->cb_obs_output_audio_source->clear();
		ui->w_audio->hide();
		blog(LOG_DEBUG, "Hide Audio");
		break;
	case Pairs::Media:
		ui->label_obs_output_media_source->hide();
		ui->cb_obs_output_media_source->hide();
		ui->cb_obs_output_media_source->clear();
		ui->w_media->hide();
		blog(LOG_DEBUG, "Hide Media");
		break;
	case Pairs::String:
		break;
	case Pairs::Boolean:
		break;
	case Pairs::Integer:
		ui->sb_int_override->hide();
		ui->label_Int_override->hide();
		break;
	case Pairs::Range:
		ui->sb_min->hide();
		ui->label_min->hide();
		ui->sb_max->hide();
		ui->label_max->hide();
		break;
	}
}
QStringList PluginWindow::translatelist(QStringList list) {
	QStringList newlist;
	for (auto item : list) {
		newlist.append(tr(item.toStdString().c_str()));
	}
	return newlist;
}
void PluginWindow::hide_all_pairs() const
{
	hide_pair(Pairs::Transition);
	hide_pair(Pairs::Audio);
	hide_pair(Pairs::Media);
	hide_pair(Pairs::Filter);
	hide_pair(Pairs::Scene);
	hide_pair(Pairs::Source);
	hide_pair(Pairs::Item);
	hide_pair(Pairs::String);
	hide_pair(Pairs::Integer);
	hide_pair(Pairs::Boolean);
	hide_pair(Pairs::Range);
	hide_pair(Pairs::Hotkey);
}
void PluginWindow::reset_to_defaults() const
{
	ui->cb_obs_output_action->setCurrentIndex(0);
	ui->cb_obs_output_transition->setCurrentIndex(0);
	ui->cb_obs_output_filter->setCurrentIndex(0);
	ui->cb_obs_output_scene->setCurrentIndex(0);
	ui->cb_obs_output_source->setCurrentIndex(0);
	ui->cb_obs_output_audio_source->setCurrentIndex(0);
	ui->cb_obs_output_media_source->setCurrentIndex(0);
	ui->sb_channel->setValue(0);
	ui->sb_norc->setValue(0);
	ui->cb_mtype->setCurrentIndex(0);
	ui->slider_value->setValue(0);
	ui->btn_add->setText("Add Mapping");
	ui->btn_delete->setEnabled(false);
	ui->table_mapping->clearSelection();
	this->ui->table_mapping->resizeColumnsToContents();
}
void PluginWindow::obs_actions_select(const QString &action) const
{
	if (!switching) {
		hide_all_pairs();
		switch (ActionsClass::string_to_action(Utils::untranslate(action))) {
		case ActionsClass::Actions::Set_Current_Scene:
			show_pair(Pairs::Scene);
			break;
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
		case ActionsClass::Actions::Set_Volume:
			show_pair(Pairs::Audio);
			break;
		case ActionsClass::Actions::Set_Mute:
			show_pair(Pairs::Audio);
			break;
		case ActionsClass::Actions::Toggle_Mute:
			show_pair(Pairs::Audio);
			break;
		case ActionsClass::Actions::Set_Source_Filter_Visibility:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Source);
			show_pair(Pairs::Filter);
			break;
		case ActionsClass::Actions::Take_Source_Screenshot:
			show_pair(Pairs::Scene);
			break;
		case ActionsClass::Actions::Play_Pause_Media:
			show_pair(Pairs::Media);
			break;
		case ActionsClass::Actions::Restart_Media:
			show_pair(Pairs::Media);
			break;
		case ActionsClass::Actions::Stop_Media:
			show_pair(Pairs::Media);
			break;
		case ActionsClass::Actions::Next_Media:
			show_pair(Pairs::Media);
			break;
		case ActionsClass::Actions::Previous_Media:
			show_pair(Pairs::Media);
			break;
		case ActionsClass::Actions::Set_Media_Time:
			show_pair(Pairs::Media);
			break;
		case ActionsClass::Actions::Scrub_Media:
			show_pair(Pairs::Media);
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
		case ActionsClass::Actions::Do_Transition:
			show_pair(Pairs::Scene);
			show_pair(Pairs::Integer);
			show_pair(Pairs::Transition);
			ui->cb_obs_output_scene->insertItem(0, "Preview Scene");
			ui->cb_obs_output_scene->setCurrentIndex(0);
			ui->cb_obs_output_transition->insertItem(0, "Current Transition");
			ui->cb_obs_output_transition->setCurrentIndex(0);
			ui->label_Int_override->setText("Duration * ");
			ui->label_obs_output_scene->setText("Scene*");
			ui->label_obs_output_transition->setText("Transition*");
			ui->sb_int_override->setValue(300);
			ui->sb_int_override->setMaximum(100000);
			ui->sb_int_override->setMinimum(0);
			ui->sb_int_override->setSuffix(" ms");
			ui->sb_int_override->setEnabled(false);

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
void PluginWindow::set_edit_mode() {}
void PluginWindow::save_edit() {}
void PluginWindow::set_min_max_range_defaults(int min, int max) const
{
	ui->sb_min->setValue(min);
	ui->sb_max->setValue(max);
}
void PluginWindow::set_range_text(QString left, QString right) const
{
	ui->label_min->setText(left);
	ui->label_max->setText(right);
}
bool PluginWindow::map_exists() const
{
	auto devicemanager = GetDeviceManager();
	const auto hooks = devicemanager->get_midi_hooks(ui->mapping_lbl_device_name->text());
	for (auto hook : hooks) {
		if ((hook->channel == ui->sb_channel->value()) && (hook->norc == ui->sb_norc->value()) && (hook->message_type == ui->cb_mtype->currentText())) {
			return true;
		}
	}
	return false;
}
MidiHook *PluginWindow::find_existing_hook() const
{
	auto devicemanager = GetDeviceManager();
	const auto hooks = devicemanager->get_midi_hooks(ui->mapping_lbl_device_name->text());
	for (auto hook : hooks) {
		if ((hook->channel == ui->sb_channel->value()) && (hook->norc == ui->sb_norc->value()) && (hook->message_type == ui->cb_mtype->currentText())) {
			return hook;
		}
	}
	return nullptr;
}
int PluginWindow::find_mapping_location(const MidiMessage &message) const
{
	auto devicemanager = GetDeviceManager();
	const auto hooks = devicemanager->get_midi_hooks(ui->mapping_lbl_device_name->text());
	for (int i = 0; i < hooks.size(); i++) {
		if ((hooks.at(i)->channel == message.channel) && (hooks.at(i)->norc == message.NORC) && (hooks.at(i)->message_type == message.message_type)) {
			if (hooks.at(i)->value_as_filter) {
				if (hooks.at(i)->value == message.value) {
					return i;
				}
			} else {
				return i;
			}
		}
	}
	return -1;
}
void PluginWindow::add_new_mapping()
{
	ui->btn_Listen_many->setChecked(false);
	ui->btn_Listen_one->setChecked(false);
	if ((!map_exists() && verify_mapping() && ui->sb_channel->value() != 0) || ((map_exists() && ui->check_use_value->isChecked())) || editmode) {
		const auto row = (editmode) ? editrow : ui->table_mapping->rowCount();
		if (!editmode)
			ui->table_mapping->insertRow(row);

		editmode = false;
		// don't delete it, because the table takes ownership of the items
		const auto channel_item = new QTableWidgetItem(QString::number(ui->sb_channel->value()));
		const auto message_type_item = new QTableWidgetItem(ui->cb_mtype->currentText());
		const auto norc_item = new QTableWidgetItem(QString::number(ui->sb_norc->value()));
		const auto action_item = new QTableWidgetItem(ui->cb_obs_output_action->currentText());
		const auto scene_item = new QTableWidgetItem(ui->cb_obs_output_scene->currentText());
		const auto source_item = new QTableWidgetItem(ui->cb_obs_output_source->currentText());
		const auto filter_item = new QTableWidgetItem(ui->cb_obs_output_filter->currentText());
		const auto transition_item = new QTableWidgetItem(ui->cb_obs_output_transition->currentText());
		const auto item_item = new QTableWidgetItem(ui->cb_obs_output_item->currentText());
		const auto audio_item = new QTableWidgetItem(ui->cb_obs_output_audio_source->currentText());
		const auto media_item = new QTableWidgetItem(ui->cb_obs_output_media_source->currentText());
		const auto int_override = new QTableWidgetItem(QString::number(ui->sb_int_override->value()));
		const auto min = new QTableWidgetItem(QString::number(ui->sb_min->value()));
		const auto max = new QTableWidgetItem(QString::number(ui->sb_max->value()));
                const auto hotkey_item = new QTableWidgetItem(ui->cb_obs_output_hotkey->currentText());
		ui->table_mapping->setItem(row, 0, channel_item);
		ui->table_mapping->setItem(row, 1, message_type_item);
		ui->table_mapping->setItem(row, 2, norc_item);
		ui->table_mapping->setItem(row, 3, action_item);
		ui->table_mapping->setItem(row, 4, scene_item);
		ui->table_mapping->setItem(row, 5, source_item);
		ui->table_mapping->setItem(row, 6, filter_item);
		ui->table_mapping->setItem(row, 7, transition_item);
		ui->table_mapping->setItem(row, 8, item_item);
		ui->table_mapping->setItem(row, 9, audio_item);
		ui->table_mapping->setItem(row, 10, media_item);
		ui->table_mapping->setItem(row, 11, int_override);
		ui->table_mapping->setItem(row, 12, min);
		ui->table_mapping->setItem(row, 13, max);
		ui->table_mapping->setItem(row, 14, hotkey_item);

		set_all_cell_colors(row);
		auto *new_midi_hook = (editmode) ? find_existing_hook() : new MidiHook();
		new_midi_hook->channel = ui->sb_channel->value();
		new_midi_hook->message_type = ui->cb_mtype->currentText();
		new_midi_hook->norc = ui->sb_norc->value();
		new_midi_hook->value_as_filter = ui->check_use_value->isChecked();
		new_midi_hook->value.emplace((ui->check_use_value->isChecked()) ? ui->slider_value->value() : NULL);
		new_midi_hook->action = ui->cb_obs_output_action->currentText();
		new_midi_hook->scene = ui->cb_obs_output_scene->currentText();
		new_midi_hook->source = ui->cb_obs_output_source->currentText();
		new_midi_hook->filter = ui->cb_obs_output_filter->currentText();
		new_midi_hook->transition = ui->cb_obs_output_transition->currentText();
		new_midi_hook->item = ui->cb_obs_output_item->currentText();
		new_midi_hook->hotkey = ui->cb_obs_output_hotkey->currentText();
		new_midi_hook->audio_source = ui->cb_obs_output_audio_source->currentText();
		new_midi_hook->media_source = ui->cb_obs_output_media_source->currentText();
		new_midi_hook->int_override.emplace(ui->sb_int_override->value());
		new_midi_hook->range_min.emplace(ui->sb_min->value());
		new_midi_hook->range_max.emplace(ui->sb_max->value());
		new_midi_hook->setAction();
		GetDeviceManager().get()->get_midi_device(ui->mapping_lbl_device_name->text())->add_MidiHook(new_midi_hook);

		GetConfig().get()->Save();
		ui->table_mapping->selectRow(row);
		this->ui->table_mapping->resizeColumnsToContents();
	} else {
		if (ui->sb_channel->value()) {
			Utils::alert_popup("Can Not Map Channel 0. \nPlease Click Listen One or Listen Many to listen for MIDI Event to map");
		}
		if (!verify_mapping()) {
			Utils::alert_popup("Mapping Missing required variable");
		}
		if (map_exists()) {
			// TODO: Fix this, and create Utils::message_to_user(QString)
			QString mess;
			mess.append("Mapping already Exists for ");
			mess.append(ui->mapping_lbl_device_name->text());
			mess.append(" , with channel # ");
			mess.append(QString::number(ui->sb_channel->value()));
			mess.append(" norc # ");
			mess.append(QString::number(ui->sb_norc->value()));
			mess.append(" and Message Type ");
			mess.append(ui->cb_mtype->currentText());
			Utils::alert_popup(mess);
		}
	}
}
void PluginWindow::add_row_from_hook(const MidiHook *hook) const
{
	const auto row = ui->table_mapping->rowCount();
	ui->table_mapping->insertRow(row);

	// don't delete it, because the table takes ownership of the items
	auto *channel_item = new QTableWidgetItem(QString::number(hook->channel));
	auto *message_type_item = new QTableWidgetItem(hook->message_type);
	auto *norc_item = new QTableWidgetItem(QString::number(hook->norc));
	auto *action_item = new QTableWidgetItem(hook->action);
	auto *scene_item = new QTableWidgetItem(hook->scene);
	auto *source_item = new QTableWidgetItem(hook->source);
	auto *filter_item = new QTableWidgetItem(hook->filter);
	auto *transition_item = new QTableWidgetItem(hook->transition);
	auto *item_item = new QTableWidgetItem(hook->item);
	auto *audio_item = new QTableWidgetItem(hook->audio_source);
	auto *media_item = new QTableWidgetItem(hook->media_source);
	auto *hotkey_item = new QTableWidgetItem(hook->hotkey);
	QTableWidgetItem *ioveritem = (hook->int_override) ? new QTableWidgetItem(QString::number(*hook->int_override)) : new QTableWidgetItem();
	QTableWidgetItem *min = (hook->range_min) ? new QTableWidgetItem(QString::number(*hook->range_min)) : new QTableWidgetItem();
	QTableWidgetItem *max = (hook->range_max) ? new QTableWidgetItem(QString::number(*hook->range_max)) : new QTableWidgetItem();
	ui->table_mapping->setItem(row, 0, channel_item);
	ui->table_mapping->setItem(row, 1, message_type_item);
	ui->table_mapping->setItem(row, 2, norc_item);
	ui->table_mapping->setItem(row, 3, action_item);
	ui->table_mapping->setItem(row, 4, scene_item);
	ui->table_mapping->setItem(row, 5, source_item);
	ui->table_mapping->setItem(row, 6, filter_item);
	ui->table_mapping->setItem(row, 7, transition_item);
	ui->table_mapping->setItem(row, 8, item_item);
	ui->table_mapping->setItem(row, 9, audio_item);
	ui->table_mapping->setItem(row, 10, media_item);
	ui->table_mapping->setItem(row, 11, ioveritem);
	ui->table_mapping->setItem(row, 12, min);
	ui->table_mapping->setItem(row, 13, max);
	ui->table_mapping->setItem(row, 14, hotkey_item);
	set_all_cell_colors(row);
}
void PluginWindow::set_all_cell_colors(const int row) const
{
	const QColor midi_color(0, 170, 255);
	const QColor action_color(170, 0, 255);
	
	for (auto col = 0; col <= ui->table_mapping->columnCount(); col++) {
		auto *const rc = ui->table_mapping->item(row, col);
		(col < 3) ? set_cell_colors(midi_color, rc) : set_cell_colors(action_color, rc);
	}
}

void PluginWindow::set_cell_colors(const QColor &color, QTableWidgetItem *item)
{
	if (item == NULL)
		return;
	const QColor background_color;
	background_color.black();
	item->setBackground(background_color);
	item->setForeground(color);
	item->setTextAlignment(Qt::AlignCenter);
}
void PluginWindow::tab_changed(const int tab) const
{
	reset_to_defaults();
	if (tab == 1) {
		ui->cb_obs_output_action->setCurrentIndex(1);
		ui->cb_obs_output_action->setCurrentIndex(0);
		ui->mapping_lbl_device_name->setText(ui->list_midi_dev->currentItem()->text());
	}
	clear_table();
	load_table();
	this->ui->table_mapping->resizeColumnsToContents();
}
void PluginWindow::clear_table() const
{
	ui->table_mapping->clearContents();
	set_headers();
	ui->table_mapping->setRowCount(0);
}
void PluginWindow::load_table() const
{
	const auto hooks = GetDeviceManager()->get_midi_hooks(ui->mapping_lbl_device_name->text());
	if (hooks.count() > 0) {
		for (auto *hook : hooks) {
			add_row_from_hook(hook);
		}
	}
}
void PluginWindow::remove_hook(MidiHook *hook) const
{
	GetDeviceManager()->get_midi_device(ui->mapping_lbl_device_name->text())->remove_MidiHook(hook);
	GetConfig()->Save();
}
void PluginWindow::delete_mapping() const
{
	if (ui->table_mapping->rowCount() > 0) {
		auto row = ui->table_mapping->selectedItems().at(0)->row();
		blog(LOG_DEBUG, "selected row to delete %i", row);
		const auto hooks = GetDeviceManager()->get_midi_device(ui->mapping_lbl_device_name->text())->GetMidiHooks();
		for (auto *hook : hooks) {
			if ((hook->channel == ui->sb_channel->value()) && (hook->norc == ui->sb_norc->value()) &&
			    (hook->message_type == ui->cb_mtype->currentText())) {
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
		}
	}
}
void PluginWindow::edit_mapping()
{
	if (ui->table_mapping->rowCount() != 0) {
		editmode = true;
		const auto dv = GetDeviceManager().get()->get_midi_hooks(ui->mapping_lbl_device_name->text());
		blog(LOG_DEBUG, "hook numners: name %s = %i", ui->mapping_lbl_device_name->text().toStdString().c_str(), dv.count());
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
		ui->cb_obs_output_hotkey->setCurrentText(selected_items.at(14)->text());
		ui->check_int_override->setChecked(check);
		ui->sb_int_override->setValue(selected_items.at(11)->text().toInt());
		ui->btn_delete->setEnabled(true);
	}

}

bool PluginWindow::verify_mapping() const
{
	auto results = 0;
	if (ui->cb_obs_output_audio_source->isVisible() && ui->cb_obs_output_audio_source->count() == 0) {
		results++;
	}
	if (ui->cb_obs_output_scene->isVisible() && ui->cb_obs_output_scene->count() == 0) {
		results++;
	}
	if (ui->cb_obs_output_source->isVisible() && ui->cb_obs_output_source->count() == 0) {
		results++;
	}
	if (ui->cb_obs_output_filter->isVisible() && ui->cb_obs_output_filter->count() == 0) {
		results++;
	}
	if (ui->cb_obs_output_transition->isVisible() && ui->cb_obs_output_transition->count() == 0) {
		results++;
	}
	if (ui->cb_obs_output_item->isVisible() && ui->cb_obs_output_item->count() == 0) {
		results++;
	}
	if (ui->cb_obs_output_media_source->isVisible() && ui->cb_obs_output_media_source->count() == 0) {
		results++;
	}
	if (ui->cb_obs_output_hotkey->isVisible() && ui->cb_obs_output_hotkey->count() == 0) {
		results++;
	}
	if (results > 0) {
		return false;
	} else {
		return true;
	}
}
void PluginWindow::on_scene_change(const QString &new_scene) const
{
	if (ui->cb_obs_output_source->isVisible()) {
		ui->cb_obs_output_source->clear();
		ui->cb_obs_output_source->addItems(Utils::get_source_names(new_scene));
	}
}
void PluginWindow::on_source_change(const QString &new_source) const
{
	if (ui->cb_obs_output_filter->isVisible()) {
		ui->cb_obs_output_filter->clear();
		ui->cb_obs_output_filter->addItems(Utils::get_filter_names(new_source));
	}
}
