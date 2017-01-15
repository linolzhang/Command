#pragma once

#include <assert.h>
#include <deque>
#include <algorithm>

#include "Graphic.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////
// ----------------------------------------
// 命令类型(无操作，创建，修改，删除，宏命令）
enum cmd_type { type_no, type_create, type_modify, type_delete, type_macro };

//////////////////////////////////////////////////////////////////////////////////////////
// 命令类的抽象类
class Command
{
protected:
	cmd_type _type; // 命令类型
	bool  _destory; // 是否删除命令里的对象

protected:
	Command();

public:
	virtual ~Command(void){}; // 虚拟析构函数，对象删除之前先调用子类的析构函数

	virtual void Execute(void) = 0;
	virtual void Unexecute(void) = 0;

	cmd_type Type();
	void NeedDestory(bool bdestory);
};

//////////////////////////////////////////////////////////////////////////////////////////
// ----------------------------------------
// 宏命令，包含若干子命令（微命令，或者组合操作）
class MacroCommand: public Command
{
private:
	deque<Command *> macro_list; // 定义宏命令的微命令列表，采用双向队列
	
public:
	MacroCommand();
	virtual ~MacroCommand();
public:
	virtual void Execute();
	virtual void Unexecute();

	void Insert(Command * cmd); // 插入命令
};

//////////////////////////////////////////////////////////////////////////////////////////
// 命令中心
// 用于管理所有命令,实现撤消/重做,只需调用该类里的undo/redo
class CommandManager
{
private:
	bool _record;     // 是否需要组合,当需要组合时,便将_macrolist中列表命令组合成一个macro_cmd命令,再插入undo列表中
	int _maxcmd;      // 最大命令列表数量
	bool _pause;      // 记录组合命令时,是否需要暂停记录.在调用record或stop时该状态自动取消

	deque<Command *> _undolist;  // undo列表,用于保存需要撤消的所有命令
	deque<Command *> _redolist;  // redo列表,用于保存需要重做的所有命令
	deque<Command *> _templist;  // 预处理命令列表,用于临时保存(用于组合宏命令)

public: 
	CommandManager();
	virtual ~CommandManager();

public:

	bool CanUndo();
	bool CanRedo();

	void Undo();
	void Redo();

	void Do(Command* cmd);

	// 注意: begin和stop必须一起使用,先使用begin标志宏命令开始,再使用stop标记宏命令结束
	void BeginRecord();   // 开始记录命令
	void Stop();

	void SetPause(bool b_pause); // 设置暂停/开始
	bool GetPause();

	bool Recording();  // _recordcount=0 返回false，否则返回true

protected:
	virtual void do_push() {};

	virtual void Push(Command* cmd); // 加入一个命令，如果正在记录宏命令，并且没有暂停，加入macro列表，否则加入undo列表
	Command* Pop(); // 一般用不到

private:
	// 将_templist中的所有命令组合成一个macro_cmd命令
	void Composite();

	void ClearUndoList();
	void ClearRedoList();
	void ClearTempList(); // 不常使用
};

//////////////////////////////////////////////////////////////////////////////////////////
// create 命令
// 创建数据
class CreateCommand: public Command
{
// 记录参数
private:
	Graphic *obj; // 一次创建一个对象，创建之前为空

public:
	CreateCommand(Graphic * obj);
	~CreateCommand(); // 可能被继承的析构函数都写成virtual

public:
	virtual void Execute();
	virtual void Unexecute();
};
//////////////////////////////////////////////////////////////////////////////////////////
// delete 命令
// ----------------------------------------
class DeleteCommand: public Command
{
// 记录参数
private:
	deque<Graphic *> obj_List; // 执行删除的列表

public:
	DeleteCommand();
	~DeleteCommand();

public:
	virtual void Execute();
	virtual void Unexecute();
};
//////////////////////////////////////////////////////////////////////////////////////////
// modify的move命令
// ----------------------------------------
class MoveCommand: public Command
{
// 记录参数
private:
    int x;
    int y;

	deque<Graphic *> obj_List; // 修改的列表，第一次执行的时候为空，执行完成保存select中的对象

public:
	MoveCommand(int x,int y);
	~MoveCommand();

public:
	virtual void Execute();
	virtual void Unexecute();
};
//////////////////////////////////////////////////////////////////////////////////////////
// modify的rotate命令
// ----------------------------------------
class RotateCommand: public Command
{
// 记录参数
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
// modify的setpoint命令
// ----------------------------------------
class AdjustPointCommand: public Command
{
// 记录参数
private:
	adjust_mode a_mode; // 点调整的模式，相对于对面还是中心
	point_type p_type; // 记录调整的是哪个点

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