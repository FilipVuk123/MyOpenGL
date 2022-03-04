#ifndef camera_h
#define camera_h

#include "KAYA/KYFGLib.h"

int camera_open(
    CAMHANDLE camHandle);

int camera_open_with_XML(
    CAMHANDLE camHandle,
    const char *xml_file_path);

int camera_update_list(
    FGHANDLE handle,
    CAMHANDLE *pCamHandleArray,
    int *pArraySize);

int camera_start(
    CAMHANDLE camHandle,
    STREAM_HANDLE streamHandle,
    int frames);

int camera_stop(
    CAMHANDLE camHandle);

int set_camera_value_int(
    FGHANDLE handle,
    const char *paramName,
    int64_t value);

int set_camera_value_enum(
    FGHANDLE handle,
    const char *paramName,
    int64_t value);

int set_camera_value_float(
    FGHANDLE handle,
    const char *paramName,
    double value);

int set_camera_value_enum_by_value_name(
    FGHANDLE handle,
    const char *paramName,
    const char *paramValueName);

int64_t get_camera_value_int(
    FGHANDLE handle,
    const char *paramName);

int64_t get_camera_value_enum(
    FGHANDLE handle,
    const char *paramName);

double get_camera_value_float(
    FGHANDLE handle,
    const char *paramName);

#endif //  camera_h
