//
//  main.cpp
//  OpenGL_BaseCode
//
//  Created by 吴宇飞 on 2018/4/2.
//  Copyright © 2018年 gyd. All rights reserved.
//

#include "GLShaderManager.h"

#include "GLTools.h"

#include <glut/glut.h>
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"
#include "GLFrame.h"
#include "GLFrustum.h"

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLBatch topBatch;
GLBatch bottomBatch;
GLBatch leftBatch;
GLBatch rightBatch;

GLShaderManager shaderManager;
GLFrame cameraFrame;
GLFrame objectFrame;
GLMatrixStack modelviewStack;
GLMatrixStack projectStack;
GLFrustum viewFrustum;
GLGeometryTransform transformPipline;

GLuint textureID;

GLuint textures[3];

GLfloat viewZ = -50.0f;

const char *imageNames[4] = {"brick.tga", "ceiling.tga", "floor.tga", "stone.tga"};

//窗口大小改变时接受新的宽度和高度，其中0,0代表窗口中视口的左下角坐标，w，h代表像素

void ChangeSize(int w,int h)

{
    glViewport(0,0, w, h);
    
    viewFrustum.SetPerspective(80.0f, float(w)/float(h), 1.0f, 120.0f);
    projectStack.LoadMatrix(viewFrustum.GetProjectionMatrix());
    transformPipline.SetMatrixStacks(modelviewStack, projectStack);
}

//为程序作一次性的设置

void SetupRC()
{
    glClearColor(0, 0, 0, 1);
    shaderManager.InitializeStockShaders();
    glEnable(GL_DEPTH_TEST);
    
    GLbyte *pbits;
    GLint iwidth, iheight, icomponent, iloop;
    GLenum format;
    
    //分配纹理对象
    glGenTextures(3, textures);
    
    for (iloop = 0; iloop < 3; iloop++) {
        //绑定纹理
        glBindTexture(GL_TEXTURE_2D, textures[iloop]);
        
        //加载TGA文件,读取纹理像素
        pbits = gltReadTGABits(imageNames[iloop], &iwidth, &iheight, &icomponent, &format);
        
        
        //设置过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        //设置环绕方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        //载入纹理
        glTexImage2D(GL_TEXTURE_2D, 0, icomponent, iwidth, iheight, 0, format, GL_UNSIGNED_BYTE, pbits);
        
        free(pbits);
        
    }
    
    
    GLfloat z;
    
    /*
     void Begin(GLenum primitive, GLuint nVerts, GLuint nTextureUnits = 0);
     参数1:绘图模式
     参数2:顶点个数
     参数3:纹理,默认等于0
     */
    bottomBatch.Begin(GL_TRIANGLE_STRIP, 28,1);
    
    //参考PPT
    for (z = 60.0f; z >= 0.0f; z -= 10.0f) {
        
        //指定左下角顶点
        bottomBatch.Vertex3f(-10.0f, -10.0f, z);
        //指定顶点对应纹理的坐标
        bottomBatch.MultiTexCoord2f(0, 0, 0);
        
        //指定右下角顶点以及纹理坐标
        bottomBatch.Vertex3f(10.0f, -10.0f, z);
        bottomBatch.MultiTexCoord2f(0, 1, 0);
        
        
        bottomBatch.Vertex3f(-10.0f, -10.0f, z-10.0f);
        //指定顶点对应纹理的坐标
        bottomBatch.MultiTexCoord2f(0, 0, 1.0f);
        
        
        bottomBatch.Vertex3f(10.0f, -10.0f, z-10.0f);
        //指定顶点对应纹理的坐标
        bottomBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        
    }
    
    bottomBatch.End();
    
    //参考PPT图6-11
    topBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    for(z = 60.0f; z >= 0.0f; z -=10.0f)
    {
        topBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        topBatch.Vertex3f(-10.0f, 10.0f, z - 10.0f);
        
        topBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        topBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);
        
        topBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        topBatch.Vertex3f(-10.0f, 10.0f, z);
        
        topBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        topBatch.Vertex3f(10.0f, 10.0f, z);
    }
    topBatch.End();
    
    //参考PPT图6-12
    
    
    //参考PPT图6-13
    rightBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    for(z = 60.0f; z >= 0.0f; z -=10.0f)
    {
        rightBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        rightBatch.Vertex3f(10.0f, -10.0f, z);
        
        rightBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        rightBatch.Vertex3f(10.0f, 10.0f, z);
        
        rightBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        rightBatch.Vertex3f(10.0f, -10.0f, z - 10.0f);
        
        rightBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        rightBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);
    }
    rightBatch.End();
    
    
    /*
    GLfloat z;
    
    bottomBatch.Begin(GL_TRIANGLE_STRIP, 40,1);
    
    for (z = 0.0f; z < 100.0f; z += 10.0f) {
        
        //左下
        bottomBatch.Vertex3f(-10.0f, -10.0f, z);
        bottomBatch.MultiTexCoord2f(0, 0, 0);
        
        //右下
        bottomBatch.Vertex3f(10.0f, -10.0f, z);
        bottomBatch.MultiTexCoord2f(0, 1.0f, 0);
        
        //左上
        bottomBatch.Vertex3f(-10.0f, -10.0f, z-10.0f);
        bottomBatch.MultiTexCoord2f(0, 0, 1.0f);
        
        //右上
        bottomBatch.Vertex3f(10.0f, -10.0f, z-10.0f);
        bottomBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        
    }
    bottomBatch.End();
    
    rightBatch.Begin(GL_TRIANGLE_STRIP, 40,1);
    
    for (z = 0.0f; z < 100.0f; z += 10.0f) {
        
        rightBatch.Vertex3f(10.0f, -10.0f, z);
        rightBatch.MultiTexCoord2f(0, 0, 0);
        
        rightBatch.Vertex3f(10.0f, 10.0f, z);
        rightBatch.MultiTexCoord2f(0, 0, 1.0f);
        
        rightBatch.Vertex3f(10.0f, -10.0f, z-10.0f);
        rightBatch.MultiTexCoord2f(0, 1.0f, 0);
        
        rightBatch.Vertex3f(10.0f, 10.0f, z-10.0f);
        rightBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        
    }
    
    rightBatch.End();
    
    leftBatch.Begin(GL_TRIANGLE_STRIP, 40, 1);
    for(z = 0.0f; z < 100.0f; z += 10.0f)
    {
        leftBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        leftBatch.Vertex3f(-10.0f, -10.0f, z);
        
        leftBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        leftBatch.Vertex3f(-10.0f, 10.0f, z);
        
        leftBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        leftBatch.Vertex3f(-10.0f, -10.0f, z - 10.0f);
        
        leftBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        leftBatch.Vertex3f(-10.0f, 10.0f, z - 10.0f);
    }
    leftBatch.End();
     */
}

//开始渲染

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    modelviewStack.PushMatrix();
    
    modelviewStack.Translate(0, 0, viewZ);
    
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE,transformPipline.GetModelViewMatrix(),0);
    
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    topBatch.Draw();
    
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    bottomBatch.Draw();
    
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    leftBatch.Draw();
    rightBatch.Draw();
    
    modelviewStack.PopMatrix();
    
    glutSwapBuffers();
}

// 清理…例如删除纹理对象
void ShutdownRC(void)
{
    glDeleteTextures(1, &textureID);
    
}

void specialKeyClick(int key, int x, int y)
{
    
}

int main(int argc,char* argv[])

{
    //设置当前工作目录，针对MAC OS X
    
    gltSetWorkingDirectory(argv[0]);
    
    //初始化GLUT库
    
    glutInit(&argc, argv);
    
    /*初始化双缓冲窗口，其中标志GLUT_DOUBLE、GLUT_RGBA、GLUT_DEPTH、GLUT_STENCIL分别指
     
     双缓冲窗口、RGBA颜色模式、深度测试、模板缓冲区*/
    
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
    
    //GLUT窗口大小，标题窗口
    
    glutInitWindowSize(800,600);
    
    glutCreateWindow("Triangle");
    
    //注册回调函数
    
    glutReshapeFunc(ChangeSize);
    
    glutDisplayFunc(RenderScene);
    
    glutSpecialFunc(specialKeyClick);
    
    //驱动程序的初始化中没有出现任何问题。
    
    GLenum err = glewInit();
    
    
    
    if(GLEW_OK != err) {
        
        fprintf(stderr,"glew error:%s\n",glewGetErrorString(err));
        
        return 1;
        
    }
    
    //调用SetupRC
    
    SetupRC();
    
    glutMainLoop();
    
    ShutdownRC();
    
    return 0;
    
}



