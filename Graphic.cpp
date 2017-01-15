#include "StdAfx.h"
#include "Graphic.h"

//-------------------------------------------------------------------
// ��ά�����
void Matrix2fMulMatrix2f(Matrix2f *m1,Matrix2f *m2)
{
	Matrix2f temp;

	temp.a11 = m1->a11*m2->a11 + m1->a12*m2->a21;
	temp.a12 = m1->a11*m2->a12 + m1->a12*m2->a22;
	temp.a21 = m1->a21*m2->a11 + m1->a22*m2->a21;
	temp.a22 = m1->a21*m2->a12 + m1->a22*m2->a22;

	*m2 = temp;
}
// ���������
CPoint Matrix2fMulPoint(Matrix2f *m,CPoint m_point)
{
	CPoint m_result;
	m_result.x = int(m->a11*m_point.x + m->a12*m_point.y + 0.5);
	m_result.y = int(m->a21*m_point.x + m->a22*m_point.y + 0.5);
	return m_result;
}
// ��ת
CPoint RotateByMatrix(Matrix2f *m, CPoint m_center,CPoint m_point)
{
	CPoint m_result;
	m_result.x = m_point.x - m_center.x;
	m_result.y = m_point.y - m_center.y;

	m_result = Matrix2fMulPoint(m,m_result);

	m_result.x += m_center.x;
	m_result.y += m_center.y;

	return m_result;
}

// ��ת
CPoint RotateByAngle(float angle, CPoint m_center,CPoint m_point)
{
	Matrix2f m;
	m.a11 = cos(angle); m.a12 = -sin(angle);
	m.a21 = sin(angle); m.a22 = cos(angle);

	return RotateByMatrix(&m, m_center,m_point);
}

// -------------------------------------------------------------------
// �㵽��ľ���
float Dist(CPoint point1, CPoint point2)
{
	return float(sqrtf( (point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y) ));
}
// �㵽ֱ�ߵ�ͶӰ��ֱ����ǰ������ȷ��
CPoint Projection(CPoint point1, CPoint point2, CPoint point3)
{
	CPoint m_point;

	float dist = float((point2.x-point1.x)*(point3.x-point1.x) + (point2.y-point1.y)*(point3.y-point1.y))/Dist(point1,point2); // ͶӰ����

	// ͶӰ������ռ�ı���
	float l = dist/Dist(point1,point2);

	m_point.x = point1.x + l*(point2.x-point1.x);
	m_point.y = point1.y + l*(point2.y-point1.y);

	return m_point;
}
// �㵽�߶ε���С����,point1,point2��ʾ�߶ζ˵�
float Dist(CPoint point1, CPoint point2, CPoint m_point)
{
	CPoint p_Projection = Projection(point1, point2, m_point);

	float distance1 = Dist(point1, p_Projection);
	float distance2 = Dist(point2, p_Projection);
	//float distance3  = distance(p_Projection, m_point);

	float line_Length = Dist(point1, point2);

	if (distance1 <= line_Length && distance2 <= line_Length) // inner
		return Dist(p_Projection, m_point);
	else
	{
		float dist1 = Dist(m_point, point1);
		float dist2 = Dist(m_point, point2);

		return (float)min(dist1,dist2);
	}  
}
float InnerAngle(CPoint point1, CPoint point2)
{
	float inner = PI/2.0;

	if( point1.x != point2.x )
	{
		inner = atanf( double(point1.y-point2.y)/double(point1.x-point2.x) );
	}

	return inner;
}
//-------------------------------------------------------------------
// graphic
graph_type Graphic::GetType()
{
	return type;
}
//-------------------------------------------------------------------
// point
CPoint MyPoint::GetPoint()
{
	CPoint m_point;
	m_point.x = this->x;
	m_point.y = this->y;

	return m_point;
}
void MyPoint::Move(int x,int y)
{
	this->x += x;
	this->y += y;
}
float MyPoint::Distance(CPoint m_point)
{
	return Dist(m_point, this->GetPoint());
}

//-------------------------------------------------------------------
// line
// ��õ�
CPoint MyLine::GetPoint(point_type p_type)
{
	CPoint m_point;

	if(p_type == LEFT)
	{
		m_point.x = point1.x;
		m_point.y = point1.y;
	}
	if(p_type == RIGHT)
	{
		m_point.x = point2.x;
		m_point.y = point2.y;
	}

	return m_point;
}
bool MyLine::AdjustPoint(adjust_mode a_mode,point_type p_type,int x,int y)
{
	if(a_mode == A_OPPOSITE)
	{
		if(p_type == LEFT)
		{
			point1.x += x;
			point1.y += y;
		}
		else if(p_type == RIGHT)
		{
			point2.x += x;
			point2.y += y;
		}

	}
	else if(a_mode == A_CENTER) // ��������ĵ��϶�
	{
		if(p_type == LEFT)
		{
			point1.x += x;
			point1.y += y;
			point2.x -= x;
			point2.y -= y;
		}
		if(p_type == RIGHT)
		{
			point1.x -= x;
			point1.y -= y;
			point2.x += x;
			point2.y += y;
		}
	}

	return true;
}
void MyLine::Move(int x,int y)
{
	this->point1.x += x;
	this->point1.y += y;

	this->point2.x += x;
	this->point2.y += y;
}

float MyLine::Distance(CPoint m_point)
{
	return Dist(point1,point2 ,m_point);
}


//-------------------------------------------------------------------
// Shape
void Shape::Rotate(float angle)
{
	rAngle 
		+= angle;
	while(rAngle < 0)
		rAngle += 2*PI;
	while(rAngle >= 2*PI)
		rAngle -= 2*PI;
}
float Shape::GetAngle()
{
	return rAngle;
}
//-------------------------------------------------------------------
// Rectangle
// �õ���������
CPoint MyRectangle::GetWorldPoint(point_type p_type)
{
	POINT m_point;

	// �õ���������
	switch (p_type)
	{
		case LEFT_TOP:
			m_point.x = mCenter.x - width/2;
			m_point.y = mCenter.y - height/2;
			break;
		case TOP_CENTER:
			m_point.x = mCenter.x;
			m_point.y = mCenter.y - height/2;
			break;
		case RIGHT_TOP:
			m_point.x = mCenter.x + width/2;
			m_point.y = mCenter.y - height/2;
			break;
		case RIGHT_MIDDLE:
			m_point.x = mCenter.x + width/2;
			m_point.y = mCenter.y;
			break;
		case RIGHT_BOTTOM:
			m_point.x = mCenter.x + width/2;
			m_point.y = mCenter.y + height/2;
			break;
		case BOTTOM_CENTER:
			m_point.x = mCenter.x;
			m_point.y = mCenter.y + height/2;
			break;
		case LEFT_BOTTOM:
			m_point.x = mCenter.x - width/2;
			m_point.y = mCenter.y + height/2;
			break;
		case LEFT_MIDDLE:
			m_point.x = mCenter.x - width/2;
			m_point.y = mCenter.y;
			break;
		case CENTER:
			m_point.x = mCenter.x;
			m_point.y = mCenter.y;
			break;
		case ROTATE_POINT:
			m_point.x = mCenter.x;
			m_point.y = mCenter.y - height/2 - 20;
			break;
	}
	return m_point;
}

// �õ���������
CPoint MyRectangle::GetPoint(point_type p_type)
{
	CPoint m_point;

	m_point = GetWorldPoint(p_type);

	// ��������������������
	m_point = RotateByAngle(rAngle, mCenter,m_point);

	return m_point;
}

// ������λ�ã���ק
bool MyRectangle::AdjustPoint(adjust_mode a_mode,point_type p_type,int x,int y)
{
	if(a_mode != A_OPPOSITE && a_mode != A_CENTER)
		return false;

	CPoint m_point = GetPoint(p_type); // �õ���ǰ�������

	m_point.x += x;
	m_point.y += y;

	CPoint opposite_point;  // �õ���ǰ������ԭ��������

	switch (p_type)
	{
	case LEFT_TOP:
		opposite_point = GetPoint(RIGHT_BOTTOM); // �õ�����ĵ㣬���ڼ���
		break;
	case TOP_CENTER:
		opposite_point =GetPoint(BOTTOM_CENTER);
		break;
	case RIGHT_TOP:
		opposite_point = GetPoint(LEFT_BOTTOM);
		break;
	case RIGHT_MIDDLE:
		opposite_point = GetPoint(LEFT_MIDDLE);
		break;
	case RIGHT_BOTTOM:
		opposite_point = GetPoint(LEFT_TOP);
		break;
	case BOTTOM_CENTER:
		opposite_point = GetPoint(TOP_CENTER);
		break;
	case LEFT_BOTTOM:
		opposite_point = GetPoint(RIGHT_TOP);
		break;
	case LEFT_MIDDLE:
		opposite_point = GetPoint(RIGHT_MIDDLE);
		break;
	default:
		return false; // ���ǿɴ�������ͣ����ش���
		break;
	}

	if(a_mode == A_OPPOSITE)
	{
		// ������
		mCenter.x = (m_point.x + opposite_point.x)/2;
		mCenter.y = (m_point.y + opposite_point.y)/2;
	}

	// ���µ����ĵ�Ļ����ϣ���������������������
	m_point = RotateByAngle(-rAngle, mCenter,m_point);

	// �ɵ�ǰ��������������������
	switch (p_type)
	{
	case LEFT_TOP:
		width = 2*abs(mCenter.x - m_point.x );
		height = 2*abs(mCenter.y - m_point.y);
		break;
	case TOP_CENTER:
		height = 2*abs(mCenter.y - m_point.y);
		break;
	case RIGHT_TOP:
		width = 2*abs(m_point.x - mCenter.x);
		height = 2*abs(mCenter.y - m_point.y);
		break;
	case RIGHT_MIDDLE:
		width = 2*abs(m_point.x - mCenter.x);
		break;
	case RIGHT_BOTTOM:
		width = 2*abs(m_point.x - mCenter.x);
		height = 2*abs(m_point.y - mCenter.y);
		break;
	case BOTTOM_CENTER:
		height = 2*abs(m_point.y - mCenter.y);
		break;
	case LEFT_BOTTOM:
		width = 2*abs(mCenter.x - m_point.x);
		height = 2*abs(m_point.y - mCenter.y);
		break;
	case LEFT_MIDDLE:
		width = 2*abs(mCenter.x - m_point.x);
		break;
	default:
		return false; // ���ǿɴ�������ͣ����ش���
		break;
	}
		
	return true;
}
void MyRectangle::Move(int x,int y)
{
	mCenter.x += x;
	mCenter.y += y;
}

float MyRectangle::Distance(CPoint m_point)
{

	float dist1 = Dist(GetPoint(LEFT_TOP),GetPoint(RIGHT_TOP),m_point);
	float dist2 = Dist(GetPoint(LEFT_TOP),GetPoint(LEFT_BOTTOM),m_point);
	float dist3 = Dist(GetPoint(RIGHT_TOP),GetPoint(RIGHT_BOTTOM),m_point);
	float dist4 = Dist(GetPoint(LEFT_BOTTOM),GetPoint(RIGHT_BOTTOM),m_point);

	return min(min(dist1,dist2),min(dist3,dist4));
}

//-------------------------------------------------------------------
// Ellipse
CPoint MyEllipse::GetWorldPoint(point_type p_type)
{
	CPoint m_point;

	// �õ���������
	switch (p_type)
	{
	case LEFT_TOP:
		m_point.x = mCenter.x - xAxis;
		m_point.y = mCenter.y - yAxis;
		break;
	case TOP_CENTER:
		m_point.x = mCenter.x;
		m_point.y = mCenter.y - yAxis;
		break;
	case RIGHT_TOP:
		m_point.x = mCenter.x + xAxis;
		m_point.y = mCenter.y - yAxis;
		break;
	case RIGHT_MIDDLE:
		m_point.x = mCenter.x + xAxis;
		m_point.y = mCenter.y;
		break;
	case RIGHT_BOTTOM:
		m_point.x = mCenter.x + xAxis;
		m_point.y = mCenter.y + yAxis;
		break;
	case BOTTOM_CENTER:
		m_point.x = mCenter.x;
		m_point.y = mCenter.y + yAxis;
		break;
	case LEFT_BOTTOM:
		m_point.x = mCenter.x - xAxis;
		m_point.y = mCenter.y + yAxis;
		break;
	case LEFT_MIDDLE:
		m_point.x = mCenter.x - xAxis;
		m_point.y = mCenter.y;
		break;
	case CENTER:
		m_point.x = mCenter.x;
		m_point.y = mCenter.y;
		break;
	case ROTATE_POINT:
		m_point.x = mCenter.x;
		m_point.y = mCenter.y - yAxis - 20;
		break;
	}
	return m_point;
}
// �õ���������
CPoint MyEllipse::GetPoint(point_type p_type)
{
	CPoint m_point;

	m_point = GetWorldPoint(p_type);

	// ��������������������
	m_point = RotateByAngle(rAngle, mCenter,m_point);

	return m_point;
}
// ������λ�ã���ק
bool MyEllipse::AdjustPoint(adjust_mode a_mode,point_type p_type,int x,int y)
{
	if(a_mode != A_OPPOSITE && a_mode != A_CENTER)
		return false;

	CPoint m_point = GetPoint(p_type); // �õ���ǰ�������

	m_point.x += x;
	m_point.y += y;

	CPoint opposite_point;  // �õ���ǰ���������������

	switch (p_type)
	{
	case LEFT_TOP:
		opposite_point = GetPoint(RIGHT_BOTTOM); // �õ�����ĵ㣬���ڼ���
		break;
	case TOP_CENTER:
		opposite_point = GetPoint(BOTTOM_CENTER);
		break;
	case RIGHT_TOP:
		opposite_point = GetPoint(LEFT_BOTTOM);
		break;
	case RIGHT_MIDDLE:
		opposite_point = GetPoint(LEFT_MIDDLE);
		break;
	case RIGHT_BOTTOM:
		opposite_point = GetPoint(LEFT_TOP);
		break;
	case BOTTOM_CENTER:
		opposite_point = GetPoint(TOP_CENTER);
		break;
	case LEFT_BOTTOM:
		opposite_point = GetPoint(RIGHT_TOP);
		break;
	case LEFT_MIDDLE:
		opposite_point = GetPoint(RIGHT_MIDDLE);
		break;
	default:
		return false; // ���ǿɴ�������ͣ����ش���
		break;
	}

	if(a_mode == A_OPPOSITE)
	{
		// ������
		mCenter.x = (m_point.x + opposite_point.x)/2;
		mCenter.y = (m_point.y + opposite_point.y)/2;
	}
	// ��������������������
	m_point = RotateByAngle(-rAngle, mCenter,m_point);

	xAxis = abs(m_point.x - mCenter.x);
	yAxis = abs(m_point.y - mCenter.y);

	return true;
}
void  MyEllipse::Move(int x,int y)
{
	mCenter.x += x;
	mCenter.y += y;
}
float MyEllipse::Distance(CPoint m_point)
{
	// ��ת������������ϵ
	m_point = RotateByAngle(-rAngle, mCenter,m_point);

	m_point.x -= mCenter.x;
	m_point.y -= mCenter.y;

	return (float(m_point.x*m_point.x)/float(xAxis*xAxis) + float(m_point.y*m_point.y)/float(yAxis*yAxis) - 1);
	
}
//-------------------------------------------------------------------
// ����ͼԪ
Graphic * MultiGraph::GetChild(int id)
{
	POSITION pos = child_List.FindIndex(id);
	return child_List.GetAt(pos);
}
void MultiGraph::InsertChild(Graphic * graph)
{
	child_List.AddTail(graph);
}
void MultiGraph::AdjustCenter()
{
}
void MultiGraph::Move(int x,int y)
{
	mCenter.x += x;
	mCenter.y += y;
}