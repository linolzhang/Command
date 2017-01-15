#pragma once

#include <assert.h>
#include <deque>
#include <algorithm>

#include "Graphic.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////
// ----------------------------------------
// ��������(�޲������������޸ģ�ɾ���������
enum cmd_type { type_no, type_create, type_modify, type_delete, type_macro };

//////////////////////////////////////////////////////////////////////////////////////////
// ������ĳ�����
class Command
{
protected:
	cmd_type _type; // ��������
	bool  _destory; // �Ƿ�ɾ��������Ķ���

protected:
	Command();

public:
	virtual ~Command(void){}; // ������������������ɾ��֮ǰ�ȵ����������������

	virtual void Execute(void) = 0;
	virtual void Unexecute(void) = 0;

	cmd_type Type();
	void NeedDestory(bool bdestory);
};

//////////////////////////////////////////////////////////////////////////////////////////
// ----------------------------------------
// ������������������΢���������ϲ�����
class MacroCommand: public Command
{
private:
	deque<Command *> macro_list; // ����������΢�����б�����˫�����
	
public:
	MacroCommand();
	virtual ~MacroCommand();
public:
	virtual void Execute();
	virtual void Unexecute();

	void Insert(Command * cmd); // ��������
};

//////////////////////////////////////////////////////////////////////////////////////////
// ��������
// ���ڹ�����������,ʵ�ֳ���/����,ֻ����ø������undo/redo
class CommandManager
{
private:
	bool _record;     // �Ƿ���Ҫ���,����Ҫ���ʱ,�㽫_macrolist���б�������ϳ�һ��macro_cmd����,�ٲ���undo�б���
	int _maxcmd;      // ��������б�����
	bool _pause;      // ��¼�������ʱ,�Ƿ���Ҫ��ͣ��¼.�ڵ���record��stopʱ��״̬�Զ�ȡ��

	deque<Command *> _undolist;  // undo�б�,���ڱ�����Ҫ��������������
	deque<Command *> _redolist;  // redo�б�,���ڱ�����Ҫ��������������
	deque<Command *> _templist;  // Ԥ���������б�,������ʱ����(������Ϻ�����)

public: 
	CommandManager();
	virtual ~CommandManager();

public:

	bool CanUndo();
	bool CanRedo();

	void Undo();
	void Redo();

	void Do(Command* cmd);

	// ע��: begin��stop����һ��ʹ��,��ʹ��begin��־�����ʼ,��ʹ��stop��Ǻ��������
	void BeginRecord();   // ��ʼ��¼����
	void Stop();

	void SetPause(bool b_pause); // ������ͣ/��ʼ
	bool GetPause();

	bool Recording();  // _recordcount=0 ����false�����򷵻�true

protected:
	virtual void do_push() {};

	virtual void Push(Command* cmd); // ����һ�����������ڼ�¼���������û����ͣ������macro�б��������undo�б�
	Command* Pop(); // һ���ò���

private:
	// ��_templist�е�����������ϳ�һ��macro_cmd����
	void Composite();

	void ClearUndoList();
	void ClearRedoList();
	void ClearTempList(); // ����ʹ��
};

//////////////////////////////////////////////////////////////////////////////////////////
// create ����
// ��������
class CreateCommand: public Command
{
// ��¼����
private:
	Graphic *obj; // һ�δ���һ�����󣬴���֮ǰΪ��

public:
	CreateCommand(Graphic * obj);
	~CreateCommand(); // ���ܱ��̳е�����������д��virtual

public:
	virtual void Execute();
	virtual void Unexecute();
};
//////////////////////////////////////////////////////////////////////////////////////////
// delete ����
// ----------------------------------------
class DeleteCommand: public Command
{
// ��¼����
private:
	deque<Graphic *> obj_List; // ִ��ɾ�����б�

public:
	DeleteCommand();
	~DeleteCommand();

public:
	virtual void Execute();
	virtual void Unexecute();
};
//////////////////////////////////////////////////////////////////////////////////////////
// modify��move����
// ----------------------------------------
class MoveCommand: public Command
{
// ��¼����
private:
    int x;
    int y;

	deque<Graphic *> obj_List; // �޸ĵ��б���һ��ִ�е�ʱ��Ϊ�գ�ִ����ɱ���select�еĶ���

public:
	MoveCommand(int x,int y);
	~MoveCommand();

public:
	virtual void Execute();
	virtual void Unexecute();
};
//////////////////////////////////////////////////////////////////////////////////////////
// modify��rotate����
// ----------------------------------------
class RotateCommand: public Command
{
// ��¼����
private:
	float angle;
	deque<Graphic *> obj_List;

public:
	RotateCommand(float angle);
	~RotateCommand();

public:
	virtual void Execute();
	virtual void Unexecute();
};
//////////////////////////////////////////////////////////////////////////////////////////
// modify��setpoint����
// ----------------------------------------
class AdjustPointCommand: public Command
{
// ��¼����
private:
	adjust_mode a_mode; // �������ģʽ������ڶ��滹������
	point_type p_type; // ��¼���������ĸ���

	int x;
	int y;

	deque<Graphic *> obj_List;

public:
	AdjustPointCommand(adjust_mode a_mode,point_type p_type,int x,int y);
	~AdjustPointCommand();

public:
	virtual void Execute();
	virtual void Unexecute();
};