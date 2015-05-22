//----Keyboard Controls----
//p - toggle view between orthographic and perspective modes
//f - toggle view between fullscreen and window modes
//x - invert background and foreground colors
//w - toggle between wireframe and solid shapes
//colors - r : red, b : blue, g : green, l : black, t : white, o : orange, e : grey, y : yellow
//-----Mouse Controls-----
//left mouse button - rotate shape
//middle mouse button - rotate camera
//right mouse button - translate shape
//(left mouse button + shift key) - translate light source
//q or esc - quit

#include <cstdlib>
#include <GL/glut.h>
#include <math.h>

#define TRANS_FACTOR 0.05
#define CUBE_ROTATE_FACTOR 0.7
#define CAMERA_ROTATE_FACTOR 0.1
#define LIGHT_TRANS_FACTOR 2
#define CUBE_VERTEX_SIZE 10

GLsizei winWidth = 500, winHeight = 500;
bool perspectiveView = false, fullscreen = false, invert = false, resetSize = false;
int MouseY, MouseX, MouseAction, Shift;
float CubeThetaX = 0, CubeThetaY = 0;
float CameraThetaX = 0, CameraThetaY = 0;
float TransZ = 0;
float LightPosX = -400, LightPosY = 400;
bool isWireframeDisplay = true;
GLfloat red = 1, green = 0.5, blue = 1;

void init()
{
	glMatrixMode(GL_PROJECTION);	
	glClearColor (0.2, 0.5, 0.8, 0.0);
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	glEnable(GL_LIGHT0);
}

void redraw()
{	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	if(perspectiveView)
		glFrustum(-6, 6, -6, 6, 10, 100);		
	else		
		glOrtho(-10, 10, -10, 10, 10, 100);
	glMatrixMode(GL_MODELVIEW);		
}

void keyboardFcn(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 'p':
	case 'P':
		perspectiveView = !perspectiveView;
		redraw();
		glutPostRedisplay();
		break;
	case 'w':
	case 'W':
		isWireframeDisplay = !isWireframeDisplay;
		redraw();
		glutPostRedisplay();
		break;
	case 'f':
	case 'F':
		fullscreen = !fullscreen;	
		resetSize = true;		
		glutPostRedisplay();
		break;
	case 'x':
	case 'X':
		invert = !invert;		
		glutPostRedisplay();
		break;
	case 'r':
	case 'R':
		red = 1;
		green = 0;
		blue = 0;
		glutPostRedisplay();
		break;
	case 'b':
	case 'B':
		red = 0;
		green = 0;
		blue = 1;
		glutPostRedisplay();
		break;
	case 'g':
	case 'G':
		red = 0;
		green = 1;
		blue = 0;
		glutPostRedisplay();
		break;
	case 'y':
	case 'Y':
		red = 1;
		green = 1;
		blue = 0;
		glutPostRedisplay();
		break;
	case 'o':
	case 'O':
		red = 1;
		green = 0.5;
		blue = 0;
		glutPostRedisplay();
		break;
	case 'v':
	case 'V':
		red = 0.5;
		green = 0;
		blue = 1;
		glutPostRedisplay();
		break;
	case 'l':
	case 'L':
		red = 0;
		green = 0;
		blue = 0;
		glutPostRedisplay();
		break;
	case 't':
	case 'T':
		red = 1;
		green = 1;
		blue = 1;
		glutPostRedisplay();
		break;
	case 'e':
	case 'E':
		red = 0.5;
		green = 0.5;
		blue = 0.5;
		glutPostRedisplay();
		break;
	case 'q':
	case 'Q':
	case 27:
		exit(0);	
	default:
		break;
	}
}

void mouseFcn(int button, int action, int x, int y)
{
	if(action == GLUT_DOWN)
	{		
		MouseX = x;
		MouseY = y;
		MouseAction = button;
		Shift = glutGetModifiers() == GLUT_ACTIVE_SHIFT;
	}
}

void mouseMotionFcn(int x, int y)
{
	int DeltaX = x - MouseX;
	int DeltaY = y - MouseY;

	if(MouseAction == GLUT_LEFT_BUTTON)
	{
		if(Shift)
		{
			LightPosX += LIGHT_TRANS_FACTOR * DeltaX;
			LightPosY += LIGHT_TRANS_FACTOR * DeltaY;
		}
		else
		{
			CubeThetaX += CUBE_ROTATE_FACTOR * DeltaX;
			CubeThetaY += CUBE_ROTATE_FACTOR * DeltaY;
		}
	}
	else if (MouseAction == GLUT_MIDDLE_BUTTON)
	{
		CameraThetaX -= CAMERA_ROTATE_FACTOR * DeltaX;
		CameraThetaY -= CAMERA_ROTATE_FACTOR * -DeltaY;
	}
	else if (MouseAction == GLUT_RIGHT_BUTTON)
		TransZ += TRANS_FACTOR * (DeltaX + DeltaY);		

	MouseX = x;
	MouseY = y;
	glutPostRedisplay();
}

void displayFcn()
{		
	GLfloat diffuseSpecularColor[]={red, green, blue, 1.0};
	GLfloat ambientColor[]={0.2, 0.5, 0.8, 0.0};	
	GLfloat lightPosition[] = {LightPosX, LightPosY, 500, 1};	

	//View Setting
	if(fullscreen)
		glutFullScreen();
	else
	{
		if(resetSize)
		{
			glutPositionWindow(250, 125);
			glutReshapeWindow(winWidth, winHeight);	
			resetSize = false;
		}
	}
	if(invert)
	{
		glColor3f(0.2, 0.5, 0.8);
		glClearColor(red, green, blue, 0.0); 
		diffuseSpecularColor[0] = 0.2;
		diffuseSpecularColor[1] = 0.5;
		diffuseSpecularColor[2] = 0.8;
		ambientColor[0] = red;
		ambientColor[1] = green;
		ambientColor[2] = blue;
	}
	else
	{
		glClearColor(0.2, 0.5, 0.8, 0.0);
		glColor3f(red, green, blue);		
	}
	if(perspectiveView)
		glutSetWindowTitle("Artman Shaded 3D Effects Visualizer (Perspective Mode)");
	else
		glutSetWindowTitle("Artman Shaded 3D Effects Visualizer (Orthographic Mode)");
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();	
	//Rotate the camera
	gluLookAt (0.0, 0.0, 20.0, CameraThetaX, CameraThetaY, 0.0, 0.0, 1.0, 0.0);	 
	//Zoom in or out
	glTranslatef(0, 0, TransZ); 
	//Rotate the Cube
	glRotatef(CubeThetaX, 0, 1, 0);       
	glRotatef(CubeThetaY, 1, 0, 0); 
	//Draw the shapes
	if(isWireframeDisplay)
	{
		glDisable(GL_LIGHTING);
		
		glutWireCube(CUBE_VERTEX_SIZE);
		//	glutWireSphere(CUBE_VERTEX_SIZE / 2, 30, 30);
		//	glutWireTeapot(CUBE_VERTEX_SIZE / 5);	
	}
	else
	{
		//Lighting effects			
		/*
		glMaterialfv(GL_FRONT, GL_EMISSION, diffuseSpecularColor);
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.5 );
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.75);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.4);
		GLfloat dirVector[] = {1, 0, 0};
		glLightf(GL_LIGHT0, GL_SPOT_DIRECTION, 1.5 );
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30);
		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.5);
		GLfloat atmoColor[] = {0.8, 0.8, 1, 1};		
		glFogfv(GL_FOG_COLOR, atmoColor);
		glFogi(GL_FOG_MODE, GL_EXP2);
		glEnable(GL_FOG);
		GLfloat color[1][2][4];
		color[0][0][0] = 1;
		color[0][0][1] = 0;
		color[0][0][2] = 0;
		color[0][0][3] = 1;
		color[0][1][0] = 0;
		color[0][1][1] = 1;
		color[0][1][2] = 0;
		color[0][1][3] = 1;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, color);
		glEnable(GL_TEXTURE_2D);
		glBegin (GL_LINES);
			glTexCoord2f (0.0, 0.0);
			glVertex3f (0, 0, 0);
			glTexCoord2f (1, 0.0);
			glVertex3f (400, 0, 0);
			glTexCoord2f (1, 1);
			glVertex3f (400, 400, 0);
			glTexCoord2f (0, 1) ;
			glVertex3f (0, 400, 0);
		glEnd();				
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		*/
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);		
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientColor);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseSpecularColor);	
		glMaterialfv(GL_FRONT, GL_SPECULAR, diffuseSpecularColor); 
		glMaterialf(GL_FRONT, GL_SHININESS, 5);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_LIGHTING);

		glutSolidCube(CUBE_VERTEX_SIZE);
		//glutSolidSphere(CUBE_VERTEX_SIZE / 2, 30, 30);
		//glutSolidTeapot(CUBE_VERTEX_SIZE / 5);		
	}
	glFlush();	
}

void winReshapeFcn(GLint newWidth, GLint newHeight)
{
	float originalaspect = (float)winWidth / (float)winHeight;
	float newaspect = (float)newWidth / (float)newHeight;
	int vw, vh;
	int woffset, hoffset;
	if(newaspect > originalaspect)
	{
		vh = newHeight;
		vw = int(originalaspect * vh);
		woffset = int((newWidth - vw) / 2);
		hoffset = 0;
	}
	else
	{
		vw = newWidth;
		vh = int(vw / originalaspect);      
		hoffset = int((newHeight - vh) / 2);
		woffset = 0;
	}
	glViewport(woffset, hoffset, vw, vh); 

	redraw();
	glClear(GL_COLOR_BUFFER_BIT);
}

void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition (250, 125);	
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow ("Artman Shaded 3D Effects Visualizer (Orthographic Mode)");
	init();
	glutDisplayFunc(displayFcn);
	glutReshapeFunc(winReshapeFcn);
	glutKeyboardFunc(keyboardFcn);
	glutMouseFunc(mouseFcn);
	glutMotionFunc(mouseMotionFcn);
	glutMainLoop();
}