package com.graphics.sg.opengles_tiger;

import android.opengl.GLES30;
import android.opengl.Matrix;

class LightParameters {
        int light_on;
        float position[] = new float[4];
        float ambient_color[] = new float[4];
        float diffuse_color[] = new float[4];
        float specular_color[] = new float[4];
        float spot_direction[] = new float[3];
        float spot_exponent;
        float spot_cutoff_angle;
};

class MaterialParameters {
    float ambient_color[] = new float[4];
    float diffuse_color[] = new float[4];
    float specular_color[] = new float[4];
    float emissive_color[] = new float[4];
    float specular_exponent;
}

class LocLightParameter {
    int light_on;
    int position;
    int ambient_color, diffuse_color, specular_color;
    int spot_direction;
    int spot_exponent;
    int spot_cutoff_angle;
    int light_attenuation_factors;
}

class LocMaterialParameter {
    int ambient_color, diffuse_color, specular_color, emissive_color;
    int specular_exponent;
}

public class PhongShadingProgram extends GLES30Program{

    final static int NUMBER_OF_LIGHT_SUPPORTED = 4;

    int locModelViewProjectionMatrix;
    int locModelViewMatrix;
    int locModelViewMatrixInvTrans;

    int locGlobalAmbientColor;
    LocLightParameter locLight[];
    LocMaterialParameter locMaterial = new LocMaterialParameter();
    int locTexture;
    int locFlagTextureMapping;
    int locFlagFog;

    int mFlagFog;
    int mFlagTextureMapping;

    LightParameters light[];
    MaterialParameters materialFloor = new MaterialParameters();
    MaterialParameters materialTiger = new MaterialParameters();

    MaterialParameters materialOptimus = new MaterialParameters();
    MaterialParameters materialDragon = new MaterialParameters();
    MaterialParameters materialCow = new MaterialParameters();
    MaterialParameters materialCar = new MaterialParameters();
    MaterialParameters materialSquare = new MaterialParameters();

    public PhongShadingProgram(String vertexShaderCode, String fragmentShaderCode) {
        super(vertexShaderCode, fragmentShaderCode);
    }

    /**
     * GLProgram에 결합 된 Shader 내 변수들의 location 인덱스를 설정하는 함수.
     */
    public void prepare() {
        locLight = new LocLightParameter[NUMBER_OF_LIGHT_SUPPORTED];
        for(int i=0 ; i<NUMBER_OF_LIGHT_SUPPORTED ; i++)
            locLight[i] = new LocLightParameter();

        locModelViewProjectionMatrix = GLES30.glGetUniformLocation(mId, "ModelViewProjectionMatrix");
        locModelViewMatrix = GLES30.glGetUniformLocation(mId, "ModelViewMatrix");
        locModelViewMatrixInvTrans = GLES30.glGetUniformLocation(mId, "ModelViewMatrixInvTrans");

        locTexture = GLES30.glGetUniformLocation(mId, "base_texture");

        locFlagTextureMapping = GLES30.glGetUniformLocation(mId, "flag_texture_mapping");
        locFlagFog = GLES30.glGetUniformLocation(mId, "flag_fog");

        locGlobalAmbientColor = GLES30.glGetUniformLocation(mId, "global_ambient_color");
        for (int i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
            String lightNumStr = "light[" + i + "]";
            locLight[i].light_on = GLES30.glGetUniformLocation(mId, lightNumStr + ".light_on");
            locLight[i].position = GLES30.glGetUniformLocation(mId, lightNumStr + ".position");
            locLight[i].ambient_color = GLES30.glGetUniformLocation(mId, lightNumStr + ".ambient_color");
            locLight[i].diffuse_color = GLES30.glGetUniformLocation(mId, lightNumStr + ".diffuse_color");
            locLight[i].specular_color = GLES30.glGetUniformLocation(mId, lightNumStr + ".specular_color");
            locLight[i].spot_direction = GLES30.glGetUniformLocation(mId, lightNumStr + ".spot_direction");
            locLight[i].spot_exponent = GLES30.glGetUniformLocation(mId, lightNumStr + ".spot_exponent");
            locLight[i].spot_cutoff_angle = GLES30.glGetUniformLocation(mId, lightNumStr + ".spot_cutoff_angle");
            locLight[i].light_attenuation_factors = GLES30.glGetUniformLocation(mId, lightNumStr + ".light_attenuation_factors");
        }

        locMaterial.ambient_color = GLES30.glGetUniformLocation(mId, "material.ambient_color");
        locMaterial.diffuse_color = GLES30.glGetUniformLocation(mId, "material.diffuse_color");
        locMaterial.specular_color = GLES30.glGetUniformLocation(mId, "material.specular_color");
        locMaterial.emissive_color = GLES30.glGetUniformLocation(mId, "material.emissive_color");
        locMaterial.specular_exponent = GLES30.glGetUniformLocation(mId, "material.specular_exponent");
    }

    /**
     * Light와 Material의 값을 설정하는 함수.
     */
    public void initLightsAndMaterial() {
        GLES30.glUseProgram(mId);

        GLES30.glUniform4f(locGlobalAmbientColor, 0.115f, 0.115f, 0.115f, 1.0f);
        for (int i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
            GLES30.glUniform1i(locLight[i].light_on, 0); // turn off all lights initially
            GLES30.glUniform4f(locLight[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
            GLES30.glUniform4f(locLight[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
            if (i == 0) {
                GLES30.glUniform4f(locLight[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
                GLES30.glUniform4f(locLight[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
            }
            else {
                GLES30.glUniform4f(locLight[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
                GLES30.glUniform4f(locLight[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
            }
            GLES30.glUniform3f(locLight[i].spot_direction, 0.0f, 0.0f, -1.0f);
            GLES30.glUniform1f(locLight[i].spot_exponent, 0.0f); // [0.0, 128.0]
            GLES30.glUniform1f(locLight[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
            GLES30.glUniform4f(locLight[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
        }

        GLES30.glUniform4f(locMaterial.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
        GLES30.glUniform4f(locMaterial.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
        GLES30.glUniform4f(locMaterial.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
        GLES30.glUniform4f(locMaterial.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
        GLES30.glUniform1f(locMaterial.specular_exponent, 0.0f); // [0.0, 128.0]

        GLES30.glUseProgram(0);


        // Material 설정.
        materialFloor.ambient_color[0] = 0.0f;
        materialFloor.ambient_color[1] = 0.05f;
        materialFloor.ambient_color[2] = 0.0f;
        materialFloor.ambient_color[3] = 1.0f;

        materialFloor.diffuse_color[0] = 0.2f;
        materialFloor.diffuse_color[1] = 0.5f;
        materialFloor.diffuse_color[2] = 0.2f;
        materialFloor.diffuse_color[3] = 1.0f;

        materialFloor.specular_color[0] = 0.24f;
        materialFloor.specular_color[1] = 0.5f;
        materialFloor.specular_color[2] = 0.24f;
        materialFloor.specular_color[3] = 1.0f;

        materialFloor.specular_exponent = 2.5f;

        materialFloor.emissive_color[0] = 0.0f;
        materialFloor.emissive_color[1] = 0.0f;
        materialFloor.emissive_color[2] = 0.0f;
        materialFloor.emissive_color[3] = 1.0f;


        materialTiger.ambient_color[0] = 0.24725f;
        materialTiger.ambient_color[1] = 0.1995f;
        materialTiger.ambient_color[2] = 0.0745f;
        materialTiger.ambient_color[3] = 1.0f;

        materialTiger.diffuse_color[0] = 0.75164f;
        materialTiger.diffuse_color[1] = 0.60648f;
        materialTiger.diffuse_color[2] = 0.22648f;
        materialTiger.diffuse_color[3] = 1.0f;

        materialTiger.specular_color[0] = 0.728281f;
        materialTiger.specular_color[1] = 0.655802f;
        materialTiger.specular_color[2] = 0.466065f;
        materialTiger.specular_color[3] = 1.0f;

        materialTiger.specular_exponent = 51.2f;

        materialTiger.emissive_color[0] = 0.1f;
        materialTiger.emissive_color[1] = 0.1f;
        materialTiger.emissive_color[2] = 0.0f;
        materialTiger.emissive_color[3] = 1.0f;


        materialOptimus.ambient_color[0] = 0.14126315452214389f;
        materialOptimus.ambient_color[1] = 0.7156540575502911f;
        materialOptimus.ambient_color[2] = 0.5966332809701865f;
        materialOptimus.ambient_color[3] = 1.0f;
        materialOptimus.diffuse_color[0] = 0.2549612779049665f;
        materialOptimus.diffuse_color[1] = 0.13972499441130348f;
        materialOptimus.diffuse_color[2] = 0.8841817085890357f;
        materialOptimus.diffuse_color[3] = 1.0f;
        materialOptimus.specular_color[0] = 0.3957942081411028f;
        materialOptimus.specular_color[1] = 0.5020279726524615f;
        materialOptimus.specular_color[2] = 0.5243016053476013f;
        materialOptimus.specular_color[3] = 1.0f;
        materialOptimus.specular_exponent = 96.3f;
        materialOptimus.emissive_color[0] = 0.1f;
        materialOptimus.emissive_color[1] = 0.1f;
        materialOptimus.emissive_color[2] = 0.1f;
        materialOptimus.emissive_color[3] = 1.0f;

        materialDragon.ambient_color[0] = 0.6518667432914536f;
        materialDragon.ambient_color[1] = 0.4015444729948403f;
        materialDragon.ambient_color[2] = 0.6460670418716485f;
        materialDragon.ambient_color[3] = 1.0f;
        materialDragon.diffuse_color[0] = 0.8445436346508925f;
        materialDragon.diffuse_color[1] = 0.4885641504435906f;
        materialDragon.diffuse_color[2] = 0.7649066068390078f;
        materialDragon.diffuse_color[3] = 1.0f;
        materialDragon.specular_color[0] = 0.7758803202254273f;
        materialDragon.specular_color[1] = 0.06550478969993179f;
        materialDragon.specular_color[2] = 0.9620346537422159f;
        materialDragon.specular_color[3] = 1.0f;
        materialDragon.specular_exponent = 3.1f;
        materialDragon.emissive_color[0] = 0.1f;
        materialDragon.emissive_color[1] = 0.1f;
        materialDragon.emissive_color[2] = 0.1f;
        materialDragon.emissive_color[3] = 1.0f;

        materialCow.ambient_color[0] = 0.7210726525991409f;
        materialCow.ambient_color[1] = 0.356063829161412f;
        materialCow.ambient_color[2] = 0.5854278450572166f;
        materialCow.ambient_color[3] = 1.0f;
        materialCow.diffuse_color[0] = 0.1936684088915186f;
        materialCow.diffuse_color[1] = 0.2217623791958626f;
        materialCow.diffuse_color[2] = 0.06649109054662494f;
        materialCow.diffuse_color[3] = 1.0f;
        materialCow.specular_color[0] = 0.7417657738733074f;
        materialCow.specular_color[1] = 0.46940868448348894f;
        materialCow.specular_color[2] = 0.6410139266514381f;
        materialCow.specular_color[3] = 1.0f;
        materialCow.specular_exponent = 26.4f;
        materialCow.emissive_color[0] = 0.1f;
        materialCow.emissive_color[1] = 0.1f;
        materialCow.emissive_color[2] = 0.1f;
        materialCow.emissive_color[3] = 1.0f;

        materialCar.ambient_color[0] = 0.38177412066045f;
        materialCar.ambient_color[1] = 0.06547722609113193f;
        materialCar.ambient_color[2] = 0.4940854706613922f;
        materialCar.ambient_color[3] = 1.0f;
        materialCar.diffuse_color[0] = 0.6604701197981685f;
        materialCar.diffuse_color[1] = 0.8002496572209964f;
        materialCar.diffuse_color[2] = 0.8401393045339253f;
        materialCar.diffuse_color[3] = 1.0f;
        materialCar.specular_color[0] = 0.39764815484445115f;
        materialCar.specular_color[1] = 0.8294460724356704f;
        materialCar.specular_color[2] = 0.8911893617205143f;
        materialCar.specular_color[3] = 1.0f;
        materialCar.specular_exponent = 63.6f;
        materialCar.emissive_color[0] = 0.1f;
        materialCar.emissive_color[1] = 0.1f;
        materialCar.emissive_color[2] = 0.1f;
        materialCar.emissive_color[3] = 1.0f;

        materialSquare.ambient_color[0] = 0.2312417264713289f;
        materialSquare.ambient_color[1] = 0.9095013503983488f;
        materialSquare.ambient_color[2] = 0.7958159672895821f;
        materialSquare.ambient_color[3] = 1.0f;
        materialSquare.diffuse_color[0] = 0.8308121551913812f;
        materialSquare.diffuse_color[1] = 0.13313320639794524f;
        materialSquare.diffuse_color[2] = 0.12539714930010548f;
        materialSquare.diffuse_color[3] = 1.0f;
        materialSquare.specular_color[0] = 0.3532509827049185f;
        materialSquare.specular_color[1] = 0.20854495273645368f;
        materialSquare.specular_color[2] = 0.44033743000881576f;
        materialSquare.specular_color[3] = 1.0f;
        materialSquare.specular_exponent = 79.0f;
        materialSquare.emissive_color[0] = 0.1f;
        materialSquare.emissive_color[1] = 0.1f;
        materialSquare.emissive_color[2] = 0.1f;
        materialSquare.emissive_color[3] = 1.0f;
    }

    public void initFlags() {

        mFlagFog = 0;
        mFlagTextureMapping = 1;

        GLES30.glUseProgram(mId);
        GLES30.glUniform1i(locFlagFog, mFlagFog);
        GLES30.glUniform1i(locFlagTextureMapping, mFlagTextureMapping);
        GLES30.glUseProgram(0);
    }

    /**
     * 퐁 쉐이딩을 위한 각종 light 관련 값을 그래픽 메모리에 전달하는 함수.
     * @param viewMatrix 현재 상태의 view matrix.
     */
    public void set_up_scene_lights(float[] viewMatrix) {
        light = new LightParameters[NUMBER_OF_LIGHT_SUPPORTED];
        for(int i=0 ; i<NUMBER_OF_LIGHT_SUPPORTED ; i++)
            light[i] = new LightParameters();

        // point_light_EC: use light 0
        light[0].light_on = 1;
        light[0].position[0] = 0.0f; light[0].position[1] = 100.0f; 	// point light position in EC
        light[0].position[2] = 0.0f; light[0].position[3] = 1.0f;

        light[0].ambient_color[0] = 0.13f; light[0].ambient_color[1] = 0.13f;
        light[0].ambient_color[2] = 0.13f; light[0].ambient_color[3] = 1.0f;

        light[0].diffuse_color[0] = 0.5f; light[0].diffuse_color[1] = 0.5f;
        light[0].diffuse_color[2] = 0.5f; light[0].diffuse_color[3] = 1.5f;

        light[0].specular_color[0] = 0.8f; light[0].specular_color[1] = 0.8f;
        light[0].specular_color[2] = 0.8f; light[0].specular_color[3] = 1.0f;

        // spot_light_WC: use light 1
        light[1].light_on = 1;
        light[1].position[0] = -200.0f; light[1].position[1] = 500.0f; // spot light position in WC
        light[1].position[2] = -200.0f; light[1].position[3] = 1.0f;

        light[1].ambient_color[0] = 0.152f; light[1].ambient_color[1] = 0.152f;
        light[1].ambient_color[2] = 0.152f; light[1].ambient_color[3] = 1.0f;

        light[1].diffuse_color[0] = 0.572f; light[1].diffuse_color[1] = 0.572f;
        light[1].diffuse_color[2] = 0.572f; light[1].diffuse_color[3] = 1.0f;

        light[1].specular_color[0] = 0.772f; light[1].specular_color[1] = 0.772f;
        light[1].specular_color[2] = 0.772f; light[1].specular_color[3] = 1.0f;

        light[1].spot_direction[0] = 0.0f; light[1].spot_direction[1] = -1.0f; // spot light direction in WC
        light[1].spot_direction[2] = 0.0f;
        light[1].spot_cutoff_angle = 20.0f;
        light[1].spot_exponent = 8.0f;


        GLES30.glUseProgram(mId);
        GLES30.glUniform1i(locLight[0].light_on, light[0].light_on);
        GLES30.glUniform4fv(locLight[0].position, 1, BufferConverter.floatArrayToBuffer(light[0].position));
        GLES30.glUniform4fv(locLight[0].ambient_color, 1, BufferConverter.floatArrayToBuffer(light[0].ambient_color));
        GLES30.glUniform4fv(locLight[0].diffuse_color, 1, BufferConverter.floatArrayToBuffer(light[0].diffuse_color));
        GLES30.glUniform4fv(locLight[0].specular_color, 1, BufferConverter.floatArrayToBuffer(light[0].specular_color));

        GLES30.glUniform1i(locLight[1].light_on, light[1].light_on);
        // need to supply position in EC for shading
        float[] positionEC = new float[4];
        Matrix.multiplyMV(positionEC, 0, viewMatrix, 0, light[1].position, 0);

        GLES30.glUniform4fv(locLight[1].position, 1, BufferConverter.floatArrayToBuffer(positionEC));
        GLES30.glUniform4fv(locLight[1].ambient_color, 1, BufferConverter.floatArrayToBuffer(light[1].ambient_color));
        GLES30.glUniform4fv(locLight[1].diffuse_color, 1, BufferConverter.floatArrayToBuffer(light[1].diffuse_color));
        GLES30.glUniform4fv(locLight[1].specular_color, 1, BufferConverter.floatArrayToBuffer(light[1].specular_color));


        float[] spot_direction = {
                light[1].spot_direction[0], light[1].spot_direction[1], light[1].spot_direction[2], 0.0f
        };

        float[] directionEC = new float[4];
        Matrix.multiplyMV(directionEC, 0, viewMatrix, 0, spot_direction, 0);

        GLES30.glUniform3fv(locLight[1].spot_direction, 1, BufferConverter.floatArrayToBuffer(directionEC));
        GLES30.glUniform1f(locLight[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
        GLES30.glUniform1f(locLight[1].spot_exponent, light[1].spot_exponent);
        GLES30.glUseProgram(0);
    }

    public void setUpMaterialFloor() {
        GLES30.glUniform4fv(locMaterial.ambient_color, 1, BufferConverter.floatArrayToBuffer(materialFloor.ambient_color));
        GLES30.glUniform4fv(locMaterial.diffuse_color, 1, BufferConverter.floatArrayToBuffer(materialFloor.diffuse_color));
        GLES30.glUniform4fv(locMaterial.specular_color, 1, BufferConverter.floatArrayToBuffer(materialFloor.specular_color));
        GLES30.glUniform1f(locMaterial.specular_exponent, materialFloor.specular_exponent);
        GLES30.glUniform4fv(locMaterial.emissive_color, 1, BufferConverter.floatArrayToBuffer(materialFloor.emissive_color));
    }

    public void setUpMaterialTiger() {
        GLES30.glUniform4fv(locMaterial.ambient_color, 1, BufferConverter.floatArrayToBuffer(materialTiger.ambient_color));
        GLES30.glUniform4fv(locMaterial.diffuse_color, 1, BufferConverter.floatArrayToBuffer(materialTiger.diffuse_color));
        GLES30.glUniform4fv(locMaterial.specular_color, 1, BufferConverter.floatArrayToBuffer(materialTiger.specular_color));
        GLES30.glUniform1f(locMaterial.specular_exponent, materialTiger.specular_exponent);
        GLES30.glUniform4fv(locMaterial.emissive_color, 1, BufferConverter.floatArrayToBuffer(materialTiger.emissive_color));
    }


    public void setUpMaterialOptimus() {
        GLES30.glUniform4fv(locMaterial.ambient_color, 1, BufferConverter.floatArrayToBuffer(materialOptimus.ambient_color));
        GLES30.glUniform4fv(locMaterial.diffuse_color, 1, BufferConverter.floatArrayToBuffer(materialOptimus.diffuse_color));
        GLES30.glUniform4fv(locMaterial.specular_color, 1, BufferConverter.floatArrayToBuffer(materialOptimus.specular_color));
        GLES30.glUniform1f(locMaterial.specular_exponent, materialOptimus.specular_exponent);
        GLES30.glUniform4fv(locMaterial.emissive_color, 1, BufferConverter.floatArrayToBuffer(materialOptimus.emissive_color));
    }

    public void setUpMaterialDragon() {
        GLES30.glUniform4fv(locMaterial.ambient_color, 1, BufferConverter.floatArrayToBuffer(materialDragon.ambient_color));
        GLES30.glUniform4fv(locMaterial.diffuse_color, 1, BufferConverter.floatArrayToBuffer(materialDragon.diffuse_color));
        GLES30.glUniform4fv(locMaterial.specular_color, 1, BufferConverter.floatArrayToBuffer(materialDragon.specular_color));
        GLES30.glUniform1f(locMaterial.specular_exponent, materialDragon.specular_exponent);
        GLES30.glUniform4fv(locMaterial.emissive_color, 1, BufferConverter.floatArrayToBuffer(materialDragon.emissive_color));
    }

    public void setUpMaterialCow() {
        GLES30.glUniform4fv(locMaterial.ambient_color, 1, BufferConverter.floatArrayToBuffer(materialCow.ambient_color));
        GLES30.glUniform4fv(locMaterial.diffuse_color, 1, BufferConverter.floatArrayToBuffer(materialCow.diffuse_color));
        GLES30.glUniform4fv(locMaterial.specular_color, 1, BufferConverter.floatArrayToBuffer(materialCow.specular_color));
        GLES30.glUniform1f(locMaterial.specular_exponent, materialCow.specular_exponent);
        GLES30.glUniform4fv(locMaterial.emissive_color, 1, BufferConverter.floatArrayToBuffer(materialCow.emissive_color));
    }

    public void setUpMaterialCar() {
        GLES30.glUniform4fv(locMaterial.ambient_color, 1, BufferConverter.floatArrayToBuffer(materialCar.ambient_color));
        GLES30.glUniform4fv(locMaterial.diffuse_color, 1, BufferConverter.floatArrayToBuffer(materialCar.diffuse_color));
        GLES30.glUniform4fv(locMaterial.specular_color, 1, BufferConverter.floatArrayToBuffer(materialCar.specular_color));
        GLES30.glUniform1f(locMaterial.specular_exponent, materialCar.specular_exponent);
        GLES30.glUniform4fv(locMaterial.emissive_color, 1, BufferConverter.floatArrayToBuffer(materialCar.emissive_color));
    }

    public void setUpMaterialSquare() {
        GLES30.glUniform4fv(locMaterial.ambient_color, 1, BufferConverter.floatArrayToBuffer(materialSquare.ambient_color));
        GLES30.glUniform4fv(locMaterial.diffuse_color, 1, BufferConverter.floatArrayToBuffer(materialSquare.diffuse_color));
        GLES30.glUniform4fv(locMaterial.specular_color, 1, BufferConverter.floatArrayToBuffer(materialSquare.specular_color));
        GLES30.glUniform1f(locMaterial.specular_exponent, materialSquare.specular_exponent);
        GLES30.glUniform4fv(locMaterial.emissive_color, 1, BufferConverter.floatArrayToBuffer(materialSquare.emissive_color));
    }


    public void set_lights1() {
        GLES30.glUseProgram(mId);
        GLES30.glUniform1i(locLight[1].light_on, light[1].light_on);
        GLES30.glUseProgram(0);
    }
}
