#include <GL/glut.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <iostream>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

using namespace std;

static CGcontext	myCgContext;
static CGprofile	myCgVertexProfile,
					myCgFragmentProfile;
static CGprogram	myCgVertexProgram,
					myCgFragmentProgram;
static CGparameter  myCgFragmentParam_decal;

static const char   *myProgramName = "Test",
					*myVertexProgramFileName = "C3E2v_varying.cg",
					*myVertexProgramName = "C3E2v_varying",
					*myFragmentProgramFileName = "C3E3f_texture.cg",
					*myFragmentProgramName = "C3E3f_texture";

const char* filename = "obraz.jpg";
//const char* filename = "jablko1.png";

static void checkForCgError(const char* situation);
static void display(void);
static void keyboard(unsigned char c, int x, int y);

int main(int argc,char **argv)
{
	int width = 0, height = 0, nrChannels = 0;

	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInit(&argc, argv);
	glutCreateWindow(myProgramName);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	
	glClearColor(0.0, 1.0, 0.0, 0.0);
	
	stbi_set_flip_vertically_on_load(true);
	unsigned char* imgData = stbi_load(filename, &width, &height, &nrChannels, 0);
	cout << nrChannels;
	unsigned int texture = 0;
	
	if (imgData != nullptr) {

		
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		if (nrChannels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
		}
		else if (nrChannels == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
		}
		
		
		stbi_image_free(imgData);
	}

	myCgContext = cgCreateContext();
	checkForCgError("creating context");
	cgGLSetDebugMode(CG_FALSE);
	cgSetParameterSettingMode(myCgContext,CG_DEFERRED_PARAMETER_SETTING);
	
// Vertex Program
//------------------------------------------------------------
	myCgVertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
	cgGLSetOptimalOptions(myCgVertexProfile);
	checkForCgError("selecting vertex profile");

	myCgVertexProgram = cgCreateProgramFromFile(myCgContext, CG_SOURCE, myVertexProgramFileName, myCgVertexProfile, myVertexProgramName, NULL);
	checkForCgError("creating vertex program from file");
	cgGLLoadProgram(myCgVertexProgram);
	checkForCgError("loading vertex program");
//------------------------------------------------------------
//Fragment Program
//------------------------------------------------------------
	myCgFragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
	cgGLSetOptimalOptions(myCgFragmentProfile);
	checkForCgError("selecting fragment profile");

	myCgFragmentProgram = cgCreateProgramFromFile(myCgContext, CG_SOURCE, myFragmentProgramFileName, myCgFragmentProfile, myFragmentProgramName, NULL);
	checkForCgError("creating fragment program");

	cgGLLoadProgram(myCgFragmentProgram);
	checkForCgError("loading fragment program");

	myCgFragmentParam_decal = cgGetNamedParameter(myCgFragmentProgram,"decal");
	checkForCgError("getting decal parameter");
	
	cgGLSetTextureParameter(myCgFragmentParam_decal, texture);
	checkForCgError("setting decal 2d texture");
	
	glutMainLoop();
	return 0;
}

static void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cgGLBindProgram(myCgVertexProgram);
	checkForCgError("binding vertex program");

	cgGLEnableProfile(myCgVertexProfile);
	checkForCgError("enabling vertex profile");

	cgGLBindProgram(myCgFragmentProgram);
	checkForCgError("binding fragment program");

	cgGLEnableProfile(myCgFragmentProfile);
	checkForCgError("enabling fragment profile");

	cgGLEnableTextureParameter(myCgFragmentParam_decal);
	checkForCgError("enabling decal texture");

	glBegin(GL_TRIANGLES);

	//first triangle
	glTexCoord2f(0.0, 0.0);
	glVertex2f(-1.0,-1.0);

	glTexCoord2f(1.0, 0.0);
	glVertex2f(1.0,-1.0);

	glTexCoord2f(1.0, 1.0);
	glVertex2f(1.0,1.0);
	
	//second triangle
	glTexCoord2f(0.0, 0.0);
	glVertex2f(-1.0, -1.0);

	glTexCoord2f(1.0, 1.0);
	glVertex2f(1.0, 1.0);

	glTexCoord2f(0.0, 1.0);
	glVertex2f(-1.0, 1.0);
	
	glEnd();

	cgGLDisableProfile(myCgVertexProfile);
	checkForCgError("disabling vertex progile");

	cgGLDisableProfile(myCgFragmentProfile);
	checkForCgError("disabling fragment profile");

	cgGLDisableTextureParameter(myCgFragmentParam_decal);
	checkForCgError("disabling decal texture");

	glutSwapBuffers();
	
}

static void keyboard(unsigned char c, int x, int y) {
	switch (c) {
	case 27:
		cgDestroyProgram(myCgVertexProgram);
		cgDestroyProgram(myCgFragmentProgram);
		cgDestroyContext(myCgContext);
		exit(0);
		break;
	}
}

static void checkForCgError(const char* situation) {
	CGerror error;
	const char* string = cgGetLastErrorString(&error);
	if (error != CG_NO_ERROR) {
		cout << ("%s: %s: %s:\n", myProgramName, situation, string);
		if (error == CG_COMPILER_ERROR) {
			cout << ("%s\n", cgGetLastListing(myCgContext));
		}
		exit(1);
	}
}
