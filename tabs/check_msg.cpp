#include "check_msg.h"
#include "ui_check_msg.h"

CheckMsg::CheckMsg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CheckMsg)
{
    ui->setupUi(this);
}

void CheckMsg::updateUi_checkMsg()
{
    power_Mode pMode = uv_interface->getPowerMode();
    bool modeSelection = uv_interface->getModeSelection();

    ControlContoursFlags controlContoursFlags = uv_interface->getControlContoursFlags();
    ControlData control = uv_interface->getControlData();
    AUVCurrentData auvData = uv_interface->getAUVCurrentData();
    FlagAH127C_bort flagAH127C_bort = uv_interface->getFlagAH127C_bort();
    FlagAH127C_pult flagAH127C_pult = uv_interface->getFlagAH127C_pult();
    int checksum_msg_gui_send = uv_interface->getChecksumMsgGuiSend();
    int checksum_msg_agent_send = uv_interface->getChecksumMsgAgentSend();
    int checksum_msg_gui_received = uv_interface->getChecksumMsgGuiReceived();

//    send

    ui->label_tab_setupMsg_send_powerMode_count->setNum(2+static_cast<int>(pMode));

    if (modeSelection == 1)
        ui->label_tab_setupMsg_send_modeAUV_selection_mode->setText("модель");
    else
        ui->label_tab_setupMsg_send_modeAUV_selection_mode->setText("агент");

    switch (uv_interface->getCSMode()) {
    case e_CSMode::MODE_MANUAL:
        ui->label_tab_setupMsg_send_cSMode_count->setText("ручной");
        break;
    case e_CSMode::MODE_AUTOMATED:
        ui->label_tab_setupMsg_send_cSMode_count->setText("автоматизированный");
        break;
    case e_CSMode::MODE_AUTOMATIC:
        ui->label_tab_setupMsg_send_cSMode_count->setText("автоматический");
        break;
    }

    if (controlContoursFlags.yaw) {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_yaw->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_yaw->setText("незамкнут");
    }

    if (controlContoursFlags.pitch) {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_pitch->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_pitch->setText("незамкнут");
    }

    if (controlContoursFlags.roll) {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_roll->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_roll->setText("незамкнут");
    }

    if (controlContoursFlags.march) {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_march->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_march->setText("незамкнут");
    }

    if (controlContoursFlags.lag) {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_lag->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_send_controlContoursFlags_data_flags_lag->setText("незамкнут");
    }

    ui->label_tab_setupMsg_send_Impact_data_count_yaw->setNum(control.yaw);
    ui->label_tab_setupMsg_send_Impact_data_count_pitch->setNum(control.pitch);
    ui->label_tab_setupMsg_send_Impact_data_count_roll->setNum(control.roll);
    ui->label_tab_setupMsg_send_Impact_data_count_march->setNum(control.march);
    ui->label_tab_setupMsg_send_Impact_data_count_lag->setNum(control.lag);
    ui->label_tab_setupMsg_send_Impact_data_count_depth->setNum(control.depth);

//    received

    if (auvData.modeAUV_Real == 1)
        ui->labelt_tab_setupMsg_received_modeAUV_selection_mode->setText("модель");
    else
        ui->labelt_tab_setupMsg_received_modeAUV_selection_mode->setText("агент");

    if (auvData.modeReal) {
        ui->label_tab_setupMsg_received_cSMode_count->setText("автоматизированный");
    } else {
        ui->label_tab_setupMsg_received_cSMode_count->setText("ручной");
    }

    if (auvData.controlReal.yaw) {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_yaw->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_yaw->setText("незамкнут");
    }

    if (auvData.controlReal.pitch) {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_pitch->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_pitch->setText("незамкнут");
    }

    if (auvData.controlReal.roll) {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_roll->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_roll->setText("незамкнут");
    }

    if (auvData.controlReal.march) {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_march->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_march->setText("незамкнут");
    }

    if (auvData.controlReal.lag) {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_lag->setText("замкнут");
    } else {
        ui->label_tab_setupMsg_received_controlContoursFlags_data_flags_lag->setText("незамкнут");
    }

    ui->label_tab_setupMsg_received_Impact_data_count_vma1->setNum(auvData.signalVMA_real.VMA1);
    ui->label_tab_setupMsg_received_Impact_data_count_vma2->setNum(auvData.signalVMA_real.VMA2);
    ui->label_tab_setupMsg_received_Impact_data_count_vma3->setNum(auvData.signalVMA_real.VMA3);
    ui->label_tab_setupMsg_received_Impact_data_count_vma4->setNum(auvData.signalVMA_real.VMA4);

//    флаги для настройки БСО

    ui->label_tab_setupMsg_flagsSetupIMU_pult_init->setNum(flagAH127C_pult.initCalibration);
    ui->label_tab_setupMsg_flagsSetupIMU_pult_save->setNum(flagAH127C_pult.saveCalibration);
    ui->label_tab_setupMsg_flagsSetupIMU_bort_end->setNum(flagAH127C_bort.startCalibration);
    ui->label_tab_setupMsg_flagsSetupIMU_bort_start->setNum(flagAH127C_bort.endCalibration);

//    количество посылок

    ui->label_tab_setupMsg_send_checksum_count->setNum(checksum_msg_gui_send);
    ui->label_tab_setupMsg_received_checksum_send_count->setNum(checksum_msg_agent_send);
    ui->labe_tab_setupMsg_received_checksum_received_count_->setNum(checksum_msg_gui_received);
}

void CheckMsg::slot_getInterface(IUserInterfaceData *interface)
{
    uv_interface = interface;
}

CheckMsg::~CheckMsg()
{
    delete ui;
}
