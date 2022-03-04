#include "myCode/grabber.h"
#include <stdio.h>
#include <string.h>

#define MAXBOARDS 4

int connect_to_grabber(FGHANDLE *handle, unsigned int grabberIndex);
void close_grabbers(FGHANDLE *handle);

int set_grabber_value_int(FGHANDLE handle, const char *paramName, int64_t value)
{
    return KYFG_SetGrabberValueInt(handle, paramName, value);
}

int set_grabber_value_enum(FGHANDLE handle, const char *paramName, int64_t value)
{
    return KYFG_SetGrabberValueEnum(handle, paramName, value);
}

void grabber_get_info()
{
    int infosize = 0;
    KY_DeviceScan(&infosize); // Retrieve the number of virtual and hardware devices connected to PC
    printf("Number of devices found: %d\n", infosize);

    for (int deviceIndex = 0; deviceIndex < infosize; deviceIndex++)
    {
        KY_DEVICE_INFO deviceInfo;
        memset(&deviceInfo, 0, sizeof(KY_DEVICE_INFO));
        deviceInfo.version = KY_MAX_DEVICE_INFO_VERSION;
        if (FGSTATUS_OK != KY_DeviceInfo(deviceIndex, &deviceInfo))
        {
            printf("wasn't able to retrive information from device #%d\n", deviceIndex);
            continue;
        }
        printf("[%d] %s on PCI slot {%d:%d:%d}: Protocol 0x%X, Generation %d\n", deviceIndex,
               deviceInfo.szDeviceDisplayName,
               deviceInfo.nBus, deviceInfo.nSlot, deviceInfo.nFunction,
               deviceInfo.m_Protocol, deviceInfo.DeviceGeneration);
    }
}

int set_grabber_value_float(FGHANDLE handle, const char *paramName, double value)
{
    return KYFG_SetGrabberValueFloat(handle, paramName, value);
}
int set_grabber_value_enum_by_value_name(FGHANDLE handle, const char *paramName, const char *paramValueName)
{
    return KYFG_SetGrabberValueEnum_ByValueName(handle, paramName, paramValueName);
}

int64_t get_grabber_value_int(FGHANDLE handle, const char *paramName)
{
    return KYFG_GetGrabberValueInt(handle, paramName);
}

int64_t get_grabber_value_enum(FGHANDLE handle, const char *paramName)
{
    return KYFG_GetGrabberValueEnum(handle, paramName);
}

double get_grabber_value_float(FGHANDLE handle, const char *paramName)
{
    return KYFG_GetGrabberValueFloat(handle, paramName);
}

int connect_to_grabber(FGHANDLE *handle, unsigned int grabberIndex)
{
    KY_DEVICE_INFO deviceInfo;
    memset(&deviceInfo, 0, sizeof(KY_DEVICE_INFO));
    deviceInfo.version = KY_MAX_DEVICE_INFO_VERSION;
    if (FGSTATUS_OK != KY_DeviceInfo(grabberIndex, &deviceInfo))
    {
        printf("wasn't able to retrive information from device #%d\n", grabberIndex);
        return -1;
    }
    if (0 == (KY_DEVICE_STREAM_GRABBER & deviceInfo.m_Flags))
    {
        printf("sorry, select device #%d is not a grabber\n", grabberIndex);
        return -1;
    }

    if ((handle[grabberIndex] = KYFG_Open(grabberIndex)) > 0) // connect to selected device
    {
        printf("Good connection to grabber #%d, handle=%X\n", grabberIndex, handle[grabberIndex]);
        return 0;
    }
    else
    {
        printf("Could not connect to grabber #%d\n", grabberIndex);
        return -1;
    }
}

void close_grabbers(FGHANDLE *handle)
{
    int infosize = 0;
    KY_DeviceScan(&infosize);
    for (int grabberIndex = 0; grabberIndex < infosize; grabberIndex++)
    {
        if (INVALID_FGHANDLE != handle[grabberIndex])
        {
            if (FGSTATUS_OK != KYFG_Close(handle[grabberIndex])) // Close the selected device and unregisters all associated routines
            {
                printf("wasn't able to close grabber #%d\n", grabberIndex);
            }
            else
            {
                printf("Grabber #%d Closed!\n", grabberIndex);
            }
        }
        else
        {
            printf("grabber #%d wasn't open\n", grabberIndex);
        }
    }
}