#include <string.h>
#include <math.h>
#include <GL/glut.h>

GLsizei winWidth = 500, winHeight = 500;
const double TWO_PI = 6.2831853;
const double PI = 3.1415926536;
int xCenter = winWidth / 2;
int yCenter = winHeight / 2;
bool scale = true;

void init()
{
	glClearColor(0, .78, 1, 1); 
	glMatrixMode(GL_PROJECTION); 
	gluOrtho2D(0.0, winWidth, 0.0, winHeight);
	glMatrixMode(GL_MODELVIEW);
}

void pointDrawing(int x, int y)
{
	glBegin(GL_POINTS); 
	glVertex2i(x, y);
	glEnd( );
	glFlush();
}

void lineDrawing(int x1, int y1, int x2, int y2)
{	
	glBegin(GL_LINES);
	glVertex2i(x1, y1);
	glVertex2i(x2, y2);
	glEnd();
	glFlush();
}

void regularPolygonDrawing(int xCenter, int yCenter, int numOfVertices, int radius, int rotateDeg = 0, GLenum mode = GL_POLYGON)
{
	glBegin(mode);
	for (int k = 0; k < numOfVertices; k++) 
	{
		GLdouble theta = (rotateDeg * PI / 180) + (TWO_PI * k / numOfVertices);
		glVertex2i (xCenter + radius * cos (theta), yCenter + radius * sin (theta));
	}
	glEnd ( );
	glFlush();
}

void circleDrawing(int xCenter, int yCenter, int radius)
{	
	glPushMatrix();
	glTranslatef(xCenter, yCenter, 0);

	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0, 0);
	for (double angle = 0.0; angle <= TWO_PI + 2; angle += PI / 24)
	glVertex2f(radius * cos(angle), radius * sin(angle));	  
	glEnd();

	glPopMatrix();
	glFlush();
}

void stringDrawing(int x, int y, char *str, void *font, int inc)
{
	int increment = x;
	for(unsigned int i = 0; i < strlen(str); i++)
	{
		glRasterPos2i(increment, y);
		glutBitmapCharacter(font, str[i]);		
		increment += inc;
	}
}

void displayFcn()
{
	glClear (GL_COLOR_BUFFER_BIT);	
	glPointSize(20);

	glColor3f(1 , 1, 1);
	circleDrawing(xCenter, yCenter, winWidth / 2);
	glColor3f(0 , 0, 0);
	regularPolygonDrawing(xCenter, yCenter, 360, winWidth  * .4, 0, GL_POINTS); //a shell
	glColor3f(0 , 1, 0);
	regularPolygonDrawing(xCenter, yCenter, 3, winWidth  / 5 + 140);
	glColor3f(0 , .5, 0);
	regularPolygonDrawing(xCenter, yCenter, 4, winWidth  / 5 + 120);
	glColor3f(0 , 0, 1);
	regularPolygonDrawing(xCenter, yCenter, 5, winWidth  / 5 + 100);
	glColor3f(0 , 0, .5);
	regularPolygonDrawing(xCenter, yCenter, 6, winWidth  / 5 + 80);
	glColor3f(1, 0, 0);
	regularPolygonDrawing(xCenter, yCenter, 8, winWidth  / 5 + 60);
	glColor3f(.5 , 0, 0);
	regularPolygonDrawing(xCenter, yCenter, 10, winWidth  / 5 + 40);
	glColor3f(0 , 1, 1);
	regularPolygonDrawing(xCenter, yCenter, 12, winWidth  / 5 + 20);

	glColor3f(1 , .5, .8);
	stringDrawing(xCenter - 72, yCenter, "Payman", GLUT_BITMAP_TIMES_ROMAN_24, 24);

	glColor3f(1 , 1, 0);
	for(int i = 0; i < 180; i += 3)
	{
		double angle = 2 * i * TWO_PI / 180;
		float x = winWidth * cos(angle) / 2;
		float y = winHeight * sin(angle) / 2;
		lineDrawing(x + xCenter, yCenter - y, -x + xCenter, yCenter + y);
	}

	glColor3f(.5 , .33, 1);
	stringDrawing(xCenter - 180, winHeight * .95, "Artman Systems Inc.", GLUT_BITMAP_HELVETICA_18, 18);

	//glTranslatef(xCenter, yCenter, 0);
	glPointSize(3);
	int red = 1, blue = 0;
	for(int i = 9; i <= 180; i += 9)
	{
		double angle =  - i * PI / 180;
		float x = winWidth * cos(angle) / 6 + xCenter;
		float y = yCenter - winHeight * sin(angle) / 6;
		glColor3f(red , 0, blue);
		red = (red + 1) % 2;
		blue = (blue + 1) % 2;
		pointDrawing(x, y);
	}
	glFlush();
}

void winReshapeFcn (GLint newWidth, GLint newHeight)
{
	glViewport (0, 0, newWidth, newHeight);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ( );
	xCenter = newWidth / 2;
	yCenter = newHeight / 2;
	gluOrtho2D (0.0, GLdouble (newWidth), 0.0, GLdouble (newHeight));

	if(scale)
	{		
		winWidth = newWidth;
		winHeight = newHeight;		
	}	
}

//Enter 's' to toggle between scale and fixed modes
void keyboardFcn(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 's':
	case 'S':
		scale = !scale;
		if(scale)
			glutSetWindowTitle("Artman Logo - Scale Mode");
		else
			glutSetWindowTitle("Artman Logo - Fixed Mode");
	default:
		break;
	}
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); 
	glutInitWindowPosition(250, 100); 
	glutInitWindowSize(winWidth, winHeight); 
	glutCreateWindow("Artman Logo - Scale Mode"); 
	init(); 
	glutDisplayFunc(displayFcn); 
	glutReshapeFunc(winReshapeFcn);
	glutKeyboardFunc(keyboardFcn);
	glutMainLoop( );
}