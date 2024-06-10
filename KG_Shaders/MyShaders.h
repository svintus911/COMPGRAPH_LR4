#ifndef MYSHADERS_H
#define MYSHADERS_H

#include <windows.h>

#include <GL\gl.h>
#include <GL\glu.h>
#include "GL\glext.h"

#include <string>


extern PFNGLDELETEOBJECTARBPROC glDeleteObjectARB;
extern PFNGLGETHANDLEARBPROC glGetHandleARB;
extern PFNGLDETACHOBJECTARBPROC glDetachObjectARB;
extern PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
extern PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
extern PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
extern PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
extern PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
extern PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
extern PFNGLVALIDATEPROGRAMARBPROC glValidateProgramARB;
extern PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;
extern PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;

extern PFNGLGETINFOLOGARBPROC glGetInfoLogARB;

//�������� ��������  ��������� � ������
extern PFNGLUNIFORM1FARBPROC glUniform1fARB;
extern PFNGLUNIFORM1IARBPROC glUniform1iARB;
extern PFNGLUNIFORM2IARBPROC glUniform2iARB;
extern PFNGLUNIFORM3FARBPROC glUniform3fARB;
extern PFNGLUNIFORM2FARBPROC glUniform2fARB;
extern PFNGLUNIFORM4FARBPROC glUniform4fARB;
extern PFNGLUNIFORM1IARBPROC glUniform1iARB;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;

extern PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;

extern PFNGLACTIVETEXTUREPROC glActiveTexture;

int loadShader(LPWSTR filename, char **shaderSrc, int *programLength);

//�������� ��� ������� �  ����� �������� ��� ��������� ��������
void initShadersFunctions();

class Shader
{
protected:


public:
	GLhandleARB program;
	GLhandleARB vertex;
	GLhandleARB fragment;

	LPWSTR VshaderFileName;
	LPWSTR FshaderFileName;

	std::string vetrshader;
	std::string pixshader;

	bool compiled = false;


	Shader();

	void LoadShaderFromFile();

	int loadPixShader(const char** text, int* lengths, int count, char** err);
	int loadVertShader(const char** text, int* lengths, int count, char** err);


	void Compile();


	void UseShader();

	static void DontUseShaders();
};


//��������� ��� ������� �� �����
//************************************
// Parameter: char * filename   ��� �����
// Parameter: char * * shaderSrc   ��������� �� ������ ��������, � ������� ���������� ��� �������. ����� ����, ��� ������ �������� ���� ����������� ������!
// Parameter: int * programLength  ��������� �� ����� ������� ��������.
//************************************
int loadShader(LPWSTR filename, char **shaderSrc, int *programLength);


#endif // !MYSHADERS_H
