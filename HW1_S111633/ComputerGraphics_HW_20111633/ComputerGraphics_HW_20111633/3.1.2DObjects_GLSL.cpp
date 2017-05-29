#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, ortho, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0

int win_width = 0, win_height = 0; 
float centerx = 0.0f, centery = 0.0f, rotate_angle = 0.0f;

GLfloat axes[4][2];
GLfloat axes_color[3] = { 0.0f, 0.0f, 0.0f };
GLuint VBO_axes, VAO_axes;

void prepare_axes(void) { // Draw axes in their MC.
	axes[0][0] = -win_width / 2.5f; axes[0][1] = 0.0f;
	axes[1][0] = win_width / 2.5f; axes[1][1] = 0.0f;
	axes[2][0] = 0.0f; axes[2][1] = -win_height / 2.5f;
	axes[3][0] = 0.0f; axes[3][1] = win_height / 2.5f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_axes(void) {
	axes[0][0] = -win_width / 2.25f; axes[1][0] = win_width / 2.25f; 
	axes[2][1] = -win_height / 2.25f;
	axes[3][1] = win_height / 2.25f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_axes(void) {
	glUniform3fv(loc_primitive_color, 1, axes_color);
	glBindVertexArray(VAO_axes);
	glDrawArrays(GL_LINES, 0, 4);
	glBindVertexArray(0);
}

GLfloat line[2][2];
GLfloat line_color[3] = { 1.0f, 0.0f, 0.0f };
GLuint VBO_line, VAO_line;

void prepare_line(void) { 	// y = x - win_height/4
	//line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height; 
	//line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	//line[1][0] = win_width / 2.5f; 
	//line[1][1] = win_width / 2.5f - win_height / 4.0f;

	// HW
	line[0][0] = 0;
	line[0][1] = 0;
	line[1][0] = 0;
	line[1][1] = 0;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_line);
	glBindVertexArray(VAO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_line(void) { 	// y = x - win_height/4
/*	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height; 
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f; 
	line[1][1] = win_width / 2.5f - win_height / 4.0f;*/

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_line(void) { // Draw line in its MC.
	// y = x - win_height/4
	glUniform3fv(loc_primitive_color, 1, line_color);
	glBindVertexArray(VAO_line);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

#define AIRPLANE_BIG_WING 0
#define AIRPLANE_SMALL_WING 1
#define AIRPLANE_BODY 2
#define AIRPLANE_BACK 3
#define AIRPLANE_SIDEWINDER1 4
#define AIRPLANE_SIDEWINDER2 5
#define AIRPLANE_CENTER 6
GLfloat big_wing[6][2] = { { 0.0, 0.0 }, { -20.0, 15.0 }, { -20.0, 20.0 }, { 0.0, 23.0 }, { 20.0, 20.0 }, { 20.0, 15.0 } };
GLfloat small_wing[6][2] = { { 0.0, -18.0 }, { -11.0, -12.0 }, { -12.0, -7.0 }, { 0.0, -10.0 }, { 12.0, -7.0 }, { 11.0, -12.0 } };
GLfloat body[5][2] = { { 0.0, -25.0 }, { -6.0, 0.0 }, { -6.0, 22.0 }, { 6.0, 22.0 }, { 6.0, 0.0 } };
GLfloat back[5][2] = { { 0.0, 25.0 }, { -7.0, 24.0 }, { -7.0, 21.0 }, { 7.0, 21.0 }, { 7.0, 24.0 } };
GLfloat sidewinder1[5][2] = { { -20.0, 10.0 }, { -18.0, 3.0 }, { -16.0, 10.0 }, { -18.0, 20.0 }, { -20.0, 20.0 } };
GLfloat sidewinder2[5][2] = { { 20.0, 10.0 }, { 18.0, 3.0 }, { 16.0, 10.0 }, { 18.0, 20.0 }, { 20.0, 20.0 } };
GLfloat center[1][2] = { { 0.0, 0.0 } };
GLfloat airplane_color[7][3] = {
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // big_wing
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // small_wing
	{ 111 / 255.0f,  85 / 255.0f, 157 / 255.0f },  // body
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // back
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder1
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder2
	{ 255 / 255.0f,   0 / 255.0f,   0 / 255.0f }   // center
};

GLuint VBO_airplane, VAO_airplane;

int airplane_clock = 0;
float airplane_s_factor = 1.0f;

void prepare_airplane() {
	GLsizeiptr buffer_size = sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back)
		+sizeof(sidewinder1)+sizeof(sidewinder2)+sizeof(center);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(big_wing), big_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing), sizeof(small_wing), small_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing), sizeof(body), body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body), sizeof(back), back);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back),
		sizeof(sidewinder1), sidewinder1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back)
		+sizeof(sidewinder1), sizeof(sidewinder2), sidewinder2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing)+sizeof(small_wing)+sizeof(body)+sizeof(back)
		+sizeof(sidewinder1)+sizeof(sidewinder2), sizeof(center), center);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_airplane);
	glBindVertexArray(VAO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_airplane() { // Draw airplane in its MC.
	glBindVertexArray(VAO_airplane);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BIG_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SMALL_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BACK]);
	glDrawArrays(GL_TRIANGLE_FAN, 17, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER1]);
	glDrawArrays(GL_TRIANGLE_FAN, 22, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER2]);
	glDrawArrays(GL_TRIANGLE_FAN, 27, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_CENTER]);
	glPointSize(5.0);
	glDrawArrays(GL_POINTS, 32, 1);
	glPointSize(1.0);
	glBindVertexArray(0);
}

//shirt
#define SHIRT_LEFT_BODY 0
#define SHIRT_RIGHT_BODY 1
#define SHIRT_LEFT_COLLAR 2
#define SHIRT_RIGHT_COLLAR 3
#define SHIRT_FRONT_POCKET 4
#define SHIRT_BUTTON1 5
#define SHIRT_BUTTON2 6
#define SHIRT_BUTTON3 7
#define SHIRT_BUTTON4 8
GLfloat left_body[6][2] = { { 0.0, -9.0 }, { -8.0, -9.0 }, { -11.0, 8.0 }, { -6.0, 10.0 }, { -3.0, 7.0 }, { 0.0, 9.0 } };
GLfloat right_body[6][2] = { { 0.0, -9.0 }, { 0.0, 9.0 }, { 3.0, 7.0 }, { 6.0, 10.0 }, { 11.0, 8.0 }, { 8.0, -9.0 } };
GLfloat left_collar[4][2] = { { 0.0, 9.0 }, { -3.0, 7.0 }, { -6.0, 10.0 }, { -4.0, 11.0 } };
GLfloat right_collar[4][2] = { { 0.0, 9.0 }, { 4.0, 11.0 }, { 6.0, 10.0 }, { 3.0, 7.0 } };
GLfloat front_pocket[6][2] = { { 5.0, 0.0 }, { 4.0, 1.0 }, { 4.0, 3.0 }, { 7.0, 3.0 }, { 7.0, 1.0 }, { 6.0, 0.0 } };
GLfloat button1[3][2] = { { -1.0, 6.0 }, { 1.0, 6.0 }, { 0.0, 5.0 } };
GLfloat button2[3][2] = { { -1.0, 3.0 }, { 1.0, 3.0 }, { 0.0, 2.0 } };
GLfloat button3[3][2] = { { -1.0, 0.0 }, { 1.0, 0.0 }, { 0.0, -1.0 } };
GLfloat button4[3][2] = { { -1.0, -3.0 }, { 1.0, -3.0 }, { 0.0, -4.0 } };

GLfloat shirt_color[9][3] = {
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f }
};

GLuint VBO_shirt, VAO_shirt;
void prepare_shirt() {
	GLsizeiptr buffer_size = sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar)
		+sizeof(front_pocket)+sizeof(button1)+sizeof(button2)+sizeof(button3)+sizeof(button4);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(left_body), left_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body), sizeof(right_body), right_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body), sizeof(left_collar), left_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar), sizeof(right_collar), right_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar),
		sizeof(front_pocket), front_pocket);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar)
		+sizeof(front_pocket), sizeof(button1), button1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar)
		+sizeof(front_pocket)+sizeof(button1), sizeof(button2), button2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar)
		+sizeof(front_pocket)+sizeof(button1)+sizeof(button2), sizeof(button3), button3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar)
		+sizeof(front_pocket)+sizeof(button1)+sizeof(button2)+sizeof(button3), sizeof(button4), button4);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_shirt);
	glBindVertexArray(VAO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_shirt() {
	glBindVertexArray(VAO_shirt);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_LEFT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_RIGHT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_LEFT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_RIGHT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_FRONT_POCKET]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON1]);
	glDrawArrays(GL_TRIANGLE_FAN, 26, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON2]);
	glDrawArrays(GL_TRIANGLE_FAN, 29, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON3]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON4]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);
	glBindVertexArray(0);
}

//house
#define HOUSE_ROOF 0
#define HOUSE_BODY 1
#define HOUSE_CHIMNEY 2
#define HOUSE_DOOR 3
#define HOUSE_WINDOW 4

GLfloat roof[3][2] = { { -12.0, 0.0 }, { 0.0, 12.0 }, { 12.0, 0.0 } };
GLfloat house_body[4][2] = { { -12.0, -14.0 }, { -12.0, 0.0 }, { 12.0, 0.0 }, { 12.0, -14.0 } };
GLfloat chimney[4][2] = { { 6.0, 6.0 }, { 6.0, 14.0 }, { 10.0, 14.0 }, { 10.0, 2.0 } };
GLfloat door[4][2] = { { -8.0, -14.0 }, { -8.0, -8.0 }, { -4.0, -8.0 }, { -4.0, -14.0 } };
GLfloat window[4][2] = { { 4.0, -6.0 }, { 4.0, -2.0 }, { 8.0, -2.0 }, { 8.0, -6.0 } };

GLfloat house_color[5][3] = {
	{ 200 / 255.0f, 39 / 255.0f, 42 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 233 / 255.0f, 113 / 255.0f, 23 / 255.0f },
	{ 44 / 255.0f, 180 / 255.0f, 49 / 255.0f }
};

GLuint VBO_house, VAO_house;
void prepare_house() {
	GLsizeiptr buffer_size = sizeof(roof)+sizeof(house_body)+sizeof(chimney)+sizeof(door)
		+sizeof(window);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(roof), roof);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof), sizeof(house_body), house_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof)+sizeof(house_body), sizeof(chimney), chimney);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof)+sizeof(house_body)+sizeof(chimney), sizeof(door), door);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof)+sizeof(house_body)+sizeof(chimney)+sizeof(door),
		sizeof(window), window);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_house);
	glBindVertexArray(VAO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_house() {
	glBindVertexArray(VAO_house);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_ROOF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 3, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_CHIMNEY]);
	glDrawArrays(GL_TRIANGLE_FAN, 7, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_DOOR]);
	glDrawArrays(GL_TRIANGLE_FAN, 11, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glBindVertexArray(0);
}

//car
#define CAR_BODY 0
#define CAR_FRAME 1
#define CAR_WINDOW 2
#define CAR_LEFT_LIGHT 3
#define CAR_RIGHT_LIGHT 4
#define CAR_LEFT_WHEEL 5
#define CAR_RIGHT_WHEEL 6

GLfloat car_body[4][2] = { { -16.0, -8.0 }, { -16.0, 0.0 }, { 16.0, 0.0 }, { 16.0, -8.0 } };
GLfloat car_frame[4][2] = { { -10.0, 0.0 }, { -10.0, 10.0 }, { 10.0, 10.0 }, { 10.0, 0.0 } };
GLfloat car_window[4][2] = { { -8.0, 0.0 }, { -8.0, 8.0 }, { 8.0, 8.0 }, { 8.0, 0.0 } };
GLfloat car_left_light[4][2] = { { -9.0, -6.0 }, { -10.0, -5.0 }, { -9.0, -4.0 }, { -8.0, -5.0 } };
GLfloat car_right_light[4][2] = { { 9.0, -6.0 }, { 8.0, -5.0 }, { 9.0, -4.0 }, { 10.0, -5.0 } };
GLfloat car_left_wheel[4][2] = { { -10.0, -12.0 }, { -10.0, -8.0 }, { -6.0, -8.0 }, { -6.0, -12.0 } };
GLfloat car_right_wheel[4][2] = { { 6.0, -12.0 }, { 6.0, -8.0 }, { 10.0, -8.0 }, { 10.0, -12.0 } };

GLfloat car_color[7][3] = {
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 216 / 255.0f, 208 / 255.0f, 174 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f }
};

GLuint VBO_car, VAO_car;
void prepare_car() {
	GLsizeiptr buffer_size = sizeof(car_body)+sizeof(car_frame)+sizeof(car_window)+sizeof(car_left_light)
		+sizeof(car_right_light)+sizeof(car_left_wheel)+sizeof(car_right_wheel);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car_body), car_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body), sizeof(car_frame), car_frame);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body)+sizeof(car_frame), sizeof(car_window), car_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body)+sizeof(car_frame)+sizeof(car_window), sizeof(car_left_light), car_left_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body)+sizeof(car_frame)+sizeof(car_window)+sizeof(car_left_light),
		sizeof(car_right_light), car_right_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body)+sizeof(car_frame)+sizeof(car_window)+sizeof(car_left_light)
		+sizeof(car_right_light), sizeof(car_left_wheel), car_left_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body)+sizeof(car_frame)+sizeof(car_window)+sizeof(car_left_light)
		+sizeof(car_right_light)+sizeof(car_left_wheel), sizeof(car_right_wheel), car_right_wheel);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car);
	glBindVertexArray(VAO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car() {
	glBindVertexArray(VAO_car);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_FRAME]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);

	glBindVertexArray(0);
}

//draw cocktail
#define COCKTAIL_NECK 0
#define COCKTAIL_LIQUID 1
#define COCKTAIL_REMAIN 2
#define COCKTAIL_STRAW 3
#define COCKTAIL_DECO 4

GLfloat neck[6][2] = { { -6.0, -12.0 }, { -6.0, -11.0 }, { -1.0, 0.0 }, { 1.0, 0.0 }, { 6.0, -11.0 }, { 6.0, -12.0 } };
GLfloat liquid[6][2] = { { -1.0, 0.0 }, { -9.0, 4.0 }, { -12.0, 7.0 }, { 12.0, 7.0 }, { 9.0, 4.0 }, { 1.0, 0.0 } };
GLfloat remain[4][2] = { { -12.0, 7.0 }, { -12.0, 10.0 }, { 12.0, 10.0 }, { 12.0, 7.0 } };
GLfloat straw[4][2] = { { 7.0, 7.0 }, { 12.0, 12.0 }, { 14.0, 12.0 }, { 9.0, 7.0 } };
GLfloat deco[8][2] = { { 12.0, 12.0 }, { 10.0, 14.0 }, { 10.0, 16.0 }, { 12.0, 18.0 }, { 14.0, 18.0 }, { 16.0, 16.0 }, { 16.0, 14.0 }, { 14.0, 12.0 } };

GLfloat cocktail_color[5][3] = {
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 0 / 255.0f, 63 / 255.0f, 122 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 191 / 255.0f, 255 / 255.0f, 0 / 255.0f },
	{ 218 / 255.0f, 165 / 255.0f, 32 / 255.0f }
};

GLuint VBO_cocktail, VAO_cocktail;
void prepare_cocktail() {
	GLsizeiptr buffer_size = sizeof(neck)+sizeof(liquid)+sizeof(remain)+sizeof(straw)
		+sizeof(deco);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_cocktail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cocktail);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(neck), neck);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck), sizeof(liquid), liquid);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck)+sizeof(liquid), sizeof(remain), remain);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck)+sizeof(liquid)+sizeof(remain), sizeof(straw), straw);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck)+sizeof(liquid)+sizeof(remain)+sizeof(straw),
		sizeof(deco), deco);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_cocktail);
	glBindVertexArray(VAO_cocktail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cocktail);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cocktail() {
	glBindVertexArray(VAO_cocktail);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_NECK]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_LIQUID]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_REMAIN]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_STRAW]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_DECO]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 8);

	glBindVertexArray(0);
}

//draw car2
#define CAR2_BODY 0
#define CAR2_FRONT_WINDOW 1
#define CAR2_BACK_WINDOW 2
#define CAR2_FRONT_WHEEL 3
#define CAR2_BACK_WHEEL 4
#define CAR2_LIGHT1 5
#define CAR2_LIGHT2 6

GLfloat car2_body[8][2] = { { -18.0, -7.0 }, { -18.0, 0.0 }, { -13.0, 0.0 }, { -10.0, 8.0 }, { 10.0, 8.0 }, { 13.0, 0.0 }, { 18.0, 0.0 }, { 18.0, -7.0 } };
GLfloat car2_front_window[4][2] = { { -10.0, 0.0 }, { -8.0, 6.0 }, { -2.0, 6.0 }, { -2.0, 0.0 } };
GLfloat car2_back_window[4][2] = { { 0.0, 0.0 }, { 0.0, 6.0 }, { 8.0, 6.0 }, { 10.0, 0.0 } };
GLfloat car2_front_wheel[8][2] = { { -11.0, -11.0 }, { -13.0, -8.0 }, { -13.0, -7.0 }, { -11.0, -4.0 }, { -7.0, -4.0 }, { -5.0, -7.0 }, { -5.0, -8.0 }, { -7.0, -11.0 } };
GLfloat car2_back_wheel[8][2] = { { 7.0, -11.0 }, { 5.0, -8.0 }, { 5.0, -7.0 }, { 7.0, -4.0 }, { 11.0, -4.0 }, { 13.0, -7.0 }, { 13.0, -8.0 }, { 11.0, -11.0 } };
GLfloat car2_light1[3][2] = { { -18.0, -1.0 }, { -17.0, -2.0 }, { -18.0, -3.0 } };
GLfloat car2_light2[3][2] = { { -18.0, -4.0 }, { -17.0, -5.0 }, { -18.0, -6.0 } };

GLfloat car2_color[7][3] = {
	{ 100 / 255.0f, 141 / 255.0f, 159 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f }
};

GLuint VBO_car2, VAO_car2;
void prepare_car2() {
	GLsizeiptr buffer_size = sizeof(car2_body)+sizeof(car2_front_window)+sizeof(car2_back_window)+sizeof(car2_front_wheel)
		+sizeof(car2_back_wheel)+sizeof(car2_light1)+sizeof(car2_light2);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car2_body), car2_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body), sizeof(car2_front_window), car2_front_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body)+sizeof(car2_front_window), sizeof(car2_back_window), car2_back_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body)+sizeof(car2_front_window)+sizeof(car2_back_window), sizeof(car2_front_wheel), car2_front_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body)+sizeof(car2_front_window)+sizeof(car2_back_window)+sizeof(car2_front_wheel),
		sizeof(car2_back_wheel), car2_back_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body)+sizeof(car2_front_window)+sizeof(car2_back_window)+sizeof(car2_front_wheel)
		+sizeof(car2_back_wheel), sizeof(car2_light1), car2_light1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body)+sizeof(car2_front_window)+sizeof(car2_back_window)+sizeof(car2_front_wheel)
		+sizeof(car2_back_wheel)+sizeof(car2_light1), sizeof(car2_light2), car2_light2);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car2);
	glBindVertexArray(VAO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car2() {
	glBindVertexArray(VAO_car2);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT1]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT2]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);

	glBindVertexArray(0);
}


// HW
//draw spaceship
#define SPACESHIP_BODY 0
#define SPACESHIP_WINDOW 1
#define SPACESHIP_HEAD 2
#define SPACESHIP_FIRE_CENTER 3
#define SPACESHIP_FIRE_LEFT 4
#define SPACESHIP_FIRE_RIGHT 5
#define SPACESHIP_WING_LEFT 6
#define SPACESHIP_WING_RIGHT 7

#define five_div_root2	3.535534

GLfloat spaceship_body[4][2] = { { 10.0, 17.0 }, { -10.0, 17.0 }, { -10.0, -17.0 }, { 10.0, -17.0 } };
GLfloat spaceship_window[8][2] = { { 5.0, 0 }, { five_div_root2, five_div_root2 }, { 0.0, 5.0 }, { -five_div_root2, five_div_root2 }, { -5.0, 0.0 }, { -five_div_root2, -five_div_root2 }, { 0.0, -5.0 }, { five_div_root2, -five_div_root2 } };
GLfloat spaceship_head[6][2] = { { 10.0, 17.0 }, { 10.0, 18.0 }, { 2.0, 23.0 }, { -2.0, 23.0 }, { -10.0, 18.0 }, { -10.0, 17.0 } };
GLfloat spaceship_fire_center[3][2] = { { 3.0, -17.0 }, { 0, -25.0 }, { -3.0, -17.0 } };
GLfloat spaceship_fire_left[3][2] = { { -3.0, -17.0 }, { -6.5, -23.0 }, { -10.0, -17.0 } };
GLfloat spaceship_fire_right[3][2] = { { 3.0, -17.0 }, { 6.5, -23.0 }, { 10.0, -17.0 } };
GLfloat spaceship_wing_left[3][2] = { { -10.0, 0 }, { -10.0, -10.0 }, {-20.0, -10.0} };
GLfloat spaceship_wing_right[3][2] = { { 10.0, 0 }, { 10.0, -10.0 }, {20.0, -10.0} };

GLfloat spaceship_color[8][3] = {
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 0 / 255.0f, 102 / 255.0f, 204 / 255.0f },
	{ 32 / 255.0f, 32 / 255.0f, 32 / 255.0f },
	{ 205 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 204 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 204 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 32 / 255.0f, 32 / 255.0f, 32 / 255.0f },
	{ 32 / 255.0f, 32 / 255.0f, 32 / 255.0f }
};

GLuint VBO_spaceship, VAO_spaceship;
void prepare_spaceship() {
	GLsizeiptr buffer_size = sizeof(spaceship_body) + sizeof(spaceship_window) + sizeof(spaceship_head) + sizeof(spaceship_fire_center)
		+ sizeof(spaceship_fire_left) + sizeof(spaceship_fire_right) + sizeof(spaceship_wing_left) + sizeof(spaceship_wing_right);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_spaceship);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_spaceship);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spaceship_body), spaceship_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(spaceship_body), sizeof(spaceship_window), spaceship_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(spaceship_body) + sizeof(spaceship_window), sizeof(spaceship_head), spaceship_head);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(spaceship_body) + sizeof(spaceship_window) + sizeof(spaceship_head), sizeof(spaceship_fire_center), spaceship_fire_center);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(spaceship_body) + sizeof(spaceship_window) + sizeof(spaceship_head) + sizeof(spaceship_fire_center),
		sizeof(spaceship_fire_left), spaceship_fire_left);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(spaceship_body) + sizeof(spaceship_window) + sizeof(spaceship_head) + sizeof(spaceship_fire_center)
		+ sizeof(spaceship_fire_left), sizeof(spaceship_fire_right), spaceship_fire_right);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(spaceship_body) + sizeof(spaceship_window) + sizeof(spaceship_head) + sizeof(spaceship_fire_center)
		+ sizeof(spaceship_fire_left) + sizeof(spaceship_fire_right), sizeof(spaceship_wing_left), spaceship_wing_left);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(spaceship_body) + sizeof(spaceship_window) + sizeof(spaceship_head) + sizeof(spaceship_fire_center)
		+ sizeof(spaceship_fire_left) + sizeof(spaceship_fire_right) + sizeof(spaceship_wing_left), sizeof(spaceship_wing_right), spaceship_wing_right);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_spaceship);
	glBindVertexArray(VAO_spaceship);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_spaceship);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_spaceship() {
	glBindVertexArray(VAO_spaceship);

	glUniform3fv(loc_primitive_color, 1, spaceship_color[SPACESHIP_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, spaceship_color[SPACESHIP_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 8);

	glUniform3fv(loc_primitive_color, 1, spaceship_color[SPACESHIP_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 6);

	glUniform3fv(loc_primitive_color, 1, spaceship_color[SPACESHIP_FIRE_CENTER]);
	glDrawArrays(GL_TRIANGLE_FAN, 18, 3);

	glUniform3fv(loc_primitive_color, 1, spaceship_color[SPACESHIP_FIRE_LEFT]);
	glDrawArrays(GL_TRIANGLE_FAN, 21, 3);

	glUniform3fv(loc_primitive_color, 1, spaceship_color[SPACESHIP_FIRE_RIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 3);

	glUniform3fv(loc_primitive_color, 1, spaceship_color[SPACESHIP_WING_LEFT]);
	glDrawArrays(GL_TRIANGLE_FAN, 27, 3);

	glUniform3fv(loc_primitive_color, 1, spaceship_color[SPACESHIP_WING_RIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 30, 3);

	glBindVertexArray(0);
}


#include <math.h>
#include <algorithm>

#define airplane_speed_init	10
#define airplane_speed_max	30
#define airplane_speed_min	1
#define airplane_speed_gap	1
#define airplane_size	1.5f
int airplane_degree = 0;
int airplane_speed = airplane_speed_init;
glm::vec2 airplane_pos = { 0, 0 };


typedef struct _HOUSE_POSITION {
	glm::vec2 pos;
	bool check;
}house_pos;

#define house_posQ_max	10
int house_posQ_now = 0;
house_pos house_posQ[house_posQ_max] = { glm::vec2(0.0f, 0.0f), false };


#define cocktail_size	2
glm::vec2 cocktail_pos = { 0, 0 };
bool cocktail_flag = false;


#include <queue>
typedef struct _CAR_MOVE {
	glm::vec2 start_pos;
	float direction;
	glm::vec2 speed;
	unsigned long time;
	bool flag;
}car_move;

#define car_speed	20
unsigned long car_time = 0;
std::queue<car_move> car_data;


glm::vec2 car2_pos = { 0, 0 };

typedef car_move shirt_move;
#define shirt_speed	3
#define shirt_total	18
float shirt_radian = (360.0f / (float)shirt_total) * TO_RADIAN;
std::queue<shirt_move> shirt_time;


#include <time.h>
uint64_t randXORseed[2] = { 0, (uint64_t)time(NULL) };
uint64_t randXOR(void) {
	uint64_t x = randXORseed[0];
	uint64_t const y = randXORseed[1];
	randXORseed[0] = y;
	x ^= x << 23; // a
	randXORseed[1] = x ^ y ^ (x >> 17) ^ (y >> 26); // b, c
	return randXORseed[1] + y;
}


typedef struct _SPACESHIP_MOVE {
	glm::vec2 pos;
	float direction;
	glm::vec2 speed;
	float size;
}spaceship_move;

#define spaceship_min_size	airplane_size * 0.7
#define spaceship_max_size	3
spaceship_move spaceship_data = { glm::vec2(0, 0), 0, glm::vec2(0, 0), 0 };


#define crazy_time_set	100
int crazy_time = 0;


#define COLLISION_THRESHOLD	20
bool collision_check(glm::vec2 a, glm::vec2 b) {
	if (abs(a.x - b.x) >= COLLISION_THRESHOLD) return false;
	if (abs(a.y - b.y) >= COLLISION_THRESHOLD) return false;
	return true;
}

bool collision_check(glm::vec2 a, float size_a, glm::vec2 b, float size_b) {
	if (abs(a.x - b.x) >= COLLISION_THRESHOLD + (size_a + size_b) * 1.5) return false;
	if (abs(a.y - b.y) >= COLLISION_THRESHOLD + (size_a + size_b) * 1.5) return false;
	return true;
}

bool window_border_check(glm::vec2 a) {
	if (abs(a.x) >= win_width / 2)	return true;
	if (abs(a.y) >= win_height / 2)	return true;
	return false;
}

bool window_border_check(glm::vec2 a, int *direction) {
	if (abs(a.x) >= win_width / 2)	{
		if (a.x < 0) *direction = 'W';
		else *direction = 'E';
		return true;
	}
	if (abs(a.y) >= win_height / 2)	{
		if (a.y < 0) *direction = 'S';
		else *direction = 'N';
		return true;
	}
	*direction = ' ';
	return false;
}

glm::vec2 symmetry(glm::vec2 p, glm::vec2 v1, glm::vec2 v2) {
	float delta_x = v2.x - v1.x;
	float delta_y = v2.y - v1.y;
	
	if (delta_x == 0.0f)		return{ 2 * v2.x - p.x, p.y };
	else if (delta_y == 0.0f)	return{ p.x, 2 * v2.y - p.y };

	float a = delta_y / delta_x;
	float b = v2.y - a * v2.x;

	float contact_x_axis = -b / a;
	float theta = atan(a);

	float x = p.x - contact_x_axis;
	float y = p.y;

	float p_angle = atan2(y, x);
	float r_angle = 2 * theta - p_angle;

	float r = sqrt(pow(x, 2.0f) + pow(y, 2.0f));

	float result_x = r * cos(r_angle) + contact_x_axis;
	float result_y = r * sin(r_angle);

	return glm::vec2{ result_x, result_y };
}

bool pause = false;
std::queue<float> pause_timerQueue;


//#define DEBUG


void display(void) {
	int i; 
	float x, r, s, delx, delr, dels;
	glm::mat4 ModelMatrix;

	glClear(GL_COLOR_BUFFER_BIT);

	ModelMatrix = glm::mat4(1.0f);
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes();

	//ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-500.0f, 0.0f, 0.0f));
	//ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	//glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	//draw_airplane();
	//
	//ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-300.0f, 0.0f, 0.0f));
	//ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	//ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	//glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	//draw_shirt();
	//
	//ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, 0.0f, 0.0f));
	//ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	//ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	//glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	//draw_house();
	//
	//ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 0.0f, 0.0f));
	//ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	//ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	//glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]); 
	//draw_car();
	//
	//ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(300.0f, 0.0f, 0.0f));
	//ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	//ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	//glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]); 
	//draw_cocktail();
	//
	//ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(500.0f, 0.0f, 0.0f));
	//ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	//ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	//glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]); 
	//draw_shirt();

	// HW
	if (pause)
		return;

	int car_count = car_data.size();
	for (int i = 0; i < car_count; i++) {
		car_move car_now = car_data.front();
		car_data.pop();

		unsigned long car_duration = car_time - car_now.time;
		float car_pos_x = car_now.start_pos.x + car_now.speed.x * car_duration;
		float car_pos_y = car_now.start_pos.y + car_now.speed.y * car_duration;

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(car_pos_x, car_pos_y, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, car_now.direction, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_car();
		
		bool car_reset = true;
		if (window_border_check(glm::vec2(car_pos_x, car_pos_y))) {
			if (car_now.flag == false) car_reset = false;

			for (int i = 0; i < house_posQ_max; i++) {
				if (!house_posQ[i].check) break;
				if (house_posQ[i].pos.x == car_now.start_pos.x && house_posQ[i].pos.y == car_now.start_pos.y)
					car_now.time = car_time;
			}
		}

		if (car_reset)
			car_data.push(car_now);
	}

	for (int i = 0; i < house_posQ_max; i++) {
		if (!house_posQ[i].check) break;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(house_posQ[i].pos.x, house_posQ[i].pos.y, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();
	}

	if (cocktail_flag) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(cocktail_pos.x, cocktail_pos.y, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(cocktail_size, cocktail_size, 0.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cocktail();
	}

	float airplain_radian = (float)airplane_degree * TO_RADIAN;
	airplane_pos.x = cos(airplain_radian) * win_width / 3;
	airplane_pos.y = sin(airplain_radian) * win_height / 3;

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(airplane_pos.x, airplane_pos.y, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, airplain_radian, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplane_size, -airplane_size, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_airplane();


	bool spaceship_data_change = true;
	int spaceship_data_border = ' ';

	if (spaceship_data.size == 0) {
		float rand_x = (int64_t)randXOR() % win_width / 2.0f;
		float rand_y = (int64_t)randXOR() % win_height / 2.0f;
		spaceship_data.pos = glm::vec2(rand_x, rand_y);
		spaceship_data_change = false;
#ifdef DEBUG
		fprintf(stdout, ">>> New Spaceship Position : (%f, %f)", spaceship_data.pos.x, spaceship_data.pos.y);
#endif // DEBUG		
	}

	else if (collision_check(spaceship_data.pos, spaceship_data.size, glm::vec2(airplane_pos.x, airplane_pos.y), airplane_size)) {
		spaceship_data_change = false;

#ifdef DEBUG
		fprintf(stdout, ">>> Airplane - Spaceship Collision : (%f, %f)", spaceship_data.pos.x, spaceship_data.pos.y);
#endif // DEBUG		
	}

	else if (window_border_check(spaceship_data.pos, &spaceship_data_border)) {		
		spaceship_data_change = false;
#ifdef DEBUG
		fprintf(stdout, ">>> Spaceship Touch Boader : (%f, %f)", spaceship_data.pos.x, spaceship_data.pos.y);
#endif // DEBUG

	}

	if (!spaceship_data_change) {		
		float rand_direction = 0;
		
		switch (spaceship_data_border) {
		case ' ': rand_direction = (((int64_t)randXOR() % 18000)) / 100.0f * TO_RADIAN; break;
		case 'W': rand_direction = (((int64_t)randXOR() % 9000)) / 100.0f * TO_RADIAN; break;
		case 'E': rand_direction = ((((int64_t)randXOR() % 9000)) / 100.0f + 180) * TO_RADIAN; break;
		case 'N': rand_direction = ((((int64_t)randXOR() % 9000)) / 100.0f - 90) * TO_RADIAN; break;
		case 'S': rand_direction = ((((int64_t)randXOR() % 9000)) / 100.0f + 90) * TO_RADIAN; break;
		}

		float rand_x_speed = cos(rand_direction) * 3;
		float rand_y_speed = sin(rand_direction) * 3;
		float rand_size = (randXOR() % ((int)(spaceship_max_size - spaceship_min_size) * 100) + 1) / 100.0f + spaceship_min_size;
		spaceship_data = { spaceship_data.pos, rand_direction - 90 * TO_RADIAN, glm::vec2(rand_x_speed, rand_y_speed), rand_size };

#ifdef DEBUG
		fprintf(stdout, " %f <%f>\n", rand_direction, rand_size);
#endif // DEBUG		
	}

	spaceship_data.pos.x = spaceship_data.pos.x + spaceship_data.speed.x;
	spaceship_data.pos.y = spaceship_data.pos.y + spaceship_data.speed.y;

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(spaceship_data.pos.x, spaceship_data.pos.y, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, spaceship_data.direction, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(spaceship_data.size, spaceship_data.size, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_spaceship();

	float car2_x = (airplane_pos.x + spaceship_data.pos.x) / 2.0f;
	float car2_y = (airplane_pos.y + spaceship_data.pos.y) / 2.0f;
	float car2_rotate = atan2(car2_pos.y - car2_y, car2_pos.x - car2_x);
	car2_pos = glm::vec2{ car2_x, car2_y };

	if (shirt_time.size() != 0) {
		for (int s = 0; s < shirt_time.size(); s++) {
			shirt_move shirt_data = shirt_time.front();
			shirt_time.pop();

			unsigned long ref_time = car_time - shirt_data.time;
			unsigned long distance = ref_time * shirt_speed;

			for (float i = 0; i < 360 * TO_RADIAN; i += shirt_radian) {
				float x_pos = distance * cos(i) + shirt_data.start_pos.x;
				float y_pos = distance * sin(i) + shirt_data.start_pos.y;

				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x_pos, y_pos, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, i - 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_shirt();
			}

			float far_pos_distance = distance / 1.5f;

			if (!(shirt_data.start_pos.x + far_pos_distance > win_width / 2.0f &&
				shirt_data.start_pos.x - far_pos_distance < -win_width / 2.0f &&
				shirt_data.start_pos.y + far_pos_distance > win_height / 2.0f &&
				shirt_data.start_pos.y - far_pos_distance < -win_height / 2.0f))
				shirt_time.push(shirt_data);
		}
	}

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(car2_x, car2_y, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, car2_rotate, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_car2();

	if (cocktail_flag) {
		float line_slope = tan(atan2(cocktail_pos.y - car2_y, cocktail_pos.x - car2_x));
		float line_contact_y_axis = cocktail_pos.y - line_slope * cocktail_pos.x;
		float line_x = std::max(win_width, win_height) / 2.25f;

		line[0][0] = line_x;
		line[0][1] = line_slope * line[0][0] + line_contact_y_axis;
		line[1][0] = -line_x;
		line[1][1] = line_slope * line[1][0] + line_contact_y_axis;
		update_line();

		ModelMatrix = glm::mat4(1.0f);
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_line();		
	}

	glFlush();
}  

// HW Timer
void airplane_timer(int value) { // HW
	(++airplane_degree) %= 360;
	glutPostRedisplay();
	glutTimerFunc(airplane_speed, airplane_timer, 0);
}

void car_timer(int value) { // HW
	car_time++;
	glutPostRedisplay();
	glutTimerFunc(car_speed, car_timer, 0);
}

void crazy_timer(int value) { // HW
	if (crazy_time == 0) return;

	shirt_time.push(shirt_move{ car2_pos, 0, glm::vec2{ 0, 0 }, car_time });

	for (int i = 0; i < house_posQ_max; i++) {
		if (!house_posQ[i].check) break;

		//float direction = atan2(house_posQ[i].pos.y, house_posQ[i].pos.x) * car_time * TO_RADIAN;
		float direction = (((int64_t)randXOR() % 18000)) / 100.0f * TO_RADIAN;
		car_move car_info = { house_posQ[i].pos, direction, glm::vec2(cos(direction), sin(direction)), car_time, false };
		car_data.push(car_info);
	}
	glutPostRedisplay();
	glutTimerFunc(crazy_time, crazy_timer, 0);
}


void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	
	// HW
	case '+': // Plus key
		airplane_speed = std::max(airplane_speed - airplane_speed_gap, airplane_speed_min);
#ifdef DEBUG
		fprintf(stdout, "$$$ Airplane Speed: %d\n", airplane_speed_max - airplane_speed + 1);
#endif // DEBUG		
		break;

	case '-': // Minus key
		airplane_speed = std::min(airplane_speed + airplane_speed_gap, airplane_speed_max);
#ifdef DEBUG
		fprintf(stdout, "$$$ Airplane Speed: %d\n", airplane_speed_max - airplane_speed + 1);
#endif // DEBUG
		break;

	case 'R':
	case 'r':
		spaceship_data.size = 0;
		break;

	case 'F':
	case 'f':
		shirt_time.push(shirt_move{ car2_pos, 0, glm::vec2{ 0, 0 }, car_time });
		break;

	case ' ':
		pause = !pause;
		
		if (pause) {
			pause_timerQueue.push(airplane_degree);
			airplane_degree = 0;
			pause_timerQueue.push(car_time);
			car_time = 0;
		}

		else {
			airplane_degree = pause_timerQueue.front();
			pause_timerQueue.pop();
			car_time = pause_timerQueue.front();
			pause_timerQueue.pop();
		}

		break;


	case 'C':
	case 'c':
		if (crazy_time == 0)	crazy_time = crazy_time_set;
		else					crazy_time = 0;
		glutTimerFunc(crazy_time, crazy_timer, 0);
		break;
	}
}

void reshape(int width, int height) {
	win_width = width, win_height = height;
	
  	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0, 
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	update_axes();
	update_line();

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_line);
	glDeleteBuffers(1, &VBO_line);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);
}

void mousepress(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		float relative_x = float(x) - win_width / 2;
		float relative_y = win_height / 2 - float(y);
		
		if (house_posQ[house_posQ_now].check) {
			cocktail_pos.x -= house_posQ[house_posQ_now].pos.x / (float)house_posQ_max;
			cocktail_pos.y -= house_posQ[house_posQ_now].pos.y / (float)house_posQ_max;

			cocktail_pos.x += relative_x / (float)house_posQ_max;
			cocktail_pos.y += relative_y / (float)house_posQ_max;
		}

		else {
			if (cocktail_flag) {
				cocktail_pos.x = (cocktail_pos.x * house_posQ_now + relative_x) / (float)(house_posQ_now + 1);
				cocktail_pos.y = (cocktail_pos.y * house_posQ_now + relative_y) / (float)(house_posQ_now + 1);
			}

			else {
				cocktail_pos.x = (cocktail_pos.x + relative_x) / (float)(house_posQ_now + 1);
				cocktail_pos.y = (cocktail_pos.y + relative_y) / (float)(house_posQ_now + 1);

				if (house_posQ_now)
					cocktail_flag = true;				
			}
		}

#ifdef DEBUG
		fprintf(stdout, "### New Cocktail Position: (%f, %f)\n", cocktail_pos.x, cocktail_pos.y);
#endif // DEBUG	

		house_posQ[house_posQ_now].pos.x = relative_x;
		house_posQ[house_posQ_now].pos.y = relative_y;
		house_posQ[house_posQ_now].check = true;		

		float direction = atan2(relative_y, relative_x);
		car_move car_info = { house_posQ[house_posQ_now].pos, direction, glm::vec2(cos(direction), sin(direction)), car_time, true };
		car_data.push(car_info);

		(++house_posQ_now) %= house_posQ_max;

#ifdef DEBUG
		fprintf(stdout, "*** New House Position: (%f, %f), %f\n", relative_x, relative_y, direction);
#endif // DEBUG			
	}

	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
		glm::vec2 fly_mid = { (airplane_pos.x + spaceship_data.pos.x) / 2.0f, (airplane_pos.y + spaceship_data.pos.y) / 2.0f };
		
		int i = 0;
		for ( ; i < house_posQ_max; i++) {
			if (!house_posQ[i].check) break;
			glm::vec2 sym_pos = symmetry(house_posQ[i].pos, cocktail_pos, fly_mid);
			house_posQ[i].pos = sym_pos;

			float direction = atan2(sym_pos.y, sym_pos.x);
			car_move car_info = { house_posQ[i].pos, direction, glm::vec2(cos(direction), sin(direction)), car_time, true };
			car_data.push(car_info);
		}
	}
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutCloseFunc(cleanup);

	// HW
	glutTimerFunc(airplane_speed, airplane_timer, 0);
	glutTimerFunc(car_speed, car_timer, 0);
	glutTimerFunc(crazy_time, car_timer, 0);
	glutMouseFunc(mousepress);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE); 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
//	glClearColor(250 / 255.0f, 128 / 255.0f, 114 / 255.0f, 1.0f);
	// HW
	glClearColor(47 / 255.0f, 103 / 255.0f, 49 / 255.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_line();
	prepare_airplane();
	prepare_shirt();
	prepare_house();
	prepare_car();
	prepare_cocktail();
	prepare_car2();

	// HW
	prepare_spaceship();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program(); 
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

    error = glewInit();
	if (error != GLEW_OK) { 
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 11
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 2DObjects_GLSL_3.1";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Mouse used: 'Left Click'  > 새로운 집과 자동차 만들기",
		"                  'Right Click' > 모든 집을 빨간 선 기준 대칭 이동",
		"",
		"    - Keys used: 'ESC'   > 종료",
		"                 'Space' > 일시 정지",
		"                 '+'     > 속도 증가",
		"                 '-'     > 속도 감소",
		"                 'R'     > 새로운 위치에 우주선 만들기",
		"                 'F'     > 축제 기념 옷 뿌리기",
		"",
		"    - Hidden key: 'C' > Crazy Mode Act/Inact"
	};

	glutInit (&argc, argv);
 	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
//	glutInitWindowSize (1200, 300);
	// HW
	glutInitWindowSize(640, 360);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop ();
}


