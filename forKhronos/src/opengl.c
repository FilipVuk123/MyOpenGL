#include "myCode/opengl.h"

GLint load_glad(GLADloadproc load)
{
	return gladLoadGLLoader(load);
}

static GLuint create_shader(FILE *fp, GLenum shader_type)
{
	fseek(fp, 0, SEEK_END);
	long file_size = ftell(fp) + 1;
	char *buffer;
	GLuint shader;
	buffer = (char *)malloc(file_size * sizeof(char));
	fseek(fp, 0, SEEK_SET);
	fread(buffer, file_size, sizeof(char), fp);
	buffer[file_size - 1] = '\0';
	shader = glCreateShader(shader_type);
	const char *shaders_src = buffer;
	glShaderSource(shader, 1, &shaders_src, NULL);
	glCompileShader(shader);

	get_shader_status(shader);
	free(buffer);
	return shader;
}

GLuint create_shader_from_source(GLenum type, GLsizei size, const GLchar **source)
{
	GLuint to_return = glCreateShader(type);
	glShaderSource(to_return, size, source, NULL);
	glCompileShader(to_return);
	return to_return;
}

GLuint load_shader_from_file(const char *filename, GLenum type)
{
	FILE *fp;
	fp = fopen(filename, "rb");
	if (!fp)
	{
		printf("Can not open shader! \n");
		return 0;
	}
	GLuint to_return = create_shader(fp, type);
	fclose(fp);
	return to_return;
}

void get_shader_status(const GLuint shader)
{
	GLint log_size = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);

	if (log_size > 0)
	{
		char *log;
		log = (char *)malloc(log_size);
		glGetShaderInfoLog(shader, log_size, NULL, log);
		printf("%s\n", log);
		free(log);
	}
}

GLuint create_program(GLuint *shaders, unsigned int shader_num)
{
	GLuint program = glCreateProgram();
	for (unsigned int i = 0; i < shader_num; i++)
	{
		glAttachShader(program, shaders[i]);
		glDeleteShader(shaders[i]);
	}

	glLinkProgram(program);
	get_program_status(program);

	return program;
}

void delete_program(GLuint program)
{
	glDeleteProgram(program);
}

void use_program(GLuint program)
{
	glUseProgram(program);
}

GLuint get_attrib_location(GLuint shader, char *name)
{
	return glGetAttribLocation(shader, name);
}

GLuint get_uniform_location(GLuint shader, char *name)
{
	return glGetUniformLocation(shader, name);
}

void get_program_status(const GLuint program)
{
	GLint log_size = 0;
	glGetShaderiv(program, GL_INFO_LOG_LENGTH, &log_size);
	if (log_size > 0)
	{
		char *log;
		log = (char *)malloc(log_size);
		glGetProgramInfoLog(program, log_size, NULL, log);
		printf("%s\n", log);
		free(log);
	}
}

void bind_texture(GLuint texture_id)
{
	glBindTexture(GL_TEXTURE_2D, texture_id);
}

void load_texture_from_file(const char *filename)
{
	texture_t tex;
	tex.data = stbi_load(filename, &tex.width, &tex.height, &tex.nrChannels, 0);
	if (tex.data)
	{
		if ((int)tex.nrChannels == 3)
			generate_texture_from_buffer(GL_TEXTURE_2D, GL_RGB, tex.width, tex.height, GL_RGB, GL_UNSIGNED_BYTE, tex.data);
		else if ((int)tex.nrChannels == 4)
			generate_texture_from_buffer(GL_TEXTURE_2D, GL_RGBA, tex.width, tex.height, GL_RGBA, GL_UNSIGNED_BYTE, tex.data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(tex.data);
	}
	else
	{
		fprintf(stderr, "In file: %s, line: %d Failed to load texture\n", __FILE__, __LINE__);
	}
}

GLuint *create_textures(const int number_of_textures)
{
	GLuint *textures = calloc(number_of_textures, sizeof(GLuint));
	;
	glGenTextures(number_of_textures, textures);
	return textures;
}

void delete_textures(const int number_of_textures, GLuint *textures)
{
	glDeleteTextures(number_of_textures, textures);
}

void enable_vertex_attrib_array(GLuint location, GLuint size, GLenum type, GLsizei stride, const GLvoid *ptr)
{
	glVertexAttribPointer(location, size, type, GL_FALSE, stride, ptr);
	glEnableVertexAttribArray(location);
}

void bind_VAOs(GLuint VAO)
{
	glBindVertexArray(VAO);
}

void draw_elements(GLenum type, GLsizei n)
{
	glDrawElements(type, n, GL_UNSIGNED_INT, 0);
}

void bind_buffer(GLenum type, GLuint buffer)
{
	glBindBuffer(type, buffer);
}

void set_buffer_data(GLenum type, GLsizeiptr size, const GLvoid *data, GLenum usage)
{
	glBufferData(type, size, data, usage);
}

void generate_buffers(const int n, GLuint *buffers)
{
	glGenBuffers(n, buffers);
}

GLuint *generate_VBOs(GLsizei n)
{
	GLuint *buffer = calloc(n, sizeof(GLuint));
	generate_buffers(n, buffer);
	return buffer;
}

GLuint *generate_VAOs(GLsizei n)
{
	GLuint *arrays = calloc(n, sizeof(GLuint));
	glGenVertexArrays(n, arrays);
	return arrays;
}

GLuint *generate_EBOs(GLsizei n)
{
	GLuint *buffer = calloc(n, sizeof(GLuint));
	generate_buffers(n, buffer);
	return buffer;
}

void delete_buffers(GLsizei n, const GLuint *buffer)
{
	glDeleteBuffers(n, buffer);
}

void delete_VAOs(GLsizei n, const GLuint *arrays)
{
	glDeleteVertexArrays(n, arrays);
}

void send_shander_4x4_matrix(GLint location, GLsizei n, GLfloat *matrix)
{
	glUniformMatrix4fv(location, n, GL_FALSE, matrix);
}

void clear_color_buffer(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	glClearColor(r, g, b, a);
}
void clear_buffer(GLbitfield mask)
{
	glClear(mask);
}
void clear_depth_buffer(GLclampd depth)
{
	glClearDepth(depth);
}

void bind_buffer_set_data(GLenum type, GLuint buffer, GLsizeiptr size, const GLvoid *data, GLenum usage)
{
	bind_buffer(type, buffer);
	set_buffer_data(type, size, data, usage);
}

void bind_vertex_object_and_draw_it(GLuint vao, GLenum type, GLsizei n)
{
	bind_VAOs(vao);
	draw_elements(type, n);
}

void set_viewport(GLint x, GLint y, GLsizei witdh, GLsizei height)
{
	glViewport(x, y, witdh, height);
}

void generate_texture_from_buffer(GLenum target, GLint internal_format, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *buffer)
{
	glTexImage2D(target, 0, internal_format, width, height, 0, format, type, buffer);
	glGenerateMipmap(target);
}

void update_texture_from_buffer(GLenum target, GLuint xoffset, GLuint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *buffer)
{
	glTexSubImage2D(target, 0, xoffset, yoffset, width, height, format, type, buffer);
	glGenerateMipmap(target);
}