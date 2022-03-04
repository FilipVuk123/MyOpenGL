#define STB_IMAGE_IMPLEMENTATION

#define MAXBOARDS 4
#define BUFSIZE 1024
#define PORT 8000

#include "KAYA/KYFGLib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h> // inet_addr
#include <unistd.h>
#include <netinet/in.h>

/*****************opengl***********************/
#include <cglm/cglm.h>
#include "json.h"
#include "myCode/opengl.h"

#include "myCode/window.h"
#include "myCode/grabber.h"
#include "myCode/camera.h"

// screen resolution
const GLuint SCR_WIDTH = 1920;
const GLuint SCR_HEIGHT = 1080;

const GLuint texWidth = 2048;
const GLuint texHeight = 1536;
/**********************************************/

int FLAG = 0;

uint8_t *pFrameMemory = NULL;

// void* mappedBuffer;

const float exposureTime = 9700.0;
const float analogBlackLevel = 20.0;
const float analogGainLevel = 1.50;

// CCM
const float RR = 1.0;
const float RG = 0.0;
const float RB = 0.0;
const float R00 = 0.0;
const float GR = 0.0;
const float GG = 1.0;
const float GB = 0.0;
const float G00 = 0.0;
const float BR = 0.0;
const float BG = 0.0;
const float BB = 2.0;
const float B00 = 0;

static void Stream_callback_func(STREAM_BUFFER_HANDLE streamBufferHandle, void *userContext);
static void processInput(GLFWwindow *window);
static int KY_init();
static void first_cam_setup(FGHANDLE handle, CAMHANDLE camHandle, int grabberIndex, int cameraIndex);

const GLfloat vertices[] = {
        // pisitions         // texture coords
         0.9f,  0.9f, 0.0f,  1.0f, 1.0f, // top right vertex
         0.9f, -0.9f, 0.0f,  1.0f, 0.0f, // bottom right vertex
        -0.9f, -0.9f, 0.0f,  0.0f, 0.0f, // bottom left vertex
        -0.9f,  0.9f, 0.0f,  0.0f, 1.0f  // top left vertex
};
const GLuint indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
};

int main()
{
    /*************opengl*****************/
    set_error_cb(error_cb);
    if (init_glfw(4, 6))
        return -1;
    GLFWwindow *window = create_GLFW_window(SCR_WIDTH, SCR_HEIGHT, "One Cam API", NULL, NULL); // glfw window object creation
    if (window == NULL)
        return -1;
    glfwMakeContextCurrent(window);
    if (!load_glad((GLADloadproc)get_proc_address))
    { // glad: load all OpenGL function pointers. GLFW gives us glfwGetProcAddress that defines the correct function based on which OS we're compiling for
        fprintf(stderr, "In file: %s, line: %d Failed to create initialize GLAD\n", __FILE__, __LINE__);
        glfwTerminate();
        return -1;
    }

    GLuint *tex = create_textures(3);
    bind_texture(tex[0]);

    GLuint shaders[2];

    shaders[0] = load_shader_from_file("./shaders/vertexShader.vert", GL_VERTEX_SHADER);
    shaders[1] = load_shader_from_file("./shaders/fragmentShader.frag", GL_FRAGMENT_SHADER);
    GLuint shaderProgram = create_program(shaders, 2);
    use_program(shaderProgram);

    GLuint posLoc = get_attrib_location(shaderProgram, "aPos");
    GLuint texLoc = get_attrib_location(shaderProgram, "aTexCoord");

    GLuint *VAOs = generate_VAOs(3);
    GLuint *VBOs = generate_VBOs(3);
    GLuint *EBOs = generate_EBOs(3);

    bind_VAOs(VAOs[0]);
    bind_buffer_set_data(GL_ARRAY_BUFFER, VBOs[0], sizeof(vertices), vertices, GL_STATIC_DRAW);
    bind_buffer_set_data(GL_ELEMENT_ARRAY_BUFFER, EBOs[0], sizeof(indices), indices, GL_STATIC_DRAW);
    enable_vertex_attrib_array(posLoc, 3, GL_FLOAT, 5 * sizeof(float), (float *)0);
    enable_vertex_attrib_array(texLoc, 2, GL_FLOAT, 5 * sizeof(float), (void *)(3 * sizeof(float)));

    GLuint *streamBuffers = calloc(10, sizeof(GLuint));
    GLsizeiptr mappedBufferSize = texWidth * texHeight * 3;
    glGenBuffers(10, streamBuffers);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, streamBuffers[0]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, mappedBufferSize, NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, streamBuffers[1]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, mappedBufferSize, NULL, GL_DYNAMIC_DRAW);

    
    /************************************/
    int ret;
    int grabberIndex = 0;
    int cameraIndex = 0;
    FGHANDLE handle[MAXBOARDS] = {INVALID_FGHANDLE};
    STREAM_HANDLE streamHandle = INVALID_STREAMHANDLE;
    CAMHANDLE camHandleArray[MAXBOARDS][KY_MAX_CAMERAS];
    int detectedCameras[MAXBOARDS];

    KY_init();
    grabber_get_info();

    printf("Selected grabber #%d\n", grabberIndex);
    connect_to_grabber(&handle[0], grabberIndex);

    int detectionCount = sizeof(camHandleArray) / sizeof(camHandleArray[0]);

    ret = camera_update_list(handle[grabberIndex], camHandleArray[grabberIndex], &detectionCount);
    printf(" camera_update_list - %x\n", ret);

    detectedCameras[grabberIndex] = detectionCount;

    printf("Found %d cameras.\n", detectedCameras[grabberIndex]);

    if (FGSTATUS_OK == camera_open(camHandleArray[grabberIndex][cameraIndex]))
    {
        first_cam_setup(handle[grabberIndex], camHandleArray[grabberIndex][cameraIndex], grabberIndex, cameraIndex);
    }
    else
    {
        printf("Camera isn't connected\n");
        goto exit;
    }

    // let KYFGLib allocate acquisition buffers
    if (FGSTATUS_OK != KYFG_StreamCreateAndAlloc(camHandleArray[grabberIndex][cameraIndex], &streamHandle, 60, 0))
    {
        printf("Failed to allocate buffer.\n");
    }
    ret = KYFG_StreamBufferCallbackRegister(streamHandle, Stream_callback_func, NULL);
    printf("KYFG_StreamBufferCallbackRegister - %x\n", ret);

    ret = camera_start(camHandleArray[grabberIndex][cameraIndex], streamHandle, 0);
    printf("KYFG_CameraStart - %x\n", ret);
    printf("\nRecording...\n");
    printf("\nOpenGL...\n");


    /****************opengl***********************/

    generate_texture_from_buffer(GL_TEXTURE_2D, GL_RGB, texWidth, texHeight, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    GLuint PBOindex = 0;
    GLuint numOfBuffers = 2;

    while (!glfwWindowShouldClose(window))
    { // render loop
        processInput(window);
        clear_color_buffer(0.2f, 0.2f, 0.2f, 1.0f);
        clear_buffer(GL_COLOR_BUFFER_BIT); // | GL_DEPTH_BUFFER_BIT);
        if (FLAG == 1)
        {
            bind_texture(tex[0]);
            PBOindex = PBOindex % numOfBuffers;
            void *mappedBuffer = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
            memcpy(mappedBuffer, pFrameMemory, texWidth * texHeight * 3);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, streamBuffers[PBOindex]);
            glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
            update_texture_from_buffer(GL_TEXTURE_2D, 0, 0, texWidth, texHeight, GL_RGB, GL_UNSIGNED_BYTE, (void *)0);
            bind_vertex_object_and_draw_it(VAOs[0], GL_TRIANGLES, 6);

            FLAG = 0;
        }
        swap_buffers(window);
        pool_events();

        // printf("%f, %f, %f, %f\n",cam.resultQuat[0], cam.resultQuat[1], cam.resultQuat[2], cam.resultQuat[3]);
    }
    printf("\nExiting...\n");
    ret = camera_stop(camHandleArray[grabberIndex][cameraIndex]);
    printf("\nKYFG_CameraStop - %x\n", ret);
exit:
    close_grabbers(&handle[0]);

    // deallocating stuff
    delete_VAOs(3, VAOs);
    delete_textures(1, tex);
    delete_buffers(3, VBOs);
    delete_buffers(3, EBOs);
    delete_program(shaderProgram);
    terminate(); // glfw: terminate, clearing all previously allocated GLFW resources.
    /*********************************************/

    return 0;
}

static void Stream_callback_func(STREAM_BUFFER_HANDLE streamBufferHandle, void *userContext)
{

    if (!streamBufferHandle)
    {
        // this callback indicates that acquisition has stopped
        return;
    }
    // as a minimum, application may want to get pointer to current frame memory and/or its numerical ID
    KYFG_BufferGetInfo(streamBufferHandle, KY_STREAM_BUFFER_INFO_BASE, &pFrameMemory, NULL, NULL);

    FLAG = 1;
}

static int KY_init()
{
    KYFGLib_InitParameters kyInit;

    memset(&kyInit, 0, sizeof(kyInit));
    kyInit.version = 1;

    if (FGSTATUS_OK != KYFGLib_Initialize(&kyInit))
    {
        printf("Library initialization failed \n ");
        return 1;
    }
    return 0;
}

static void first_cam_setup(FGHANDLE handle, CAMHANDLE camHandle, int grabberIndex, int cameraIndex)
{
    int ret;
    printf("\nCamera Setup starts here!\n");

    ret = set_camera_value_int(camHandle, "Width", texWidth); // sets camera width
    printf("SET 'Width' - %x\n", ret);

    ret = set_camera_value_int(camHandle, "Height", texHeight); // sets camera height
    printf("SET 'Height' - %x\n", ret);

    ret = set_camera_value_enum_by_value_name(camHandle, "PixelFormat", "BayerRG8"); // sets format
    printf("SET 'PixelFormat' - %x\n", ret);

    ret = set_camera_value_float(camHandle, "ExposureTime", exposureTime);
    printf("SET 'ExposureTime' - %x\n", ret);

    ret = set_camera_value_float(camHandle, "AnalogBlackLevel", analogBlackLevel);
    printf("SET 'AnalogBlackLevel' - %x\n", ret);

    ret = set_camera_value_float(camHandle, "AnalogGainLevel", analogGainLevel);
    printf("SET 'AnalogGainLevel' - %x\n", ret);

    ret = set_camera_value_enum_by_value_name(camHandle, "AcquisitionMode", "Continuous");
    printf("SET 'AcquisitionMode' - %x\n", ret);

    ret = set_camera_value_enum_by_value_name(camHandle, "BalanceWhiteAuto", "Off");
    printf("SET 'BalanceWhiteAuto' - %x\n", ret);

    ret = set_camera_value_enum_by_value_name(camHandle, "BlackLevelAuto", "Off");
    printf("SET 'BlackLevelAuto' - %x\n", ret);

    int64_t width = 0, height = 0, totalFrames = 0;
    double exposureTime;
    width = get_camera_value_int(camHandle, "Width");                 // 2048
    height = get_camera_value_int(camHandle, "Height");               // 1536
    totalFrames = get_camera_value_int(camHandle, "RXFrameCounter");  // Returns max int for 64bit -> 2^63 - 1 = 9223372036854775807
    exposureTime = get_camera_value_float(camHandle, "ExposureTime"); // 1536
    printf("Callback of buffer %X, width: %ld, height: %ld, total frames acquired: %ld\n", camHandle, width, height, totalFrames);
    printf("Exposure time = %f\n", exposureTime);

    printf("\nGrabber Setup starts here!\n");

    ret = get_grabber_value_int(handle, "CameraSelector"); // sets selector
    if (ret != 0)
    {
        ret = set_grabber_value_int(handle, "CameraSelector", 0);
        printf("SET 'CameraSelector' - %x\n", ret);
    }

    ret = set_grabber_value_enum_by_value_name(handle, "PixelFormat", "RGB8"); // sets RGB8
    printf("SET 'PixelFormat' - %x\n", ret);

    ret = set_grabber_value_enum(handle, "DebayerMode", 0);
    printf("SET 'DebayerMode' - %x\n", ret);

    ret = set_grabber_value_float(handle, "ColorTransformationRR", RR);
    printf("SET 'ColorTransformationRR' - %x\n", ret);

    ret = set_grabber_value_float(handle, "ColorTransformationGG", GG);
    printf("SET 'ColorTransformationGG' - %x\n", ret);

    ret = set_grabber_value_float(handle, "ColorTransformationBB", BB);
    printf("SET 'ColorTransformationBB' - %x\n", ret);

    ret = set_grabber_value_float(handle, "ColorTransformationRG", RG);
    printf("SET 'ColorTransformationRG' - %x\n", ret);

    ret = set_grabber_value_float(handle, "ColorTransformationRB", RB);
    printf("SET 'ColorTransformationRB' - %x\n", ret);

    ret = set_grabber_value_float(handle, "ColorTransformationGR", GR);
    printf("SET 'ColorTransformationGR' - %x\n", ret);

    ret = set_grabber_value_float(handle, "ColorTransformationGB", GB);
    printf("SET 'ColorTransformationGB' - %x\n", ret);

    ret = set_grabber_value_float(handle, "ColorTransformationBR", BR);
    printf("SET 'ColorTransformationBR' - %x\n", ret);

    ret = set_grabber_value_float(handle, "ColorTransformationBG", BG);
    printf("SET 'ColorTransformationBG' - %x\n", ret);

    ret = set_grabber_value_float(handle, "ColorTransformationR0", R00);
    printf("SET 'ColorTransformationR0' - %x\n", ret);

    ret = set_grabber_value_float(handle, "ColorTransformationB0", B00);
    printf("SET 'ColorTransformationB0' - %x\n", ret);

    ret = set_grabber_value_float(handle, "ColorTransformationG0", G00);
    printf("SET 'ColorTransformationG0' - %x\n", ret);

    printf("\nGET GREBBER VALUES!\n");
    width = 0, height = 0, totalFrames = 0;
    int64_t debayerMode = 0;
    double ColorTransformationRR, ColorTransformationGG, ColorTransformationBB;
    totalFrames = get_grabber_value_int(handle, "RXFrameCounter");
    height = get_grabber_value_int(handle, "Height");
    width = get_grabber_value_int(handle, "Width");
    ColorTransformationRR = get_grabber_value_float(handle, "ColorTransformationRR");
    ColorTransformationGG = get_grabber_value_float(handle, "ColorTransformationGG");
    ColorTransformationBB = get_grabber_value_float(handle, "ColorTransformationBB");
    debayerMode = get_grabber_value_int(handle, "DebayerMode");

    printf("Callback of buffer %X, width: %ld, height: %ld, total frames acquired: %ld\n", handle, width, height, totalFrames);
    printf("DebayerMode = %ld, RR = %f, GG = %f, BB = %f\n", debayerMode, ColorTransformationRR, ColorTransformationGG, ColorTransformationBB);
    printf("\n");
}

static void processInput(GLFWwindow *window){ // keeps all the input code
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {// closes window on ESC
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}