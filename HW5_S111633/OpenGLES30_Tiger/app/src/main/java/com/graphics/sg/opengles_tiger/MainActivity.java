package com.graphics.sg.opengles_tiger;

import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.LinearLayout;

public class MainActivity extends AppCompatActivity{

    GLES30Renderer mRenderer;

    CheckBox mCheckBoxTexture;
    CheckBox mCheckBoxFog;

    static final int TYPE_TEXTURE = 0;
    static final int TYPE_FOG     = 1;

    private boolean touchAvailable = false;
    private boolean toogleLight = false;
    private float[] previousX = {0, 0, 0};
    private float[] currentX = {0, 0, 0};
    private float[] previousY = {0, 0, 0};
    private float[] currentY = {0, 0, 0};
    private float fovy = 45.0f;

    int view_width = 0;
    int view_height = 0;
    float view_centerX, view_centerY;

    /*
    if (view_width == 0 && view_width == 0) {
        View linearLayout = (View) this.findViewById(R.id.surface_view);
        view_width = linearLayout.getWidth();
        view_height = linearLayout.getHeight();

        view_centerX = view_width / 2.0f;
        view_centerY = view_height / 2.0f;
    }
    */

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        mCheckBoxTexture = (CheckBox) findViewById(R.id.checkbox_texture);
        mCheckBoxTexture.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                mRenderer.toggleTexture(TYPE_TEXTURE, isChecked);
            }
        });
        mCheckBoxFog = (CheckBox) findViewById(R.id.checkbox_fog);
        mCheckBoxFog.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                 mRenderer.toggleTexture(TYPE_FOG, isChecked);
            }
        });

        // layout에 있는 GLSurfaceView 불러오기.
        GLSurfaceView glSurfaceView = (GLSurfaceView) findViewById(R.id.surface_view);
        // Renderer를 설정해주기 전에 클라이언트 버전을 먼저 설정해준다.
        glSurfaceView.setEGLContextClientVersion(3);
        // 커스텀 렌더러를 설정.
        mRenderer = new GLES30Renderer(this);
        glSurfaceView.setRenderer(mRenderer);
    }


    @Override
    public boolean onTouchEvent(MotionEvent e) {
        int touchCount = e.getPointerCount();
        if (touchCount < 1 || touchCount > 3) return false;

        switch (e.getAction() & MotionEvent.ACTION_MASK) {
            /*
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN:
                touchAvailable = true;
                toogleLight = true;
                for (int i = 0; i < e.getPointerCount(); i++) {
                    currentX[i] = e.getY(i);
                    currentY[i] = e.getY(i);
                    previousX[i] = currentX[i];
                    previousY[i] = currentY[i];
                }
                break;
            case MotionEvent.ACTION_MOVE:
                if (!touchAvailable) return false;
                for (int i = 0; i < e.getPointerCount(); i++) {
                    previousX[i] = currentX[i];
                    previousY[i] = currentY[i];
                    currentX[i] = e.getY(i);
                    currentY[i] = e.getY(i);
                }

                if (touchCount == 3) {
                    toogleLight = false;
                    float dist = currentX[0] - previousX[0];

                    Matrix.translateM(mRenderer.mViewMatrix, 0, dist, 0f, 0f);

                }
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
                touchAvailable = false;
                if (toogleLight) {
                    mRenderer.setLight1();
                    toogleLight = false;
                }
                break;
            */
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN:
                touchAvailable = true;
                toogleLight = true;
                for (int i = 0; i < touchCount; i++) {
                    currentX[i] = e.getY(i);
                    currentY[i] = e.getY(i);
                    previousX[i] = currentX[i];
                    previousY[i] = currentY[i];
                }
                break;

            case MotionEvent.ACTION_MOVE:
                if (!touchAvailable) return false;
                for (int i = 0; i < touchCount; i++) {
                    previousX[i] = currentX[i];
                    previousY[i] = currentY[i];
                    currentX[i] = e.getX(i);
                    currentY[i] = e.getY(i);
                }

                if (touchCount == 1) {
                    float pointX = currentX[0] - previousX[0];
                    float pointY = currentY[0] - previousY[0];

                    float x = 3.0f;
                    if(pointX < 0) x *= -1;

                    float y = 3.0f;
                    if(pointY < 0) y *= -1;

                    mRenderer.cam_move(x, 0, y);
                }

                else if (touchCount == 2) {
                    float prevX = previousX[0] - previousX[1];
                    float prevY = previousY[0] - previousY[1];
                    float currX = currentX[0] - currentX[1];
                    float currY = currentY[0] - currentY[1];

                    float pre = (float) (Math.pow(prevX, 2.0f) + Math.pow(prevY, 2.0f));
                    float cur = (float) (Math.pow(currX, 2.0f) + Math.pow(currY, 2.0f));

                    if (pre - cur > 0)
                        fovy = Math.min(150.0f, fovy + 5.0f);

                    else
                        fovy = Math.max(15.0f, fovy - 5.0f);

                    mRenderer.cam_zoom(fovy);
                }

                else if (touchCount == 3) {
                    float distX = 0;
                    float distY = 0;

                    for (int i = 0; i < touchCount; i++) {
                        distX += currentX[i] - previousX[i];
                        distY += currentY[i] - previousY[i];
                    }

                    float angleX = 0.5f;
                    if (distX < 0) angleX *= -1;

                    float angleY = 0.5f;
                    if (distY < 0) angleY *= -1;

                    mRenderer.cam_rotation(angleX, angleY);
                }

                break;
        }
        return true;
    }
}