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
	inline Point cross(Point p)
	{
		Point t(y * p.z - p.y * z, z * p.x - p.z * x, x * p.y - p.x * y);
		return t;		
	}
	GLfloat getNorm()
	{
		return sqrt(x * x + y * y + z * z);
	}
	GLfloat getSquareNorm()
	{
		return (x * x + y * y + z * z);
	}
	Point Normalize()
	{
		float n = getNorm();
		x /= n;
		y /= n;
		z /= n;
		return *this;
	}
};

class Shape
{
public:
	GLfloat diffRGB[3];
	GLfloat specRGB[3];

	Shape() {}
	Shape(GLfloat *Color, GLfloat *SpecColor)
	{		
		for(int i = 0; i < 3; i++)
		{
			diffRGB[i] = Color[i];
			specRGB[i] = SpecColor[i];
		}
	}
	Shape(GLfloat RColor, GLfloat GColor, GLfloat BColor, 
		GLfloat SpecRColor, GLfloat SpecGColor, GLfloat SpecBColor)
	{		
		diffRGB[0] = RColor;
		diffRGB[1] = GColor;
		diffRGB[2] = BColor;
		specRGB[0] = SpecRColor;
		specRGB[1] = SpecGColor;
		specRGB[2] = SpecBColor;

	}
	virtual Point getNormal(Point p)
	{
		Point n(0, 0, 1);
		return n;
	}
};

class Sphere: public Shape 
{
public:
	Point center;
	GLfloat radius;	

	Sphere() {}

	Sphere(Point Center, GLfloat Radius, GLfloat *Color, GLfloat *SpecColor):Shape(Color, SpecColor)
	{
		center = Center;
		radius = Radius;		
	}
	Sphere(GLfloat xCenter, GLfloat yCenter, GLfloat zCenter, GLfloat Radius, 
		GLfloat RColor, GLfloat GColor, GLfloat BColor,
		GLfloat SpecRColor, GLfloat SpecGColor, GLfloat SpecBColor)
		:Shape(RColor, GColor, BColor, SpecRColor, SpecGColor, SpecBColor)
	{
		center.x = xCenter;
		center.y = yCenter;
		center.z = zCenter;
		radius = Radius;			
	}

	Point getNormal(Point p)
	{
		Point n = (p - center).Normalize();
		return n;
	}
};

class Plane: public Shape
{
public:
	Point p0;
	Point normal;

	Plane() {}

	Plane(Point P0, Point Normal, GLfloat *Color, GLfloat *SpecColor):Shape(Color, SpecColor)
	{
		p0 = P0;
		normal = Normal.Normalize();		
	}
	Plane(Point P0, Point Normal, GLfloat RColor, GLfloat GColor, GLfloat BColor
		,GLfloat SpecRColor, GLfloat SpecGColor, GLfloat SpecBColor)
		:Shape(RColor, GColor, BColor, SpecRColor, SpecGColor, SpecBColor)
	{
		p0 = P0;
		normal = Normal;			
	}
	virtual Point getNormal(Point p)
	{
		return normal;
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
		r = dir.Normalize();
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
class Light
{
public:
	Point coordiante;
	Point direction;
	GLfloat angle;
	GLfloat radialAttenuationA0, radialAttenuationA1, radialAttenuationA2;
	GLfloat angularAttenuationExponent;
	GLfloat specularExponent;

	Light()	{}

	Light(GLfloat x, GLfloat y, GLfloat z, 
			GLfloat SExponent, GLfloat AExponent, GLfloat theta,
			GLfloat xdir, GLfloat ydir, GLfloat zdir, 
			GLfloat A0, GLfloat A1, GLfloat A2)
	{
		coordiante.x = x;
		coordiante.y = y;
		coordiante.z = z;
		direction.x = xdir;
		direction.y = ydir;
		direction.z = zdir;
		direction.Normalize();
		angle = theta;
		angularAttenuationExponent = AExponent;
		specularExponent = SExponent;
		radialAttenuationA0 = A0;
		radialAttenuationA1 = A1;
		radialAttenuationA2 = A2;
	}
};
/////////////////////////////////////////
//Frustrum
int width, height;
GLfloat backScreen, frontScreen;
const GLint xpixels = 512, ypixels = 512;
GLfloat angularDistance;

//Camera
Point viewPoint, cameraDir, viewUp;

//Light
Light light;
bool LightEnabled = false;

//Shapes
const int numOfSpheres = 6;
const int numOfPlanes = 4;
Sphere Spheres[numOfSpheres];
Plane Planes[numOfPlanes];

//Global
const float PI = 3.1415926f;
bool quadraticEquation = true;
bool drawSpheres = true;
bool drawPlanes = true;
bool reCompute = true;
GLfloat backgroundColor[3] = {0.04, 0.75, 0.96};
GLubyte pixmap[xpixels * ypixels * 3];
/////////////////////////////////////////
void setupCamera(GLfloat eyex, GLfloat eyey, GLfloat eyez,
				 GLfloat viewUpx = 0, GLfloat viewUpy = 1, GLfloat viewUpz = 0, 
				 GLfloat dirx = 0, GLfloat diry = 0, GLfloat dirz = 1)
{
	viewPoint.x = eyex;
	viewPoint.y = eyey;
	viewPoint.z = eyez;
	cameraDir.x = dirx;
	cameraDir.y = diry;
	cameraDir.z = -dirz;
	viewUp.x = viewUpx;
	viewUp.y = viewUpy;
	viewUp.z = viewUpz;
}

void setupFrustrum(GLfloat xmin, GLfloat xmax, GLfloat ymin, GLfloat ymax, 
				   GLfloat znear, GLfloat zfar, GLfloat frustrumHalfAngle = (PI / 12))
{
	//assume symmetric frustrum
	width = xmax - xmin;
	height = ymax - ymin;
	frontScreen = -znear;
	backScreen = -zfar;
	angularDistance = 2 * znear * tan(frustrumHalfAngle);
}

void setupLight(GLfloat x, GLfloat y, GLfloat z, 
				GLfloat SExponent, GLfloat AExponent, GLfloat theta = 180,
				GLfloat xdir = 0, GLfloat ydir = 0, GLfloat zdir = 1, 
				GLfloat A0 = 0, GLfloat A1 = 0, GLfloat A2 = 1)
{
	Light t(x, y, x, SExponent, AExponent, theta, xdir, ydir, zdir, A0, A1, A2);
	light = t;
	LightEnabled = true;
}
/////////////////////////////////////////
Sphere getSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat r, 
				 GLfloat cr, GLfloat cg, GLfloat cb, GLfloat scr, GLfloat scg, GLfloat scb)
{
	Sphere s(x, y, z, r, cr, cg, cb, scr, scg, scb);
	return s;
}

Plane getPlane(GLfloat x0, GLfloat y0, GLfloat z0, GLfloat nx, GLfloat ny, GLfloat nz,
			   GLfloat cr, GLfloat cg, GLfloat cb, GLfloat scr, GLfloat scg, GLfloat scb)
{
	Point p0(x0, y0, z0);
	Point normal(nx, ny, nz);
	Plane p(p0, normal, cr, cg, cb, scr, scg, scb);
	return p;
}
/////////////////////////////////////////
void createSpheres() 
{	 
	Spheres[0] = getSphere(1, 0, -5, 0.45, 0, 1, 0.25, 0.7, 0.7, 0.7);  
	Spheres[1] = getSphere(0.5, 1, -5.3, 0.4, 0, 0, 0, 0.7, 0.7, 0.7);
	Spheres[2] = getSphere(-1.2, 1.3, -5, 1, 1, 1, 0, 0.7, 0.7, 0.7);
	Spheres[3] = getSphere(1.25, 2, -5.7, 1.3, 1, 0, 0.5, 0.7, 0.7, 0.7);
	Spheres[4] = getSphere(-0.4, -0.5, -4.5, 0.6, 0.5, 0.25, 0.25, 0.7, 0.7, 0.7);
	Spheres[5] = getSphere(0, 0, -5.6, 1.2, 1, 0.5, 0, 0.7, 0.7, 0.7);
}

void createPlanes() 
{	 
	Planes[0] = getPlane(1, 0, -8, 1, 0, 0, 0.5, 0.5, 0.5, 0.7, 0.7, 0.7);
	Planes[1] = getPlane(-1, -0.5, -12, 0, 1, 0, 1, 0, 1, 0.7, 0.7, 0.7);
	Planes[2] = getPlane(0, 2, -6, 0, 0, 1, 0, 1, 1, 0.7, 0.7, 0.7);
	Planes[3] = getPlane(-0.5, 2.5, -3, 0.71, 0.71, 0, 1, 1, 1, 0.7, 0.7, 0.7);
}
/////////////////////////////////////////
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

//calculating plane intersection
bool planeIntersection(Ray *r, Plane p) 
{	
	if((r->r * p.normal) == 0)
		return false;

	GLfloat t = -(p.normal * (r->p0 - p.p0)) / (r->r * p.normal);
	r->t = t;
	r->getVector(); //calculate hit vector
	return t >= 0;  
}
/////////////////////////////////////////
bool shoot(Ray *ray, Shape *rshape) 
{  
	bool hit = false;	
	float distance = INT_MAX;
	float t = 0;

	if(drawSpheres)
	{
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
					 *rshape = tmp;
					 t = ray->t;
				}
			}
		}
	}
	
	if(drawPlanes)
	{
		for(int i = 0; i < numOfPlanes; i++)
		{
			Plane tmp = Planes[i];
			bool isHitting = planeIntersection(ray, tmp);
			if(isHitting)
			{
				hit = true;			
				float d = ray->getVector().getNorm();
				if(d < distance)
				{
					 distance = d;
					 *rshape = tmp;
					 t = ray->t;
				}
			}
		}
	}

	ray->t = t;
	ray->getVector();
	return hit;
}

void directShade(Shape hitShape, Ray hitRay, Point L, GLfloat *color)
{
	Point hitPoint = hitRay.getVector();		
	Point N = hitShape.getNormal(hitPoint);
	float NL = N * L;

	if(NL > 0)
	{
		//calculate specular effects
		double intensitySpec = 0.0;
		double intensityDiff = NL;
		Point V = hitRay.r;
		Point R = (L - (N * (2 * (NL)))).Normalize();
		double LR = L * R;	
		double VR = V * R;

		if (VR > 0.0) 
		   intensitySpec = pow(VR , (double)light.specularExponent);

		//calculate radial and angular attenuation
		float d = (light.coordiante - hitPoint).getNorm();
		float radatten;
		if(d >= 1)
		{
			light.radialAttenuationA0 = 0;
			light.radialAttenuationA1 = 1 / (d * d);
			light.radialAttenuationA0 = 1 / d;
			radatten = 15;
		}
		else
		{
			light.radialAttenuationA0 = d;
			light.radialAttenuationA1 = d * d * 100;
			light.radialAttenuationA0 = d * 10;
			radatten = 30;
		}		
		radatten /= (light.radialAttenuationA0 + light.radialAttenuationA1 * d + light.radialAttenuationA2 * d * d);
		Point vlight = light.direction;
		float OL = L * vlight;	//L = vobj
		float angatten = 0;
		if(OL >= cos(light.angle))
			angatten = pow(OL, light.angularAttenuationExponent);	
	
		//apply effects to colors
		for(int i = 0; i < 3; i++)
		{
			color[i] = intensityDiff * hitShape.diffRGB[i] + intensitySpec * hitShape.specRGB[i];
			color[i] *= angatten  * radatten;

			if(color[i] > 1)
				color[i] = 1;
			else if(color[i] < 0)
				color[i] = 0;
		}
	}
}

void shade(Shape hitShape, Ray hitRay, GLfloat *color)
{
	if(!LightEnabled)	
		for(int i = 0; i < 3; i++)
			color[i] = hitShape.diffRGB[i];	
	else
	{
		Point hitPoint = hitRay.getVector();
		Point ui = (light.coordiante - hitPoint).Normalize();
		Ray ri(hitPoint, ui, 0);	//ray from hit point to light	
		Shape s;

		for(int i = 0; i < 3; i++)
			color[i] = backgroundColor[i] * 0.1;				
		
		//if(!shoot(&ri, &s) || ri.t >= (light.coordiante - hitPoint).getNorm())	//in case light is visible		
			directShade(hitShape, hitRay, ui, color);
	}
}

void drawShapes()
{
	//Note that: uc = cameraDir and xc = viewPoint
	GLfloat zp = -frontScreen - viewPoint.z;
	Point x0 = viewPoint + (cameraDir * zp);
	Point ux = (cameraDir.cross(viewUp)).Normalize();	
	Point uy = cameraDir.cross(ux) * -1;

	for (int i = 0; i < width; i++) 
	{
		float cy =  (angularDistance / ypixels) * (i - ypixels / 2 + ((ypixels % 2 == 0)? 0: 0.5));
		for (int j = 0; j < height; j++) 
		{
			float cx = (angularDistance / xpixels) * (j - xpixels / 2 + ((xpixels % 2 == 0)? 0: 0.5));      	
			Point dir = (x0 + (ux * cx) + (uy * cy) - viewPoint).Normalize();
			Ray r(viewPoint, dir, 1);
			GLfloat c[3] = {backgroundColor[0], backgroundColor[1], backgroundColor[2]};
			Shape s;

			if (shoot(&r, &s))
				shade(s, r, c);		//shade(s, viewPoint + (cameraDir * r.t), c, r.r);
			
			if ((i >= 0) && (i < width) && (j >= 0) && (j < height) /*&& (r.getVector().z >= -backScreen)*/)
				for(int k = 0; k < 3; k++)
					pixmap[(width * i + j) * 3 + k] = (char)(c[k] * 255); 
		}
	}
}
/////////////////////////////////////////
void drawShapePixels()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glRasterPos3f(0, 0, 0);
  glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixmap);
}
  
void init()
{
	setupLight(-0.5, 0.5, -5, 80, 8, 120, 0, 0.1, 1);	
	setupFrustrum(-xpixels / 2, xpixels / 2, -ypixels / 2, ypixels / 2, 1, -1);
	setupCamera(0.0,0.0,0.05);	//viewPoint
	createSpheres();
	createPlanes();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 0);		
}

void winReshapeFcn(GLint newWidth, GLint newHeight)
{	
	reCompute = true;	
}

void displayFcn()
{
	if(quadraticEquation)
		glutSetWindowTitle("Artman Sphere-Plane Raycaster (Quadratic Equation)");
	else
		glutSetWindowTitle("Artman Sphere-Plane Raycaster (Vector Operation)");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(reCompute)
	{
		drawShapes();
		reCompute = false;
	}
	drawShapePixels();
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
	glutCreateWindow("Artman Sphere-Plane Raycaster (Quadratic Equation)"); 

	init();
	glutDisplayFunc(displayFcn);
	glutReshapeFunc(winReshapeFcn);
	glutKeyboardFunc(keyboardFcn);
	glutMainLoop();
}