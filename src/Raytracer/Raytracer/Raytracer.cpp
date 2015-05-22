//----Keyboard Controls----
//T - toggle between "quadratic equation" and "vector operation" for calculation of sphere intersecting point
//+(Plus Sign) - Zoom in
//-(Hyphen-Minus) - Zoom out
//*(Asterisk-Star) - Restet to actual size
//H - Flip horizontally
//V - Flip vertically
//R - Rotate 180 degree;
//Q or ESC - Exit

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

using namespace std;
////////////////////////////
//Tracing
const float MAX_ATTENUATION = 0.03;
const int MAX_RECURSION_LEVEL = 8;

//Frustrum
int width, height;
GLfloat backScreen, frontScreen;
GLint xpixels = 600, ypixels = 600;
GLfloat angularDistance;

//Global
const float PI = 3.1415926f;
GLfloat backgroundColor[3] = {0, 0, 0};
bool quadraticEquation = true;
GLfloat *pixmap;
////////////////////////////
class Color 
{
public:
	float rgb[3];
	Color() 
	{
		for(int i = 0; i < 3; i++)
			rgb[i] = 0;			
	}

	void validate()
	{
		for(int i = 0; i < 3; i++)
		{
			if(rgb[i] > 1)
				rgb[i] = 1;
			else if(rgb[i] < 0)
				rgb[i] = 0;
		}			
	}

	Color(float r, float g, float b)
	{
		rgb[0] = r;
		rgb[1] = g;
		rgb[2] = b;
	}
	
	void setColor(float r, float g, float b)
	{
		rgb[0] = r;
		rgb[1] = g;
		rgb[2] = b;
		validate();
	}		
	
	void addColor(const Color &c)
	{
		for(int i = 0; i < 3; i++)
			rgb[i] += c.rgb[i];			
		validate();
	}
		
	void multiplyColor(const Color &c)
	{
		for(int i = 0; i < 3; i++)
			rgb[i] *= c.rgb[i];	
		validate();
	}
};

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
	Point(const Point &src) 
	{
       x = src.x;
       y = src.y;
       z = src.z;    
	}
	Point()
	{
		x = 0;
		y = 0;
		z = 0;
	}	
	void setEntries(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}	
	inline Point operator-(const Point &p) const
	{
		Point t(x - p.x, y - p.y, z - p.z);
		return t;
	}
	inline Point operator+(const Point &p) const
	{
		Point t(x + p.x, y + p.y, z + p.z);
		return t;
	}
	inline Point operator*(GLfloat c)
	{
		Point t(x * c, y * c, z * c);
		return t;
	}
	inline GLfloat operator*(const Point &p) const
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
	float getDistanceFrom(Point p)
	{
		Point t = (*this) - p;
		return t.getNorm();
	}
	Point Normalize()
	{
		float n = getNorm();
		x /= n;
		y /= n;
		z /= n;
		return *this;
	}
	Point operator=(Point p)
	{
		x = p.x;
		y = p.y;
		z = p.z;

		return *this;
	}	
};

class Ray 
{
public:
	Point startPoint;
	Point direction;
	
	Ray() {}

	Ray(const Ray &ray)
	{
		startPoint = ray.startPoint;
		direction  = ray.direction;
	}
	
	Ray(const Point &p, const Point &d)
	{
		startPoint = p;
		direction  = d;
	}

	const Ray &operator=(const Ray &s)
	{
		startPoint = s.startPoint;
		direction  = s.direction;
		return *this;
	}
};
////////////////////////////
class PointLight
{
public:
	Point coordiante;
	GLfloat radialAttenuationA0, radialAttenuationA1, radialAttenuationA2;
	GLfloat intensity;
	Color color;
	bool enabled;
	
	PointLight()
	{
		enabled = false;
	}

	PointLight(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b,
			GLfloat Intensity = 1, GLfloat A0 = 0, GLfloat A1 = 0, GLfloat A2 = 1)
	{
		coordiante.x = x;
		coordiante.y = y;
		coordiante.z = z;	
		color.rgb[0] = r;
		color.rgb[1] = g;
		color.rgb[2] = b;
		intensity = Intensity;
		radialAttenuationA0 = A0;
		radialAttenuationA1 = A1;
		radialAttenuationA2 = A2;
		enabled = true;
	}

	void setEntries(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b,
		GLfloat Intensity = 1, GLfloat A0 = 0, GLfloat A1 = 0, GLfloat A2 = 1)
	{
		coordiante.x = x;
		coordiante.y = y;
		coordiante.z = z;	
		color.rgb[0] = r;
		color.rgb[1] = g;
		color.rgb[2] = b;
		intensity = Intensity;
		radialAttenuationA0 = A0;
		radialAttenuationA1 = A1;
		radialAttenuationA2 = A2;
		enabled = true;
	}
};

class DirectionalLight: public PointLight
{
public:
	Point coordiante;
	Point direction;
	GLfloat angle;
	GLfloat radialAttenuationA0, radialAttenuationA1, radialAttenuationA2;
	GLfloat angularAttenuationExponent;
	GLfloat specularExponent;

	DirectionalLight()	{}

	DirectionalLight(GLfloat x, GLfloat y, GLfloat z, GLfloat xdir, GLfloat ydir, GLfloat zdir,
		GLfloat r, GLfloat g, GLfloat b, GLfloat AExponent, GLfloat theta,
			GLfloat Intensity = 1, GLfloat A0 = 0, GLfloat A1 = 0, GLfloat A2 = 0)
			:PointLight(x, y, z, Intensity, A0, A1, A2)
	{		
		direction.x = xdir;
		direction.y = ydir;
		direction.z = zdir;
		direction.Normalize();
		angle = theta;
		angularAttenuationExponent = AExponent;		
	}
};
////////////////////////////
class Shape 
{
public:
	Point intersectionPoint;
	Point normal;
	Ray   *reflection;
	Ray   *ray;
	Color  reflective;	//needed when calculating non-direct reflections
	Color  diff;
	Color  spec;	//needed when calculating direct reflections
	double attenuation;
	double ambientFraction;
	double specularExponent;

	Shape()
	{
		reflection = NULL;		
		ray = NULL;
		attenuation = 0.001;
	}
	
	Shape(Color diffuse, Color specular, Color reflexive, 
		double AmbientFraction, double SpecularExponent)
	{		
		for(int i = 0; i < 3; i++)
		{
			diff.rgb[i] = diffuse.rgb[i];
			spec.rgb[i] = specular.rgb[i];
			reflective.rgb[i] = reflexive.rgb[i];
		}

		diff.validate();
		spec.validate();
		reflective.validate();

		ambientFraction = AmbientFraction;
		specularExponent = SpecularExponent;
		attenuation = 0.001;
		reflection = NULL;		
		ray = NULL;		
	}

	Shape(GLfloat RColor, GLfloat GColor, GLfloat BColor, 
		GLfloat SpecRColor, GLfloat SpecGColor, GLfloat SpecBColor,
		GLfloat ReflecRColor, GLfloat ReflecGColor, GLfloat ReflecBColor, 
		double AmbientFraction, double SpecularExponent)
	{		
		diff.rgb[0] = RColor;
		diff.rgb[1] = GColor;
		diff.rgb[2] = BColor;
		spec.rgb[0] = SpecRColor;
		spec.rgb[1] = SpecGColor;
		spec.rgb[2] = SpecBColor;
		reflective.rgb[0] = ReflecRColor;
		reflective.rgb[1] = ReflecGColor;
		reflective.rgb[2] = ReflecBColor;

		diff.validate();
		spec.validate();
		reflective.validate();

		ambientFraction = AmbientFraction;
		specularExponent = SpecularExponent;
		attenuation = 0.001;
		reflection = NULL;		
		ray = NULL;
	}

	void calculateReflection(const Ray &ray)
	{
		Point R = (ray.direction - normal * ((2.0 * (ray.direction * normal)))).Normalize();
		if (reflection == NULL)
			reflection = new Ray(intersectionPoint, R);
		else
		{
			reflection->startPoint = intersectionPoint;
			reflection->direction  = R;
		}
	}
	
	virtual bool intersection(Ray &ray)
	{
		return false;
	}
	
	virtual Color getMaterialColor(PointLight &light) 
	{
		return diff;
	}				
	
	virtual Color getReflectiveColor(Color percent)
	{
		Color output;
		for(int i = 0; i < 3; i++)
			output.rgb[i] = reflective.rgb[i] * percent.rgb[i];			
		return output;
	}	
};

class Sphere: public Shape 
{
public:
	Point center;
	float radius;
	
	Sphere() { }
	
	Sphere(Point Center, GLfloat Radius, Color diffuse, Color specular, Color reflexive,
			double AmbientFraction, double SpecularExponent)
		:Shape(diffuse, specular, reflexive, AmbientFraction, SpecularExponent)
	{
		center = Center;
		radius = Radius;		
	}

	Sphere(GLfloat xCenter, GLfloat yCenter, GLfloat zCenter, GLfloat Radius, 
		GLfloat RColor, GLfloat GColor, GLfloat BColor,
		GLfloat SpecRColor, GLfloat SpecGColor, GLfloat SpecBColor, 
		GLfloat ReflecRColor, GLfloat ReflecGColor, GLfloat ReflecBColor, 
		double AmbientFraction, double SpecularExponent)
		:Shape(RColor, GColor, BColor, SpecRColor, SpecGColor, SpecBColor, 
		ReflecRColor, ReflecGColor, ReflecBColor, AmbientFraction, SpecularExponent)
	{
		center.x = xCenter;
		center.y = yCenter;
		center.z = zCenter;
		radius = Radius;			
	}

	bool intersection(Ray &ray)
	{			
		ray.direction.Normalize();
		delete this->ray;
		this->ray = new Ray(ray);
		double t;

		if(quadraticEquation)
		{
			//calculating sphere intersection using quadratic equation	
			Point p = ray.startPoint - center;
				  
			double a = 1;	// v is normal and we dont't need v.getSquareNorm();
			double b = 2 * (ray.direction * p);
			double c = p.getSquareNorm() - radius * radius;
			double D = b * b - 4 * a * c;	//discriminant (Delta)
	  
			if (D < 0) 
				return false; 

			D = sqrt(D);
			t = (-b - D) / (2*a);   
			if (t < 0) 
				t = (-b + D) / (2*a);
		}
		else
		{
			//calculating sphere intersection using vector operations	
			t = ray.direction * (center - ray.startPoint);
			Point hitPoint = ray.startPoint + (ray.direction * t);
			GLfloat d = (hitPoint - center).getNorm();

			if(d > radius)
				return false;
				
			if(d != radius)
			{
				GLfloat a = sqrt(radius * radius - d * d);
				if( t - a >= 0)
					t = t - a;
				else
					t = t + a;	
			}
		}

		if(t < 0)
			return false;
		
		intersectionPoint = ray.startPoint + (ray.direction * t);
		normal = (intersectionPoint - center).Normalize();
		attenuation = 0.001 * fabs(ray.direction * normal);
		calculateReflection(ray);	
		return true;		
	}	

	Color getMaterialColor(PointLight &light)  
	{		
		Color  color;
		Point L = light.coordiante - intersectionPoint;
		float d = L.getNorm();
		L.Normalize();		
		double intensityDiff = L * normal;
		double intensityRef =0.0;			
		Point V = ray->direction;		
		Point R = (L - (normal * (2 * (intensityDiff)))).Normalize();
		double VR = V * R;		

		if(intensityDiff > 0)
		{	
			float radatten;
			if(d >= 1)
			{
				light.radialAttenuationA0 = 1;
				light.radialAttenuationA1 = 1;
				light.radialAttenuationA2 = 1;
				radatten = 15;
			}
			else
			{
				light.radialAttenuationA0 = d;
				light.radialAttenuationA1 = 10;
				light.radialAttenuationA2 = 100;
				radatten = 30;
			}		
			radatten /= (light.radialAttenuationA0 + light.radialAttenuationA1 * d + light.radialAttenuationA2 * d * d);			
			intensityDiff *= light.intensity;
			intensityDiff += ambientFraction;	//ambient color is the same as diffuse color bu multiplied by ambient fraction

			if (VR > 0.0) 
				intensityRef = pow(VR, specularExponent);

			color.setColor((intensityDiff * diff.rgb[0] + intensityRef * spec.rgb[0] * light.color.rgb[0]) * radatten,
		               (intensityDiff * diff.rgb[1] + intensityRef * spec.rgb[1] * light.color.rgb[1]) * radatten,
		               (intensityDiff * diff.rgb[2] + intensityRef * spec.rgb[2] * light.color.rgb[2]) * radatten);
		}
		return color;
	}
};

class Plane: public Shape 
{
public:
	Point p0;

	Plane()
	{
		p0.setEntries(0, 0, 0);
		normal.setEntries(0, 0, 1);
	}

	Plane(Point P0, Point Normal)
	{
		p0 = P0;
		normal = Normal.Normalize();
	}

	Plane(Point P0, Point Normal, Color diffuse, Color specular, Color reflexive
		, double AmbientFraction, double SpecularExponent)
		:Shape(diffuse, specular, reflexive, AmbientFraction, SpecularExponent)
	{
		p0 = P0;
		normal = Normal.Normalize();		
	}

	Plane(Point P0, Point Normal, GLfloat RColor, GLfloat GColor, GLfloat BColor,
		GLfloat SpecRColor, GLfloat SpecGColor, GLfloat SpecBColor, 
		GLfloat ReflecRColor, GLfloat ReflecGColor, GLfloat ReflecBColor
		, double AmbientFraction, double SpecularExponent)
		:Shape(RColor, GColor, BColor, SpecRColor, SpecGColor, SpecBColor, 
		ReflecRColor, ReflecGColor, ReflecBColor, AmbientFraction, SpecularExponent)
	{
		p0 = P0;
		normal = Normal;			
	}

	bool intersection(Ray &ray) 
	{
		this->ray = &ray;

		if((ray.direction * normal) == 0)
			return false;

		float t = -(normal * (ray.startPoint - p0)) / (ray.direction * normal);		
			
		if(t < 0)
			return false;

		intersectionPoint = ray.startPoint + (ray.direction * t);
		return true;		
	}	
};

class ChessBoard: public Plane
{
public:
	ChessBoard():Plane() {}
	ChessBoard(Point p0, Point Normal): Plane(p0, Normal) {}	
	
	Color getMaterialColor(PointLight &light) 
	{
		Color  color;
		Point L = (light.coordiante - intersectionPoint).Normalize();	
		float NL = normal * L;
	
		if(NL > 0)
		{
			double x = intersectionPoint.x;
			double y = intersectionPoint.y;
			double f;
			if (x < 0)
				x = fabs(x + 1.0);
			if (y < 0)
				y = fabs(y + 1.0);
			x  = fabs(8 * x);
			y  = fabs(8 * y);
			
			//making background color or white squares
			f  = (float)(2.0f * (double)(((int)x % 2) ^ ((int)y % 2)));			
			f  = f * NL;
			color.setColor(f,f,f);
		}
		return color;
	}
};
////////////////////////////
//Camera
Point viewPoint, cameraDir, viewUp;

//Light
const int numOfLights = 3;
PointLight lights[numOfLights];

//Shapes
const int maximumSpahes = 10;
int numOfShapes = 0;
Shape *shapes[maximumSpahes];
////////////////////////////
void setupLights()
{
	lights[0].setEntries(2.0, 2.0, 1.5, 1, 0.85, 0);	//near yellow light with full intensity
	lights[1].setEntries(1.0, 1.0, 5.5, 1, 1, 1);	//far white light with full intensity
	lights[2].setEntries(-0.5, 0, 1.75, 0, 0, 1, 0.25);	//blue light with quarter intensity
}

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

void makeProperView()
{
	frontScreen = (viewPoint.z + 10) / cameraDir.z - viewPoint.z;
	angularDistance = 2;
}
////////////////////////////
void addShapeToScene(Shape *s)
{
	shapes[numOfShapes++] = s;
}
////////////////////////////	
Color RayTracer(Ray *ray, Shape *last, int level, float attenuation)
{
	double distance = INT_MAX;
	Color  color;
	Ray *reflect = NULL;
	
	if (ray != NULL && level < MAX_RECURSION_LEVEL && attenuation < MAX_ATTENUATION)
	{
		for (int i = 0; i < numOfShapes; i++) 
		{
			Shape *s = shapes[i];
			if (s != last && s->intersection(*ray))
			{
				double dist = ray->startPoint.getDistanceFrom(s->intersectionPoint);
				if (distance > dist)
				{
					distance = dist;
					color.setColor(backgroundColor[0], backgroundColor[1], backgroundColor[2]);

					for(int k = 0; k < numOfLights; k++)
					{
						PointLight light = lights[k];
						if(!light.enabled)
							continue;

						Point L =  light.coordiante - s->intersectionPoint ;
						float lightDistance = L.getNorm();
						L.Normalize();
						Ray lightRay(s->intersectionPoint, L);
						bool lightVisible = true;						
						for (int j = 0; j < numOfShapes; j++) 
						{
							Shape *tmp = shapes[j];
							if(tmp != s && tmp->intersection(lightRay) 
								&& lightRay.startPoint.getDistanceFrom(tmp->intersectionPoint) < lightDistance)
							{
								lightVisible = false;
								break;
							}								
						}							
						if(lightVisible)							
							color.addColor(s->getMaterialColor(light));		
					}

					if (s->reflection != NULL)
					{
						reflect = new Ray(*s->reflection);
						color.addColor(s->getReflectiveColor(RayTracer(reflect, s, level + 1, attenuation + s->attenuation )));
						delete reflect;
					}						
				}
			}
		}
	}
	return color;
}
////////////////////////////
void drawPixels()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glRasterPos3f(0, 0, 0);
	glDrawPixels(ypixels, xpixels, GL_RGB, GL_FLOAT, pixmap);
}

void drawShapes()
{
	numOfShapes = 0;
	delete []pixmap;
	pixmap = new GLfloat[ypixels * xpixels * 3];

	ChessBoard chessBoard;	
	Sphere sphere1(-0.1, 0.3, 0.45, 0.30, 0.8, 0.0, 0.0, 1.0, 1.0, 1.0, 0.9, 0.8, 0.8, 0, 100);
	Sphere sphere2(-0.55, -0.1, 0.2, 0.25, 0.25, 0.8, 0.25, 1.0, 0.85, 1.0, 0.6, 0.75, 0.6, 0.2, 70);
	Sphere sphere3(-0.6, 0.6, 0.4, 0.35, 0.1, 0.1, 0.1, 1.0, 1.0, 1.0, 0.9, 0.9, 0.9, 0.01, 15);
	Sphere sphere4(0.6, -0.25, 0.4, 0.36, 0.0, 0.3, 1.0, 1.0, 1.0, 1.0, 0.9, 0.9, 0.9, 0.3, 60);	
	Sphere sphere5(0.6, 0.6, 0.35, 0.28, 0.5, 0.5, 0, 0.7, 0.7, 0, 0.7, 0.7, 0.7, 0.06, 200);
	Sphere sphere6(-0.15, -0.70, 0.3, 0.45, 0.5, 0, 1, 1.0, 1.0, 1.0, 0.9, 0.9, 0.9, 0.5, 5);	

	addShapeToScene(&chessBoard);
	addShapeToScene(&sphere1);
	addShapeToScene(&sphere2);
	addShapeToScene(&sphere3);
	addShapeToScene(&sphere4);
	addShapeToScene(&sphere5);
	addShapeToScene(&sphere6);
		
	GLfloat zp = -frontScreen - viewPoint.z;
	Point x0 = viewPoint + (cameraDir * zp);
	Point ux = (cameraDir.cross(viewUp)).Normalize();	
	Point uy = cameraDir.cross(ux) * -1;

	for (int i = 0; i < height; i++) 
	{			
		float cy =  (angularDistance / ypixels) * (i - ypixels / 2 + ((ypixels % 2 == 0)? 0: 0.5));
		for (int j = 0; j < width; j++) 
		{
			float cx = (angularDistance / xpixels) * (j - xpixels / 2 + ((xpixels % 2 == 0)? 0: 0.5));      	
			Point dir = (x0 + (ux * cx) + (uy * cy) - viewPoint).Normalize();
			Ray ray(viewPoint, dir);		
			Color color = RayTracer(&ray, NULL, 0, 0);			
			for(int k = 0; k < 3; k++)
				pixmap[(width * i + j) * 3 + k] = color.rgb[k]; 
		}
	}

	drawPixels();
}
////////////////////////////
void init()
{
	glClearColor(0.04, 0.75, 0.96, 0);
	setupLights();
	setupFrustrum(-xpixels / 2, xpixels / 2, -ypixels / 2, ypixels / 2, 50, -1, 0.02);	
	setupCamera(0.0, 0.0, 50);	//viewPoint
	makeProperView();
}

void displayFcn() 
{
	if(quadraticEquation)
		glutSetWindowTitle("Artman Raytracer (Quadratic Equation) - Keyboard Control(-+*TRHVQ)");
	else
		glutSetWindowTitle("Artman Raytracer (Vector Operation) - Keyboard Control(-+*TRHVQ)");

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glClearColor(0.04, 0.75, 0.96, 0);
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	
	drawShapes();
	glFlush();
	cout<<"Done!"<<endl;
}

void reshapeFcn(int newWidth, int newHeight)
{
	int min = newWidth < newHeight? newWidth: newHeight;
	if (min % 2 != 0)
		++min;
	xpixels = min;
	ypixels = min;
	init();
}

void keyboardFcn(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 't':
	case 'T':
		quadraticEquation = !quadraticEquation;
		glutPostRedisplay();
		break;
	case 43:
		angularDistance -= 0.5;
		glutPostRedisplay();
		break;
	case 45:
		angularDistance += 0.5;
		glutPostRedisplay();
		break;
	case 42:
		angularDistance = 2;
		glutPostRedisplay();
		break;
	case 'h':
	case 'H':
		cameraDir.z *= -1;
		frontScreen = (viewPoint.z + 10) / cameraDir.z - viewPoint.z;		
		glutPostRedisplay();
		break;
	case 'r':
	case 'R':		
		viewUp.y *= -1;
		glutPostRedisplay();
		break;
	case 'v': 
	case 'V':
		cameraDir.z *= -1;	
		viewUp.y *= -1;
		frontScreen = (viewPoint.z + 10) / cameraDir.z - viewPoint.z;
		glutPostRedisplay();
		break;
	case 27:
	case 'q':
	case 'Q':
		delete []pixmap;
		exit(0);
		break;
	}	
}
////////////////////////////
void main(int argc, char* argv[]){

	glutInit(&argc,argv);
	glutInitWindowSize(xpixels, ypixels);
	glutInitWindowPosition(212, 84);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutCreateWindow("Artman Raytracer (Quadratic Equation) - Keyboard Control(-+*TRHVQ)"); 

	init();
	glutDisplayFunc(displayFcn);
	glutReshapeFunc(reshapeFcn);
	glutKeyboardFunc(keyboardFcn);
	glutMainLoop();	

	delete []pixmap;
}