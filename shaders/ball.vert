#version 400

uniform mat4 mat_projection;
uniform mat4 mat_view;
uniform mat4 mat_modelview;

uniform vec3 wLightPosition;    // world space

layout(location = 0) in vec3 mPosition; // model space
layout(location = 1) in vec3 mNormal;   // model space
layout(location = 2) in vec2 mUV;
layout(location = 3) in vec3 mTangent;
layout(location = 4) in vec3 mBitangent;

out vec2 vUV;
out vec3 vLiDirect; // tangent space
out vec3 vHalf;

const vec4 eOrig = vec4(0, 0, 0, 1);

void main() {
    // position
    vec4 ePosition = mat_modelview *vec4(mPosition, 1);
    vec3 vPosition = mPosition;
    gl_Position = mat_projection *ePosition;

    // tangent space matrix
    mat3 mat_toTan = transpose(mat3(
        mTangent,
        mBitangent,
        mNormal
    ));

    // uv
    vUV = mUV;

    // light direction
    vec4 eLight = mat_view *vec4(wLightPosition, 1);
    vec4 mLight = inverse(mat_modelview) *eLight;
    vec3 mLiDirect = vec3(mLight) - vPosition;
    vLiDirect = mat_toTan *mLiDirect;
    vLiDirect = normalize(vLiDirect);

    // camera direction
    vec4 mOrig = inverse(mat_modelview) *eOrig;
    vec3 mVeiw = vec3(mOrig) - vPosition;

    // halfway vector
    vec3 mHalfway = vec3(mLiDirect) + mVeiw;
    mHalfway = normalize(mHalfway);
    vHalf = mat_toTan *mHalfway;
    vHalf = vHalf;

}
