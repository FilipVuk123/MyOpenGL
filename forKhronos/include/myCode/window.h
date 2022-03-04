#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <stdio.h>

/// This function initializes GLFW and returns 0 on success.
/// IMPORTANT: Use beafore initializing glad!!!
/// @param major_version sets GL major version
/// @param minor_version sets GL minor version
int init_glfw(
    const int major_version,
    const int minor_verion);

// This function sets eroor callback.
// use it with   error_cb as its param
void set_error_cb(
    GLFWerrorfun cb);

void error_cb(int n, const char *errmsg);

// This function returns glproc need for initializing glad loader.
GLFWglproc get_proc_address(
    const char *procname);

// This function creates GLFW window and returns a pointer.
// Returns NULL on failure.
GLFWwindow *create_GLFW_window(
    int width, int height,
    const char *title,
    GLFWmonitor *monitor,
    GLFWwindow *share);

// This function makes a window current -> makes context current for window
void make_window_current(
    GLFWwindow *window);

// This function sets current window to full screen
void GLFW_make_window_full_screen();

// This function sets framebuffer callback
void set_frame_buffer_cb(
    GLFWwindow *window,
    GLFWframebuffersizefun cb);

// This function sets cursor position callback
void set_cursor_position_cb(
    GLFWwindow *window,
    GLFWcursorposfun cb);

// This function sets input mode
void set_input_mode(
    GLFWwindow *window,
    int mode,
    int value);

// This function sets wheel scroll callback
void set_scroll_cb(
    GLFWwindow *window,
    GLFWscrollfun cb);

// This function sets the user-defined pointer of the specified window
void set_window_user_pointer(
    GLFWwindow *window,
    void *ptr);

// This function returns the current value of the user-defined pointer of the specified window. Initial is NULL
void *get_window_user_pointer(
    GLFWwindow *window);

// This function processes only those events that are already in the event queue and then returns immediately.
// Processing events will cause the window and input callbacks associated with those events to be called.
void pool_events();

// This function swaps the front and back buffers of the specified window when rendering
void swap_buffers(
    GLFWwindow *window);

void terminate();

#endif