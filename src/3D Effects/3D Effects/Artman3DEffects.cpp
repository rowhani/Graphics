//----Keyboard Controls----
//p - toggle view between orthographic and perspective modes
//f - toggle view between fullscreen and window modes
//x - invert background and foreground colors
//q or esc - quit

#include <cstdlib>
#include <GL/glut.h>
#include <math.h>

#define TRANS_FACTOR 0.05
#define CUBE_ROTATE_FACTOR 0.7
#define CAMERA_ROTATE_FACTOR 0.1
#define CUBE_VERTEX_SIZE 10

GLsizei winWidth = 500, winHeight = 500;
bool perspectiveView = false, fullscreen = false, invert = false, resetSize = false;
int MouseY, MouseX, MouseAction;
float CubeThetaX = 0, CubeThetaY = 0;
float CameraThetaX = 0, CameraThetaY = 0;
float TransZ = 0;

void init()
{
	glMatrixMode(GL_PROJECTION);	
	glClearColor (0.2, 0.5, 0.8, 0.0);
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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
	}
}

void mouseMotionFcn(int x, int y)
{
	int DeltaX = x - MouseX;
	int DeltaY = y - MouseY;

	if(MouseAction == GLUT_LEFT_BUTTON)
	{
		CubeThetaX += CUBE_ROTATE_FACTOR * DeltaX;
		CubeThetaY += CUBE_ROTATE_FACTOR * DeltaY;
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
		glClearColor(0.3, 1.0, 0.2, 0.0);
		glColor3f(0.2, 0.5, 0.8);
	}
	else
	{
		glColor3f(0.3, 1.0, 0.2);
		glClearColor(0.2, 0.5, 0.8, 0.0); 
	}
	if(perspectiveView)
		glutSetWindowTitle("Artman 3D Effects Visualizer (Perspective Mode)");
	else
		glutSetWindowTitle("Artman 3D Effects Visualizer (Orthographic Mode)");
	glClear (GL_COLOR_BUFFER_BIT);
	glLoadIdentity();	
	//Rotate the camera
	gluLookAt (0.0, 0.0, 20.0, CameraThetaX, CameraThetaY, 0.0, 0.0, 1.0, 0.0);	 
	//Zoom in or out
	glTranslatef(0, 0, TransZ); 
	//Rotate the Cube
	glRotatef(CubeThetaX, 0, 1, 0);       
	glRotatef(CubeThetaY, 1, 0, 0); 
	//Draw the shapes
	glutWireCube(CUBE_VERTEX_SIZE);
	glutWireSphere(CUBE_VERTEX_SIZE / 2, 30, 30);
	glutSolidTeapot(CUBE_VERTEX_SIZE / 5);
	glFlush();
}

void winReshapeFcn(GLint newWidth, GLint newHeight)
{
	glViewport(0, 0, newWidth, newHeight);	
	redraw();
	glClear(GL_COLOR_BUFFER_BIT);
}

void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition (250, 125);	
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow ("Artman 3D Effects Visualizer (Orthographic Mode)");
	init();
	glutDisplayFunc(displayFcn);
	glutReshapeFunc(winReshapeFcn);
	glutKeyboardFunc(keyboardFcn);
	glutMouseFunc(mouseFcn);
	glutMotionFunc(mouseMotionFcn);
	glutMainLoop();
}