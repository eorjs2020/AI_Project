#pragma once
#ifndef _MATHMANAGER_H_
#define _MATHMANAGER_H_

#include <algorithm>
#include <cmath>
#include "SDL.h"

class MathManager
{
public: // Public methods.
	static double Distance(const double& x1, const double& x2, const double& y1, const double& y2);
	static double Distance(const int& x1, const int& x2, const int& y1, const int& y2);
	static double AngleBetweenPoints(const double dy, const double dx);
	static void SetDeltas(const double angle, double& dx, double& dy, double fx = 1.0, double fy = 1.0);
	static double SetDeltaX(const double angle, double fx = 1.0);
	static double SetDeltaY(const double angle, double fy = 1.0);
	static double Deg2Rad(const double angle);
	static double Rad2Deg(const double angle);
	static SDL_Rect ConvertFRect2Rect(const SDL_FRect& r);
	static double Angle180(double a);
	static double Angle360(double a);
	static double LerpD(double first, double second, double perc);
	static double LerpRad(double a, double b, double factor);
	static SDL_Point HalfwayPoint(const SDL_Point& from, const SDL_Point& to);

private: // Private properties.
	MathManager() {}
};

typedef MathManager MAMA;

#endif