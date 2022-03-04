#include "myCode/camera.h"

int camera_update_list(FGHANDLE handle, CAMHANDLE *pCamHandleArray, int *pArraySize)
{
    return KYFG_UpdateCameraList(handle, pCamHandleArray, pArraySize);
}

int camera_start(CAMHANDLE camHandle, STREAM_HANDLE streamHandle, int frames)
{
    return KYFG_CameraStart(camHandle, streamHandle, frames);
}

int camera_open(CAMHANDLE camHandle)
{
    return KYFG_CameraOpen2(camHandle, NULL);
}
int camera_open_with_XML(CAMHANDLE camHandle, const char *xml_file_path)
{
    return KYFG_CameraOpen2(camHandle, xml_file_path);
}

int set_camera_value_int(FGHANDLE handle, const char *paramName, int64_t value)
{
    return KYFG_SetCameraValueInt(handle, paramName, value);
}

int camera_stop(CAMHANDLE camHandle)
{
    return KYFG_CameraStop(camHandle);
}

int set_camera_value_enum(FGHANDLE handle, const char *paramName, int64_t value)
{
    return KYFG_SetCameraValueEnum(handle, paramName, value);
}

int set_camera_value_float(FGHANDLE handle, const char *paramName, double value)
{
    return KYFG_SetCameraValueFloat(handle, paramName, value);
}
int set_camera_value_enum_by_value_name(FGHANDLE handle, const char *paramName, const char *paramValueName)
{
    return KYFG_SetCameraValueEnum_ByValueName(handle, paramName, paramValueName);
}

int64_t get_camera_value_int(FGHANDLE handle, const char *paramName)
{
    return KYFG_GetCameraValueInt(handle, paramName);
}

int64_t get_camera_value_enum(FGHANDLE handle, const char *paramName)
{
    return KYFG_GetCameraValueEnum(handle, paramName);
}

double get_camera_value_float(FGHANDLE handle, const char *paramName)
{
    return KYFG_GetCameraValueFloat(handle, paramName);
}
