//
//  main.cpp
//  OpenGLPrimitiveDemo
//
//  Created by apple on 2020/7/9.
//  Copyright © 2020 yinhe. All rights reserved.
//

#include <stdio.h>
#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLGeometryTransform.h"

#include <glut/glut.h>

GLShaderManager shaderManager; // 着色器管理类

GLMatrixStack modelViewMatrix; // 模型视图矩阵
GLMatrixStack projectionMatrix; // 投影矩阵

GLFrame cameraFrame; // 观察者
GLFrame objectFrame; // 物体

GLFrustum viewFrustum; // 投影

GLBatch lineLoopBatch; // 线环批次类


GLGeometryTransform transformPipeLine; // 几何变换管道


// 定义3个点
GLfloat vCoast[9] = {
    3, 3, 0,
    0, 3, 0,
    3, 0, 0
};

GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f }; // 绿色
GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // 黑色


void changeSize(int w, int h) {
    // 设置视口
    glViewport(0, 0, w, h);
    
    // 创建透视投影
    //参数1：垂直方向上的视场角度
    //参数2：视口纵横比 = w/h
    //参数3：近裁剪面距离
    //参数4：远裁剪面距离
    viewFrustum.SetPerspective(35.0, float(w) / float(h), 1.0, 500.0);
    
    // 通过设置的投影方式获得投影矩阵，并将其存入投影矩阵中
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    // 初始化模型视图矩阵堆栈，压入一个单元矩阵
    modelViewMatrix.LoadIdentity();
}

void setupRC() {
    // 背景颜色
    glClearColor(0.7, 0.7, 0.7, 1.0);
    
    // 初始化固定着色器
    shaderManager.InitializeStockShaders();
    
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    // 设置变换管道中的模型视图矩阵和投影矩阵
    transformPipeLine.SetMatrixStacks(modelViewMatrix, projectionMatrix);
    
    // 设置下观察者的位置
    cameraFrame.MoveForward(-15.0);
    
    
    
    lineLoopBatch.Begin(GL_LINE_LOOP, 3);
    lineLoopBatch.CopyVertexData3f(vCoast);
    lineLoopBatch.End();
    
    
}

void specialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_UP) {
        // 绕x轴旋转
        objectFrame.RotateWorld(m3dDegToRad(-5.0), 1.0, 0.0, 0.0);
    }
    if (key == GLUT_KEY_DOWN) {
        // 绕x轴旋转
        objectFrame.RotateWorld(m3dDegToRad(5.0), 1.0, 0.0, 0.0);
    }
    if (key == GLUT_KEY_LEFT) {
        // 绕y轴旋转
        objectFrame.RotateWorld(m3dDegToRad(-5.0), 0.0, 1.0, 0.0);
    }
    if (key == GLUT_KEY_RIGHT) {
        // 绕y轴旋转
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    }
    glutPostRedisplay();
}


void renderScene() {
    // 清理缓存区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // 压栈
    modelViewMatrix.PushMatrix();
    
    
    // 获取观察者矩阵
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    // 观察者矩阵*矩阵堆栈的顶部矩阵，相乘的结果随后存储在堆栈的顶部
    // 观察者矩阵*栈顶单元矩阵 = 新观察者矩阵，压栈
    modelViewMatrix.MultMatrix(mCamera);
    
    
    // 获取物体的矩阵
    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    // 物体矩阵*矩阵堆栈的顶部矩阵，相乘的结果随后存储在堆栈的顶部
    modelViewMatrix.MultMatrix(mObjectFrame);
    
    
    
    //
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeLine.GetModelViewProjectionMatrix(), vGreen);
    
    
    glPointSize(10.0);
    lineLoopBatch.Draw();
    glLineWidth(10.0);
    
    // 出栈
    modelViewMatrix.PopMatrix();
    
    // 交换缓冲区
    glutSwapBuffers();
    
}

int main(int argc,char* argv[]) {
    
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL图元");
    
    
    glutReshapeFunc(changeSize);
    glutDisplayFunc(renderScene);
    glutSpecialFunc(specialKeys);
    
    
    GLenum status = glewInit();
    if (status != GLEW_OK) {
        printf("error: %s\n", glewGetString(status));
        return 1;
    }
    
    setupRC();
    
    glutMainLoop();
    
    return 0;
}
