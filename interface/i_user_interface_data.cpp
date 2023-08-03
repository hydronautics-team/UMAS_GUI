#include "i_user_interface_data.h"

IUserInterfaceData::IUserInterfaceData() : IBasicData()
{

}

ControlData IUserInterfaceData::getControlData() {
    return agent.control;
}
