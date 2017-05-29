/*********************************  START: camera *********************************/
typedef struct _Camera {
	glm::vec3 prp, vrp, vup;
	float zoom_factor;
	float fovy, aspect_ratio, near_c, far_c;
	int move;
} Camera;

Camera camera;
enum _CameraType { CAMERA_WORLD_VIEWER, CAMERA_DRIVER, CAMERA_OBJECT } camera_type;

void set_Camera_for_object(void) {
	camera.prp = glm::vec3(0.0f, 10.0f, 75.0f);
	camera.vrp = glm::vec3(0.0f, 10.0f, 0.0f);
	camera.vup = glm::vec3(0.0f, 1.0f, 0.0f);
	ViewMatrix = glm::lookAt(camera.prp, camera.vrp, camera.vup);
//	camera.vup = glm::vec3(ViewMatrix[0].y, ViewMatrix[1].y, ViewMatrix[2].y); // in this example code, make vup always equal to the v direction.

//	camera.move = 0;
//	camera.fovy = 30.0f, camera.aspect_ratio = 1.0f; camera.near_c = 5.0f; camera.far_c = 10000.0f;
//	camera.zoom_factor = 2.0f; // will be used for zoomming in and out.

//	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

void set_ViewMatrix_for_object(void) {
	ViewMatrix = glm::lookAt(camera.prp, camera.vrp, camera.vup);
}

void set_ViewMatrix_for_world_viewer(void) {
	ViewMatrix = glm::lookAt(camera.prp, camera.vrp, camera.vup);
}

#define  ModelMatrix_CAR_BODY_to_DRIVER_angle	5.0f
float ModelMatrix_CAR_BODY_to_DRIVER_rotation = 0;
float ModelMatrix_CAR_BODY_to_DRIVER_updown = 0;
float ModelMatrix_CAR_BODY_to_DRIVER_seat = 0;

float cam_z_rotation_angle = 0;
float cam_object_z_rotation_angle = 0;


void set_ViewMatrix_for_driver(void) {
	glm::mat4 Matrix_CAMERA_driver_inverse;

	ModelMatrix_CAR_BODY_to_DRIVER = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.5f, 2.5f));
	ModelMatrix_CAR_BODY_to_DRIVER = glm::rotate(ModelMatrix_CAR_BODY_to_DRIVER, ModelMatrix_CAR_BODY_to_DRIVER_rotation * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_CAR_BODY_to_DRIVER = glm::rotate(ModelMatrix_CAR_BODY_to_DRIVER, ModelMatrix_CAR_BODY_to_DRIVER_updown * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix_CAR_BODY_to_DRIVER = glm::rotate(ModelMatrix_CAR_BODY_to_DRIVER, ModelMatrix_CAR_BODY_to_DRIVER_seat * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix_CAR_BODY_to_DRIVER = glm::rotate(ModelMatrix_CAR_BODY_to_DRIVER,
		TO_RADIAN*90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	Matrix_CAMERA_driver_inverse = ModelMatrix_CAR_BODY * ModelMatrix_CAR_BODY_to_DRIVER;

	ViewMatrix = glm::affineInverse(Matrix_CAMERA_driver_inverse);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

/*
void set_ViewMatrix_for_driver2(void) { // This version does not use the glm::affineInverse(*) function.
	glm::mat4 Matrix_CAMERA;

	Matrix_CAMERA = glm::rotate(glm::mat4(1.0f), -rotation_angle_car, glm::vec3(0.0f, 1.0f, 0.0f));
	Matrix_CAMERA = glm::translate(Matrix_CAMERA, glm::vec3(20.0f, 4.89f, 0.0f));
	Matrix_CAMERA = glm::rotate(Matrix_CAMERA, 90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	Matrix_CAMERA = glm::translate(Matrix_CAMERA, glm::vec3(-3.0f, 0.5f, 2.5f));
	Matrix_CAMERA = glm::rotate(Matrix_CAMERA, TO_RADIAN*90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	ViewMatrix = glm::affineInverse(Matrix_CAMERA);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}
*/

void initialize_camera(void) {
	camera_type = CAMERA_WORLD_VIEWER;

	//ViewMatrix = glm::lookAt(glm::vec3(0.0f, 10.0f, 75.0f), glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	camera.prp = glm::vec3(0.0f, 10.0f, 75.0f);
	camera.vrp = glm::vec3(0.0f, 10.0f, 0.0f);
	camera.vup = glm::vec3(0.0f, 1.0f, 0.0f);
	ViewMatrix = glm::lookAt(camera.prp, camera.vrp, camera.vup);
	camera.vup = glm::vec3(ViewMatrix[0].y, ViewMatrix[1].y, ViewMatrix[2].y); // in this example code, make vup always equal to the v direction.

	camera.move = 0;
	camera.fovy = 30.0f, camera.aspect_ratio = 1.0f; camera.near_c = 5.0f; camera.far_c = 10000.0f;
	camera.zoom_factor = 2.0f; // will be used for zoomming in and out.

	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	// the transformation that moves the driver's camera frame from car body's MC to driver seat
/*	ModelMatrix_CAR_BODY_to_DRIVER = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.5f, 2.5f));
	ModelMatrix_CAR_BODY_to_DRIVER = glm::rotate(ModelMatrix_CAR_BODY_to_DRIVER,
		TO_RADIAN*90.0f, glm::vec3(0.0f, 1.0f, 0.0f));*/
}

//#define CAM_TSPEED 0.05f
//void renew_cam_position(int del) {
//	camera.pos += CAM_TSPEED*del*(-camera.naxis);
//}
//
//#define CAM_RSPEED 0.1f
//void renew_cam_orientation_rotation_around_v_axis(int angle) {
//	glm::mat3 RotationMatrix;
//
//	RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0), CAM_RSPEED*TO_RADIAN*angle, camera.vaxis));
//	camera.uaxis = RotationMatrix*camera.uaxis;
//	camera.naxis = RotationMatrix*camera.naxis;
//}
/*********************************  END: camera *********************************/

#define CAMERA_AXIS	3
#define CAMERA_AXIS_U	0
#define CAMERA_AXIS_V	1
#define CAMERA_AXIS_N	2

int CAMERA_AXIS_SELECT = CAMERA_AXIS_N;


void cam_world_position_frontback(int del) {
	glm::vec3 vec;

	vec = glm::normalize(camera.vrp - camera.prp);
	vec *= del;

	camera.prp += vec;
	camera.vrp += vec;
	camera.vup += vec;
}

void cam_world_position_leftright(int del) {
	glm::vec3 vec, vpn;

	vpn = camera.prp - camera.vrp;
	vec = glm::normalize(glm::cross(camera.vup, vpn));

	vec *= del;

	camera.prp += vec;
	camera.vrp += vec;
}

void cam_world_position_updown(int del) {
	glm::vec3 vec_u, vec_v, vpn;

	vpn = camera.prp - camera.vrp;
	vec_u = glm::cross(camera.vup, vpn);
	vec_v = glm::normalize(glm::cross(vpn, vec_u));

	vec_v *= del;

	camera.prp += vec_v;
	camera.vrp += vec_v;
}

void cam_object_postion(int del) {
	glm::vec3 vec;

	vec = glm::normalize(camera.vrp - camera.prp);
	vec *= del;
	
	camera.prp += vec;
//	camera.vrp += vec;
}

void cam_object_z_rotation(float angle) {
	glm::vec3 vec;

	vec = glm::normalize(camera.vup);

	float rad = angle * TO_RADIAN;
	vec.x = vec.x + sin(rad);
	vec.y = vec.y + cos(rad);

	camera.vup = vec;
}