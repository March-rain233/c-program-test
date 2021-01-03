#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<stdint.h>
#include<math.h>
#include<Windows.h>
#include<process.h>
#include<conio.h>
#include<graphics.h>
#include<easyx.h>
#include<io.h>
//宏定义区域
using namespace std;
//命名空间
#define width 1080
#define height 720
#define HandleSum 20
#define LessonNum 10
//结构体区域
typedef struct
{
	int gridWidth;
	int gridHeight;
}Grids;

typedef struct
{
	double x;
	double y;
	double radius;
}Circle;

typedef struct
{
	char lessonname[30];
	char classname[9][30];
	int num;
	float time1;//理论课时
	float time2;//实践课时
}Lesson;

typedef struct
{
	int ID;
	char name[30];
	char sex[10];
	char level[30];
	Lesson lesson[LessonNum];
}Teacher;

typedef struct
{
	int ID;
	char key[26];
}Admin;

typedef struct teachers
{
	Teacher data;
	struct teachers* next;
}Teachers;
//图方便放在这里的结构体，大概只会调用一次?
struct Clickjudgment
{
	Circle cir;
	int stringHeight;
	int stringWidth;
	int size;
	char str[25];
	int fun;
	Grids grid;
	int x;
	int y;
	double widthBlock;
	double heighBlock;
	int handlenum;
	string* Str;
};
//全局变量区域
int IDsum = 1;
bool ButtonDelete = false;
Teachers* Head;
HANDLE hd[HandleSum];
int HandleNum = 0;
//函数声明
void SecondMenu();
//判断功能相关
bool CircleJudge(double mouse_x, double mouse_y, Circle cir)
{
	double temp, y_up, y_down;
	temp = mouse_x - cir.x;
	temp = fabs(temp);
	if (temp > cir.radius)
	{
		return false;
	}
	temp = cir.radius * cir.radius - temp * temp;
	temp = pow(temp, 0.5);
	y_down = cir.y + temp;
	y_up = cir.y - temp;
	if (mouse_y <= y_down && mouse_y >= y_up)
	{
		return true;
	}
	return false;
}
//功能函数区域
float CalculateLesson(Lesson le)
{
	float result;
	if (le.num == 2)
	{
		result = 1.5 * (le.time1 + le.time2);
	}
	if (le.num == 3)
	{
		result = 2 * (le.time1 + le.time2);
	}
	if (le.num >= 4)
	{
		result = 2.5 * (le.time1 + le.time2);
	}
	return result;
}
float CalculateTotalLesson(Teacher te)
{
	float result = 0;
	for (int i = 0; i < LessonNum; i++)
	{
		if (strcmp(te.lesson[i].lessonname, "NULL") == 0)
		{
			break;
		}
		result += CalculateLesson(te.lesson[i]);
	}
	return result;
}
int ToInt(char ori[])
{
	int result = 0;
	for (int i = 0; i < strlen(ori); i++)
	{
		result = result * 10 + ori[i] - '0';
	}
	return result;
}
float ToFloat(char ori[])
{
	float result = 0;
	int flag = 0;
	for (int i = 0; i < strlen(ori); i++)
	{
		if (ori[i] == '.')
		{
			flag = strlen(ori) - i -1;
		}
		else
		{
			result = result * 10 + ori[i] - '0';
		}
	}
	result = result / pow(10, flag);
	return result;
}
void SaveIDsum()
{
	FILE* fp;
	errno_t err = fopen_s(&fp, "IDsum.sav", "w");
	if (err != 0)
	{
		printf("SaveIDsum Error\n");
		exit(0);
	}
	if (fp)
	{
		fprintf_s(fp, "%d", IDsum);
		fclose(fp);
	}
}
void LoadIDsum()
{
	FILE* fp;
	errno_t err = fopen_s(&fp, "IDsum.sav", "r");
	if (err != 0)
	{
		printf("LoadIDsum Error\n");
		exit(0);
	}
	if (fp)
	{
		fscanf_s(fp, "%d", &IDsum);
		fclose(fp);
	}
}
void LoadTeachers()
{
	FILE* fp;
	errno_t err;
	Teachers* p1;
	p1 = NULL;
	if (_access("./Teacher.data", 0))
	{
		return;
	}
	else
	{
		err = fopen_s(&fp, "./Teacher.data", "rb");
	}
	if (err != 0)
	{
		printf("Open Teacher.data Error\n");
		exit(0);
	}
	bool first = true;
	if (!feof(fp))
	{
		if (first)
		{
			Head = p1 = (Teachers*)malloc(sizeof(Teachers));
		}
		else
		{
			p1->next = (Teachers*)malloc(sizeof(Teachers));
			p1 = p1->next;
		}
		fread(&p1->data, sizeof(Teacher), 1, fp);
	}
	if (p1 != NULL) 
	{
		p1->next = NULL;
	}
	fclose(fp);
}
void SaveTeachers()
{
	FILE* fp;
	errno_t err;
	Teachers* p1;
	err = fopen_s(&fp, "./Teacher.data", "wb");
	if (err != 0)
	{
		printf("Creat Teacher.data Error\n");
		exit(0);
	}
	p1 = Head;
	while (p1 != NULL)
	{
		fwrite(&p1->data, sizeof(Teacher), 1, fp);
		p1 = p1->next;
	}
	fclose(fp);
}
void Init()//生成界面
{
	initgraph(width, height, 1);
	setbkcolor(WHITE);
	cleardevice();
}
Grids* GridInit(int widthRate, int heightRate)//网格初始化
{
	Grids* grid;
	grid = (Grids*)malloc(sizeof(Grids));
	if (grid)
	{
		grid->gridWidth = width / widthRate;
		grid->gridHeight = height / heightRate;
	}
	return grid;
}
void New_Teacher()
{
	FILE* fp;
	Teacher te;
	errno_t err;
	if (!_access("./Teacher.data", 0))
	{
		err = fopen_s(&fp, "./Teacher.data", "wb");
	}
	else
	{
		err = fopen_s(&fp, "./Teacher.data", "ab");
	}
	if (err != 0)
	{
		printf("Open Teacher.data Error\n");
		exit(0);
	}
	if (InputBox(te.name, 30, "输入名字", "输入", NULL, 0, 0, false))
	{
		char ID[26];
		if (InputBox(ID, 26, "输入教师号", "输入", NULL, 0, 0, false))
		{
			te.ID = ToInt(ID);
			char sex[10];
			char temp[50];
			InputBox(sex, 10, "输入性别", "输入", NULL, 0, 0, true);
			if (strcmp(sex, "男") == 0 || strcmp(sex, "女") == 0)
			{
				strcpy_s(te.sex, sex);
			}
			else
			{
				strcpy_s(te.sex, "无");
			}
			InputBox(te.level, 30, "输入职称", "输入", NULL, 0, 0, true);
			MessageBox(NULL, "课程名字输入NULL来停止输入!", "提示", MB_OK);
			bool stop = false;
			for (int i = 0; i < LessonNum; i++)
			{
				char num[4];
				if (!stop)
				{
					InputBox(te.lesson[i].lessonname, 30, "输入课程名字", "输入", NULL, 0, 0, true);
				}
				if (strcmp(te.lesson[i].lessonname, "NULL") == 0)
				{
					stop = true;
				}
				if (!stop)
				{
					InputBox(num, 2, "输入课程数量", "输入", NULL, 0, 0, true);
					te.lesson[i].num = ToInt(num);
					for (int j = 0; j < 9; j++)
					{
						if (j < te.lesson[i].num)
						{
							InputBox(te.lesson[i].classname[j], 4, "输入班级名称", "输入", NULL, 0, 0, true);
						}
						else
						{
							strcpy_s(te.lesson[i].classname[j], "NULL");
						}
					}
					InputBox(num, 4, "输入理论课时", "输入", NULL, 0, 0, true);
					te.lesson[i].time1 = ToFloat(num);
					InputBox(num, 4, "输入实验课时", "输入", NULL, 0, 0, true);
					te.lesson[i].time2 = ToFloat(num);
				}
				else
				{
					strcpy_s(te.lesson[i].lessonname, "NULL");
					for (int j = 0; j < 9; j++)
					{
						strcpy_s(te.lesson[i].classname[j], "NULL");
					}
					te.lesson[i].num = 0;
					te.lesson[i].time1 = 0;
					te.lesson[i].time2 = 0;
				}
			}
			fwrite(&te, sizeof(Teacher), 1, fp);
			Teachers* p1;
			p1 = Head;
			if (p1 == NULL)
			{
				Head = p1 = (Teachers*)malloc(sizeof(Teachers));
				p1->data = te;
			}
			else 
			{
				while (p1->next != NULL)
				{
					p1 = p1->next;
				}
				p1->next = (Teachers*)malloc(sizeof(Teachers));
				p1->next->data = te;
			}
			MessageBox(NULL, "录入成功", "输入", MB_OK);
		}
		else
		{
			fclose(fp);
			MessageBox(NULL, "未录入", "error", MB_OK);
			return;
		}
	}
	else
	{
		fclose(fp);
		MessageBox(NULL, "未录入", "error", MB_OK);
		return;
	}
	fclose(fp);
}
void LogIn()
{
	char ID[11];
	char password[26];
	char temp[50];
	int identity;
	Admin admin;
	if (IDsum == 1)
	{
		MessageBox(NULL, "还未有管理员账户，请注册", "error", MB_OK);
		return;
	}
	if (InputBox(ID, 11, "输入ID", "登录", NULL, 0, 0, false))
	{
		FILE* fp;
		fp = NULL;
		bool find = false;
		errno_t err = fopen_s(&fp, "./Adimin.data", "rb");
		if (err != 0)
		{
			printf("Open Admin.data Error\n");
			exit(0);
		}
		while (!find && !feof(fp))
		{
			fread(&admin, sizeof(Admin), 1, fp);
			if (admin.ID == ToInt(ID))
			{
				find = true;
				strcpy_s(password, admin.key);
			}
		}
		if (!find)
		{
			MessageBox(NULL, "未寻找到此ID\n请重新输入", "error", MB_OK);
		}
		else
		{
			char key[25];
			if (InputBox(key, 26, "输入密码", "登录", NULL, 0, 0, false))
			{
				if (strcmp(key, password) == 0)
				{
					sprintf_s(temp, 50, "成功登录，欢迎管理员%d", admin.ID);
					MessageBox(NULL, temp, "登录", MB_OK);
					ButtonDelete = true;
					//载入操作界面,待填
				}
				else
				{
					MessageBox(NULL, "密码错误，请重新验证账号密码", "error", MB_OK);
				}
			}
		}
		fclose(fp);
	}
}
void UserRegister()
{
	char key[26];
	char temp[50];
	if (InputBox(key, 26, "输入密码", NULL, NULL, 0, 0, false))
	{
		FILE* fp;
		Admin admin;
		if (IDsum == 0)
		{
			errno_t err = fopen_s(&fp, "./Adimin.data", "wb");
			if (err != 0)
			{
				printf("Creat Admin.data Error\n");
				exit(0);
			}
		}
		else
		{
			errno_t err = fopen_s(&fp, "./Adimin.data", "ab");
			if (err != 0)
			{
				printf("Open Admin.data Error\n");
				exit(0);
			}
		}
		admin.ID = IDsum++;
		SaveIDsum();
		strcpy_s(admin.key, key);
		fwrite(&admin, sizeof(Admin), 1, fp);
		fclose(fp);
		sprintf_s(temp, 50, "您的ID为%d\n密码为%s", IDsum - 1, key);
		MessageBox(NULL, temp, "创建成功", MB_OK);
		ButtonDelete = true;
	}
}
DWORD WINAPI Cirfun(LPVOID pM)
{
	Clickjudgment* click;
	click = (Clickjudgment*)pM;
	printf("线程%d启动\n", click->handlenum);
	//printf("%s\n", click->str);
	while (!ButtonDelete)
	{
		//while (MouseHit())
		{
			MOUSEMSG msg = GetMouseMsg();
			switch (msg.uMsg)
			{
			case (WM_MOUSEMOVE):
				if (CircleJudge(msg.x, msg.y, click->cir))
				{
					setfillcolor(LIGHTBLUE);
					settextcolor(BLACK);
					settextstyle(click->stringHeight, click->stringWidth, "黑体");
					setbkmode(TRANSPARENT);
					setlinestyle(PS_SOLID, 0, NULL);
					fillcircle(click->cir.x, click->cir.y, click->cir.radius);
					outtextxy(click->cir.x - click->stringWidth * click->size, click->cir.y - click->stringHeight / 2, click->str);
				}
				else
				{
					setfillcolor(LIGHTRED);
					settextcolor(BLACK);
					settextstyle(click->stringHeight, click->stringWidth, "黑体");
					setbkmode(TRANSPARENT);
					setlinestyle(PS_SOLID, 0, NULL);
					fillcircle(click->cir.x, click->cir.y, click->cir.radius);
					outtextxy(click->cir.x - click->stringWidth * click->size, click->cir.y - click->stringHeight / 2, click->str);
				}
				break;
			case (WM_LBUTTONDOWN):
				if (CircleJudge(msg.x, msg.y, click->cir))
				{
					setfillcolor(LIGHTBLUE);
					settextcolor(BLACK);
					settextstyle(click->stringHeight, click->stringWidth, "黑体");
					setbkmode(TRANSPARENT);
					setlinestyle(PS_SOLID, 0, NULL);
					fillcircle(click->cir.x, click->cir.y, click->cir.radius);
					outtextxy(click->cir.x - click->stringWidth * click->size, click->cir.y - click->stringHeight / 2, click->str);
					setfillcolor(LIGHTRED);
					settextcolor(BLACK);
					fillcircle(click->cir.x, click->cir.y, click->cir.radius);
					outtextxy(click->cir.x - click->stringWidth * click->size, click->cir.y - click->stringHeight / 2, click->str);
					switch (click->fun)
					{
					case(1):
						LogIn();
						break;
					case(2):
						UserRegister();
						break;
					case(3):
						ButtonDelete = true;
						break;
					case(4):
						New_Teacher();
						break;
					case(5):
						break;
					default:
						break;
					}
				}
				break;
			default:
				FlushMouseMsgBuffer();
				break;
			}
		}
	}
	printf("线程%d关闭\n", HandleNum);
	return NULL;
}
DWORD WINAPI Textfun(LPVOID pM)
{
	Clickjudgment* click;
	click = (Clickjudgment*)pM;
	string* str;
	str = click->Str;
	char temp;
	printf("线程%d启动\n", click->handlenum);
	while (!ButtonDelete)
	{
		MOUSEMSG msg = GetMouseMsg();
		fflush(stdin);
		switch (msg.uMsg)
		{
		case(WM_MOUSEMOVE):
			if (msg.x >= click->x && msg.x <= click->x + click->grid.gridWidth * click->widthBlock && msg.y >= click->y && msg.y <= click->y + click->grid.gridHeight * click->heighBlock)
			{
				printf("%s", str);
				temp = _getch();
				if ((str->size() + 1) * click->stringWidth <= click->widthBlock * click->grid.gridWidth)
				{
					if (temp == 8)
					{
						if (!str->empty())
						{
							str->pop_back();
						}
					}
					else
					{
						str += temp;
					}
				}
				else
				{
					if (temp == 8)
					{
						if (!str->empty())
						{
							str->pop_back();
						}
					}
				}
				setfillcolor(RGB(200, 200, 200));
				fillrectangle(click->x, click->y, click->x + click->grid.gridWidth * click->widthBlock, click->y + click->grid.gridHeight * click->heighBlock);
				outtextxy(click->x, click->y + click->grid.gridHeight * click->heighBlock * 0.5 - click->stringHeight * 0.5, str->c_str());
			}
			else
			{
				setfillcolor(WHITE);
				fillrectangle(click->x, click->y, click->x + click->grid.gridWidth * click->widthBlock, click->y + click->grid.gridHeight * click->heighBlock);
				outtextxy(click->x, click->y + click->grid.gridHeight * click->heighBlock * 0.5 - click->stringHeight * 0.5, str->c_str());
			}
			break;
		default:
			FlushMouseMsgBuffer();
			break;
		}
	}
	return NULL;
}
//UI区域
//	UI组件
void CircleButton(int x, int y, int radius, string str, int fun)
{
	int stringHeight, stringWidth;
	int size;
	Circle cir;
	cir.radius = radius;
	cir.x = x;
	cir.y = y;
	stringHeight = radius / 2;
	stringWidth = stringHeight / 2;
	size = str.size();
	size = size / 2;
	setfillcolor(LIGHTRED);
	settextcolor(BLACK);
	settextstyle(stringHeight, stringWidth, "黑体");
	setbkmode(TRANSPARENT);
	fillcircle(x, y, radius);
	outtextxy(x - stringWidth * size, y - stringHeight / 2, str.c_str());
	Clickjudgment* clickjudgment;
	clickjudgment = (Clickjudgment*)malloc(sizeof(Clickjudgment));
	clickjudgment->cir = cir;
	clickjudgment->size = size;
	strcpy_s(clickjudgment->str, str.c_str());
	clickjudgment->stringHeight = stringHeight;
	clickjudgment->stringWidth = stringWidth;
	clickjudgment->fun = fun;
	clickjudgment->handlenum = HandleNum + 1;
	hd[HandleNum++] = CreateThread(NULL, 0, Cirfun, (void*)clickjudgment, 0, NULL);
}
void TextBox(int x, int y, Grids* grid, double widthBlock, double heighBlock, bool writing, int size, string* stringcopy)
{
	int stringHeight, stringWidth;
	string* str;
	str = stringcopy;
	stringWidth = grid->gridWidth * widthBlock / size;
	stringHeight = stringWidth * 2;
	setlinecolor(BLACK);
	setfillcolor(WHITE);
	setlinestyle(PS_SOLID, 0, NULL);
	fillrectangle(x, y, x + grid->gridWidth * widthBlock, y + grid->gridHeight * heighBlock);
	settextcolor(BLACK);
	settextstyle(stringHeight, stringWidth, "黑体");
	outtextxy(x, y + grid->gridHeight * heighBlock * 0.5 - stringHeight * 0.5, str->c_str());
	if (writing)
	{
		Clickjudgment* clickjudgment;
		clickjudgment = (Clickjudgment*)malloc(sizeof(Clickjudgment));
		clickjudgment->grid = *grid;
		clickjudgment->heighBlock = heighBlock;
		clickjudgment->widthBlock = widthBlock;
		clickjudgment->stringHeight = stringHeight;
		clickjudgment->stringWidth = stringWidth;
		clickjudgment->x = x;
		clickjudgment->y = y;
		clickjudgment->size = size;
		clickjudgment->Str = stringcopy;
		clickjudgment->handlenum = HandleNum + 1;
		strcpy_s(clickjudgment->str, str->c_str());
		hd[HandleNum++] = CreateThread(NULL, 0, Textfun, (void*)clickjudgment, 0, NULL);
	}
}
void TextBox(int x, int y, Grids* grid, double widthBlock, double heighBlock, int size,const char str[], bool middle)//中文字符*4 非中文*2
{
	int stringHeight, stringWidth;
	stringWidth = grid->gridWidth * widthBlock / size;
	stringHeight = stringWidth * 2;
	setlinecolor(BLACK);
	setfillcolor(WHITE);
	setlinestyle(PS_SOLID, 0, NULL);
	fillrectangle(x, y, x + grid->gridWidth * widthBlock, y + grid->gridHeight * heighBlock);
	settextcolor(BLACK);
	settextstyle(stringHeight, stringWidth, "黑体");
	if (middle)
	{
		outtextxy(x + (grid->gridWidth * widthBlock - stringWidth * size *0.5) * 0.5, y + (grid->gridHeight * heighBlock - stringHeight) * 0.5, str);
	}
	else 
	{
		outtextxy(x, y + (grid->gridHeight * heighBlock - stringHeight) * 0.5, str);
	}
}
//UI面板
void MainMenu()//生成主菜单
{
	//变量定义
	IMAGE Background;
	Grids* grid;
	int radius;
	//基础变量赋值
	grid = GridInit(6, 6);
	radius = width / 18;
	//图片赋值
	loadimage(&Background, "./Background.png", width, height, false);
	//菜单初始化
	cleardevice();
	putimage(0, 0, &Background);
	setfillcolor(LIGHTRED);
	//生成按钮
	HandleNum = 0;
	ButtonDelete = false;
	CircleButton(grid->gridWidth * 5, grid->gridHeight * 5, radius, "登录", 1);
	CircleButton(grid->gridWidth * 4, grid->gridHeight * 5, radius, "注册", 2);
	WaitForMultipleObjects(HandleNum, hd, false, INFINITE);
	for (int i = 0; i < HandleNum; i++)
	{
		TerminateThread(hd[i], 0);
	}
	SecondMenu();
}
void SecondMenu()
{
	//变量定义
	IMAGE Background;
	Grids* grid;
	int radius;
	//基础变量赋值
	grid = GridInit(6, 6);
	radius = width / 18;
	//图片赋值
	loadimage(&Background, "./Background2.png", width / 2, height, false);
	//菜单初始化
	setbkcolor(WHITE);
	cleardevice();
	putimage(width / 4, 0, &Background);
	setfillcolor(LIGHTRED);
	//生成按钮
	HandleNum = 0;
	ButtonDelete = false;
	CircleButton(grid->gridWidth * 5, grid->gridHeight * 5, radius, "浏览信息", 3);
	CircleButton(grid->gridWidth * 1, grid->gridHeight * 5, radius, "添加信息", 4);
	WaitForMultipleObjects(HandleNum, hd, false, INFINITE);
	for (int i = 0; i < HandleNum; i++)
	{
		TerminateThread(hd[i], 0);
	}
}
void List()
{
	Grids* grid;
	grid = GridInit(8, 8);
	TextBox(grid->gridWidth * 2, grid->gridHeight * 1, grid, 1, 0.5, 8, "教师",true);
}
int main()
{
	//LoadIDsum();
	//LoadTeachers();
	Init();
	List();
	//MainMenu();
	system("pause");
	//closegraph();
	//SaveTeachers();
	//SaveIDsum();
	return 0;
}