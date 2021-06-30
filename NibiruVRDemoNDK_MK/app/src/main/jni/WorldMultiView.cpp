/*
 * TestCardboard.cpp
 *
 *  Created on: 2016��3��22��
 *      Author: Steven
 */

#include "NVRScene.h"
#include <EGL/egl.h>
#include "time.h"
#include <unistd.h>			// for usleep
#include <GLES3/gl3.h>
#include "nvr/include/NVRUtils.h"

#define CAMERA_Z 0.1
#define TIME_DELTA 0.9
#define Z_NEAR 0.1
#define Z_FAR 100

class WorldMultiView: public NVRScene {
public:
	WorldMultiView() {

		LIGHT_POS_IN_WORLD_SPACE.x = 0.0;
		LIGHT_POS_IN_WORLD_SPACE.y = 2.0;
		LIGHT_POS_IN_WORLD_SPACE.z = 0.0;
		LIGHT_POS_IN_WORLD_SPACE.w = 1.0;

	}

	void onSceneDestroyed()
	{

	}

	void setAssets(JNIEnv *env, jobject &assets) {
		mUtils.setAssetsManger(env, assets);
	}

	void setNVRApi(NibiruVRApi *api) {
		mNVRApi = api;
	}

	void onPreDraw(float * headPosition) {

		modelCube.setRotate(0, TIME_DELTA * rcount, 0.5, 0.5, 1.0);

		if (support6Dof) {
			camera.setLookAtM(0, headPosition[0], headPosition[1],
					headPosition[2] + CAMERA_Z, headPosition[0],
					headPosition[1], headPosition[2], 0.0f, 1.0f, 0.0f);
		} else {
			camera.setLookAtM(0, 0.0f, 0.0f, CAMERA_Z, 0.0f, 0.0f, 0.0f, 0.0f,
					1.0f, 0.0f);
		}
		glCheck("ready to draw");

		rcount += 1;
//		 camera.lookAt()
	}

	void onDrawEye(nvr_Eye *eye) {

	}

	void onDrawMultiView(nvr_Eye *eyes) {
		glEnable (GL_DEPTH_TEST);

		mat_view[0] = eyes[0].eyeRotationView * camera;
		mat_view[1] = eyes[1].eyeRotationView * camera;

		mat_model_view[0] = mat_view[0] * modelCube;
		mat_model_view[1] = mat_view[1] * modelCube;

		mat_model_view_projection[0] = eyes[0].projection * mat_model_view[0];
		mat_model_view_projection[1] = eyes[1].projection * mat_model_view[1];

		drawCube();

		mat_model_view[0] = mat_view[0] * modelFloor;
		mat_model_view[1] = mat_view[1] * modelFloor;

		mat_model_view_projection[0] = eyes[0].projection * mat_model_view[0];
		mat_model_view_projection[1] = eyes[1].projection * mat_model_view[1];

		drawFloor();
	}

	void onPostDraw() {

	}

	void drawCube() {
		glUseProgram(cubeProgram);

		glUniformMatrix4fv(mCubeModelProg, 1, GL_FALSE, modelCube.value_ptr());
////
////
		glUniformMatrix4fv(mCubeModelViewProjProg, 2, GL_FALSE,
				mat_model_view_projection[0].value_ptr());

		glBindBuffer(GL_ARRAY_BUFFER, cubeVertices);

		glVertexAttribPointer(mCubePosProg, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(mCubePosProg);

		glBindBuffer(GL_ARRAY_BUFFER, cubeColor);

		glVertexAttribPointer(mCubeColorProg, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(mCubeColorProg);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glCheck("draw cube");
	}

	void drawFloor() {
		glUseProgram(floorProgram);

		glUniformMatrix4fv(mFloorModelProg, 1, GL_FALSE,
				modelFloor.value_ptr());

		glUniformMatrix4fv(mFloorModelViewProjProg, 2, GL_FALSE,
				mat_model_view_projection[0].value_ptr());

		glBindBuffer(GL_ARRAY_BUFFER, floorVertices);

		glVertexAttribPointer(mFloorPosProg, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(mFloorPosProg);

		glBindBuffer(GL_ARRAY_BUFFER, floorColor);
		glVertexAttribPointer(mFloorColorProg, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(mFloorColorProg);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glCheck("draw floor");
	}

	void initWorld() {
		glClearColor(0.1f, 0.1f, 0.1f, 0.5f);

		rcount = 0;
		initData();

		ShaderUnit cube;
		mUtils.loadShaderUnit(&cube, "multiview_cube_vertex.shader",
				"multiview_cube_fragment.shader");

		cubeProgram = cube.mProgram;
		glUseProgram(cubeProgram);

		glCheck("load cube program");

		mCubePosProg = glGetAttribLocation(cubeProgram, "vertexPosition");
		mCubeColorProg = glGetAttribLocation(cubeProgram, "a_Color");

		mCubeModelProg = glGetUniformLocation(cubeProgram, "model");
		mCubeModelViewProjProg = glGetUniformLocation(cubeProgram,
				"modelViewProjection");

		NLOGI("cube programe : %d, %d , %d , %d", mCubePosProg, mCubeColorProg,
				mCubeModelProg, mCubeModelViewProjProg);

		ShaderUnit floor;
		mUtils.loadShaderUnit(&floor, "multiview_floor_vertex.shader",
				"multiview_floor_fragment.shader");

		floorProgram = floor.mProgram;
		glUseProgram(floorProgram);

		glCheck("load floor program");

		mFloorPosProg = glGetAttribLocation(floorProgram, "vertexPosition");
		mFloorColorProg = glGetAttribLocation(floorProgram, "a_Color");

		mFloorModelProg = glGetUniformLocation(floorProgram, "model");
		mFloorModelViewProjProg = glGetUniformLocation(floorProgram,
				"modelViewProjection");

		NLOGI("floor programe : %d , %d , %d , %d", mFloorPosProg,
				mFloorColorProg, mFloorModelProg, mFloorModelViewProjProg);

		modelCube.setIdentity(0);
		modelCube.translate(0.0f, 0.0f, 0.0f, -8.0f);

		modelFloor.setIdentity(0);
		modelFloor.translate(0.0f, 0.0f, -25.0f, 0.0f);

		if (mNVRApi != NULL) {
			mNVRApi->nvr_ShowGaze();
		}

		glCheck("init completed");
	}

	void onSceneCreated() {
		initWorld();
	}

	GLuint makeData(float *data, int len) {
		GLuint result = 0;
		glGenBuffers(1, &result);
		glBindBuffer(GL_ARRAY_BUFFER, result);
		glBufferData(GL_ARRAY_BUFFER, len, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return result;
	}

	void initData() {
		float cube_coords[] = {
				// Front face
				-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

				// Right face
				1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f,

				// Back face
				1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f,
				-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f,

				// Left face
				-1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
				-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,

				// Top face
				-1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
				1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,

				// Bottom face
				1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, };

		cubeVertices = makeData(cube_coords, sizeof(cube_coords));

		float cube_color[] = {
				// front, green
				0, 0.5273, 0.2656, 1.0, 0, 0.5273, 0.2656, 1.0, 0, 0.5273,
				0.2656, 1.0, 0, 0.5273, 0.2656, 1.0, 0, 0.5273, 0.2656, 1.0, 0,
				0.5273, 0.2656, 1.0,

				// right, blue
				0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f, 0.9023f, 1.0f,
				0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f, 0.9023f, 1.0f,
				0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f, 0.9023f, 1.0f,

				// back, also green
				0, 0.5273, 0.2656, 1.0, 0, 0.5273, 0.2656, 1.0, 0, 0.5273,
				0.2656, 1.0, 0, 0.5273, 0.2656, 1.0, 0, 0.5273, 0.2656, 1.0, 0,
				0.5273, 0.2656, 1.0,

				// left, also blue
				0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f, 0.9023f, 1.0f,
				0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f, 0.9023f, 1.0f,
				0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f, 0.9023f, 1.0f,

				// top, red
				0.8359375f, 0.17578125f, 0.125f, 1.0f, 0.8359375f, 0.17578125f,
				0.125f, 1.0f, 0.8359375f, 0.17578125f, 0.125f, 1.0f, 0.8359375f,
				0.17578125f, 0.125f, 1.0f, 0.8359375f, 0.17578125f, 0.125f,
				1.0f, 0.8359375f, 0.17578125f, 0.125f, 1.0f,

				// bottom, also red
				0.8359375f, 0.17578125f, 0.125f, 1.0f, 0.8359375f, 0.17578125f,
				0.125f, 1.0f, 0.8359375f, 0.17578125f, 0.125f, 1.0f, 0.8359375f,
				0.17578125f, 0.125f, 1.0f, 0.8359375f, 0.17578125f, 0.125f,
				1.0f, 0.8359375f, 0.17578125f, 0.125f, 1.0f, };

//		   int i = 0;
//
//		   while( i < (sizeof(cube_color) / sizeof(float)) ){
//			   LOGI("%f %f %f %f\n", cube_color[i], cube_color[i+1], cube_color[i+2], cube_color[i+3]);
//			   i += 4;
//		   }

		cubeColor = makeData(cube_color, sizeof(cube_color));

		float cube_normal[] = {
				// Front face
				0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

				// Right face
				1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

				// Back face
				0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
				0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,

				// Left face
				-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
				0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

				// Top face
				0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

				// Bottom face
				0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
				-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f };

		cubeNormal = makeData(cube_normal, sizeof(cube_normal));

		float floor_coords[] = { 200, 0, -200, -200, 0, -200, -200, 0, 200, 200,
				0, -200, -200, 0, 200, 200, 0, 200, };

		floorVertices = makeData(floor_coords, sizeof(floor_coords));

		float floor_normal[] = { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, };

		floorNormals = makeData(floor_normal, sizeof(floor_normal));

		float floor_color[] = { 0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f,
				0.9023f, 1.0f, 0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f,
				0.9023f, 1.0f, 0.0f, 0.3398f, 0.9023f, 1.0f, 0.0f, 0.3398f,
				0.9023f, 1.0f, };

		floorColor = makeData(floor_color, sizeof(floor_color));

	}
private:
	NVRUtils mUtils;

	GLuint cubeVertices;
	GLuint cubeColor;
	GLuint cubeNormal;

	GLuint floorVertices;
	GLuint floorColor;
	GLuint floorNormals;

	GLuint cubeProgram;
	GLuint floorProgram;

	GLuint mCubePosProg;
	GLuint mCubeColorProg;

	GLuint mCubeModelProg;
	GLuint mCubeModelViewProjProg;

	GLuint mFloorPosProg;
	GLuint mFloorColorProg;

	GLuint mFloorModelProg;
	GLuint mFloorModelViewProjProg;

	int rcount;

	mat4 modelCube;
	mat4 camera;
	mat4 headView;

//   mat4 view;
//   mat4 modelViewProjection;
//   mat4 modelView;

	mat4 modelFloor;

	mat4 mat_view[2];
	mat4 mat_model_view[2];
	mat4 mat_model_view_projection[2];

	vec4 LIGHT_POS_IN_WORLD_SPACE;

	NibiruVRApi *mNVRApi = NULL;
};

