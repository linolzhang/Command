#include "StdAfx.h"
#include "Command.h"

//////////////////////////////////////////////////////////////////////////////////////////
// 使用两个链表表示页面内的状态，显示列表和选择列表（合并成一个不方便操作）
deque<Graphic *> display_List; // 用于显示的对象链表，对象不完全删除
deque<Graphic *> select_List;  // 用于显示的对象链表，对象不完全删除

//////////////////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------
// 命令基类
Command::Command()
{
	_type = type_no;  // 定义操作类型
	_destory = false; // 定义析构时是否需要删除保存在命令里的对象指针
}
cmd_type Command::Type()
{ 
	return _type; 
}
// -----------------------------------------------
void Command::NeedDestory(bool bdestory)
{ 
	_destory = bdestory;
}

//////////////////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------
// 组合命令类
MacroCommand::MacroCommand()
{
	_destory = false;
	_type =  type_macro;
}
// -----------------------------------------------
MacroCommand::~MacroCommand()
{
	deque<Command *>::iterator ite;
	for(ite = macro_list.begin(); ite != macro_list.end(); ++ite)
	{
		if((*ite)->Type() == type_create) // 注：macro和create的一致
			(*ite)->NeedDestory(_destory);
		else if((*ite)->Type() == type_delete) // delete的需要取反
			(*ite)->NeedDestory(!_destory);
		else if((*ite)->Type() == type_macro)
			(*ite)->NeedDestory(_destory);

		delete *ite;
	}
}
// -----------------------------------------------
void MacroCommand::Execute()
{
	deque<Command *>::iterator ite;
	for(ite = macro_list.begin(); ite != macro_list.end(); ++ite)
	{
		(*ite)->Execute();
	}
}
// -----------------------------------------------
void MacroCommand::Unexecute() 
{ 
	deque<Command *>::iterator ite;
	for(ite = macro_list.begin(); ite != macro_list.end(); ++ite)
	{
		(*ite)->Unexecute();
	}
}
// -----------------------------------------------
void MacroCommand::Insert(Command * cmd)
{
	macro_list.push_back(cmd);
}
//////////////////////////////////////////////////////////////////////////////////////////
// 命令管理类
// -----------------------------------------------
CommandManager::CommandManager()
{
	_record = false;
	_maxcmd = 100;
	_pause = false;
}
// -----------------------------------------------
CommandManager::~CommandManager() 
{
	ClearUndoList();
	ClearRedoList();
	ClearTempList();
}
// -----------------------------------------------
bool CommandManager::CanUndo() {   return !_undolist.empty();  }
bool CommandManager::CanRedo() {   return !_redolist.empty();  }
// -----------------------------------------------
void CommandManager::Undo()
{
	if(_undolist.empty())
		return;

	Command* cmd = _undolist.back();
	cmd->Unexecute();

	_redolist.push_back(cmd);
	_undolist.pop_back();
}
// -----------------------------------------------
void CommandManager::Redo()
{
	if(_redolist.empty())
		return;

	Command* cmd = _redolist.back();
	cmd->Execute();
	_undolist.push_back(cmd);
	_redolist.pop_back();
}
// -----------------------------------------------
void CommandManager::Do(Command* cmd)
{
	assert(cmd != NULL);
	Push(cmd);
	cmd->Execute();
}
// -----------------------------------------------
void CommandManager::BeginRecord()
{
	_record = true;
	_pause = false;
}
void CommandManager::Stop()   
{ 
	_record = false;
	Composite();
	_pause = false; 
}
// -----------------------------------------------
void CommandManager::SetPause(bool b_pause)
{
	_pause = b_pause; 
}
bool CommandManager::GetPause()   
{ 
	return _pause;
}
// -----------------------------------------------
bool CommandManager::Recording()
{ 
	return _record; 
}
// -----------------------------------------------
void CommandManager::Push(Command* cmd)
{
	assert(cmd != NULL);

	// begin 启动的情况,加入一个命令时进行判断
	if(_record && !_pause)
	{
		_templist.push_back(cmd); // 加入templist列表
	}
	else
	{
		if (!_redolist.empty())
			ClearRedoList();

		_undolist.push_back(cmd);
		do_push();
	}
}
Command * CommandManager::Pop()
{
	if(_undolist.empty())
		return NULL;

	Command* pcmd = _undolist.back();
	_undolist.pop_back();

	return pcmd;
}
// -----------------------------------------------
void CommandManager::Composite() // 命令的组合
{
	// _microlist为空或还没有到组合时机时不组合
	if(_templist.empty() || _record)
		return;

	if(_templist.size() == 1)
	{
		Push(_templist.front()); // 如果预处理队列当前只有一个命令，直接push到undo列表
	}
	else
	{
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		    // 如果里面都是同一类型的命令，合成一个命令
		    // 比如move x1,y1  x2,y2,  x3,y3 ~~
		    // 合成move x1+x2+x3+…,y1+y2+y3+…
		// 注意，_macrolist 可能会有两种情况：微命令或者组合命令
		// 我们只封装组合命令，微命令在这里直接合成，并在stop()时存入undo列表
		// 否则定义一个MacroCommand，完成多个命令的封装
		MacroCommand * pMacroCmd = new MacroCommand;
		deque<Command *>::iterator ite = _templist.begin();

		for(; ite != _templist.end(); ++ite)
		{
			pMacroCmd ->Insert(*ite);
		}
		Push(pMacroCmd );
	}
	_templist.clear();
}
// -----------------------------------------------
void CommandManager::ClearUndoList()
{
	deque<Command *>::iterator ite = _undolist.begin();
	for(; ite != _undolist.end(); ++ite)
	{
		if((*ite)->Type() == type_create) // 在undo里创建的对象，false，不删除
			(*ite)->NeedDestory(false);
		else if((*ite)->Type() == type_delete) // 在undo里的delete命令，true，开始不删除，命令被删除时对象一起删除
			(*ite)->NeedDestory(true);
		else if((*ite)->Type() == type_macro)
			(*ite)->NeedDestory(false);  // 设置为和create的一致

		delete *ite;
	}
	_undolist.clear();
}

void CommandManager::ClearRedoList() // redo的正好相反，在redo里create的删除，在redo里delete的不删
{
	deque<Command *>::iterator ite = _redolist.begin();
	for(; ite != _redolist.end(); ++ite)
	{
		if((*ite)->Type() == type_create)
			(*ite)->NeedDestory(true);
		else if((*ite)->Type() == type_delete)
			(*ite)->NeedDestory(false);
		else if((*ite)->Type() == type_macro)
			(*ite)->NeedDestory(true);

		delete *ite;
	}
	_redolist.clear();
}
void CommandManager::ClearTempList()
{
	deque<Command *>::iterator ite = _templist.begin();
	for(; ite != _templist.end(); ++ite)
	{
		if((*ite)->Type() == type_create)
			(*ite)->NeedDestory(true);
		else if((*ite)->Type() == type_delete)
			(*ite)->NeedDestory(false);
		else if((*ite)->Type() == type_macro)
			(*ite)->NeedDestory(true);

		delete *ite;
	}
	_templist.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
// 创建一个对象，create命令
CreateCommand::CreateCommand(Graphic * obj)
{
	_type = type_create;  // 定义操作类型

	this->obj = obj;
	select_List.push_back(obj);  // 刚被创建的时候加入选择列表，方便修改
}
CreateCommand::~CreateCommand()
{
	if(_destory)                 // 根据需要，在析构时删除对象
		delete obj;
}
void CreateCommand::Execute()
{
	display_List.push_back(obj); // 加入显示列表
}
void CreateCommand::Unexecute()
{
	deque<Graphic *>::iterator ite = display_List.begin();
	for(; ite != display_List.end(); ++ite)
	{
		if(*ite == obj)
		{
			display_List.erase(ite); // 从显示列表中删除创建元素的指针
			break;
		}
	}

}
//////////////////////////////////////////////////////////////////////////////////////////
// 删除一组对象，delete命令
DeleteCommand::DeleteCommand()
{
	_type = type_delete;      // 定义操作类型

	// 复制select列表
	deque<Graphic *>::iterator ite = select_List.begin();
	for(; ite != select_List.end(); ++ite)
	{
		obj_List.push_back(*ite);
	}
}
DeleteCommand::~DeleteCommand()
{
	if(_destory)             // 根据需要，在析构时删除对象
	{
		deque<Graphic *>::iterator ite = obj_List.begin();
		for(; ite != obj_List.end(); ++ite)
		{
			delete *ite;
		}
	}
}

void DeleteCommand::Execute()
{
	for(deque<Graphic *>::iterator ite1 = obj_List.begin(); ite1 != obj_List.end(); ++ite1)
	{
		for(deque<Graphic *>::iterator ite2 = display_List.begin(); ite2 != display_List.end(); ++ite2)
			{
				if(*ite2 == *ite1)
				{
					display_List.erase(ite2); // 从显示列表中删除元素的指针
					break;
				}
			}
	}
}
void DeleteCommand::Unexecute()
{
	// 将对象指针全部插入显示列表
	deque<Graphic *>::iterator ite = obj_List.begin();
	for(; ite != obj_List.end(); ++ite)
	{
		display_List.push_back(*ite);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 修改一组对象，move命令
MoveCommand::MoveCommand(int x,int y)
{
	_type = type_modify;  // 定义操作类型
	this->x = x;
	this->y = y;

	// 复制列表
	deque<Graphic *>::iterator ite = select_List.begin();
	for(; ite != select_List.end(); ++ite)
	{
		obj_List.push_back(*ite);
	}
}
MoveCommand::~MoveCommand()
{
}
// -----------------------------------------------
void MoveCommand::Execute()
{
	// obj_List列表中的所有对象执行move操作
	deque<Graphic *>::iterator ite = obj_List.begin();
	for(; ite != obj_List.end(); ++ite)
	{
		(*ite)->Move(x,y);
	}	
}
void MoveCommand::Unexecute()
{
	// obj_List列表中的所有对象执行反操作
	deque<Graphic *>::iterator ite = obj_List.begin();
	for(; ite != obj_List.end(); ++ite)
	{
		(*ite)->Move(-x,-y);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
// 修改一组对象，rotate命令
RotateCommand::RotateCommand(float angle)
{
	_type = type_modify;  // 定义操作类型
	this->angle = angle;

	// 复制列表
	deque<Graphic *>::iterator ite = select_List.begin();
	for(; ite != select_List.end(); ++ite)
	{
		obj_List.push_back(*ite);
	}
}
RotateCommand::~RotateCommand()
{
}
// -----------------------------------------------
void RotateCommand::Execute()
{
	// obj_List列表中的所有对象执行rotate操作
	deque<Graphic *>::iterator ite = obj_List.begin();
	for(; ite != obj_List.end(); ++ite)
	{
		// 根据对象的类型进行指针类型转换
		((Shape *)(*ite))->Rotate(angle);
	}	
}
void RotateCommand::Unexecute()
{
	// obj_List列表中的所有对象执行反操作
	deque<Graphic *>::iterator ite = obj_List.begin();
	for(; ite != obj_List.end(); ++ite)
	{
		// 根据对象的类型进行指针类型转换
		((Shape *)(*ite))->Rotate(-angle);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
// 修改单个对象，AdjustPoint命令，调整对象（拖拽）
AdjustPointCommand::AdjustPointCommand(adjust_mode a_mode,point_type p_type,int x,int y)
{
	_type = type_modify;  // 定义操作类型

	this->a_mode = a_mode;
	this->p_type = p_type;
	this->x = x;
	this->y = y;

	// 复制列表
	deque<Graphic *>::iterator ite = select_List.begin();
	for(; ite != select_List.end(); ++ite)
	{
		obj_List.push_back(*ite);
	}
}
AdjustPointCommand::~AdjustPointCommand()
{
}
// -----------------------------------------------
void AdjustPointCommand::Execute()
{
	// obj_List列表中的所有对象执行Adjust操作
	deque<Graphic *>::iterator ite = obj_List.begin();
	for(; ite != obj_List.end(); ++ite)
	{
		// 根据对象的类型进行指针类型转换
		switch((*ite)->GetType())
		{
		case M_LINE:
			((MyLine *)(*ite))->AdjustPoint(a_mode,p_type,x,y);
			break;
		case M_RECTANGLE:
			((MyRectangle *)(*ite))->AdjustPoint(a_mode,p_type,x,y);
			break;
		case M_ELLIPSE:
		    ((MyEllipse *)(*ite))->AdjustPoint(a_mode,p_type,x,y);
			break;
		}
	}	

}
void AdjustPointCommand::Unexecute()
{
	// obj_List列表中的所有对象执行Adjust操作
	deque<Graphic *>::iterator ite = obj_List.begin();
	for(; ite != obj_List.end(); ++ite)
	{
		// 根据对象的类型进行指针类型转换
		switch((*ite)->GetType())
		{
		case M_LINE:
			((MyLine *)(*ite))->AdjustPoint(a_mode,p_type,-x,-y);
			break;
		case M_RECTANGLE:
			((MyRectangle *)(*ite))->AdjustPoint(a_mode,p_type,-x,-y);
			break;
		case M_ELLIPSE:
			((MyEllipse *)(*ite))->AdjustPoint(a_mode,p_type,-x,-y);
			break;
		}
	}
}