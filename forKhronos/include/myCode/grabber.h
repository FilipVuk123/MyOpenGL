#ifndef grabber_h
#define grabber_h

#include "KAYA/KYFGLib.h"

int connect_to_grabber(
    FGHANDLE *handle,
    unsigned int grabberIndex);

void close_grabbers(
    FGHANDLE *handle);

int set_grabber_value_int(
    FGHANDLE handle,
    const char *paramName,
    int64_t value);

void grabber_get_info();

int set_grabber_value_enum(
    FGHANDLE handle,
    const char *paramName,
    int64_t value);

int set_grabber_value_float(
    FGHANDLE handle,
    const char *paramName,
    double value);

int set_grabber_value_enum_by_value_name(
    FGHANDLE handle,
    const char *paramName,
    const char *paramValueName);

int64_t get_grabber_value_int(
    FGHANDLE handle,
    const char *paramName);

int64_t get_grabber_value_enum(
    FGHANDLE handle,
    const char *paramName);

double get_grabber_value_float(
    FGHANDLE handle,
    const char *paramName);
#endif //  grabber_h
