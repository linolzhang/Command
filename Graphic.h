#pragma once
#include "StdAfx.h"
#include "math.h"

#define PI 3.14159265359f

// �����������(��ԶԽǵĵ�������������ĵĵ�����
enum adjust_mode { A_OPPOSITE,A_CENTER };

// ͼԪ����
enum graph_type { M_NULL, M_POINT, M_LINE, M_RECTANGLE, M_ELLIPSE, M_MULTIGRAPH };

// ������
enum point_type { LEFT_TOP, TOP_CENTER, RIGHT_TOP, RIGHT_MIDDLE, RIGHT_BOTTOM, BOTTOM_CENTER, LEFT_BOTTOM, LEFT_MIDDLE, CENTER, LEFT, RIGHT, ROTATE_POINT};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// -------------------------------------------------------------------
// ��ά������
typedef struct _Matrix2f
{
	float a11,a12;
	float a21,a22;
} Matrix2f;

// ��ά�����
void Matrix2fMulMatrix2f(Matrix2f *m1,Matrix2f *m2);
// ���������
CPoint Matrix2fMulPoint(Matrix2f *m,CPoint m_point);
// ��ת
CPoint RotateByMatrix(Matrix2f *m, CPoint m_center,CPoint m_point);

// ��ת
CPoint RotateByAngle(float angle, CPoint m_center,CPoint m_point);

// -------------------------------------------------------------------
// �㵽ֱ�ߵ�ͶӰ��ֱ����ǰ������ȷ��
CPoint Projection(CPoint point1, CPoint point2, CPoint point3);
// �㵽��ľ���
float  Dist(CPoint point1, CPoint point2);
// �㵽�߶ε���С����,point1,point2��ʾ�߶ζ˵�
float Dist(CPoint point1, CPoint point2, CPoint m_point);
// ����ȷ����ֱ�ߵ�б��
float InnerAngle(CPoint point1, CPoint point2);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����ͼԪ����
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
// ��ͼԪ���㣬�ߣ�
// �㶨��
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
// �߶ζ���
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
// ����ͼԪ�������ģ�����ת�ģ�
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
// �������
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
// ������Բ
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
// ����ͼԪ����
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
