//Компьютерная графика. Курсовая работа
//Построение икосаэдра и отображение его в 3-х видах и 2- проекциях
#include <Windows.h>
#include <GL\GL.H>
#include <GL\GLU.H>
#include <GL\glut.h>
#include <cmath>
//Массивы видимости
bool VGR1[20],VGR2[20],VGR3[20],VGR5[20],/*граней*/VRB1[30],VRB2[30],VRB3[30],VRB5[30];//и ребер; 0-видно, 1-не видно
//Массивы общего описания
				// 0     1     2     3     4     5     6     7     8       9     10    11    12    13    14
int MR[30][2] = {{1,2},{2,3},{3,4},{4,5},{5,1},{6,7},{7,8},{8,9},{9,10},{10,6},{1,8},{8,5},{5,7},{7,4},{4,6},
				// 15    16     17     18    19    20     21     22     23     24     25    26    27    28    29
				 {6,3},{3,10},{10,2},{2,9},{9,1},{11,6},{11,7},{11,8},{11,9},{11,10},{0,1},{0,2},{0,3},{0,4},{0,5}
				};//массив ребер по вершинам икосаэдра
				   // 0         1         2         3         4         5         6         7         8          9
int MGR[20][3] = {{25,0,26},{26,1,27},{27,2,28},{28,3,29},{29,4,25},{5,20,21},{6,21,22},{7,22,23},{8,23,24},{9,24,20},
				   // 10        11        12        13        14        15        16        17        18         19
				  {10,7,19},{11,10,4},{12,6,11},{13,12,3},{14,5,13},{2,15,14},{16,9,15},{1,17,16},{17,18,8},{18,0,19}
                 };//массив граней по ребрам
				   // 0       1       2       3       4        5       6        7        8          9
int MGV[20][3] = {{0,1,2},{0,2,3},{0,3,4},{0,4,5},{0,5,1},{7,6,11},{8,7,11},{9,8,11},{10,9,11},{6,10,11},
				  // 10      11      12     13      14       15      16       17       18       19
				  {1,8,9},{5,8,1},{5,7,8},{4,7,5},{4,6,7},{4,3,6},{3,10,6},{3,2,10},{10,2,9},{9,2,1}};//массив граней по вершинам
//Определение структур ребер заполнения
struct edgepars {int ygr; float x; float dx; int dy;} SR[3]/*список ребер заполнения*/,
													  SAR[3]/*список активных ребер заполнения*/,dd/*структура дублирования*/;
//Параметры визуализации
struct Camera {float Ro; float teta; float fi;} Cam1={150.0f,0.0f,89.99f}, Cam2={150.0f,0.0f,90.0f}, Cam3={150.0f,0.0f,90.0f};//точки наблюдения
float d=300.0f/*40.0f*/,c1[4]={300.0f,800.0f,300.0f,900.0f},c2[4]={500.0f,500.0f,200.0f,180.0f};
double a=0.5*atan(1.0)/45.0; float OrthoK=2.0f; int MyVisMode=0;
//Массивы координат икосаэдра
struct wIc { float wXic[12]; float wYic[12]; float wZic[12];} wIc1, wIc2, wIc3, wIc4;//мировые координаты
struct eIc { float eXic[12]; float eYic[12]; float eZic[12];} eIc1, eIc2, eIc3, eIc4;//видовые координаты
struct pIc { float pXic[12]; float pYic[12];} pIc1, pIc2, pIc3, pIc4;//экранные координаты
int R=40;//Радиус рассекаемого цилиндра 
double l=72.0*atan(1.0)/45.0, d1=180.0*atan(1.0)/45.0, q;
//Определение мировых координат икосаэдра
void WcCalc(wIc (&Ici))
{
	Ici.wXic[0]=0.0f;Ici.wYic[0]=0.0f;Ici.wZic[0]=R*sqrt(3/*5*//2.0);
	Ici.wXic[11]=0.0f;Ici.wYic[11]=0.0f;Ici.wZic[11]=-R*sqrt(3/*5*//2.0);
	for(int i=1;i<11;i++)
   {	
		if(i<6)
	   { Ici.wZic[i]=R/2.0; q=l*i+d1;}
		else
	   { Ici.wZic[i]=-R/2.0; q=l*i; }
		Ici.wXic[i]=R*cos(q);Ici.wYic[i]=R*sin(q);
   }
}
//Перевод из мировых координат в видовые для центрального одноточечного проецирования
void EWTranslate(wIc (&wIci),eIc (&eIci),Camera (&Cami))
{
	for(int i=0;i<12;i++)
	{
		eIci.eXic[i]=-wIci.wXic[i]*sin(Cami.teta)+wIci.wYic[i]*cos(Cami.teta);
		eIci.eYic[i]=-wIci.wXic[i]*cos(Cami.fi)*cos(Cami.teta)-wIci.wYic[i]*cos(Cami.fi)*sin(Cami.teta)+wIci.wZic[i]*sin(Cami.fi);
		eIci.eZic[i]=-wIci.wXic[i]*sin(Cami.fi)*cos(Cami.teta)-wIci.wYic[i]*sin(Cami.fi)*sin(Cami.teta)-wIci.wZic[i]*cos(Cami.fi)+Cami.Ro;
	}
}
//Перевод из видовых координат в проекционные при центральном одноточечном проецировании
void PETranslate_Central(eIc (&eIci),pIc (&pIci),int c)
{
		for(int i=0;i<12;i++)
	{
		pIci.pXic[i]=(eIci.eXic[i]/eIci.eZic[i])*d+c1[c];
		pIci.pYic[i]=(eIci.eYic[i]/eIci.eZic[i])*d+c2[c];
	}
}
//Ортографическое проецирование на плоскость XY
void PETranslate_OrthoXY(wIc (&wIci),pIc (&pIci),int c, float koef)
{
	for(int i=0;i<12;i++)
	{
		pIci.pXic[i]=wIci.wXic[i]*koef+c1[c];
		pIci.pYic[i]=wIci.wYic[i]*koef+c2[c];
	}
}
//Ортографическое проецирование на плоскость YZ
void PETranslate_OrthoYZ(wIc (&wIci),pIc (&pIci), int c, float koef)
{
		for(int i=0;i<12;i++)
	{
		pIci.pXic[i]=wIci.wYic[i]*koef+c1[c];
		pIci.pYic[i]=wIci.wZic[i]*koef+c2[c];
	}
}
//Ортографическое проецирование на плоскость XZ
void PETranslate_OrthoXZ(wIc (&wIci),pIc (&pIci), int c, float koef)
{
		for(int i=0;i<12;i++)
	{
		pIci.pXic[i]=wIci.wXic[i]*koef+c1[c];
		pIci.pYic[i]=wIci.wZic[i]*koef+c2[c];
	}
}
//Функция инициализация начальной видимости граней икосаэдра (все видимые)
void InitVisible(bool *pVGR, bool *pVRB)
{
	for (size_t i=0,j=0;i<20,j<30;++i,++j)
	{ 
		pVGR[i]=0;
		pVRB[i]=0; 
	}
}
//Определение видимости граней
void ChangeVisibleSides(pIc (&pIci), bool *pVGR)
{
	for(int i=0;i<20;++i)
	{
		int f1,f2,f3;
		f1=MGV[i][0];f2=MGV[i][1];f3=MGV[i][2];
		int dx1=pIci.pXic[f2]-pIci.pXic[f1];
		int dx2=pIci.pXic[f3]-pIci.pXic[f2];
		int dy1=pIci.pYic[f2]-pIci.pYic[f1];
		int dy2=pIci.pYic[f3]-pIci.pYic[f2];
	 if((dx1*dy2-dx2*dy1)>0)pVGR[i]=1;
	}
}
//Определение видимости ребер
void ChangeVisibleEdges(bool *pVGR, bool *pVRB)
{
	for(int i=0;i<19;i++)
	{
		if(pVGR[i]==1)
		{
		  for(int j=i+1;j<20;j++)
			if(pVGR[j]==1)
			  for(int l=0;l<3;l++)
				for(int m=0;m<3;m++)
				  if(MGR[i][l]==MGR[j][m])
					{ int a=MGR[i][l];pVRB[a]=1; }
		}
	}
}
//Заполнение граней
void FillSides(bool (&VGR)[20], pIc (&pIci))
{
 for(int i=0;i<3;++i)
 {
	SR[i].ygr=SR[i].dy=0; SR[i].x=SR[i].dx=0;
	SAR[i].ygr=SAR[i].dy=0; SAR[i].x=SAR[i].dx=0;		
 }
   dd.ygr=dd.dy=0; dd.x=dd.dx=0;
 //
 for(int i=0;i<20;i++)
   if(VGR[i]==0)//Если грань видимая
   {
	   int max=0,b=0/*b-кол-во элементов в САР*/;
	  for(int a=0;a<3;a++)
	   {
		   int a1=MGR[i][a],a2=MR[a1][0],a3=MR[a1][1];
		//Определение списка ребер (границ закрашивания)	
   /*{{*/  if ((int)pIci.pYic[a2]!=(int)pIci.pYic[a3])
		  {
			  if ((int)pIci.pYic[a2]<(int)pIci.pYic[a3])
			  {
				  int a4=a2;a2=a3;a3=a4;
			  }
			  SR[b].ygr=(int)pIci.pYic[a2];
			  SR[b].x=pIci.pXic[a2];
			 float d1=pIci.pXic[a2]-pIci.pXic[a3]; float d2=(int)pIci.pYic[a3]-(int)pIci.pYic[a2];
			 float d3=d1/d2;
			 SR[b].dx=d3;
			 SR[b++].dy=(int)pIci.pYic[a2]-(int)pIci.pYic[a3];
	/*}}*/		if((int)pIci.pYic[a2]>max) max=(int)pIci.pYic[a2];//Попутно определяется max(ygr),т.е. сканирующую строку
		  }
	   }
	  int j2=0;
	  //Начальное определние состава САР
	/*{{*/ for(int c=0;c<b;c++)
		  if(SR[c].ygr==max) SAR[j2++]=SR[c];
	/*}}*/  
	 while(j2!=0)
	  {
		  for(int ii=0;ii<j2;ii++)
		  {
			  for(int t=ii+1;t<j2+1;t++)
			  {
				  if(SAR[ii].dy==0)//Если для ii-ого элемента САР неосталось активных строк, то мы его исключаем из САР, а последний ставим на его место
				  {
					  SAR[ii].dy=SAR[j2-1].dy; SAR[ii].dx=SAR[j2-1].dx; SAR[ii].ygr=SAR[j2-1].ygr; SAR[ii].x=SAR[j2-1].x;
					  SAR[j2-1].dy=0; SAR[j2-1].dx=0; SAR[j2-1].ygr=0; SAR[j2-1].x=0;
				  }
				  if((SAR[ii].x>SAR[t].x)&&(SAR[t].x!=0))//Сортируем элементы САР по возрастанию компоненты x
				  {
					  dd.x=SAR[ii].x; dd.ygr=SAR[ii].ygr; dd.dx=SAR[ii].dx; dd.dy=SAR[ii].dy;
					  SAR[ii].x=SAR[t].x; SAR[ii].ygr=SAR[t].ygr; SAR[ii].dx=SAR[t].dx; SAR[ii].dy=SAR[t].dy;
					  SAR[t].x=dd.x; SAR[t].ygr=dd.ygr; SAR[t].dy=dd.dy; SAR[t].dx=dd.dx;
				  }
			  }
		  }
		  //Заполнаем найденную область
	/*{{*/	 for(int j=0;j<j2;j=j+2)//переходим к следующей группе заполнения
			  for(float k=SAR[j].x;k<SAR[j+1].x;k++)
			  {   glColor4f(1.0f,1.0f,1.0f,1.0f);
				  glPushMatrix();
				  glBegin(GL_POINTS);
				    glVertex2f(k,max);
				  glEnd();
				  glPopMatrix();
    /*}}*/	  }
			max--;//Уменьшаем номер сканирующей строки
		   for(int j=0,k=j2;j<k;j++)
			  {
				  SAR[j].dy--; SAR[j].ygr--; SAR[j].x+=SAR[j].dx;
				 if(SAR[j].dy==0)j2--;
			  }
		   //Переопределение состава САР для общего цикла закрашивания по новой сканирующей строке
		   for(int a=0;a<b;a++)
			   if(SR[a].ygr==max)
				   for(int jj=0;jj<4;jj++)
					   if(SAR[jj].dy==0)
					   { SAR[jj]=SR[a];j2++;break; }
	  }
   }
}
//Перегрузка функции отображения для каркаса
void DispIcos(pIc (&pIci))
{	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glBegin(GL_LINES);
		for(int i=0;i<30;i++)
		{
		int a=MR[i][0];
		int b=MR[i][1];
		glVertex2f(pIci.pXic[a],pIci.pYic[a]);glVertex2f(pIci.pXic[b],pIci.pYic[b]);
		}
	 glEnd();
}
//Перегрузка функции отображения для видимых сторон и ребер
void DispIcos(pIc (&pIci),bool *pVRB)
{   glColor4f(1.0f,1.0f,1.0f,1.0f);
	glBegin(GL_LINES);
		for(int i=0;i<30;i++)
		{
			if(pVRB[i]==0)
		   { 
			 int a=MR[i][0],b=MR[i][1];
		     glVertex2f(pIci.pXic[a],pIci.pYic[a]);glVertex2f(pIci.pXic[b],pIci.pYic[b]);
		   }
		}
	 glEnd();
}
//Перегрузка функции отображения для закрашенного тела
void DispIcos(pIc (&pIci),bool *pVRB, bool (&pVGR)[20])
{   FillSides(pVGR,pIci);
	glColor4f(0.0f,0.0f,1.0f,1.0f);
	glBegin(GL_LINES);
		for(int i=0;i<30;i++)
		{
			if(pVRB[i]==0)
		   { int a=MR[i][0],b=MR[i][1];glVertex2f(pIci.pXic[a],pIci.pYic[a]);glVertex2f(pIci.pXic[b],pIci.pYic[b]); }
		}
	 glEnd();
}
//Пространство имен режимов
namespace MyModes
{
	void Mode0()//Режим 0 - каркасные модели
	{
		glPushMatrix();
		  DispIcos(pIc1); DispIcos(pIc2); DispIcos(pIc3); DispIcos(pIc4);
		glPopMatrix();
	   glutSwapBuffers();
	}
	//
	void Mode1()//Режим 1 - применение 1 и 2 этапов алгоритам Робертса
	{
		glPushMatrix();
		  DispIcos(pIc1,VRB1); DispIcos(pIc2,VRB2); DispIcos(pIc3,VRB3); DispIcos(pIc4,VRB5);
		glPopMatrix();
	   glutSwapBuffers();
	}
	void Mode2()//Режим 2 - закрашенные модели
	{
	    glPushMatrix();
		  DispIcos(pIc1,VRB1,VGR1); DispIcos(pIc2,VRB2,VGR2); DispIcos(pIc3,VRB3,VGR3); DispIcos(pIc4,VRB5,VGR5);
	    glPopMatrix();
	   glutSwapBuffers();
	}
}
//
void Display()
{	
	glClear(GL_COLOR_BUFFER_BIT);
	//Транслирование
	PETranslate_OrthoYZ(wIc1,pIc1,0,OrthoK);PETranslate_OrthoXZ(wIc2,pIc2,1,OrthoK);PETranslate_OrthoXY(wIc3,pIc3,2,OrthoK);
	EWTranslate(wIc4,eIc4,Cam1); PETranslate_Central(eIc4,pIc4,3);
	//Инициализация видимости
	InitVisible(VGR1,VRB1); InitVisible(VGR2,VRB2); InitVisible(VGR3,VRB3); InitVisible(VGR5,VRB5);
	//Установка видимости
	ChangeVisibleSides(pIc1,VGR1); ChangeVisibleEdges(VGR1,VRB1); ChangeVisibleSides(pIc2,VGR2); ChangeVisibleEdges(VGR2,VRB2);
	ChangeVisibleSides(pIc3,VGR3); ChangeVisibleEdges(VGR3,VRB3); ChangeVisibleSides(pIc4,VGR5); ChangeVisibleEdges(VGR5,VRB5);
	//Рисование
	glColor4f(1.0f,1.0f,1.0f,1.0f);
		switch(MyVisMode)
		{
		case 0: MyModes::Mode0(); break;
		case 1: MyModes::Mode1(); break;
		case 2: MyModes::Mode2(); break;
		}
}
//Вращение по оси Х в мировых координатах
void Movie_RotateX(wIc (&wIci), double angle)
{
	for(int i=0;i<12;i++)
	{
		wIci.wYic[i]=wIci.wYic[i]*cos(angle)-wIci.wZic[i]*sin(angle);
		wIci.wZic[i]=wIci.wYic[i]*sin(angle)+wIci.wZic[i]*cos(angle);
	}
}
//Вращение по оси У в мировых координатах
void Movie_RotateY(wIc (&wIci), double angle)
{
	for(int i=0;i<12;i++)
	{
		wIci.wXic[i]=wIci.wXic[i]*cos(angle)+wIci.wZic[i]*sin(angle);
		wIci.wZic[i]=-wIci.wXic[i]*sin(angle)+wIci.wZic[i]*cos(angle);
	}
}
//Вращение по оси Z в мировых координатах
void Movie_RotateZ(wIc (&wIci), double angle)
{
	for(int i=0;i<12;i++)
	{
		wIci.wXic[i]=wIci.wXic[i]*cos(angle)-wIci.wYic[i]*sin(angle);
		wIci.wYic[i]=wIci.wXic[i]*sin(angle)+wIci.wYic[i]*cos(angle);
	}
}
//
void Timer(int)
{
	Movie_RotateZ(wIc1,a);
	Movie_RotateZ(wIc2,a);
	Movie_RotateZ(wIc3,-a);
	Movie_RotateZ(wIc4,a);
	//
	glutPostRedisplay();
	glutTimerFunc(30,Timer,0);
}
//
void Change(int w, int h)
{
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f,w,0.0f,h,-1.0f,1.0f);
}
//Контроль масштаба при центральном одноточечном проецировании
void SKeyboard(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_UP: case GLUT_KEY_LEFT: d+=2; break;
	case GLUT_KEY_DOWN: case GLUT_KEY_RIGHT: d-=2; break;
	}
}
//Контроль масштаба при ортографическом проецировании
void Keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 'w': case 'a': OrthoK+=0.02;break;
	case 's': case 'd': OrthoK-=0.02;break;
	case '0': MyVisMode=0; break;
	case '1': MyVisMode=1; break;
	case '2': MyVisMode=2; break;
	}
}
//
int main(int argc, char* argv[])
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowSize(1300,650);
	glutInitWindowPosition(0,0);
	glutCreateWindow(" Icosahedron ");
	WcCalc(wIc1);WcCalc(wIc2);WcCalc(wIc3);WcCalc(wIc4);
	glutDisplayFunc(Display);
	Change(1300,650);
	glutTimerFunc(30,Timer,0);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SKeyboard);
	glClearColor(0.08f,0.25f,0.36f,0.89f);
	glutMainLoop();
	return 0;
}