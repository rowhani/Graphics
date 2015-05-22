//Mouse Key Description
//1) middle button:	change curve drawing mode to Cardinal or Bezier
//2) Entry Mode;	left button: insert a point, right button: change to edit mode, 
//mouse motion with left button down: change the inserted point coordinates
//3) Edit Mode;	left button on an edge: insert a point, right button on a point: delete the point,
//mouse motion with left button down on a ponit: change the selected point coordinates
//Keyboard Key Description
//Q or ESC: quit

#include <cstdlib>
#include <GL/glut.h>
#include "Vector.h"

#define PNUM  1000
#define ENTRY_MODE 0
#define EDIT_MODE 1
#define HIT_RADIUS 5

GLsizei winWidth = 500, winHeight = 500;
Vector2d vertices[PNUM];
int pnum = 0, selIndex = 0, last = 0, mode = ENTRY_MODE, mouseButten;
bool Bezier = false;
const int CURVE_POINTS = 1000;

void init()
{
	glClearColor(.32, .23, .68, 1); 
	glMatrixMode(GL_PROJECTION); 
	gluOrtho2D(0.0, winWidth, 0.0, winHeight);
	glMatrixMode(GL_MODELVIEW);
}

void insertPoint(int inx, Vector2d p)
{
	int i;
	for(i = pnum ;i > inx + 1; i--)
		vertices[i] = vertices[i - 1];
	vertices[i] = p;
	++pnum;
}

void insertPoint(Vector2d p)
{
	vertices[pnum] = p;
	++pnum;
}

void removePoint(int inx)
{
	for(int i = inx; i < pnum - 1; i++)
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
		if((mpoint - vertices[selIndex]).normsqr() < HIT_RADIUS * 4)
			return selIndex;		
	}	

	Vector2d a, b, c;
	for(i = 0; i < pnum; i++)
	{
		a = vertices[(i + 1) % pnum] - vertices[i];		
		b = mop - vertices[i];	
		c = ((a.normalize()) * b) * a.normalize() - b;	//c is the projection of component b in the oppisite perpendicular direction of a
		if(c.norm() < HIT_RADIUS)
			return i;
	}	
	return -1;
}

void drawPoint(int x, int y, int r)
{
	double inr = 2 * PI / 16;
	double theta;
	glBegin(GL_POLYGON);
	for(theta = 0.0; theta < 2 * PI; theta += inr)
		glVertex2f(x + r * cos(theta), y + r * sin(theta));
	glEnd();
}

void drawPolygon()
{
	//draw points and lines
	glColor3f(0.8, 0.1, 0.1);
	for(int i = 0; i < pnum; i++)
		drawPoint(vertices[i].x, vertices[i].y, HIT_RADIUS);

	glColor3f(1, 1, 0.5);	
	glBegin(mode == ENTRY_MODE? GL_LINE_STRIP: GL_LINE_LOOP);	
	for(int i = 0; i < pnum; i++)
		glVertex2i(vertices[i].x, vertices[i].y);
	glEnd();
	glFlush();
}

//draw the curve using Bezier method
void drawCurveBezier()
{
	if(pnum < 2)
		return;

	glColor3f(0, 1, 1);
	if(mode == EDIT_MODE)
	{
		vertices[pnum] = vertices[0];
		++pnum;
	}

	GLint *c = new GLint[pnum];
	int n = pnum - 1;
	for(int k = 0; k <= n; k++)
	{
		c[k] = 1;
		for(int j = n; j >= k + 1; j--)
			c[k] *= j;
		for (int j = n - k; j >= 2; j--)
			c[k] /= j;
	}
	for(float j = 0; j <= CURVE_POINTS; j++)
	{
		float u = j / CURVE_POINTS;
		Vector2d bezPt;
		for(int k = 0; k < pnum; k++)
		{			
			float bezBlendFCN = c[k] * pow(u, k) * pow(1 - u, n - k);
			bezPt.x += vertices[k].x * bezBlendFCN;
			bezPt.y += vertices[k].y * bezBlendFCN;
		}
		drawPoint(bezPt.x, bezPt.y, 1);
	}
	if(mode == EDIT_MODE)	
		--pnum;
	
	delete []c;
}

//draw the curve using cardinal method
void drawCurveCardinal()
{
	Vector2d *pt;
	float s = 0.5, u, u3, u2;
	Vector2d p, pk, pl, pm, pn;

	int i, n = pnum - 1;
	if(mode == EDIT_MODE)
		n++;
	glColor3f(0, 1, 1);	

	if(pnum >= 2)		
	{		
		pt = &vertices[0];
		pl.set(pt->x, pt->y);
		pk = pl;
		pt = &vertices[1];
		pm.set(pt->x,pt->y);		
		
		for( i = 1 ;i <= n; i++)
		{		
			pt = &vertices[i + 1];
			if(pt->x == 0 && pt->y == 0)
			{	
				if(mode == EDIT_MODE)
				{	
					pn.set(vertices[0].x,vertices[0].y);
					pt = &vertices[0];
				}
				else 
				 pn = pm;
			}
			else
				 pn.set(pt->x, pt->y);

			float incr = 1 / (float)CURVE_POINTS;					
			for(u = incr; u <= 1; u += incr)
			{
				u3 = u * u * u;
				u2 = u * u;
				p = pk * (-s * u3 + 2 * s * u2 - s * u) + pl * ((2 - s) * u3 + (s - 3)* u2 + 1) + pm * ((s - 2) * u3 + (3 - 2 * s) * u2 + s * u) + pn * (s * u3 - s * u2);
				drawPoint(p.x, p.y, 1);							
			}
			pk = pl;
			pl = pm;
			pm = pn;
		}
	 }
}

void redraw()
{	
	glClear (GL_COLOR_BUFFER_BIT); 
	drawPolygon();
	if(Bezier)
	{
		glutSetWindowTitle("Artman Spline Curve (Bezier Method)");
		drawCurveBezier();
	}
	else
	{
		glutSetWindowTitle("Artman Spline Curve (Cardinal Method)");
		drawCurveCardinal();
	}	
	glutSwapBuffers();
	glFlush ( );    
}

void mouseFcn(GLint button, GLint action, GLint xMouse, GLint yMouse)
{
	int i;
	int start, end;
	mouseButten = button;
	//if left button is pressed in Entry mode, draw a point and a line to last point
	if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN && mode == ENTRY_MODE ) 
	{
		vertices[pnum].x = xMouse;
		vertices[pnum].y = winHeight - yMouse;		
		++pnum;
		redraw();
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
			drawPoint(vertices[pnum - 1].x, vertices[pnum - 1].y, HIT_RADIUS);
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
		{
			glBegin(GL_LINES);
				glVertex2d(vertices[start].x, vertices[start].y);
				glVertex2d(vertices[end].x, vertices[end].y);
			glEnd();
		}
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
		mode = EDIT_MODE;		
		last = -1;
		redraw();
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
	else if(button == GLUT_MIDDLE_BUTTON && action == GLUT_UP)
	{
		Bezier = !Bezier;
		redraw();
	}
}

void mouseMotionFcn(int x,int y)
{
	if(mouseButten != GLUT_LEFT_BUTTON)
		return;

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
	case 27:
		exit(0);		
	default:
		break;
	}
}

void displayFcn()
{
	glClear (GL_COLOR_BUFFER_BIT);
	glColor3f(0 , 1, 0);
	glPointSize(HIT_RADIUS);
	glLineWidth(3);
	redraw();
}

void winReshapeFcn (GLint newWidth, GLint newHeight)
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

void main(int argc, char** argv)
{
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); 
	glutInitWindowPosition(250, 100); 
	glutInitWindowSize(winWidth, winHeight); 
	glutCreateWindow("Artman Spline Curve (Cardinal Method)"); 
	init(); 
	glutDisplayFunc(displayFcn);    
	glutReshapeFunc(winReshapeFcn);
	glutMouseFunc(mouseFcn);
	glutMotionFunc(mouseMotionFcn);
	glutKeyboardFunc(keyboardFcn);
	glutMainLoop( );
}