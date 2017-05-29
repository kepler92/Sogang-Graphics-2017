#define _CRT_SECURE_NO_WARNINGS

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> //inverse, affineInverse, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewProjectionMatrix, ViewMatrix, ProjectionMatrix;

glm::mat4 ModelMatrix_CAR_BODY, ModelMatrix_CAR_WHEEL, ModelMatrix_CAR_NUT, ModelMatrix_CAR_DRIVER;
glm::mat4 ModelMatrix_CAR_BODY_to_DRIVER; // computed only once in initialize_camera()

glm::mat4 ModelMatrix_big_cow, ModelMatrix_small_cow;


#include "Camera.h"
#include "Geometry.h"

#define car_run		100.0f
#define car_stop	0
int car_timestamp = 0;

float car_speed = car_run;
float rotation_angle_car = 0.0f;


#define teapot_run		100.0f
#define teapot_stop	0
int teapot_timestamp = 0;

float teapot_speed = teapot_run;
float rotation_teapot = 0.0f;



//void draw_objects_in_world(void) {
//	glm::mat4 ModelMatrix_OBJECT;
//	int i;
//	float angle, size, self_rotation_angle;
//
//	ModelMatrix_OBJECT = glm::translate(glm::mat4(1.0f), glm::vec3(42.0f, 4.0f, 36.0f));
//	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, 10.0f*rotation_angle_car, glm::vec3(0.0f, 1.0f, 0.0f));
//	ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(6.0f, 6.0f, 6.0f));
//	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
//	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
//
//	glUniform3f(loc_primitive_color, 1.000f, 0.271f, 0.000f); // color name: OrangeRed
//	draw_geom_obj(GEOM_OBJ_ID_COW); // draw cow
//
//	ModelMatrix_OBJECT = glm::translate(glm::mat4(1.0f), glm::vec3(-25.0f, 4.0f, -35.0f));
//	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, -rotation_angle_car, glm::vec3(0.0f, 1.0f, 0.0f));
//	ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(3.0f, 3.0f, 3.0f));
//	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
//	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
//
//	glUniform3f(loc_primitive_color, 0.980f, 0.980f, 0.824f); // color name: LightGoldenrodYellow  
//	draw_geom_obj(GEOM_OBJ_ID_COW); // draw cow
//
//	ModelMatrix_OBJECT = glm::translate(glm::mat4(1.0f), glm::vec3(-28.0f, 4.0f, -45.0f));
//	ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(5.0f, 5.0f, 5.0f));
//	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
//	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
//
//	glUniform3f(loc_primitive_color, 0.741f, 0.718f, 0.420f); // color name: DarkKhaki
//	draw_geom_obj(GEOM_OBJ_ID_COW); // draw cow
//
//	ModelMatrix_OBJECT = glm::translate(glm::mat4(1.0f), glm::vec3(-42.0f, 2.0f, 26.0f));
//	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, -TO_RADIAN*90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
//	ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(2.0f, 2.0f, 2.0f));
//	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
//	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
//
//	glUniform3f(loc_primitive_color, 0.000f, 1.000f, 0.498f); // color name: SpringGreen
//	draw_geom_obj(GEOM_OBJ_ID_TEAPOT); // draw teapot
//
//	ModelMatrix_OBJECT = glm::translate(glm::mat4(1.0f), glm::vec3(22.0f, 4.0f, -15.0f));
//	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, 5.0f*rotation_angle_car, glm::vec3(0.0f, 1.0f, 0.0f));
//	ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(1.5f, 1.5f, 1.5f));
//	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
//	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
//
//	glUniform3f(loc_primitive_color, 0.529f, 0.808f, 0.980f); // color name: LightSkyBlue
//	draw_geom_obj(GEOM_OBJ_ID_TEAPOT); // draw teapot
//
//	for (i = 0; i < 18; i++) {
//		angle = TO_RADIAN*20.0f*i;
//		size = 1.0f + 2.0f*fabs(cosf(angle));
//		if (i % 2 == 0) {
//			self_rotation_angle = 0.1f*i*rotation_angle_car;
//			glUniform3f(loc_primitive_color, 1.000f, 0.843f, 0.000f); // color name: Gold
//		}
//		else {
//			self_rotation_angle = -0.5f*i*rotation_angle_car;
//			glUniform3f(loc_primitive_color, 0.804f, 0.361f, 0.361f); // color name: IndianRed
//		}
//
//		ModelMatrix_OBJECT = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
//		ModelMatrix_OBJECT = glm::translate(ModelMatrix_OBJECT, glm::vec3(45.0f, 2.0f, 0.0f));
//		ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, self_rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));
//		ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(size, size, size));
//		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
//		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
//
//		draw_geom_obj(GEOM_OBJ_ID_BOX); // draw teapot
//
//		ModelMatrix_OBJECT = glm::translate(ModelMatrix_OBJECT, glm::vec3(0.0f, 2.2f, 0.0f));
//		ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, 10.0f*rotation_angle_car, glm::vec3(0.0f, 1.0f, 0.0f));
//		ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, -TO_RADIAN*90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
//		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
//		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
//
//		switch (i % 3) {
//		case 0:
//			glUniform3f(loc_primitive_color, 0.941f, 1.000f, 0.941f); // color name: HoneyDue
//			break;
//		case 1:
//			glUniform3f(loc_primitive_color, 0.804f, 0.361f, 0.361f); // color name: IndianRed
//			break;
//		case 2:
//			glUniform3f(loc_primitive_color, 0.184f, 0.310f, 0.310f); // color name: DarkSlateGray
//			break;
//		}
//
//		draw_geom_obj(GEOM_OBJ_ID_TEAPOT); // draw teapot
//	}
//}

float wall_size = 150.0f;
float wall_bottom = 0.0f;

#define cow_run		100.0f
#define cow_stop	0
#define cow_gap		2
int cow_timestamp = 0;

int cow_flag = cow_run;
float cow_speed = 2.0f;
glm::vec3 cow_position(-wall_size / 2.0f + cow_gap, wall_bottom + 2.0f, -wall_size / 2.0f + cow_gap);
//glm::vec3 cow_position(0, wall_bottom + 2.0f, 0);
int cow_rotation = 0;
float rotation_angle_cow = cow_rotation * TO_RADIAN;


void draw_objects_in_world(void) {
	glm::mat4 ModelMatrix_OBJECT;
//	int i;
//	float angle, size, self_rotation_angle;

	// 정적물체
	glm::mat4 Matrix_TIGER = glm::rotate(glm::mat4(1.0f), -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	Matrix_TIGER = glm::scale(Matrix_TIGER, glm::vec3(0.05f, 0.05f, 0.05f));

	ModelMatrix_OBJECT = glm::translate(glm::mat4(1.0f), glm::vec3(-500.0f, -1200.0f, 0.0f));
	ModelMatrix_OBJECT = Matrix_TIGER * ModelMatrix_OBJECT;
	ModelViewProjectionMatrix = ViewProjectionMatrix *  ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_geom_obj(GEOM_OBJ_ID_TIGER); // 호랑이

	ModelMatrix_OBJECT = glm::translate(glm::mat4(1.0f), glm::vec3(-1200.0f, 2.0f, 0.0f));
	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, 90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix_OBJECT = Matrix_TIGER * ModelMatrix_OBJECT;
	ModelViewProjectionMatrix = ViewProjectionMatrix *  ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_geom_obj(GEOM_OBJ_ID_TIGER, 0.5f, 0.5f, 0.5f); // 호랑이

	ModelMatrix_OBJECT = glm::translate(glm::mat4(1.0f), glm::vec3(1200.0f, 150.0f, 0.0f));
	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, -90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix_OBJECT = Matrix_TIGER * ModelMatrix_OBJECT;
	ModelViewProjectionMatrix = ViewProjectionMatrix *  ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_geom_obj(GEOM_OBJ_ID_TIGER, 0.7f, 0.1f, 0.3f); // 호랑이

	ModelMatrix_OBJECT = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 8.0f, -50.0f));
	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, 135.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(6.0f, 6.0f, 6.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_geom_obj(GEOM_OBJ_ID_COW, 1.000f, 0.271f, 0.000f); // draw cow

	ModelMatrix_OBJECT = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, 5.0f, -30.0f));
	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, -90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(15.0f, 15.0f, 15.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_geom_obj(GEOM_OBJ_ID_COW, 0.980f, 0.980f, 0.824f); // draw cow

	ModelMatrix_OBJECT = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f, 10.0f, -45.0f));
	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, -135.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(30.0f, 30.0f, 30.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_geom_obj(GEOM_OBJ_ID_COW, 0.980f, 0.980f, 0.824f); // draw cow

	ModelMatrix_OBJECT = glm::rotate(glm::mat4(1.0f), 0.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix_OBJECT = glm::translate(ModelMatrix_OBJECT, glm::vec3(-30.0f, 4.0f, -50.0f));
	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(1.0, 1.0, 1.0));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_geom_obj(GEOM_OBJ_ID_TEAPOT, 0.654, 0.3674, 0.90); // draw teapot

	ModelMatrix_OBJECT = glm::rotate(glm::mat4(1.0f), 0.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix_OBJECT = glm::translate(ModelMatrix_OBJECT, glm::vec3(30.0f, 4.0f, 50.0f));
	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, 135.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(1.0, 1.0, 1.0));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_geom_obj(GEOM_OBJ_ID_TEAPOT, 0.8, 0.0, 0.7); // draw teapot


	// 동적 물체 추가
	float self_rotation_angle = rotation_teapot;
	ModelMatrix_OBJECT = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f));
	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, self_rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(1.0f, 1.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_geom_obj(GEOM_OBJ_ID_BOX, 1, 1, 1); // draw teapot_box

	ModelMatrix_OBJECT = glm::translate(ModelMatrix_OBJECT, glm::vec3(0.0f, 2.2f, 0.0f));
	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, self_rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, -TO_RADIAN*90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_geom_obj(GEOM_OBJ_ID_TEAPOT, 1, 1, 1); // draw teapot
}

void draw_wall() {
	glm::mat4 ModelMatrix_OBJECT;

	// 바닥
	ModelMatrix_OBJECT = glm::translate(glm::mat4(1.0f), glm::vec3(wall_size / -2.0f, wall_bottom, -wall_size / 2.0f));
	ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(wall_size, wall_size, wall_size));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_geom_obj(GEOM_OBJ_ID_SQUARE16, 0.184f, 0.310f, 0.310f); // draw square

	// 오른쪽
	ModelMatrix_OBJECT = glm::translate(glm::mat4(1.0f), glm::vec3(-wall_size / 2.0f, wall_bottom, 0));
	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, 90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_OBJECT = glm::translate(ModelMatrix_OBJECT, glm::vec3(wall_size / -2.0f, 0, 0));
	ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(wall_size, wall_size, wall_size));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_geom_obj(GEOM_OBJ_ID_SQUARE16, 0.184f, 0.310f, 0.310f); // draw square

	// 왼쪽
	ModelMatrix_OBJECT = glm::translate(glm::mat4(1.0f), glm::vec3(wall_size / 2.0f, wall_bottom, 0));
	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, -90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_OBJECT = glm::translate(ModelMatrix_OBJECT, glm::vec3(wall_size / -2.0f, 0, 0));
	ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(wall_size, wall_size, wall_size));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_geom_obj(GEOM_OBJ_ID_SQUARE16, 0.184f, 0.310f, 0.310f); // draw square

	// 아래
	ModelMatrix_OBJECT = glm::translate(glm::mat4(1.0f), glm::vec3(0, wall_bottom, 0));
	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix_OBJECT = glm::translate(ModelMatrix_OBJECT, glm::vec3(wall_size / -2.0f, wall_size / -2.0f, 0));
	ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(wall_size, wall_size, wall_size));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_geom_obj(GEOM_OBJ_ID_SQUARE16, 0.184f, 0.310f, 0.310f); // draw square

	// 위
	ModelMatrix_OBJECT = glm::translate(glm::mat4(1.0f), glm::vec3(0, wall_size + wall_bottom, 0));
	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, 90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix_OBJECT = glm::translate(ModelMatrix_OBJECT, glm::vec3(wall_size / -2.0f, wall_size / -2.0f, 0));
	ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(wall_size, wall_size, wall_size));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_geom_obj(GEOM_OBJ_ID_SQUARE16, 0.184f, 0.310f, 0.310f); // draw square
}

#define rad 1.7f
#define ww 1.0f
void draw_wheel_and_nut() {
	// angle is used in Hierarchical_Car_Correct later
	int i;

	glUniform3f(loc_primitive_color, 0.000f, 0.808f, 0.820f); // color name: DarkTurquoise
	draw_geom_obj(GEOM_OBJ_ID_CAR_WHEEL); // draw wheel

	for (i = 0; i < 5; i++) {
		ModelMatrix_CAR_NUT = glm::rotate(ModelMatrix_CAR_WHEEL, TO_RADIAN*72.0f*i, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix_CAR_NUT = glm::translate(ModelMatrix_CAR_NUT, glm::vec3(rad - 0.5f, 0.0f, ww));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_NUT;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

		glUniform3f(loc_primitive_color, 0.690f, 0.769f, 0.871f); // color name: LightSteelBlue
		draw_geom_obj(GEOM_OBJ_ID_CAR_NUT); // draw i-th nut
	}
}

void draw_car() {
	glUniform3f(loc_primitive_color, 0.498f, 1.000f, 0.831f); // color name: Aquamarine3
	draw_geom_obj(GEOM_OBJ_ID_CAR_BODY);

	glLineWidth(2.0f);
	draw_axes(); // draw MC axes of body
	glLineWidth(1.0f);

	ModelMatrix_CAR_DRIVER = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.0f, 0.5f, 2.5f));
	ModelMatrix_CAR_DRIVER = glm::rotate(ModelMatrix_CAR_DRIVER, TO_RADIAN*90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_DRIVER;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(5.0f);
	draw_axes(); // draw camera frame at driver seat
	glLineWidth(1.0f);
	

	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.9f, -3.5f, 4.5f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, -car_speed / 5.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, rotation_angle_car * car_speed / 5.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut();  // draw wheel 0

	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(3.9f, -3.5f, 4.5f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, rotation_angle_car * car_speed / 5.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut();  // draw wheel 1

	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.9f, -3.5f, -4.5f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, -car_speed / 5.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, rotation_angle_car * car_speed / 5.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix_CAR_WHEEL = glm::scale(ModelMatrix_CAR_WHEEL, glm::vec3(1.0f, 1.0f, -1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut();  // draw wheel 2

	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(3.9f, -3.5f, -4.5f));
	ModelMatrix_CAR_WHEEL = glm::rotate(ModelMatrix_CAR_WHEEL, rotation_angle_car * car_speed / 5.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix_CAR_WHEEL = glm::scale(ModelMatrix_CAR_WHEEL, glm::vec3(1.0f, 1.0f, -1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut();  // draw wheel 3
}

void draw_cow_twins() {
	//ModelMatrix_big_cow = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_big_cow = glm::translate(glm::mat4(1.0f), cow_position);
	ModelMatrix_big_cow = glm::rotate(ModelMatrix_big_cow, -rotation_angle_cow, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_big_cow = glm::scale(ModelMatrix_big_cow, glm::vec3(6.5f, 6.5f, 6.5f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_big_cow;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3f(loc_primitive_color, 1, 1, 1);
	draw_geom_obj(GEOM_OBJ_ID_COW);

	ModelMatrix_small_cow = glm::translate(ModelMatrix_big_cow, glm::vec3(0.15f, 0.3f, 0.0f));
	ModelMatrix_small_cow = glm::scale(ModelMatrix_small_cow, glm::vec3(0.3f, 0.3f, 0.3f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_small_cow;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3f(loc_primitive_color, 0.5, 0.5, 0.5);
	draw_geom_obj(GEOM_OBJ_ID_COW);

	glm::mat4 ModelMatrix_small_cow2 = glm::translate(ModelMatrix_small_cow, glm::vec3(0.15f, 0.3f, -0.10f));
	ModelMatrix_small_cow2 = glm::scale(ModelMatrix_small_cow2, glm::vec3(0.3f, 0.3f, 0.3f));
	ModelMatrix_small_cow2 = glm::rotate(ModelMatrix_small_cow2, -30.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_small_cow2;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3f(loc_primitive_color, 1, 1, 1);
	draw_geom_obj(GEOM_OBJ_ID_COW);

	glm::mat4 ModelMatrix_small_cow3 = glm::translate(ModelMatrix_small_cow, glm::vec3(0.15f, 0.3f, 0.10f));
	ModelMatrix_small_cow3 = glm::scale(ModelMatrix_small_cow3, glm::vec3(0.3f, 0.3f, 0.3f));
	ModelMatrix_small_cow3 = glm::rotate(ModelMatrix_small_cow3, 30.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_small_cow3;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3f(loc_primitive_color, 1, 1, 1);
	draw_geom_obj(GEOM_OBJ_ID_COW);
}



//#define rad 1.7f
//#define ww 1.0f
//void draw_wheel_and_nut() {
//	// angle is used in Hierarchical_Car_Correct later
//	int i;
//
//	glUniform3f(loc_primitive_color, 0.000f, 0.808f, 0.820f); // color name: DarkTurquoise
//	draw_geom_obj(GEOM_OBJ_ID_CAR_WHEEL); // draw wheel
//
//	for (i = 0; i < 5; i++) {
//		ModelMatrix_CAR_NUT = glm::rotate(ModelMatrix_CAR_WHEEL, TO_RADIAN*72.0f*i, glm::vec3(0.0f, 0.0f, 1.0f));
//		ModelMatrix_CAR_NUT = glm::translate(ModelMatrix_CAR_NUT, glm::vec3(rad - 0.5f, 0.0f, ww));
//		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_NUT;
//		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
//
//		glUniform3f(loc_primitive_color, 0.690f, 0.769f, 0.871f); // color name: LightSteelBlue
//		draw_geom_obj(GEOM_OBJ_ID_CAR_NUT); // draw i-th nut
//	}
//}
//
//void draw_car_dummy(void) {
//	glUniform3f(loc_primitive_color, 0.498f, 1.000f, 0.831f); // color name: Aquamarine
//	draw_geom_obj(GEOM_OBJ_ID_CAR_BODY); // draw body
//
//	glLineWidth(2.0f);
//	draw_axes(); // draw MC axes of body
//	glLineWidth(1.0f);
//
//	ModelMatrix_CAR_DRIVER = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.0f, 0.5f, 2.5f));
//	ModelMatrix_CAR_DRIVER = glm::rotate(ModelMatrix_CAR_DRIVER, TO_RADIAN*90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
//	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_DRIVER;
//	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
//	glLineWidth(5.0f);
//	draw_axes(); // draw camera frame at driver seat
//	glLineWidth(1.0f);
//
//	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.9f, -3.5f, 4.5f));
//	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
//	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
//	draw_wheel_and_nut();  // draw wheel 0
//
//	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(3.9f, -3.5f, 4.5f));
//	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
//	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
//	draw_wheel_and_nut();  // draw wheel 1
//
//	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.9f, -3.5f, -4.5f));
//	ModelMatrix_CAR_WHEEL = glm::scale(ModelMatrix_CAR_WHEEL, glm::vec3(1.0f, 1.0f, -1.0f));
//	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
//	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
//	draw_wheel_and_nut();  // draw wheel 2
//
//	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(3.9f, -3.5f, -4.5f));
//	ModelMatrix_CAR_WHEEL = glm::scale(ModelMatrix_CAR_WHEEL, glm::vec3(1.0f, 1.0f, -1.0f));
//	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_WHEEL;
//	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
//	draw_wheel_and_nut();  // draw wheel 3
//}

/*********************************  START: callbacks *********************************/
int flag_draw_world_objects = 1;

void display(void) {
	glm::mat4 ModelMatrix_big_cow, ModelMatrix_small_cow;
	glm::mat4 ModelMatrix_big_box, ModelMatrix_small_box;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ModelMatrix_CAR_BODY = glm::rotate(glm::mat4(1.0f), -rotation_angle_car, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_CAR_BODY = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(20.0f, 4.89f, 0.0f));
	ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, 90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

	if (camera_type == CAMERA_OBJECT) set_ViewMatrix_for_object();
	else if (camera_type == CAMERA_DRIVER) set_ViewMatrix_for_driver();
	
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix_CAR_BODY;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_car();

	draw_cow_twins();

	ModelViewProjectionMatrix = glm::scale(ViewProjectionMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(2.0f);
	draw_axes();
	glLineWidth(1.0f);

	ModelViewProjectionMatrix = ViewProjectionMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_path();

	if (flag_draw_world_objects)
		draw_objects_in_world();

	draw_wall();

	glutSwapBuffers();
}

/*
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'f':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glutPostRedisplay();
		break;
	case 'l':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glutPostRedisplay();
		break;
	case 'd':
		camera_type = CAMERA_DRIVER;
		glutPostRedisplay();
		break;
	case 'w':
		camera_type = CAMERA_WORLD_VIEWER;
		set_ViewMatrix_for_world_viewer();
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
		break;
	case 'o':
		flag_draw_world_objects = 1 - flag_draw_world_objects;
		glutPostRedisplay();
		break;
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}
*/



void timer_scene(int timestamp_scene) {
	car_timestamp = timestamp_scene;

	rotation_angle_car = (timestamp_scene % 360)*TO_RADIAN;
	glutPostRedisplay();

	if (car_speed)
		glutTimerFunc(car_speed, timer_scene, (timestamp_scene + 1) % INT_MAX);
}

void timer_tee(int timestamp_scene) {
	teapot_timestamp = timestamp_scene;

	rotation_teapot = (timestamp_scene % 360)*TO_RADIAN;
	glutPostRedisplay();

	if (teapot_speed)
		glutTimerFunc(teapot_speed, timer_tee, (timestamp_scene + 1) % INT_MAX);
}

void timer_cow(int timestamp_scene) {
	cow_timestamp = timestamp_scene;

	cow_position.x = cow_position.x + cos(rotation_angle_cow) * cow_speed;
	cow_position.y = wall_bottom + 2.0f;
	cow_position.z = cow_position.z + sin(rotation_angle_cow) * cow_speed;

	if ((abs(cow_position.x) >= wall_size / 2) ||
		(abs(cow_position.z) >= wall_size / 2)) {

		cow_rotation = (cow_rotation + 90) % 360;
		rotation_angle_cow = cow_rotation * TO_RADIAN;

		int x_pos = wall_size / 2.0f;
		int z_pos = wall_size / 2.0f;

		switch (cow_rotation) {
		case 0:
			cow_position = glm::vec3(-wall_size / 2.0f + cow_gap, wall_bottom + 2.0f, -wall_size / 2.0f + cow_gap);
			break;
		case 90 :
			cow_position = glm::vec3(wall_size / 2.0f - cow_gap, wall_bottom + 2.0f, -wall_size / 2.0f + cow_gap);
			break;
		case 180:
			cow_position = glm::vec3(wall_size / 2.0f - cow_gap, wall_bottom + 2.0f, wall_size / 2.0f - cow_gap);
			break;
		case 270:
			cow_position = glm::vec3(-wall_size / 2.0f + cow_gap, wall_bottom + 2.0f, wall_size / 2.0f - cow_gap);
			break;
		}
	}

	glutPostRedisplay();

	if (cow_flag)
		glutTimerFunc(cow_flag, timer_cow, (timestamp_scene + 1) % INT_MAX);
}


unsigned int leftbutton_pressed = 0;
int prevx, prevy;

void mouse(int button, int state, int x, int y)  {
	if (camera_type == CAMERA_WORLD_VIEWER) {
		if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
			prevx = x, prevy = y;
			leftbutton_pressed = 1;
			glutPostRedisplay();
		}
		else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP)) {
			leftbutton_pressed = 0;
			glutPostRedisplay();
		}
	}

/*	else if (camera_type == CAMERA_DRIVER) {
		if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
			ModelMatrix_CAR_BODY_to_DRIVER_seat += ModelMatrix_CAR_BODY_to_DRIVER_angle;

		else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP))
			ModelMatrix_CAR_BODY_to_DRIVER_seat -= ModelMatrix_CAR_BODY_to_DRIVER_angle;
	}*/

	else if (camera_type == CAMERA_OBJECT) {
		if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
			prevx = x, prevy = y;
			leftbutton_pressed = 1;
			glutPostRedisplay();
		}
		else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP)) {
			leftbutton_pressed = 0;
			glutPostRedisplay();
		}
	}
}

#define CAM_ROT_SENSITIVITY_WORLD 0.15f
void camera_motion_1(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	float delx, dely;

	if (leftbutton_pressed) {
		delx = (float)(x - prevx), dely = -(float)(y - prevy);
		prevx = x, prevy = y;

		mat4_tmp = glm::translate(glm::mat4(1.0f), camera.prp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY_WORLD*delx*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		mat4_tmp = glm::translate(mat4_tmp, -camera.prp);

		camera.vrp = glm::vec3(mat4_tmp*glm::vec4(camera.vrp, 1.0f));
		camera.vup = glm::vec3(mat4_tmp*glm::vec4(camera.vup, 0.0f));

		vec3_tmp = glm::cross(camera.vup, camera.vrp - camera.prp);
		mat4_tmp = glm::translate(glm::mat4(1.0f), camera.prp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY_WORLD*dely*TO_RADIAN, vec3_tmp);
		mat4_tmp = glm::translate(mat4_tmp, -camera.prp);

		camera.vrp = glm::vec3(mat4_tmp*glm::vec4(camera.vrp, 1.0f));
		camera.vup = glm::vec3(mat4_tmp*glm::vec4(camera.vup, 0.0f));

		ViewMatrix = glm::lookAt(camera.prp, camera.vrp, camera.vup);

		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
	}
}

void camera_motion_2(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	float delx, dely;

	if (leftbutton_pressed) {
		delx = (float)(x - prevx), dely = -(float)(y - prevy);
		prevx = x, prevy = y;

		mat4_tmp = glm::translate(glm::mat4(1.0f), camera.prp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY_WORLD*delx*TO_RADIAN, camera.vup);
		mat4_tmp = glm::translate(mat4_tmp, -camera.prp);

		camera.vrp = glm::vec3(mat4_tmp*glm::vec4(camera.vrp, 1.0f));
		camera.vup = glm::vec3(mat4_tmp*glm::vec4(camera.vup, 0.0f));

		vec3_tmp = glm::cross(camera.vup, camera.vrp - camera.prp);
		mat4_tmp = glm::translate(glm::mat4(1.0f), camera.prp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY_WORLD*dely*TO_RADIAN, vec3_tmp);
		mat4_tmp = glm::translate(mat4_tmp, -camera.prp);

		camera.vrp = glm::vec3(mat4_tmp*glm::vec4(camera.vrp, 1.0f));
		camera.vup = glm::vec3(mat4_tmp*glm::vec4(camera.vup, 0.0f));

		ViewMatrix = glm::lookAt(camera.prp, camera.vrp, camera.vup);

		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
	}
}

void camera_motion_3(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_right, vec3_up, vec3_view, vec3_tmp;
	float delx, dely, length;

	if (leftbutton_pressed) {
		delx = (float)(x - prevx), dely = -(float)(y - prevy);
		prevx = x, prevy = y;

		length = sqrtf(delx*delx + dely*dely);

		if (length == 0.0f) return;

		vec3_view = camera.vrp - camera.prp; // note that vec3_view is the opposite to the n direction.
		vec3_up = glm::normalize(camera.vup);
		vec3_right = glm::normalize(glm::cross(vec3_up, -vec3_view));

		vec3_tmp = delx*vec3_right + dely*vec3_up;
		vec3_tmp = glm::cross(vec3_tmp, vec3_view);

		mat4_tmp = glm::translate(glm::mat4(1.0f), camera.prp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY_WORLD*length*TO_RADIAN, vec3_tmp);
		mat4_tmp = glm::translate(mat4_tmp, -camera.prp);

		camera.vrp = glm::vec3(mat4_tmp*glm::vec4(camera.vrp, 1.0f));
		camera.vup = glm::vec3(mat4_tmp*glm::vec4(camera.vup, 0.0f));

		ViewMatrix = glm::lookAt(camera.prp, camera.vrp, camera.vup);

		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
	}
}


#define CAM_ROT_SENSITIVITY_OBJECT 0.15f
void object_motion_1(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	float delx, dely;

	if (leftbutton_pressed) {
		delx = (float)(x - prevx), dely = -(float)(y - prevy);
		prevx = x, prevy = y;

		mat4_tmp = glm::translate(glm::mat4(1.0f), camera.vrp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY_OBJECT*delx*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		mat4_tmp = glm::translate(mat4_tmp, -camera.vrp);

		camera.prp = glm::vec3(mat4_tmp*glm::vec4(camera.prp, 1.0f));
		camera.vup = glm::vec3(mat4_tmp*glm::vec4(camera.vup, 0.0f));

		vec3_tmp = glm::cross(camera.vup, camera.vrp - camera.prp);
		mat4_tmp = glm::translate(glm::mat4(1.0f), camera.vrp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY_OBJECT*dely*TO_RADIAN, vec3_tmp);
		mat4_tmp = glm::translate(mat4_tmp, -camera.vrp);

		camera.prp = glm::vec3(mat4_tmp*glm::vec4(camera.prp, 1.0f));
		camera.vup = glm::vec3(mat4_tmp*glm::vec4(camera.vup, 0.0f));

		ViewMatrix = glm::lookAt(camera.prp, camera.vrp, camera.vup);

		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
	}
}

void object_motion_2(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	float delx, dely;

	if (leftbutton_pressed) {
		delx = (float)(x - prevx), dely = -(float)(y - prevy);
		prevx = x, prevy = y;

		mat4_tmp = glm::translate(glm::mat4(1.0f), camera.vrp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY_OBJECT*delx*TO_RADIAN, camera.vup);
		mat4_tmp = glm::translate(mat4_tmp, -camera.vrp);

		camera.prp = glm::vec3(mat4_tmp*glm::vec4(camera.prp, 1.0f));
		camera.vup = glm::vec3(mat4_tmp*glm::vec4(camera.vup, 0.0f));

		vec3_tmp = glm::cross(camera.vup, camera.vrp - camera.prp);

		mat4_tmp = glm::translate(glm::mat4(1.0f), camera.vrp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY_OBJECT*dely*TO_RADIAN, vec3_tmp);
		mat4_tmp = glm::translate(mat4_tmp, -camera.vrp);

		camera.prp = glm::vec3(mat4_tmp*glm::vec4(camera.prp, 1.0f));
		camera.vup = glm::vec3(mat4_tmp*glm::vec4(camera.vup, 0.0f));

		ViewMatrix = glm::lookAt(camera.prp, camera.vrp, camera.vup);

		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
	}
}

void object_motion_3(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_right, vec3_up, vec3_view, vec3_tmp;
	float delx, dely, length;

	if (leftbutton_pressed) {
		delx = (float)(x - prevx), dely = -(float)(y - prevy);
		prevx = x, prevy = y;

		length = sqrtf(delx*delx + dely*dely);

		if (length == 0.0f) return;

		vec3_view = camera.vrp - camera.prp; // note that vec3_view is the opposite to the n direction.
		vec3_up = glm::normalize(camera.vup);
		vec3_right = glm::normalize(glm::cross(vec3_up, -vec3_view));

		vec3_tmp = delx*vec3_right + dely*vec3_up;
		vec3_tmp = glm::cross(vec3_tmp, vec3_view);

		mat4_tmp = glm::translate(glm::mat4(1.0f), camera.vrp);
		mat4_tmp = glm::rotate(mat4_tmp, CAM_ROT_SENSITIVITY_OBJECT*length*TO_RADIAN, vec3_tmp);
		mat4_tmp = glm::translate(mat4_tmp, -camera.vrp);

		camera.prp = glm::vec3(mat4_tmp*glm::vec4(camera.prp, 1.0f));
		camera.vup = glm::vec3(mat4_tmp*glm::vec4(camera.vup, 0.0f));

		ViewMatrix = glm::lookAt(camera.prp, camera.vrp, camera.vup);

		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
	}
}



#define CAMERA_KEY_MOVE_SPEED	1
#define CAMERA_KEY_ROTATION_SPEED	30

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case '1': case '!':
		camera_type = CAMERA_WORLD_VIEWER;
		CAMERA_AXIS_SELECT = CAMERA_AXIS_N;
		camera.zoom_factor = 2.0f;
		cam_z_rotation_angle = 0;

		glutMotionFunc(camera_motion_1);

		ProjectionMatrix = glm::perspective(camera.zoom_factor*15.0f*TO_RADIAN, camera.aspect_ratio, 1.0f, 1000.0f);

		set_ViewMatrix_for_world_viewer();
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
		break;

	case '2': case '@':
		camera_type = CAMERA_DRIVER;
		CAMERA_AXIS_SELECT = CAMERA_AXIS_N;
		camera.zoom_factor = 2.0f;
		ProjectionMatrix = glm::perspective(camera.zoom_factor*15.0f*TO_RADIAN, camera.aspect_ratio, 1.0f, 1000.0f);

		ModelMatrix_CAR_BODY_to_DRIVER_rotation = 0;
		ModelMatrix_CAR_BODY_to_DRIVER_updown = 0;
		ModelMatrix_CAR_BODY_to_DRIVER_seat = 0;

		glutPostRedisplay();
		break;

	case '3': case '#':
		camera_type = CAMERA_OBJECT;
		CAMERA_AXIS_SELECT = CAMERA_AXIS_N;
		camera.zoom_factor = 2.0f;
		cam_object_z_rotation_angle = 0;

		glutMotionFunc(object_motion_1);

		set_Camera_for_object();
		ProjectionMatrix = glm::perspective(camera.zoom_factor*15.0f*TO_RADIAN, camera.aspect_ratio, 1.0f, 1000.0f);
		
		set_ViewMatrix_for_object();
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
		break;

	case 'r': case 'R':
		CAMERA_AXIS_SELECT = (CAMERA_AXIS_SELECT + CAMERA_AXIS - 1) % CAMERA_AXIS;
		printf("Control motion_%d\n", CAMERA_AXIS_SELECT);
		if (camera_type == CAMERA_WORLD_VIEWER) {
			switch (CAMERA_AXIS_SELECT) {
			case CAMERA_AXIS_N: glutMotionFunc(camera_motion_1); break;
			case CAMERA_AXIS_U: glutMotionFunc(camera_motion_2); break;
			case CAMERA_AXIS_V: glutMotionFunc(camera_motion_3); break;
			}
		}

		else if (camera_type == CAMERA_OBJECT) {
			switch (CAMERA_AXIS_SELECT) {
			case CAMERA_AXIS_N: glutMotionFunc(object_motion_1); break;
			case CAMERA_AXIS_U: glutMotionFunc(object_motion_2); break;
			case CAMERA_AXIS_V: glutMotionFunc(object_motion_3); break;
			}
		}
		break;

	case 't': case 'T':
		CAMERA_AXIS_SELECT = (CAMERA_AXIS_SELECT + 1) % CAMERA_AXIS;
		printf("Control motion_%d\n", CAMERA_AXIS_SELECT);
		if (camera_type == CAMERA_WORLD_VIEWER) {
			switch (CAMERA_AXIS_SELECT) {
			case CAMERA_AXIS_N: glutMotionFunc(camera_motion_1); break;
			case CAMERA_AXIS_U: glutMotionFunc(camera_motion_2); break;
			case CAMERA_AXIS_V: glutMotionFunc(camera_motion_3); break;
			}
		}

		else if (camera_type == CAMERA_OBJECT) {
			switch (CAMERA_AXIS_SELECT) {
			case CAMERA_AXIS_N: glutMotionFunc(object_motion_1); break;
			case CAMERA_AXIS_U: glutMotionFunc(object_motion_2); break;
			case CAMERA_AXIS_V: glutMotionFunc(object_motion_3); break;
			}
		}
		break;

	case '+': case '=':
		wall_size += 10;
		glutPostRedisplay();
		break;

	case '-': case '_':
		wall_size -= 10;
		if (wall_size <= 0)
			wall_size = 10;
		glutPostRedisplay();
		break;

	case '[': // 다운키
		wall_bottom -= 10;
		glutPostRedisplay();
		break;
	case ']':
		wall_bottom += 10;
		glutPostRedisplay();
		break;

	case 'i': case 'I':
		if (car_speed) 
			car_speed = car_stop;
		else {
			car_speed = car_run;
			glutTimerFunc(car_speed, timer_scene, (car_timestamp + 1) % INT_MAX);
		}
		break;

	case 'o': case 'O':
		if (teapot_speed) teapot_speed = teapot_stop;
		else {
			teapot_speed = teapot_run;
			glutTimerFunc(teapot_speed, timer_tee, (teapot_timestamp + 1) % INT_MAX);
		}
		break;

	case 'p': case 'P':
		if (cow_flag) cow_flag = cow_stop;
		else {
			cow_flag = cow_run;
			glutTimerFunc(cow_flag, timer_cow, (cow_timestamp + 1) % INT_MAX);
		}
		break;

	case 'z': case 'Z':
		camera.zoom_factor *= 0.95f;
		if (camera.zoom_factor < 0.1f)
			camera.zoom_factor = 0.1f;
		ProjectionMatrix = glm::perspective(camera.zoom_factor*15.0f*TO_RADIAN, camera.aspect_ratio, 1.0f, 1000.0f);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
		break;
		
	case 'x': case 'X':
		camera.zoom_factor *= 1.05f;
		if (camera.zoom_factor > 6.0f)
			camera.zoom_factor = 6.0f;
		ProjectionMatrix = glm::perspective(camera.zoom_factor*15.0f*TO_RADIAN, camera.aspect_ratio, 1.0f, 1000.0f);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		glutPostRedisplay();
		break;

	case 'w': case 'W':
		if (camera_type == CAMERA_WORLD_VIEWER) {
			cam_world_position_frontback(CAMERA_KEY_MOVE_SPEED);
			set_ViewMatrix_for_world_viewer();
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}
		else if (camera_type == CAMERA_DRIVER)
			ModelMatrix_CAR_BODY_to_DRIVER_updown -= ModelMatrix_CAR_BODY_to_DRIVER_angle;

		else if (camera_type == CAMERA_OBJECT) {
			cam_object_postion(CAMERA_KEY_MOVE_SPEED * 10);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}

		glutPostRedisplay();
		break;

	case 's': case 'S':
		if (camera_type == CAMERA_WORLD_VIEWER) {
			cam_world_position_frontback(-CAMERA_KEY_MOVE_SPEED);
			set_ViewMatrix_for_world_viewer();
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}
		else if (camera_type == CAMERA_DRIVER)
			ModelMatrix_CAR_BODY_to_DRIVER_updown += ModelMatrix_CAR_BODY_to_DRIVER_angle;

		else if (camera_type == CAMERA_OBJECT) {
			cam_object_postion(-CAMERA_KEY_MOVE_SPEED * 10);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}

		glutPostRedisplay();
		break;

	case 'a': case 'A':
		if (camera_type == CAMERA_WORLD_VIEWER) {
			cam_world_position_leftright(-CAMERA_KEY_MOVE_SPEED);
			set_ViewMatrix_for_world_viewer();
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}
		else if (camera_type == CAMERA_DRIVER)
			ModelMatrix_CAR_BODY_to_DRIVER_rotation += ModelMatrix_CAR_BODY_to_DRIVER_angle;

		glutPostRedisplay();
		break;

	case 'd': case 'D':
		if (camera_type == CAMERA_WORLD_VIEWER) {
			cam_world_position_leftright(CAMERA_KEY_MOVE_SPEED);
			set_ViewMatrix_for_world_viewer();
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}
		else if (camera_type == CAMERA_DRIVER)
			ModelMatrix_CAR_BODY_to_DRIVER_rotation -= ModelMatrix_CAR_BODY_to_DRIVER_angle;

		glutPostRedisplay();
		break;

	case 'q': case 'Q':
		if (camera_type == CAMERA_WORLD_VIEWER) {
			cam_z_rotation_angle -= 10;
			cam_object_z_rotation(cam_z_rotation_angle);
			set_ViewMatrix_for_world_viewer();
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}
		else if (camera_type == CAMERA_DRIVER)
			ModelMatrix_CAR_BODY_to_DRIVER_seat += ModelMatrix_CAR_BODY_to_DRIVER_angle;

		else if (camera_type == CAMERA_OBJECT) {
			cam_object_z_rotation_angle -= 10;
			cam_object_z_rotation(cam_object_z_rotation_angle);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}

		glutPostRedisplay();
		break;

	case 'e': case 'E':
		if (camera_type == CAMERA_WORLD_VIEWER) {
			cam_z_rotation_angle += 10;
			cam_object_z_rotation(cam_z_rotation_angle);
			set_ViewMatrix_for_world_viewer();
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}
		else if (camera_type == CAMERA_DRIVER)
			ModelMatrix_CAR_BODY_to_DRIVER_seat -= ModelMatrix_CAR_BODY_to_DRIVER_angle;

		else if (camera_type == CAMERA_OBJECT) {
			cam_object_z_rotation_angle += 10;
			cam_object_z_rotation(cam_object_z_rotation_angle);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}

		glutPostRedisplay();
		break;

	case 'f': case 'F':
		if (camera_type == CAMERA_WORLD_VIEWER) {
			cam_world_position_updown(CAMERA_KEY_MOVE_SPEED);
			set_ViewMatrix_for_world_viewer();
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}
		break;

	case 'g': case 'G':
		if (camera_type == CAMERA_WORLD_VIEWER) {
			cam_world_position_updown(-CAMERA_KEY_MOVE_SPEED);
			set_ViewMatrix_for_world_viewer();
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		}
		break;

	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}


void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	
	camera.aspect_ratio = (float)width / height;

	ProjectionMatrix = glm::perspective(TO_RADIAN*camera.fovy, camera.aspect_ratio, camera.near_c, camera.far_c);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	glutPostRedisplay();
}


void cleanup(void) {
	glDeleteVertexArrays(1, &axes_VAO);
	glDeleteBuffers(1, &axes_VBO);
}
/*********************************  END: callbacks *********************************/

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(camera_motion_1);
	glutReshapeFunc(reshape);
	glutTimerFunc(car_speed, timer_scene, 0);
	glutTimerFunc(teapot_speed, timer_tee, 0);
	glutTimerFunc(cow_speed, timer_cow, 0);	
	glutCloseFunc(cleanup);
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
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	ViewMatrix = glm::mat4(1.0f);
	ProjectionMatrix = glm::mat4(1.0f);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix; 

	ModelMatrix_CAR_BODY = glm::mat4(1.0f);
	ModelMatrix_CAR_WHEEL = glm::mat4(1.0f);
	ModelMatrix_CAR_NUT = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_axes(); 
	prepare_path();
	prepare_geom_obj(GEOM_OBJ_ID_CAR_BODY, "Data/car_body_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_CAR_WHEEL, "Data/car_wheel_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_CAR_NUT, "Data/car_nut_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_COW, "Data/cow_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_TEAPOT, "Data/teapot_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_BOX, "Data/box_triangles_v.txt", GEOM_OBJ_TYPE_V);

	// 4.5.5
	prepare_geom_obj(GEOM_OBJ_ID_SQUARE16, "Data/Square16_triangles_vnt.geom", 8 * sizeof(float));
	prepare_geom_obj(GEOM_OBJ_ID_TIGER, "Data/Tiger_00_triangles_vnt.geom", 8 * sizeof(float));
	prepare_geom_obj(GEOM_OBJ_ID_COW2, "Data/Cow_triangles_v.geom", 3 * sizeof(float));
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
	initialize_camera();
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

void print_message(const char * m) {
	fprintf(stdout, "%s\n\n", m);
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

#define N_MESSAGE_LINES 2
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 4.7.1.Hier_Car_Dummy_Driver_GLSL";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: 'f', l', 'd', 'w', 'o', 'ESC'",
											"    - Mouse used: L-Click and move" };

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutMainLoop();
}
