//----Keyboard Controls----
//p - toggle view between orthographic and perspective modes
//f - toggle view between fullscreen and window modes
//w - toggle between wireframe and solid shapes
//background colors - r : red, b : blue, g : green, l : black, t : white, o : orange, e : grey, y : yellow
//-----Mouse Controls-----
//left mouse button - rotate shape
//middle mouse button - rotate camera
//right mouse button - translate shape
//(left mouse button + shift key) - translate light source
//q or esc - quit

#include <cstdlib>
#include <GL/glut.h>
#include <math.h>

#define TRANS_FACTOR 0.005
#define SHAPE_ROTATE_FACTOR 0.7
#define CAMERA_ROTATE_FACTOR 0.1
#define LIGHT_TRANS_FACTOR 2

GLsizei winWidth = 550, winHeight = 550;
bool perspectiveView = true, fullscreen = false, resetSize = false;
int MouseY, MouseX, MouseAction, Shift;
float CubeThetaX = 0, CubeThetaY = 0;
float CameraThetaX = 0, CameraThetaY = 0;
float TransZ = 0;
float LightPosX = -400, LightPosY = 400;
bool isWireframeDisplay = false;
GLfloat red = 0.2, green = 0.5, blue = 0.8;
bool isEnd = false;

void init()
{
	glMatrixMode(GL_PROJECTION);		
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	glEnable(GL_LIGHT0);
/*	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);*/
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
			CubeThetaX += SHAPE_ROTATE_FACTOR * DeltaX;
			CubeThetaY += SHAPE_ROTATE_FACTOR * DeltaY;
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

void drawTexture(GLfloat *diffuseSpecular, GLfloat shininess = 50)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseSpecular);	
	glMaterialfv(GL_FRONT, GL_SPECULAR, diffuseSpecular); 
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void drawHead()
{	
	GLfloat head[] = {0, 0.5, 0, 0};
	
	glPushMatrix();	
	glTranslatef(0, -2, 0);
	if(isWireframeDisplay)
		glutWireSphere(4, 50, 10);
	else
	{
		drawTexture(head);
		glutSolidSphere(4, 50, 10);
	}
	glPopMatrix();
}

void drawEyes()
{	
	GLfloat corneas[] = {1, 1, 1, 0.0};
	GLfloat pupils[] = {0, 0, 0, 0.0};

	//draw left eye
	glPushMatrix();		
	glTranslatef(-2, -0.5, -3.25);
	if(isWireframeDisplay)
	{
		glutWireSphere(0.6, 20, 10);
		glutWireSphere(0.3, 20, 10);
	}
	else
	{
		drawTexture(corneas);
		glutSolidSphere(0.6, 20, 10);
		drawTexture(pupils, 200);
		glutSolidSphere(0.3, 20, 10);
	}
	glPopMatrix();
	//draw right eye
	glPushMatrix();		
	glTranslatef(2, -0.5, -3.25);
	if(isWireframeDisplay)
		glutWireSphere(0.6, 20, 10);
	else
	{
		drawTexture(corneas);
		glutSolidSphere(0.6, 20, 10);
		drawTexture(pupils, 200);	
		glutSolidSphere(0.3, 20, 10);
	}
	glPopMatrix();	
}

void drawNose()
{	
	GLfloat nose[] = {0.5, 0, 0, 0.0};

	glPushMatrix();	
	glScalef(0.85, 0.85, 0.85);
	glTranslatef(0, -1.5, -4.5);
	if(isWireframeDisplay)
		glutWireIcosahedron();
	else
	{
		drawTexture(nose, 100);
		glutSolidIcosahedron();
	}
	glPopMatrix();
}

void drawEars()
{	
	GLfloat ears[] = {0.5, 0.5, 0, 0.0};

	//draw left ear
	glPushMatrix();	
	glTranslatef(-4, -1.25, -0.75);
	glRotated(90, 0, 1, 0);
	if(isWireframeDisplay)
		glutWireTorus(0.3, 0.6, 50, 10);
	else
	{
		drawTexture(ears);
		glutSolidTorus(0.3, 0.6, 50, 10);
	}
	glPopMatrix();
	//draw right ear
	glPushMatrix();	
	glTranslatef(4, -1.25, -0.75);
	glRotated(90, 0, 1, 0);
	if(isWireframeDisplay)
		glutWireTorus(0.3, 0.6, 50, 10);
	else
	{
		drawTexture(ears);
		glutSolidTorus(0.3, 0.6, 50, 10);
	}
	glPopMatrix();
}

void drawTeeth(GLfloat xoffset)
{	
	GLfloat teeth[] = {1, 1, 1, 0.0};

	glPushMatrix();	
	glTranslatef(xoffset, -3.8, -3.25);	
	if(isWireframeDisplay)
		glutWireCube(0.5);
	else
	{
		drawTexture(teeth);
		glutSolidCube(0.5);
	}
	glPopMatrix();
}

void drawMouth()
{
	drawTeeth(-1);
	drawTeeth(-0.45);
	drawTeeth(0.1);
	drawTeeth(0.65);
	drawTeeth(1.2);
}

void drawHat()
{	
	GLfloat urceolates[] = {0.5, 0, 1, 0};
	GLfloat cones[] = {1, 0.5 , 0, 0};

	//draw left cone
	glPushMatrix();	
	glTranslatef(-1.9, 0.75, -0.75);
	glRotated(-90, 1, 0, 0);
	glRotated(-30, 0, 1, 0);

	if(isWireframeDisplay)
		glutWireCone(2, 6, 20, 10);
	else
	{
		drawTexture(cones);
		glutSolidCone(2, 6, 20, 10);
	}
	glPopMatrix();	
	//draw the left urceolate
	glPushMatrix();	
	glTranslatef(-5, 6, -0.75);		
	if(isWireframeDisplay)
		glutWireSphere(0.5, 10, 5);
	else
	{
		drawTexture(urceolates);
		glutSolidSphere(0.5, 10, 5);
	}
	glPopMatrix();
	//draw right cone
	glPushMatrix();	
	glTranslatef(1.9, 0.75, -0.75);
	glRotated(-90, 1, 0, 0);
	glRotated(30, 0, 1, 0);

	if(isWireframeDisplay)
		glutWireCone(2, 6, 20, 10);
	else
	{
		drawTexture(cones);
		glutSolidCone(2, 6, 20, 10);
	}
	glPopMatrix();	
	//draw the left urceolate
	glPushMatrix();	
	glTranslatef(5, 6, -0.75);		
	if(isWireframeDisplay)
		glutWireSphere(0.5, 10, 5);
	else
	{
		drawTexture(urceolates);
		glutSolidSphere(0.5, 10, 5);
	}
	glPopMatrix();
}

void drawNecklace()
{	
	GLfloat necklace[] = {1, 0, 1, 0.0};

	//draw the ring
	glPushMatrix();	
	glTranslatef(0, -6, 0.45);
	glRotatef(90, 1, 0, 0);

	if(isWireframeDisplay)
		glutWireTorus(0.5, 1.5, 5, 10);
	else
	{	
		drawTexture(necklace);
		glutSolidTorus(0.5, 1.5, 5, 10);
	}	
	glPopMatrix();
	//draw the baseline
	glPushMatrix();	
	glTranslatef(0, -9.45, 0.45);
	glRotatef(90, 1, 0, 0);
	GLUquadricObj *cylinder = gluNewQuadric();

	if(isWireframeDisplay)
		gluQuadricDrawStyle(cylinder, GLU_LINE);
	else		
		drawTexture(necklace);

	gluDisk(cylinder, 0.5, 1.5, 8, 10);
	glPopMatrix();
}

void drawNeck()
{	
	GLfloat neck[] = {0, 0.5, 0, 0.0};

	glPushMatrix();	
	glTranslatef(0, -6.4, 0.45);
	glRotatef(90, 1, 0, 0);
	GLUquadricObj *cylinder = gluNewQuadric();

	if(isWireframeDisplay)
		gluQuadricDrawStyle(cylinder, GLU_LINE);
	else		
		drawTexture(neck);

	gluCylinder(cylinder, 1.5, 1.5, 3, 15, 5);
	glPopMatrix();
}

void drawHierarchialClown()
{
	drawHat();
	drawHead();
	drawEyes();
	drawEars();
	drawNose();
	drawMouth();
	drawNecklace();
	drawNeck();
}

void displayFcn()
{			
	glColor4f(1, 1, 1, 0);
	glClearColor(red, green, blue, 0.0); 	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//View Setting
	if(fullscreen)
		glutFullScreen();
	else
	{
		if(resetSize)
		{
			glutPositionWindow(237, 109);
			glutReshapeWindow(winWidth, winHeight);	
			resetSize = false;
		}
	}

	if(perspectiveView)
		glutSetWindowTitle("Artman Hierarchial Shaded 3D Effects Visualizer (Perspective Mode)");
	else
		glutSetWindowTitle("Artman Hierarchial Shaded 3D Effects Visualizer (Orthographic Mode)");
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();	
	//Rotate the camera
	gluLookAt (0.0, 0.0, 20.0, CameraThetaX, CameraThetaY, 0.0, 0.0, 1.0, 0.0);	 
	//Zoom in or out
	glTranslatef(0, 0, TransZ); 
	//Rotate the Cube
	glRotatef(CubeThetaX, 0, -1.5, 0);   //-1.5 is the center of picture in y axis    
	glRotatef(CubeThetaY, 1, 0, 0); 
	//Draw the shapes
	if(isWireframeDisplay)	
		glDisable(GL_LIGHTING);	
	else
	{		
		GLfloat ambientColor[]={red, green, blue, 0.0};	
		GLfloat lightPosition[] = {LightPosX, LightPosY, 500, 1};
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);		
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientColor);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_LIGHTING);				
	}
	//display the hierarchial shape
	drawHierarchialClown();

	glFlush();
	glutSwapBuffers();	//this avoids flickering along with GLUT_DOUBLE in init
}

void idleFunc()
{
	if(TransZ >= 8 || TransZ <= -15)
		isEnd = !isEnd;
	if(isEnd)
	{
		TransZ -= 0.05;
		CameraThetaX -= 0.01;
		CameraThetaY -= 0.01;
		LightPosX -= 0.1;
		LightPosY -= 0.1;
	}
	else
	{
		TransZ += 0.05;
		CameraThetaX += 0.01;
		CameraThetaY += 0.01;
		LightPosX += 0.1;
		LightPosY += 0.1;
	}

	++CubeThetaX;
	++CubeThetaY;

	glutPostRedisplay();
}

void main (int argc, char** argv)
{
	glutInit (&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition (237, 109);	
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow ("Artman Hierarchial Shaded 3D Effects Visualizer (Perspective Mode)");
	init();
	glutDisplayFunc(displayFcn);
	glutReshapeFunc(winReshapeFcn);
	glutKeyboardFunc(keyboardFcn);
	glutMouseFunc(mouseFcn);
	glutMotionFunc(mouseMotionFcn);
	glutIdleFunc(idleFunc);
	glutMainLoop();
}