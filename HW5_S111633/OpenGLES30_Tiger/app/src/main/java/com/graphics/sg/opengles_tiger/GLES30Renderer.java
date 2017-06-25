package com.graphics.sg.opengles_tiger;

import android.content.Context;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;

import java.text.DecimalFormat;

import javax.microedition.khronos.opengles.GL10;

public class GLES30Renderer implements GLSurfaceView.Renderer{

    private Context mContext;

    private Axis mAxis;
    private Floor mFloor;
    private Tiger mTiger;

    private Objects mOptimus;
    private Objects mDragon;
    private Objects mCow;
    private Objects mCar;
    private Objects mSquare;

    public float fovy = 45.0f;
    public float ratio = 1.0f;

    // Animation frame index
    final static int N_TIGER_FRAMES = 12;
    int cur_frame_tiger = 0;
    int rotation_angle_tiger = 0;

    public float[] mMVPMatrix = new float[16];
    public float[] mProjectionMatrix = new float[16];
    public float[] mModelViewMatrix = new float[16];
    public float[] mModelMatrix = new float[16];
    public float[] mViewMatrix = new float[16];
    public float[] mModelViewInvTrans = new float[16];

    // Texture
    final static int TEXTURE_ID_FLOOR = 0;
    final static int TEXTURE_ID_TIGER = 1;

    final static int TEXTURE_ID_OPTIMUS = 2;
    final static int TEXTURE_ID_DRAGON = 3;
    final static int TEXTURE_ID_COW = 4;
    final static int TEXTURE_ID_CAR = 5;
    final static int TEXTURE_ID_SQUARE = 6;


    public int mDragon_PathNow = 0;
    public int mDragon_PathStamp = 0;
    public float[][] mDragon_Path = new float[4][2];


    // OpenGL Handles
    private PhongShadingProgram mPhongShadingProgram;
    private SimpleProgram mSimpleProgram;

    public GLES30Renderer(Context context) {
        mContext = context;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, javax.microedition.khronos.egl.EGLConfig config) {
        GLES30.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        GLES30.glEnable(GLES30.GL_DEPTH_TEST);

        // matrix, offset, eye xyz, center xyz, top xyz.
        // View Matrix 설정. 고정 된 카메라이기 때문에 한 번만 호출한다.
        Matrix.setLookAtM(mViewMatrix, 0, 300f, 300f, 300f, 0f, 0f, 0f, 0f, 1.0f, 0f);

        // GL Program을 생성.
        // phong shading을 수행하는 프로그램.
        mPhongShadingProgram = new PhongShadingProgram(
                AssetReader.readFromFile("phong.vert", mContext),
                AssetReader.readFromFile("phong.frag", mContext) );
        mPhongShadingProgram.prepare();
        mPhongShadingProgram.initLightsAndMaterial();
        mPhongShadingProgram.initFlags();
        mPhongShadingProgram.set_up_scene_lights(mViewMatrix);

        // 축을 그릴 때 사용하는 프로그램.
        mSimpleProgram = new SimpleProgram(
                AssetReader.readFromFile("simple.vert", mContext),
                AssetReader.readFromFile("simple.frag", mContext) );
        mSimpleProgram.prepare();

        // 화면에 그릴 오브젝트들을 생성.
        // 축 오브젝트 로드.
        mAxis = new Axis();
        mAxis.prepare();

        // 바닥 오브젝트 로드.
        mFloor = new Floor();
        mFloor.prepare();
        mFloor.setTexture(AssetReader.getBitmapFromFile("grass_tex.jpg", mContext), TEXTURE_ID_FLOOR);


        // Tiger 오브젝트 로드.
        int nBytesPerVertex = 8 * 4;        // 3 for vertex, 3 for normal, 2 for texcoord, 4 is sizeof(float)
        int nBytesPerTriangles = nBytesPerVertex * 3;

        // 12개에 해당하는 각 오브젝트들을 모두 로드해서 Tiger 객체에 추가한다.
        mTiger = new Tiger();
        for(int i=0 ; i<N_TIGER_FRAMES ; i++) {
            DecimalFormat dfm = new DecimalFormat("00");
            String num = dfm.format(i);
            String filename = "Tiger_" + num + "_triangles_vnt.geom";
            mTiger.addGeometry(AssetReader.readGeometry(filename, nBytesPerTriangles, mContext));
        }
        mTiger.prepare();
        mTiger.setTexture(AssetReader.getBitmapFromFile("tiger_tex.jpg", mContext), TEXTURE_ID_TIGER);


        // Optimus 오브젝트 로드
        mOptimus = new Objects();
        mOptimus.addGeometry(AssetReader.readGeometry("optimus_vnt.geom", nBytesPerTriangles, mContext));
        mOptimus.prepare();
        mOptimus.setTexture(AssetReader.getBitmapFromFile("texture3.jpg", mContext), TEXTURE_ID_OPTIMUS);

        // Dragon 오브젝트 로드
        mDragon = new Objects();
        mDragon.addGeometry(AssetReader.readGeometry("dragon_vnt.geom", nBytesPerTriangles, mContext));
        mDragon.prepare();
        mDragon.setTexture(AssetReader.getBitmapFromFile("texture1.jpg", mContext), TEXTURE_ID_DRAGON);

        // Cow 오브젝트 로드
        mCow = new Objects();
        mCow.addGeometry(AssetReader.readGeometry("Cow_triangles_vn.geom", nBytesPerTriangles, mContext));
        mCow.prepare();
        mCow.setTexture(AssetReader.getBitmapFromFile("texture2.jpg", mContext), TEXTURE_ID_COW);

        // Car 오브젝트 로드
        mCar = new Objects();
        mCar.addGeometry(AssetReader.readGeometry("Car_triangles_vnt.geom", nBytesPerTriangles, mContext));
        mCar.prepare();
        mCar.setTexture(AssetReader.getBitmapFromFile("texture4.jpg", mContext), TEXTURE_ID_CAR);

        // Square 오브젝트 로드
        mSquare = new Objects();
        mSquare.addGeometry(AssetReader.readGeometry("Square16_triangles_vnt.geom", nBytesPerTriangles, mContext));
        mSquare.prepare();

        // Dragon Path 설정
        mDragon_Path[0][0] = -250.0f; mDragon_Path[0][1] = 250.0f;
        mDragon_Path[1][0] = 250.0f; mDragon_Path[1][1] = 250.0f;
        mDragon_Path[2][0] = 250.0f; mDragon_Path[2][1] = -250.0f;
        mDragon_Path[3][0] = -250.0f; mDragon_Path[3][1] = -250.0f;
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        int pId;
        int timestamp = getTimeStamp();

        cur_frame_tiger = timestamp % N_TIGER_FRAMES;
        rotation_angle_tiger = timestamp % 360;

        float slow_timestamp = (timestamp / 50.0f);
        float optimus_height = (float) Math.abs(Math.sin(slow_timestamp % 360));

        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT | GLES30.GL_DEPTH_BUFFER_BIT);

        mPhongShadingProgram.set_lights1();
        Matrix.perspectiveM(mProjectionMatrix, 0, fovy, ratio, 0.1f, 2000.0f);

        // Simple Program을 이용해서 축을 그린다.
        mSimpleProgram.use();
        pId = mSimpleProgram.getProgramID();

        // Axis 그리기 영역.
        Matrix.setIdentityM(mModelMatrix, 0);
        Matrix.scaleM(mModelMatrix, 0, 50f, 50f, 50f);
        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);

        mAxis.draw(pId, mMVPMatrix);

        // 생성한 glProgram을 이용해서 오브젝트들을 화면에 그린다.
        mPhongShadingProgram.use();

        GLES30.glUniform1i(mPhongShadingProgram.locFlagFog, mPhongShadingProgram.mFlagFog);
        GLES30.glUniform1i(mPhongShadingProgram.locFlagTextureMapping, mPhongShadingProgram.mFlagTextureMapping);

        // Model Matrix 설정.
        Matrix.setIdentityM(mModelMatrix, 0);
        Matrix.translateM(mModelMatrix, 0, -250f, 0f, 250f);
        Matrix.scaleM(mModelMatrix, 0, 500f, 500f, 500f);
        Matrix.rotateM(mModelMatrix, 0, -90.0f, 1f, 0f, 0f);
        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        Matrix.invertM(mModelViewInvTrans, 0, mModelViewMatrix, 0);
        Matrix.transposeM(mModelViewInvTrans, 0, mModelViewInvTrans, 0);

        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewProjectionMatrix, 1, false, mMVPMatrix, 0);
        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrix, 1, false, mModelViewMatrix, 0);
        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrixInvTrans, 1, false, mModelViewInvTrans, 0);

        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, mFloor.mTexId[0]);
        //GLES30.glUniform1i(mPhongShadingProgram.locTexture, 1);
        GLES30.glUniform1i(mPhongShadingProgram.locTexture, TEXTURE_ID_FLOOR);

        mPhongShadingProgram.setUpMaterialFloor();
        mFloor.draw();

        // Tiger 그리기 영역.
        Matrix.setIdentityM(mModelMatrix, 0);
        Matrix.rotateM(mModelMatrix, 0, -rotation_angle_tiger, 0f, 1f, 0f);
        Matrix.translateM(mModelMatrix, 0, 200.0f, 0.0f, 0.0f);
        Matrix.rotateM(mModelMatrix, 0, -90.0f, 1.0f, 0.0f, 0.0f);
        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        Matrix.transposeM(mModelViewInvTrans, 0, mModelViewMatrix, 0);
        Matrix.invertM(mModelViewInvTrans, 0, mModelViewInvTrans, 0);

        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewProjectionMatrix, 1, false, mMVPMatrix, 0);
        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrix, 1, false, mModelViewMatrix, 0);
        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrixInvTrans, 1, false, mModelViewInvTrans, 0);

        //GLES30.glUniform1i(mPhongShadingProgram.locFlagTextureMapping, 0);
        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, mTiger.mTexId[0]);
        GLES30.glUniform1i(mPhongShadingProgram.locTexture, TEXTURE_ID_TIGER);

        mPhongShadingProgram.setUpMaterialTiger();
        mTiger.draw(cur_frame_tiger);


        // Cow
        Matrix.setIdentityM(mModelMatrix, 0);
        Matrix.rotateM(mModelMatrix, 0, rotation_angle_tiger, 0f, 1f, 0f);
        Matrix.translateM(mModelMatrix, 0, 50.0f, 50.0f, 0.0f);
        Matrix.rotateM(mModelMatrix, 0, 90.0f, 0.0f, 1.0f, 0.0f);
        Matrix.scaleM(mModelMatrix, 0, 100.0f, 100.0f, 100.0f);
        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        Matrix.transposeM(mModelViewInvTrans, 0, mModelViewMatrix, 0);
        Matrix.invertM(mModelViewInvTrans, 0, mModelViewInvTrans, 0);

        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewProjectionMatrix, 1, false, mMVPMatrix, 0);
        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrix, 1, false, mModelViewMatrix, 0);
        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrixInvTrans, 1, false, mModelViewInvTrans, 0);

        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, mCow.mTexId[0]);
        GLES30.glUniform1i(mPhongShadingProgram.locTexture, TEXTURE_ID_COW);

        mPhongShadingProgram.setUpMaterialCow();
        mCow.draw(0);

        // Cow_Tree
        for (int i = -1; i <= 1; i++) {
            float[] mModelMatrixSamllCow = new float[16];
            System.arraycopy(mModelMatrix, 0, mModelMatrixSamllCow, 0, mModelMatrix.length);

            Matrix.scaleM(mModelMatrixSamllCow, 0 , 0.01f, 0.01f, 0.01f);
            Matrix.rotateM(mModelMatrixSamllCow, 0, (120.0f * i + 90.0f) % 360, 0.0f, 1.0f, 0.0f);
            Matrix.translateM(mModelMatrixSamllCow, 0, 0.0f, 0.0f, 70.0f);
            Matrix.rotateM(mModelMatrixSamllCow, 0, (-90.0f -120.0f * i) % 360, 0.0f, 1.0f, 0.0f);
            Matrix.scaleM(mModelMatrixSamllCow, 0 ,50.0f, 50.0f, 50.0f);

            Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrixSamllCow, 0);
            Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
            Matrix.transposeM(mModelViewInvTrans, 0, mModelViewMatrix, 0);
            Matrix.invertM(mModelViewInvTrans, 0, mModelViewInvTrans, 0);

            GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewProjectionMatrix, 1, false, mMVPMatrix, 0);
            GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrix, 1, false, mModelViewMatrix, 0);
            GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrixInvTrans, 1, false, mModelViewInvTrans, 0);

            GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, mCow.mTexId[0]);
            GLES30.glUniform1i(mPhongShadingProgram.locTexture, TEXTURE_ID_COW);

            mPhongShadingProgram.setUpMaterialCow();
            mCow.draw(0);
        }

        // Optimus
        Matrix.setIdentityM(mModelMatrix, 0);
        Matrix.translateM(mModelMatrix, 0, 0.0f, 10.0f + optimus_height * 100, 0.0f);
        Matrix.rotateM(mModelMatrix, 0, -rotation_angle_tiger, 0f, 1f, 0f);
        Matrix.rotateM(mModelMatrix, 0, -90.0f, 1.0f, 0.0f, 0.0f);
        Matrix.scaleM(mModelMatrix, 0, 0.2f, 0.2f, 0.2f);
        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        Matrix.transposeM(mModelViewInvTrans, 0, mModelViewMatrix, 0);
        Matrix.invertM(mModelViewInvTrans, 0, mModelViewInvTrans, 0);

        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewProjectionMatrix, 1, false, mMVPMatrix, 0);
        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrix, 1, false, mModelViewMatrix, 0);
        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrixInvTrans, 1, false, mModelViewInvTrans, 0);

        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, mOptimus.mTexId[0]);
        GLES30.glUniform1i(mPhongShadingProgram.locTexture, TEXTURE_ID_OPTIMUS);

        mPhongShadingProgram.setUpMaterialOptimus();
        mOptimus.draw(0);

        // Dragon
        Matrix.setIdentityM(mModelMatrix, 0);

        float dragon_x = 0;
        float dragon_z = 0;
        float dragon_speed = 3.0f;

        if(mDragon_PathNow == 0) {
            dragon_x = mDragon_Path[0][0] + dragon_speed * (timestamp - mDragon_PathStamp);
            dragon_z = mDragon_Path[0][1];

            if (dragon_x > 250.0f) {
                mDragon_PathNow = 1;
                mDragon_PathStamp = timestamp;
                dragon_x = mDragon_Path[1][0];
            }
        }

        else if(mDragon_PathNow == 1) {
            dragon_x = mDragon_Path[1][0];
            dragon_z = mDragon_Path[1][1] -  dragon_speed * (timestamp - mDragon_PathStamp);

            if (dragon_z < -250.0f) {
                mDragon_PathNow = 2;
                mDragon_PathStamp = timestamp;
                dragon_z = mDragon_Path[2][1];
            }
        }

        else if(mDragon_PathNow == 2) {
            dragon_x = mDragon_Path[2][0] - dragon_speed * (timestamp - mDragon_PathStamp);
            dragon_z = mDragon_Path[2][1];

            if (dragon_x < -250.0f) {
                mDragon_PathNow = 3;
                mDragon_PathStamp = timestamp;
                dragon_x = mDragon_Path[3][0];
            }
        }

        else if(mDragon_PathNow == 3) {
            dragon_x = mDragon_Path[3][0];
            dragon_z = mDragon_Path[3][1] + dragon_speed * (timestamp - mDragon_PathStamp);

            if (dragon_z > 250.0f) {
                mDragon_PathNow = 0;
                mDragon_PathStamp = timestamp;
                dragon_z = mDragon_Path[0][0];
            }
        }

        Matrix.translateM(mModelMatrix, 0, dragon_x, 100.0f, dragon_z);
        Matrix.rotateM(mModelMatrix, 0, 90.0f * mDragon_PathNow, 0.0f, 1.0f, 0.0f);

        Matrix.rotateM(mModelMatrix, 0, -90.0f, 1.0f, 0.0f, 0.0f);
        Matrix.scaleM(mModelMatrix, 0, 7.0f, 7.0f, 7.0f);
        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        Matrix.transposeM(mModelViewInvTrans, 0, mModelViewMatrix, 0);
        Matrix.invertM(mModelViewInvTrans, 0, mModelViewInvTrans, 0);

        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewProjectionMatrix, 1, false, mMVPMatrix, 0);
        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrix, 1, false, mModelViewMatrix, 0);
        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrixInvTrans, 1, false, mModelViewInvTrans, 0);

        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, mDragon.mTexId[0]);
        GLES30.glUniform1i(mPhongShadingProgram.locTexture, TEXTURE_ID_DRAGON);

        mPhongShadingProgram.setUpMaterialDragon();
        mDragon.draw(0);

        Matrix.setIdentityM(mModelMatrix, 0);
        //Matrix.rotateM(mModelMatrix, 0, rotation_angle_tiger, 0f, 1f, 0f);
        Matrix.translateM(mModelMatrix, 0, 100.0f, 10.0f, 100.0f);
        //Matrix.rotateM(mModelMatrix, 0, 90.0f, 0.0f, 1.0f, 0.0f);
        Matrix.scaleM(mModelMatrix, 0, 15.0f, 15.0f, 15.0f);
        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        Matrix.transposeM(mModelViewInvTrans, 0, mModelViewMatrix, 0);
        Matrix.invertM(mModelViewInvTrans, 0, mModelViewInvTrans, 0);

        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewProjectionMatrix, 1, false, mMVPMatrix, 0);
        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrix, 1, false, mModelViewMatrix, 0);
        GLES30.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrixInvTrans, 1, false, mModelViewInvTrans, 0);

        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, mCar.mTexId[0]);
        GLES30.glUniform1i(mPhongShadingProgram.locTexture, TEXTURE_ID_CAR);

        mPhongShadingProgram.setUpMaterialCar();
        mCar.draw(0);


        mSimpleProgram.use();
        pId = mSimpleProgram.getProgramID();

        // Axis 그리기 영역.
        Matrix.scaleM(mMVPMatrix, 0, 20f, 20f, 20f);
        mAxis.draw(pId, mMVPMatrix);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        GLES30.glViewport(0, 0, width, height);

        ratio = (float) width / height;

        Matrix.perspectiveM(mProjectionMatrix, 0, fovy, ratio, 0.1f, 2000.0f);
    }

    static int prevTimeStamp = 0;
    static int currTimeStamp = 0;
    static int totalTimeStamp = 0;

    private int getTimeStamp() {
        Long tsLong = System.currentTimeMillis() / 100;

        currTimeStamp = tsLong.intValue();
        if(prevTimeStamp != 0) {
            totalTimeStamp += (currTimeStamp - prevTimeStamp);
        }
        prevTimeStamp = currTimeStamp;

        return totalTimeStamp;
    }

    // CheckBox 조작 관련 함수.
    public void toggleTexture(int type, boolean onTexture) {
        int toggleVal;
        if(onTexture) toggleVal = 1;
        else          toggleVal = 0;

        if(type == 0)           // Texture
            mPhongShadingProgram.mFlagTextureMapping = toggleVal;
        else                    // Fog
            mPhongShadingProgram.mFlagFog = toggleVal;
    }

    public void setLight1() {
        mPhongShadingProgram.light[1].light_on = 1 - mPhongShadingProgram.light[1].light_on;
    }

    public void cam_move(float x, float y, float z) {
        float[] mTmpMatrix = new float[16];
        Matrix.setIdentityM(mTmpMatrix, 0);

        Matrix.translateM(mTmpMatrix, 0, x, y, z);
        Matrix.multiplyMM(mViewMatrix, 0, mTmpMatrix, 0 ,mViewMatrix, 0);
        Matrix.multiplyMM(mViewMatrix, 0, mTmpMatrix, 0 ,mViewMatrix, 0);
    }

    public void cam_move(float x, float y) { cam_move(x, y, 0.0f); }

    public void cam_zoom(float new_fovy) {
        fovy = new_fovy;
    }

    public void cam_rotation(float angleX, float angleY) {
        float[] mTmpMatrix = new float[16];
        Matrix.setIdentityM(mTmpMatrix, 0);

        Matrix.rotateM(mTmpMatrix, 0, angleX, 0.0f, 1.0f, 0.0f);
        Matrix.rotateM(mTmpMatrix, 0, angleY, 1.0f, 0.0f, 0.0f);
        Matrix.multiplyMM(mViewMatrix, 0, mTmpMatrix, 0 ,mViewMatrix, 0);
    }
}
