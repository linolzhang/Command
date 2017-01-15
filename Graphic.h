#pragma once
#include "StdAfx.h"
#include "math.h"

#define PI 3.14159265359f

// 点调整的类型(相对对角的调整，相对于中心的调整）
enum adjust_mode { A_OPPOSITE,A_CENTER };

// 图元类型
enum graph_type { M_NULL, M_POINT, M_LINE, M_RECTANGLE, M_ELLIPSE, M_MULTIGRAPH };

// 点类型
enum point_type { LEFT_TOP, TOP_CENTER, RIGHT_TOP, RIGHT_MIDDLE, RIGHT_BOTTOM, BOTTOM_CENTER, LEFT_BOTTOM, LEFT_MIDDLE, CENTER, LEFT, RIGHT, ROTATE_POINT};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// -------------------------------------------------------------------
// 二维矩阵定义
typedef struct _Matrix2f
{
	float a11,a12;
	float a21,a22;
} Matrix2f;

// 二维矩阵乘
void Matrix2fMulMatrix2f(Matrix2f *m1,Matrix2f *m2);
// 矩阵乘向量
CPoint Matrix2fMulPoint(Matrix2f *m,CPoint m_point);
// 旋转
CPoint RotateByMatrix(Matrix2f *m, CPoint m_center,CPoint m_point);

// 旋转
CPoint RotateByAngle(float angle, CPoint m_center,CPoint m_point);

// -------------------------------------------------------------------
// 点到直线的投影，直线由前面两点确定
CPoint Projection(CPoint point1, CPoint point2, CPoint point3);
// 点到点的距离
float  Dist(CPoint point1, CPoint point2);
// 点到线段的最小距离,point1,point2表示线段端点
float Dist(CPoint point1, CPoint point2, CPoint m_point);
// 两点确定的直线的斜率
float InnerAngle(CPoint point1, CPoint point2);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 所有图元基类
class Graphic
{
protected:
	graph_type type;

public:
	graph_type GetType();

public:
	Graphic()
	{
		type = M_NULL;
	};

	virtual ~Graphic(){};

public:
	virtual float Distance(CPoint m_point){ return 0.0f;};
	virtual void Move(int x,int y){};
};

// ------------------------------------------------------
// 简单图元（点，线）
// 点定义
class MyPoint:public Graphic
{
private:
	int x;
	int y;

public:
	MyPoint(int p_x,int p_y)
	{
		type = M_POINT;
		x = p_x;
		y = p_y;
	};

	~MyPoint(){};

public:
	CPoint GetPoint();

public:
	virtual void Move(int x,int y);
	virtual float Distance(CPoint m_point);
};
// -------------------------------------
// 线段定义
class MyLine:public Graphic
{
private:
	POINT point1;
	POINT point2;

public:
	MyLine(CPoint p1,CPoint p2)
	{
		type = M_LINE;
		point1 = p1;
		point2 = p2;
	};
	MyLine(int x1,int y1,int x2, int y2)
	{
		type = M_LINE;
		point1.x  = x1; point1.y  = y1;
		point2.x  = x2; point2.y  = y2;
	};

	~MyLine(){};

public:
	CPoint GetPoint(point_type p_type);
	bool AdjustPoint(adjust_mode a_mode,point_type p_type,int x,int y);

public:
	virtual void Move(int x,int y);
	virtual float Distance(CPoint m_point);
};

//////////////////////////////////////////////////////////////////
// 基本图元（有中心，可旋转的）
class Shape:public Graphic
{
protected:
	float rAngle;

public:
	virtual void Rotate(float angle);

public:
	float GetAngle();
	Shape()
	{
		type = M_NULL;
	};
public:
	~Shape(){};
};

// -------------------------------------
// 定义矩形
class MyRectangle:public Shape
{
private:
	CPoint mCenter;
	int width;
	int height;

public:
	MyRectangle(CPoint bPoint,int width,int height)
	{
		type = M_RECTANGLE;
		rAngle = 0.0f;

		mCenter.x = bPoint.x + int(float(width)/2.0 + 0.5);
		mCenter.y = bPoint.y + int(float(height)/2.0 + 0.5);
		this->width = width;
		this->height = height;
	};
	MyRectangle(int x, int y,int width,int height)
	{
		type = M_RECTANGLE;
		rAngle = 0.0f;

		mCenter.x = x + int(float(width)/2.0 + 0.5);
		mCenter.y = y + int(float(height)/2.0 + 0.5);
		this->width = width;
		this->height = height;
	};

	~MyRectangle(){};

public:
	CPoint GetWorldPoint(point_type p_type);

	CPoint GetPoint(point_type p_type);
	bool AdjustPoint(adjust_mode a_mode,point_type p_type,int x,int y);

public:
	virtual void Move(int x,int y);
	virtual float Distance(CPoint m_point);

};

// -------------------------------------
// 定义椭圆
class MyEllipse:public Shape
{
private:
	CPoint mCenter;
	int xAxis;
	int yAxis;

public:
	MyEllipse(CPoint bPoint,int xAxis,int yAxis)
	{
		type = M_ELLIPSE;
		rAngle = 0.0f;

		mCenter.x = bPoint.x + xAxis;
		mCenter.y = bPoint.y + yAxis;

		this->xAxis = xAxis;
		this->yAxis = yAxis;
	};
	MyEllipse(int x, int y,int xAxis,int yAxis)
	{
		type = M_ELLIPSE;
		rAngle = 0.0f;

		mCenter.x = x + xAxis;
		mCenter.y = y + yAxis;

		this->xAxis = xAxis;
		this->yAxis = yAxis;
	};

	virtual ~MyEllipse(){};

public:
	CPoint GetWorldPoint(point_type p_type);

	CPoint GetPoint(point_type p_type);
	bool AdjustPoint(adjust_mode a_mode,point_type p_type,int x,int y);

public:
	virtual void Move(int x,int y);
	virtual float Distance(CPoint m_point);

};

//////////////////////////////////////////////////////////////////

// -------------------------------------
// 复合图元定义
class MultiGraph:public Graphic
{
private:
	CPoint mCenter;
	CList<Graphic *> child_List;

public:
	MultiGraph()
	{
		type = M_MULTIGRAPH;
	};

	~MultiGraph(){};

public:
	void AdjustCenter();
	virtual void Move(int x,int y);

public:
	Graphic * GetChild(int id);
	void InsertChild(Graphic * graph);
};
