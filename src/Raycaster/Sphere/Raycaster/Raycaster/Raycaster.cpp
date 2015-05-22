//----Keyboard Controls----
//t - toggle between "quadratic equation" and "vector operation" for calculation 
//of sphere intersecting point

#include <math.h>
#include <cstdlib>
#include <iostream>
#include <GL/glut.h>

using namespace std;
/////////////////////////////////////////
class Point 
{
public:
	GLfloat x;
	GLfloat y;
	GLfloat z; 

	Point(GLfloat X, GLfloat Y, GLfloat Z)
	{
		x = X;
		y = Y;
		z = Z;
	}
	Point()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	inline Point operator-(Point p)
	{
		Point t(x - p.x, y - p.y, z - p.z);
		return t;
	}
	inline Point operator+(Point p)
	{
		Point t(x + p.x, y + p.y, z + p.z);
		return t;
	}
	inline Point operator*(GLfloat c)
	{
		Point t(x * c, y * c, z * c);
		return t;
	}
	inline GLfloat operator*(Point p)
	{
		return x * p.x + y * p.y + z * p.z;	
	}
	GLfloat getNorm()
	{
		return sqrt(x * x + y * y + z * z);
	}
	GLfloat getSquareNorm()
	{
		return (x * x + y * y + z * z);
	}
};

class Sphere 
{
public:
	Point center;
	GLfloat radius;
	GLfloat RGB[3];

	Sphere() {}

	Sphere(Point Center, GLfloat Radius, GLfloat *Color)
	{
		center = Center;
		radius = Radius;
		for(int i = 0; i < 3; i++)
			RGB[i] = Color[i];
	}
	Sphere(GLfloat xCenter, GLfloat yCenter, GLfloat zCenter, GLfloat Radius, 
		GLfloat RColor, GLfloat GColor, GLfloat BColor)
	{
		center.x = xCenter;
		center.y = yCenter;
		center.z = zCenter;
		radius = Radius;
		RGB[0] = RColor;
		RGB[1] = GColor;
		RGB[2] = BColor;		
	}
};

class Ray 
{
public:
	Point p0;
	Point r;	//unit direction vector
	GLfloat t;
	Point vect;
	Ray(Point P0, Point dir, GLfloat T)
	{
		p0 = P0;
		r = dir;
		t = T;
		vect = p0 + (r * t);
	}
	Ray() {}
	Point getVector()
	{
		vect = p0 + (r * t);
		return vect;
	}
};
/////////////////////////////////////////
int width, height;
GLfloat backScreen, frontScreen;
const GLint xpixels = 512, ypixels = 512;
Point viewPoint;
GLfloat backgroundColor[3] = {0.04, 0.75, 0.96};
const int numOfSpheres = 6;
const float PI = 3.1415926f;
Sphere Spheres[numOfSpheres];
GLubyte pixmap[xpixels * ypixels * 3];
GLfloat angularDistance; 
bool quadraticEquation = true;
/////////////////////////////////////////
void setupCamera(GLfloat eyex, GLfloat eyey, GLfloat eyez)
{
	viewPoint.x = eyex;
	viewPoint.y = eyey;
	viewPoint.z = eyez;
}

void setupFrustrum(GLfloat xmin, GLfloat xmax, GLfloat ymin, GLfloat ymax, 
				   GLfloat znear, GLfloat zfar, GLfloat frustrumHalfAngle = PI / 12)
{
	//assume symmetric frustrum
	width = xmax - xmin;
	height = ymax - ymin;
	frontScreen = -znear;
	backScreen = -zfar;
	angularDistance = 2 * znear * tan(frustrumHalfAngle);
}

void setupLight(GLfloat xdir, GLfloat ydir, GLfloat zdir, GLfloat cutoff, GLfloat exponent)
{
	GLfloat direction[] = {xdir, ydir, zdir};
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, cutoff);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, exponent);
	glShadeModel (GL_SMOOTH);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
}
/////////////////////////////////////////
Sphere getSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat cr, GLfloat cg, GLfloat cb)
{
	Sphere s(x, y, z, r, cr, cg, cb);
	return s;
}

//calculating sphere intersection using quadratic equation
bool sphereIntersectionQuadratic(Ray *r, Sphere s) 
{	
	Point p = r->p0 - s.center;
	Point v = r->r;
  
	double a = 1;	// v is normal and we dont't need v.getSquareNorm();
	double b = 2 * (v * p);
	double c = p.getSquareNorm() - s.radius * s.radius;
	double D = b * b - 4 * a * c;	//discriminant (Delta)
  
	if (D < 0) 
		return false; 

	D = sqrt(D);
	double t = (-b - D) / (2*a);   
	if (t < 0) 
		t = (-b + D) / (2*a);    
	r->t = t;
	r->getVector(); //calculate hit vector
	return t >= 0;  
}

//calculating sphere intersection using vector operations
bool sphereIntersectionVector(Ray *r, Sphere s) 
{	
	GLfloat t = r->r * (s.center - r->p0);
	Ray xclose(r->p0, r->r, t);
	GLfloat d = (xclose.getVector() - s.center).getNorm();

	if(d > s.radius)
		return false;

	if(d == s.radius)
		r->t = t;				
	else
	{
		GLfloat a = sqrt(s.radius * s.radius - d * d);
		if( t - a >= 0)
			t = t - a;
		else
			t = t + a;	
	}		   
	r->t = t;
	r->getVector(); //calculate hit vector
	return t >= 0;  
}

bool shoot(Ray *ray, Sphere *rsphere) 
{  
	bool hit = false;	
	float distance = INT_MAX;

	for(int i = 0; i < numOfSpheres; i++)
	{
		Sphere tmp = Spheres[i];
		bool isHitting = (quadraticEquation? sphereIntersectionQuadratic(ray, tmp): sphereIntersectionVector(ray, tmp));
		if(isHitting)
		{
			hit = true;			
			float d = ray->getVector().getNorm();
			if(d < distance)
			{
				 distance = d;
				 *rsphere = tmp;			  
			}
		}
	}
	return hit;
}

void shade(Sphere hitSphere, Point hitPoint, GLfloat *color)
{
	/*assume the shpere is monocolor, otherwise hitPoint determines 
	the hit point on the sphere to get the shading effects*/
	for(int i = 0; i < 3; i++)
		color[i] = hitSphere.RGB[i];
}

void createSpheres() 
{	 
	Spheres[0] = getSphere(1, 0, -8, 0.45, 0, 1, 0.25);  
	Spheres[1] = getSphere(0.5, 1, -9, 0.6, 0, 0, 0);
	Spheres[2] = getSphere(-1.2, 1.3, -10, 1, 1, 1, 0);
	Spheres[3] = getSphere(1.25, 2, -11, 0.5, 1, 0, 0.5);
	Spheres[4] = getSphere(-0.4, -0.5, -8.5, 0.9, 0.5, 0.25, 0.25);
	Spheres[5] = getSphere(0, 0, -16, 2, 1, 0.5, 0);
}

void drawSpheres()
{
	for (int i = 0; i < width; i++) 
	{
		float cy =  (angularDistance / ypixels) * (i - ypixels / 2 + ((ypixels % 2 == 0)? 0: 0.5));
		for (int j = 0; j < height; j++) 
		{
			float cx = (angularDistance / xpixels) * (j - xpixels / 2 + ((xpixels % 2 == 0)? 0: 0.5));      
			Point P1(cx, cy, frontScreen);
			Point dir = (P1 - viewPoint) * (1 / (P1 - viewPoint).getNorm());
			Ray r(viewPoint, dir, 1);
			GLfloat c[3] = {backgroundColor[0], backgroundColor[1], backgroundColor[2]};
			Sphere s;

			if (shoot(&r, &s))	  
				shade(s, r.getVector(),c);			
			
			if ((i >= 0) && (i < width) && (j >= 0) && (j < height) /*&& (r.getVector().z >= -backScreen)*/)
				for(int k = 0; k < 3; k++)
					pixmap[(width * i + j) * 3 + k] = (char)(c[k] * 255); 
		}
	}
}
/////////////////////////////////////////
void drawSpherePixels()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glRasterPos3f(0.0,0.0,0.0);
  glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixmap);
}
  
void init()
{
	setupLight(1, 0, 0, 30, 2.5);
	setupFrustrum(-xpixels / 2, xpixels / 2, -ypixels / 2, ypixels / 2, 1, -1);
	setupCamera(0.0,0.0,0.05);//viewPoint
	createSpheres();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 0);		
}

void winReshapeFcn(GLint newWidth, GLint newHeight)
{
	float originalaspect = (float)width / (float)height;
	float newaspect = (float)width / (float)height;
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
}

void displayFcn()
{
	if(quadraticEquation)
		glutSetWindowTitle("Artman Sphere Raycaster (Quadratic Equation)");
	else
		glutSetWindowTitle("Artman Sphere Raycaster (Vector Operation)");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawSpheres();
	drawSpherePixels();
	glFlush();
	cout<<"Done!"<<endl;
}

void keyboardFcn(unsigned char key, int x, int y)
{
	if(key == 't' || key == 'T')
		quadraticEquation = !quadraticEquation;
	glutPostRedisplay();
}
void main (int argc, char** argv) 
{	
	glutInit(&argc,argv);
	glutInitWindowSize(xpixels, ypixels);
	glutInitWindowPosition(256, 128);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutCreateWindow("Artman Sphere Raycaster (Quadratic Equation)"); 

	init();
	glutDisplayFunc(displayFcn);
	glutReshapeFunc(winReshapeFcn);
	glutKeyboardFunc(keyboardFcn);
	glutMainLoop();
}