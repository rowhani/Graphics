#include <cstdlib>
#include <GL/glut.h>
#include "Vector.h"

#define PNUM  100
#define ENTRY_MODE 0
#define EDIT_MODE 1

GLsizei winWidth = 500, winHeight = 500;
Vector2d vertices[PNUM];
int pnum = 0, selIndex = 0, last = 0, mode = ENTRY_MODE;

void init()
{
	glClearColor(.32, .23, .68, 1); 
	glMatrixMode(GL_PROJECTION); 
	gluOrtho2D(0.0, winWidth, 0.0, winHeight);
	glMatrixMode(GL_MODELVIEW);
}

void lineDrawing(int p1, int p2)
{	
	glBegin(GL_LINES);
	glVertex2i(vertices[p1].x, vertices[p1].y);
	glVertex2i(vertices[p2].x, vertices[p2].y);
	glEnd();
	glFlush();
}

void pointDrawing(int x, int y)
{
	glBegin (GL_POINTS); 
	glVertex2i (x, y);
	glEnd ( );
	glFlush();
}

void insertPoint(int inx, Vector2d p)
{
	int i;
	for(i = pnum ;i > inx + 1; i--)
		vertices[i] = vertices[i - 1];
	vertices[i] = p;
	++pnum;
}

void removePoint(int inx)
{
	int i;
	for(i = inx; i < pnum - 1; i++)
		vertices[i] = vertices[i + 1];
	--pnum;
}

int checkForSelectedPoint(bool fromBeginning, Vector2d mpoint, Vector2d mop)
{ 	
	int i;
	selIndex = fromBeginning? 0: selIndex + 1;

	//if the absolute magnitude of the main point from a point is less than 20 (2*10), select that point
	for(; selIndex < pnum; selIndex++)
	{
		if((mpoint - vertices[selIndex]).normsqr() < 20)
			return selIndex;		
	}	

	Vector2d a, b, c;
	for(i = 0; i < pnum; i++)
	{
		a = vertices[(i + 1) % pnum] - vertices[i];		
		b = mop - vertices[i];	
		c = ((a.normalize()) * b) * a.normalize() - b;	//c is the projection of component b in the oppisite perpendicular direction of a
		if(c.norm() < 5)
			return i;
	}	
	return -1;
}

void redraw()
{	
	glClear (GL_COLOR_BUFFER_BIT); 
	//draw points and lines
	pointDrawing(vertices[0].x, vertices[0].y);
	for(int i = 1; i < pnum; i++)
	{
		pointDrawing(vertices[i].x, vertices[i].y);		
		lineDrawing(i - 1, i);
	}
	//close polygon
	if(mode == EDIT_MODE)
		lineDrawing(0, pnum - 1);
	glFlush ( );    
}

void mouseFcn(GLint button, GLint action, GLint xMouse, GLint yMouse)
{
	int i;
	int start, end;
	//if left button is pressed in Entry mode, draw a point and a line to last point
	if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN && mode == ENTRY_MODE ) 
	{
		vertices[pnum].x = xMouse;
		vertices[pnum].y = winHeight - yMouse;
		pointDrawing(vertices[pnum].x, vertices[pnum].y);
		lineDrawing(last, pnum);
		++pnum;
	}
	//if left button is released in Entry mode, freeze the point
	else if (button == GLUT_LEFT_BUTTON && action == GLUT_UP && mode == ENTRY_MODE )  
	{
		vertices[pnum - 1].x = xMouse;
		vertices[pnum - 1].y = winHeight - yMouse;	
		Vector2d empty;
		i = checkForSelectedPoint(true, vertices[pnum - 1], empty);
		if(i < 0)
		{			
			pointDrawing(vertices[pnum - 1].x, vertices[pnum - 1].y);
			start = last;
			end = pnum - 1;
			last = pnum - 1;
		}
		else
		{
			vertices[pnum - 1].x = vertices[i].x;
			vertices[pnum - 1].y = vertices[i].y;
			start = last;
			last = i;
			end = i;
		}		
		if(pnum > 1)
			lineDrawing(start, end);			
	}

	//if left button is pressed in Edit mode, find the selected point to be moved
	else if(button == GLUT_LEFT_BUTTON && action == GLUT_DOWN && mode == EDIT_MODE)
	{
		Vector2d point;	
		Vector2d empty;
		point.x = xMouse;
		point.y = winHeight - yMouse;
		i = checkForSelectedPoint(true, point, empty);
		if(i >= 0)
			last = i;
	}
	//if left button is released in Edit mode, freeze or insert a point
	else if(button == GLUT_LEFT_BUTTON && action == GLUT_UP && mode == EDIT_MODE)
	{
		if(last < 0)
		{
			Vector2d addP;
			Vector2d empty;
			addP.x = xMouse;
			addP.y = winHeight - yMouse;
			int i = checkForSelectedPoint(true, empty, addP);
			if(i >= 0)
			{
				insertPoint(i, addP);
				redraw();
			}
		}
		else
		{
			Vector2d temp = vertices[last];		
			Vector2d empty;
			while(last >= 0)
			{
				vertices[last].x = xMouse;
				vertices[last].y = winHeight - yMouse;
				last = checkForSelectedPoint(false, temp, empty);
			}
			redraw();
			last = -1;
		}		
	}
	//if right button is released in Entry mode, close the polygon and go to Edit mode
	else if (button == GLUT_RIGHT_BUTTON && action == GLUT_UP && mode == ENTRY_MODE )
	{
		lineDrawing(0, last);
		mode = EDIT_MODE;		
		last = -1;
	}
	//if right button is released in Edit mode, remove the selected point
	else if (button == GLUT_RIGHT_BUTTON && action == GLUT_UP && mode == EDIT_MODE)
	{
		int Vdeleted[10], i = 0;
		Vector2d temp;
		Vector2d empty;
		temp.x = xMouse;
		temp.y = winHeight - yMouse;		
		Vdeleted[i] = checkForSelectedPoint(true, temp, empty);
		while(Vdeleted[i] >= 0)
			Vdeleted[++i] = checkForSelectedPoint(false, temp, empty);		
		while(--i >= 0)
			removePoint(Vdeleted[i]);		
		redraw();
	}
}

void mouseMotionFcn(int x,int y)
{
	if (mode == ENTRY_MODE) 
	{
		vertices[pnum - 1].x = x;
		vertices[pnum - 1].y = winHeight - y;			
	}
	//if in Edit mode, find the selected point to move
	else
	{
		if(last > 0)
		{		
			Vector2d tmp = vertices[last];
			Vector2d empty;
			while(last >= 0)
			{
				vertices[last].x = x;
				vertices[last].y = winHeight - y;
				last = checkForSelectedPoint(false, tmp, empty);
			}

			Vector2d point(x, winHeight - y);			
			int i = checkForSelectedPoint(true, point, empty);
			if(i >= 0)
				last = i;		
		}		
	}
	redraw();	
}

void keyboardFcn(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 'q':
	case 'Q':
		exit(0);		
	default:
		break;
	}
}

void displayFcn()
{
	glClear (GL_COLOR_BUFFER_BIT);
	glColor3f(0 , 1, 0);
	glPointSize(10);
	redraw();
}

void winReshapeFcn (GLint newWidth, GLint newHeight)
{
	glViewport (0, 0, newWidth, newHeight);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ( );
	gluOrtho2D (0.0, GLdouble (newWidth), 0.0, GLdouble (newHeight));

	winWidth = newWidth;
	winHeight = newHeight;
	redraw();
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); 
	glutInitWindowPosition(250, 100); 
	glutInitWindowSize(winWidth, winHeight); 
	glutCreateWindow("Artman Polygon Editor"); 
	init(); 
	glutDisplayFunc(displayFcn);      
	glutMouseFunc(mouseFcn);
	glutMotionFunc(mouseMotionFcn);
	glutKeyboardFunc(keyboardFcn);
	glutMainLoop( );
}