// IrisRec.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv/cxcore.h"
#include "opencv/cvaux.h"
#include <iostream>
#include <list>
#include <vector>
#include <math.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
using namespace cv;


#define pi 3.1415926
#define min(a,b) (((a)<(b))?(a):(b))

//////////////////////////////////////////////////////////////////////////
#define BASELINEPOS 1305
#define BASELINERANGE 20
#define UPPER0       372
#define LOWER0       360
#define ROIH0       1260      
#define SPECIAL0     370
#define SPECIAL1     40
#define SPECIAL2     25
#define VFLAWLEN     50
//////////////////////////////////////////////////////////////////////////
typedef struct tagPHTPoint
{
    CvPoint Point;
    int LineNumber;
}PHTPoint;

const int distant_used_for_PHT=8;
const int Character_length=128;//特?征÷向ò量?的?长¤度è为a128
const char* cascadeName = ".//haarcascade_eye_tree_eyeglasses.xml";//nestedCascadeName   Preprocess!!

int num;


typedef struct tagCIRCLE
{
	int x;//圆2的?X坐?标括?；?
	int y;//圆2的?Y坐?标括?
	int r;//圆2的?半?径?；?
}CIRCLE;//圆2的?坐?标括?

typedef struct tagPHTPointGROUP
{	CvPoint a;
	CvPoint b;
	CvPoint c;
}PHTPointGROUP;//确ā?定¨圆2上?的?3个?点?

typedef struct tagPointInformation
{
	bool useful;// 记?录?该?点?的?状痢?态?信?息￠
	// 外猘接ó矩?形?的?描è述?参?量?
	long left;
	long right;
	long top;
	long bottom;
	long size;//面?积y
	long xsum;//x坐?标括?和í
	long ysum;//y坐?标括?和í
}PointInformation;
//虹?膜¤特?征÷点?的?描è述?参?数簓

typedef struct tagCharacterVector
{  
	// 外猘接ó矩?形?的?描è述?参?量?
	long left;
	long right;
	long top;
	long bottom;
	//特?征÷点?的?种?类え?
	long type;
	//特?征÷点?的?重?心?坐?标括?
	long x;
	long y;
}CharacterVector;

typedef struct FileName_Chara
{
	char * FileName;//文?件t名?
	CharacterVector * Chara;//特?征÷点?
}FileName_Chara;//特?征÷值μ点?的?存?储洹?


inline float distance(CIRCLE a,CIRCLE b)//返う?回?点?组哩?的?距à离?
{
   return  sqrt((pow((double)(a.x-b.x),2)+pow((double)(a.y-b.y),2)+pow((double)(a.r-b.r),2)));
}

inline float mid(float a,float b,float  c)//返う?回?中D值μ
{
	if(a>b&&a<c||a>c&&a<b)
		  return a;
	if(a<b&&b<c||a>b&&b>c)
		  return b;
	if(a<c&&b>c||a>c&&b<c)
		  return c; 
}

void writeAngle(float xi,char * num)
{
	FILE *fp=fopen("C:\\1\\Angle.txt","a+");
	if(fp==NULL)
	{
		return;
	}
	fprintf(fp,"%f     %s \n",xi,num);
	fclose(fp);
}

void writeImgInfo(int xi,int yi,int xj,int yj,char * msg)
{
	FILE *fp=fopen("C:\\1\\imgInfoOfCoord.txt","a+");
	if(fp==NULL)
	{
		return;
	}
	fprintf(fp,"%d   %d   %d   %d    %s \n",xi,yi,xj,yj, msg);
	fclose(fp);
}
void writeCliped(int xi,int yi,char * msg)
{
	FILE *fp=fopen("C:\\make\\log\\imgInfoOfOri.txt","a+");
	if(fp==NULL)
	{
		return;
	}
	fprintf(fp,"%d   %d   %s \n",xi,yi,msg);
	fclose(fp);
}


void writeImgInfoClip(int xi,int yi,int ri,int xo,int yo,int ro)
{
	FILE *fp=fopen("C:\\make\\log\\imgInfoClip.txt","a+");
	if(fp==NULL)
	{
		return;
	}
	fprintf(fp,"%d   %d   %d   %d   %d   %d\n",xi,yi,ri,xo,yo,ro);
	fclose(fp);
}
void writelog(int tid,char * msg)
{
	FILE *fp=fopen("C:\\make\\log\\logerrorriscplus.txt","a+");
	if(fp==NULL)
	{
		return;
	}
	fprintf(fp,"%d:%s \n",tid,msg);
	fclose(fp);
}
void writeslog(int tid, std::string msg)
{
	FILE *fp=fopen("C:\\make\\log\\logerrorriscplus.txt","a+");
	if(fp==NULL)
	{
		return;
	}
	fprintf(fp,"%d:%s \n",tid,const_cast<char*>(msg.c_str()));
	fclose(fp);
}
void writeslogTime(int tid,DWORD t)
{
	FILE *fp=fopen("C:\\make\\log\\time.txt","a+");
	if(fp==NULL)
	{
		return;
	}
	fprintf(fp,"%d: time cost = %d ms\n",tid,t);
	fclose(fp);
}


void getFiles( string path, vector<string>& files )  
{  
	//文件句柄  
	long   hFile   =   0;  
	//文件信息  
	struct _finddata_t fileinfo;  
	string p;  
	if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)  
	{  
		do  
		{  
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if((fileinfo.attrib &  _A_SUBDIR))  
			{  
				if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)  
					getFiles( p.assign(path).append("\\").append(fileinfo.name), files );  
			}  
			else  
			{  
				files.push_back(p.assign(path).append("\\").append(fileinfo.name) );  
			}  
		}while(_findnext(hFile, &fileinfo)  == 0);  
		_findclose(hFile);  
	}  
}











long Threshold(IplImage *src,long lWidth,long lHeight)
{
    long Count[256];
    unsigned char valb;
    for (int i = 0; i < 256; i++)
    {
        Count[i] = 0;
    }

    long i;
    for(i=0;i<lHeight;i++)
    {
        for(long j=0;j<lWidth;j++)
        {
           valb =((unsigned char *)(src->imageData+src->widthStep*i))[j];
            // 计数加1
            Count[valb]+=1;
        }
    }
    //对数组进行平滑运算
    Count[0]=0;
    Count[1]=(Count[1]+Count[0])/2;
    for(i=3;i<255;i++)
    {
        Count[i]=(Count[i]+Count[i-1]+Count[i+1])/3;
    }

    long difference[256];
    difference[0]=0;
    for(int i=1;i<256;i++)
        difference[i]=Count[i]-Count[i-1];

    unsigned char  threshold=0;//阈值点
    long max=Count[0];
    for(int i=1;i<256;i++)
    {
        if(max<Count[i])
            max=Count[i];
        if((difference[i]<0)&&(difference[i+1]>=0)&&(max/2>Count[i+1])&&max>200)
        {
            threshold=(unsigned char)i+1;
            break;
        }
     }
     return threshold;
}

void PupilContour(IplImage *lpRecorder,long lWidth,long lHeight,long threshold,IplImage * lpDIBBits)
{
    //图像每行的字节数
//	LONG lLineBytes;
//  LPSTR  lpSrc;
    unsigned char lpSrc;
    // 计算图像每行的字节数
//	lLineBytes=WIDTHBYTES(lWidth*8);
	

    for(long i=1;i<lHeight-1;i++)
    {
        for(long j=1;j<lWidth-1;j++)
        {
            //lpSrc=lpDIBBits+lLineBytes*i+j;
            lpSrc =((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*i))[j];
            unsigned char lpSrc_right=((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*i))[j+1];
            unsigned char lpSrc_left =((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*i))[j-1];
            unsigned char lpSrc_top  =((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(i+1)))[j];
            unsigned char lpSrc_bottom=((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(i-1)))[j];

            //该点的4个邻域
            //LPSTR lpSrc_right=lpDIBBits+lLineBytes*i+j+1;
            //LPSTR lpSrc_left=lpDIBBits+lLineBytes*i+j-1;
            //LPSTR lpSrc_top=lpDIBBits+lLineBytes*(i+1)+j;
            //LPSTR lpSrc_bottom=lpDIBBits+lLineBytes*(i-1)+j;
            if(lpSrc<threshold)
            {
                if(!((lpSrc_right<threshold)&&(lpSrc_left<threshold)
                    &&(lpSrc_top<threshold)&&(lpSrc_bottom<threshold)))//边界点
                        ((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*i))[j]=0;
                  //  *(lpRecorder+lLineBytes*i+j)=0;
            }
         }
    }
    for(long i=1;i<lHeight-1;i++)//去除边缘毛刺；
    {
        for(long j=1;j<lWidth-1;j++)
        {
            lpSrc=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*i))[j];
        //	lpSrc=lpRecorder+lLineBytes*i+j;
            if(lpSrc==(unsigned char)0)
            {
                char a1=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*i))[j+1];
                char a2=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*i))[j-1];
                char a3=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*(i+1)))[j];
                char a4=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*(i+1)))[j-1];
                char a5=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*(i+1)))[j+1];
                char a6=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*(i-1)))[j-1];
                char a8=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*(i-1)))[j];
                char a7=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*(i-1)))[j+1];

            //	char a1=*(lpRecorder+lLineBytes*i+j+1);
            //	char a2=*(lpRecorder+lLineBytes*i+j-1);
            //	char a3=*(lpRecorder+lLineBytes*(i+1)+j);
            //	char a4=*(lpRecorder+lLineBytes*(i+1)+j-1);
            //	char a5=*(lpRecorder+lLineBytes*(i+1)+j+1);
            //	char a6=*(lpRecorder+lLineBytes*(i-1)+j-1);
            //	char a8=*(lpRecorder+lLineBytes*(i-1)+j);
            //	char a7=*(lpRecorder+lLineBytes*(i-1)+j+1);
                unsigned char a=a1+a2+a3+a4+a5+a6+a7+a8;
                if(a>6)
                    ((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*i))[j]=1; //1
                   // *(lpRecorder+lLineBytes*i+j)=1;
            }
         }

    }
    for(long i=lHeight-2;i>0;i--)//去除边缘毛刺；
    {
         for(long j=lWidth-2;j>0;j--)
         {
            lpSrc=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*i))[j];
            //lpSrc=lpRecorder+lLineBytes*i+j;
            if(lpSrc==(unsigned char)0)
            {
                char a1=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*i))[j+1];
                char a2=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*i))[j-1];
                char a3=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*(i+1)))[j];
                char a4=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*(i+1)))[j-1];
                char a5=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*(i+1)))[j+1];
                char a6=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*(i-1)))[j-1];
                char a7=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*(i-1)))[j+1];
                char a8=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*(i-1)))[j];




            //	char a1=*(lpRecorder+lLineBytes*i+j+1);
            //	char a2=*(lpRecorder+lLineBytes*i+j-1);
            //	char a3=*(lpRecorder+lLineBytes*(i+1)+j);
            //	char a4=*(lpRecorder+lLineBytes*(i+1)+j-1);
            //	char a5=*(lpRecorder+lLineBytes*(i+1)+j+1);
            //	char a6=*(lpRecorder+lLineBytes*(i-1)+j-1);
            //	char a7=*(lpRecorder+lLineBytes*(i-1)+j+1);
            //	char a8=*(lpRecorder+lLineBytes*(i-1)+j);
                unsigned char a=a1+a2+a3+a4+a5+a6+a7+a8;
                if(a>6)
                    ((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*i))[j]=1;//1
                    //lpSrc=(unsigned char)1;
            }
         }
    }
}



void FindPHTPoint(IplImage* hSourceImage,std::list<PHTPoint> *PHTlist,long lHeight,long lWidth)
{
    //是否扫描到一个边界点
    bool bFindPoint;
   //是否跟踪到了一条线的末端；
    bool MeetTheEndOfLine;
    bool bFindStartPoint;
    //起始边界点与当前边界点
   CvPoint StartPoint,CurrentPoint;
    //八个方向和起始扫描方向
    int Direction[8][2]={{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0}};
    int BeginDirect;
//	LPSTR SourceImage=(char *)::GlobalLock(hSourceImage);
//    ::GlobalUnlock(hSourceImage);
    int LineNumber=0;//用于指定跟踪的线段编号；
    for(int j=1;j<lHeight-1;j++)//关注i、j与点x、y之间的关系
    {
        for(int i=1;i<lWidth-1;i++)
        {
            //先找到最左上方的边界点
            bFindStartPoint=false;//区分两种情况:1曲线又回到了起点
            MeetTheEndOfLine=false;//  2跟踪到一条直线的末端。
            int PointNumberOfThisType=0;//线段上的点的数目。
            if(((unsigned char *)(hSourceImage->imageData+hSourceImage->widthStep*j))[i]==0)
            {
               StartPoint=cvPoint(i,j);
               //由于起始点是在左下方，故起始扫描沿左上方向
               BeginDirect=0;
               //跟踪边界
               //从初始点开始扫描
               CurrentPoint.x=StartPoint.x;
               CurrentPoint.y=StartPoint.y;
               PHTPoint PhtPoint;
               PhtPoint.Point=StartPoint;
               PhtPoint.LineNumber=LineNumber;
               PHTlist->push_back(PhtPoint);
               //PHTlist->AddTail(PhtPoint);
               PointNumberOfThisType=1;
               int pixel_length=0;//此点是用来确定象素之间距，根据间距来确定选取PHT变换的点；
               while(!MeetTheEndOfLine&&!bFindStartPoint)
               {
                    bFindPoint=false;
                    int TurnNumber=0;//统计旋转次数的变量；如果是一条线段的端点，
                                 //则因在寻找过程中已经将跟踪过的点清零，此点为一孤立点；
                    while(!bFindPoint)
                    {
                        //沿扫描方向查看一个像素
           unsigned char a= ((unsigned char *)(hSourceImage->imageData+hSourceImage->widthStep*(CurrentPoint.y+Direction[BeginDirect][1])))[CurrentPoint.x+Direction[BeginDirect][0]];
          // unsigned char a=*(SourceImage+(CurrentPoint.y+Direction[BeginDirect][1])*lWidth+CurrentPoint.x+Direction[BeginDirect][0]);
                        if((CurrentPoint.y + Direction[BeginDirect][1]==StartPoint.y)&&(CurrentPoint.x
                            +Direction[BeginDirect][0]== StartPoint.x))
                        {
                            bFindStartPoint=true;
                            CurrentPoint.x=0;
                            CurrentPoint.y=0;
                            LineNumber++;
                            break;
                        }
                        if(a==0)
                        {
                            pixel_length++;
                            //此部分是用于寻找PHT变换的各个点的
                            if(pixel_length==distant_used_for_PHT)
                            {
                                pixel_length=0;
                                PhtPoint.Point=CurrentPoint;
                                PhtPoint.LineNumber=LineNumber;
                                PHTlist->push_back(PhtPoint);
                                PointNumberOfThisType++;
                            }
                            bFindPoint = true;
                            //找到下一点后将该点标志
                            ((unsigned char *)(hSourceImage->imageData+hSourceImage->widthStep*(CurrentPoint.y)))[CurrentPoint.x]=1;
                        //	*(SourceImage+(CurrentPoint.y)*lWidth+CurrentPoint.x)=1;
                            CurrentPoint.y=CurrentPoint.y+Direction[BeginDirect][1];
                            CurrentPoint.x=CurrentPoint.x+Direction[BeginDirect][0];
                            BeginDirect--;
                            if(BeginDirect==-1)
                                BeginDirect=7;
                            BeginDirect--;
                            if(BeginDirect==-1)
                                BeginDirect=7;
                        }
                        else
                        {
                            //扫描方向顺时针旋转一格
                            BeginDirect++;
                            if(BeginDirect==8)
                                BeginDirect=0;
                            TurnNumber++;
                            if(TurnNumber==8)
                            {
                                MeetTheEndOfLine=true;
                                LineNumber+=1;
                                ((unsigned char *)(hSourceImage->imageData+hSourceImage->widthStep*(CurrentPoint.y)))[CurrentPoint.x]=1;
                            //	*(SourceImage+(CurrentPoint.y)*lWidth+CurrentPoint.x)=1;
                                bFindPoint=true;
                            }

                        }
                    }//while(!bFindPoint)
                }//while(!MeetTheEndOfLine&&!bFindStartPoint)
                if(MeetTheEndOfLine||PointNumberOfThisType<4)//排除上述取圆规则误取的点集；
                {
                    for(i=0;i<PointNumberOfThisType;i++)
                        PHTlist->pop_back();
                       // PHTlist->RemoveTail();
                    LineNumber--;
                }
            }//if

        }//for
    }//for
    return ;
}

//std::list<PHTPoint> *PHTlist   CList<PHTPoint, PHTPoint&> * PHTlist

CIRCLE PHT(std::list<PHTPoint> *PHTlist)
{
	//long a=PHTlist->GetCount();
	  long a=PHTlist->size();
	  std::list<PHTPointGROUP>  PHTGroupList;
   //CList<PHTPointGROUP,PHTPointGROUP&> PHTGroupList;//储存用于PHT变换的点组的列表
	
	  std::list<PHTPoint>::iterator pos =PHTlist->begin();
	  std::list<PHTPoint>::iterator pos_begin=pos;
	  std::list<PHTPoint>::iterator pos_end;
//	POSITION pos=PHTlist->GetHeadPosition(); 
//	POSITION pos_begin=pos; 
//	POSITION pos_end;
	PHTPoint point=*pos;
	pos++;
	//PHTPoint point=PHTlist->GetNext(pos); 
	PHTPointGROUP PHTGroup;
	int UsefulPointNumber=1;
	int LineNumber=point.LineNumber;
	bool FinishCircle=false;	
	while(!FinishCircle)
	{ 
		bool MeetTheEnd=false;
		do
		{
			//point=PHTlist->GetNext(pos);
			  point=*pos;
			  pos++;
			  UsefulPointNumber++;
			if(pos == PHTlist->end())   //pos==NULL
			{
				MeetTheEnd=true;
				FinishCircle=true;
			}
			else
			{  
				if(point.LineNumber!=LineNumber) 
				{  
					LineNumber=point.LineNumber;
					MeetTheEnd=true;
					UsefulPointNumber--;
					pos--;
				//	PHTlist->GetPrev(pos);
				}
			}//else
		}while(!MeetTheEnd);
		pos_end=pos;
		int Step=UsefulPointNumber/3;
		pos=pos_begin;
		for(int i=0;i<UsefulPointNumber;i++)
		{
			PHTPoint point;
			//point=PHTlist->GetNext(pos);
			 point=*pos;
			pos++;
		}
		int i;
		for(i=0;i<UsefulPointNumber;i++)
		{
			PHTPoint point;
			point= *pos_begin;
			//point=PHTlist->GetAt(pos_begin);
			pos=pos_begin;
			for(int j=0;j<i;j++)
			{
			  point=*pos;    ////////
			  pos++;
			  //point=PHTlist->GetNext(pos);
			}
				
		    PHTGroup.a=point.Point;
            for(int j=0;j<Step;j++)
			{
			  point=*pos;
			  pos++;
				//point=PHTlist->GetNext(pos);
				if(pos == PHTlist->end()||pos==pos_end)   //pos == NULL
					pos=pos_begin;
			}
			 
            PHTGroup.b=point.Point;
			for(int j=0;j<Step;j++)
			{
				point=*pos;
			    pos++;
				//point=PHTlist->GetNext(pos);
				if(pos == PHTlist->end()||pos==pos_end)   //pos == NULL
					pos=pos_begin;
			}
			PHTGroup.c=point.Point;
			PHTGroupList.push_back(PHTGroup);
			//PHTGroupList.AddTail(PHTGroup);
		}
		UsefulPointNumber=1;
		pos=pos_end;
	}//while(!FinishCircle)


	  std::list<PHTPointGROUP>::iterator pos1 =PHTGroupList.begin();
	//pos = PHTGroupList.begin();
	a   = PHTGroupList.size();
	//pos=PHTGroupList.GetHeadPosition();
//	a=PHTGroupList.GetCount();
	// CArray and vector!!!!!!
    std::vector<CIRCLE> circle(a);
//	CArray<CIRCLE,CIRCLE&> circle;
//	circle.SetSize(a);
	int CircleNumber=0;
	while(pos1 != PHTGroupList.end())    //  pos!=NULL
	{
		//PHTPointGROUP PHTGroup=PHTGroupList.GetNext(pos);
		PHTPointGROUP PHTGroup=*pos1;
			  pos1++;
		if((PHTGroup.a.y-PHTGroup.b.y)==0||(PHTGroup.b.y-PHTGroup.c.y)==0)
			continue;
		//排除计算式分母为零的点组
		float k1=-(float)(PHTGroup.a.x-PHTGroup.b.x)/(float)(PHTGroup.a.y-PHTGroup.b.y);
		float k2=-(float)(PHTGroup.b.x-PHTGroup.c.x)/(float)(PHTGroup.b.y-PHTGroup.c.y);
		float d1=((PHTGroup.a.y+PHTGroup.b.y)-k1*(PHTGroup.a.x+PHTGroup.b.x))/2.0;
		float d2=((PHTGroup.b.y+PHTGroup.c.y)-k2*(PHTGroup.b.x+PHTGroup.c.x))/2.0;
		if(k1==k2)
			continue;
		float ac=-(d1-d2)/(k1-k2);
		float bc=k1*ac+d1;
		float  rc=sqrt(pow((PHTGroup.a.x-ac),2)+pow((PHTGroup.a.y-bc),2));
		circle[CircleNumber].x=(int)(ac+0.5); ////
		circle[CircleNumber].y=(int)(bc+0.5);////
		circle[CircleNumber].r=(int)(rc+0.5);////
        CircleNumber++;
	}//while
	std::vector<int> pointsum(CircleNumber);
//	CArray<int,int> pointsum;
//	pointsum.SetSize(CircleNumber);
	int i;
	for(i=0;i<CircleNumber;i++)
		pointsum[i]=0;
	for(i=0;i<CircleNumber;i++)
	{
		for (int j=0;j<CircleNumber;j++)
		{
			if(distance(circle[i],circle[j])<2.5) //
				pointsum[i]+=1;
		}
	}
	int max=0;
	for(i=1;i<CircleNumber;i++)
	{
		if (pointsum[i-1]<pointsum[i])
			max=i;
	}
	CIRCLE MaxCircle;
	MaxCircle.x=0;
	MaxCircle.y=0;
	MaxCircle.r=0;
	int counter=0;
	for(i=max;i<CircleNumber;i++)
	{
		if(pointsum[i]==pointsum[max])
		{
			MaxCircle.x+=circle[i].x;
			MaxCircle.y+=circle[i].y;
			MaxCircle.r+=circle[i].r;
			counter++;
		}
	}
	MaxCircle.x=(float)MaxCircle.x/(float)counter+0.5;
	MaxCircle.y=(float)MaxCircle.y/(float)counter+0.5;
	MaxCircle.r=(float)MaxCircle.r/(float)counter+0.5;
	return MaxCircle;
}


CIRCLE OuterCircle(IplImage * lpDIBBits,long lWidth,long lHeight,CIRCLE InnerCircle)
{ 
    CIRCLE MaxCircle,MaxCircle1;
	//最终确定的圆
	MaxCircle.r=0;
	MaxCircle.x=0;
	MaxCircle.y=0;
	//初步确定的圆
	MaxCircle1.x=0;
	MaxCircle1.y=0;
	MaxCircle1.r=0;
	float max=0;
	//图像每行的字节数
//	LONG lLineBytes;
	const int r_step=4;//每次搜索的半径步长即△t
	const int angle_step=1;//每次搜索的角度步长即△θ
	const int centre_range=5;//圆心变动范围
	//计算图像每行的字节数
//	lLineBytes=WIDTHBYTES(lWidth*8);
	for(long l=-centre_range;l<=centre_range;l++)//求外圆
	{
		for(long k=-centre_range;k<=centre_range;k++)
		{
			int from=InnerCircle.r+4*centre_range;//从外圆开始搜索搜索起始位置
			int to  = min(from+5*InnerCircle.r,min(InnerCircle.x+k,lWidth-InnerCircle.x-k));//搜索终止位置；
			int Array_Number=(to-from)/r_step;
			float * array_for_circle_integration=new float[Array_Number];//
			float * convolution=new float[Array_Number];//
			for(long m=0;m<Array_Number;m++)
			{
				array_for_circle_integration[m]=0;
				convolution[m]=0;
			}
			for(long r=from;r<to-r_step;r=r+r_step)//
			{    
				int sum_number=0;
			    for(int i=-r_step/2;i<r_step/2;i++)//
				{	
					for(float angle=30;angle>-30;angle=angle-angle_step)
					{   
						if(InnerCircle.y+l+(long)((r+i)*sin(angle/360*2*pi))<0||
							InnerCircle.y+l+(long)((r+i)*sin(angle/360*2*pi))>lHeight) 
							continue;
		
                         //unsigned char w=(unsigned char)*(lpDIBBits+(InnerCircle.y+l+(long)((r+i)*sin(angle/360*2*pi)))*lLineBytes+k+InnerCircle.x+(long)((r+i)*cos(angle/360*2*pi)));
		            
unsigned char w=((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(InnerCircle.y+l+(long)((r+i)*sin(angle/360*2*pi)))))[k+InnerCircle.x+(long)((r+i)*cos(angle/360*2*pi))];
						array_for_circle_integration[(r-from)/r_step]+=w;  
						sum_number++;
					}
				}
				for(int i=-r_step/2;i<r_step/2;i++)//
				{
					for(float angle=150;angle<210;angle=angle+angle_step)
					{    
						if(InnerCircle.y+l+(long)((r+i)*sin(angle/360*2*pi))<0||
							InnerCircle.y+l+(long)((r+i)*sin(angle/360*2*pi))>lHeight) 
							continue;
						////////////////
						                   
//                       array_for_circle_integration[(r-from)/r_step]+=(unsigned char)*(lpDIBBits+(InnerCircle.y+l+(long)((r+i)*sin(angle/360*2*pi)))*lLineBytes+k+InnerCircle.x+(long)((r+i)*cos(angle/360*2*pi)));
array_for_circle_integration[(r-from)/r_step]+=((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(InnerCircle.y+l+(long)((r+i)*sin(angle/360*2*pi)))))[k+InnerCircle.x+(long)((r+i)*cos(angle/360*2*pi))];
						sum_number++;
					}
				}
				array_for_circle_integration[(r-from)/r_step]=array_for_circle_integration[(r-from)/r_step]/sum_number;
			 }
			convolution[0]=(8*array_for_circle_integration[0]+array_for_circle_integration[1])/9.0;
			for(long r=from+r_step;r<to-2*r_step;r=r+r_step)//求卷积
			{    
				convolution[(r-from)/r_step]=(8*array_for_circle_integration[(r-from)/r_step]
				+array_for_circle_integration[(r-from-r_step)/r_step]+
				 array_for_circle_integration[(r-from+r_step)/r_step])/10.0;
			}
			convolution[(to-r_step-from)/r_step]=(8*array_for_circle_integration[(to-r_step-from)/r_step]
				+array_for_circle_integration[(to-2*r_step-from)/r_step])/9.0;
		     for(long i=1;i<(to-from-r_step)/r_step;i++)//环量积分zuizhi
			 {   
				float a=convolution[i]-convolution[i-1];
				if(a>max)
				{      
					max=a;
					MaxCircle1.r=from+(i-1)*r_step;
					MaxCircle1.x=InnerCircle.x+k;
					MaxCircle1.y=InnerCircle.y+l;
				}
			}
         delete[] array_for_circle_integration;
	     delete[] convolution;
		}//for
	}//for
	//return MaxCircle1;
	 //一下内容是是以一定步长初步求得最大圆之后，再在每一个象素内精细求解
	long * TheMax=new long[r_step];
	int sum_number=0;
	int i=0;
	for(long  r=MaxCircle1.r-r_step/2+1;r<MaxCircle1.r+r_step/2;r++)//环量积分
	{  
		for(float angle=30;angle>-30;angle=angle-angle_step)//左半部分
		{
			if(MaxCircle1.y+(long)(r*sin(angle/360*2*pi))<0||
				MaxCircle1.y+(long)(r*sin(angle/360*2*pi))>lHeight) 
					continue;
			///////
          
            TheMax[i]+=((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(MaxCircle1.y+(long)(r*sin(angle/360*2*pi)))))[MaxCircle1.x+(long)(r*cos(angle/360*2*pi))];
			                                     //TheMax[i]+=*(lpDIBBits+(MaxCircle1.y+(long)(r*sin(angle/360*2*pi)))*lLineBytes+ MaxCircle1.x+(long)(r*cos(angle/360*2*pi)));        
			sum_number++;
		}
		for(float angle=150;angle<210;angle=angle+angle_step)//右半部分
		{
			if(MaxCircle1.y+(long)(r*sin(angle/360*2*pi))<0||
				MaxCircle1.y+(long)(r*sin(angle/360*2*pi))>lHeight) 
					continue;

			////////     
            TheMax[i]+=((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(MaxCircle1.y+(long)(r*sin(angle/360*2*pi)))))[MaxCircle1.x+(long)(r*cos(angle/360*2*pi))];
		                                         //TheMax[i]+=*(lpDIBBits+(MaxCircle1.y+(long)(r*sin(angle/360*2*pi)))*lLineBytes+ MaxCircle1.x+(long)(r*cos(angle/360*2*pi)));  
			sum_number++;
		}
		TheMax[i]=TheMax[i]/sum_number;
		i++;
	}
	long differ=0;//寻找最大值即使求最大的MAX
	for(int j=1;j<=i;j++)
	{   
		if(abs(TheMax[j]-TheMax[j-1])>differ)
		{
			differ=TheMax[j]-TheMax[j-1];
			MaxCircle.x=MaxCircle1.x;
			MaxCircle.y=MaxCircle1.y;
			MaxCircle.r=MaxCircle1.r-r_step/2+1+j;
		}
	}
	delete[] TheMax;
	return MaxCircle;
}

//unsigned char DLInsAlgorithm(LPSTR lpDIB,float x,float y)
unsigned char DLInsAlgorithm(IplImage * lpDIBBits,float x,float y)
{ 
	//LPSTR lpDIBBits;
	long lWidth,lHeight;
	long i1,i2;
	long j1,j2;
	unsigned char f1,f2,f3,f4;
	unsigned char f12,f34;
	float LIMIT=0.0001;
	//long lLineBytes;
	//lpDIBBits=::FindDIBBits(lpDIB);
	lWidth=lpDIBBits->width;
	lHeight=lpDIBBits->height;

	//lWidth=::DIBWidth(lpDIB);
	//lHeight=::DIBHeight(lpDIB);
	//lLineBytes=WIDTHBYTES(lWidth*8);
	i1=(long)x;
	i2=i1+1;
	j1=(long)y;
	j2=j1+1;
	if((x<0)||(x>lWidth-1)||(y<0)||(y>lHeight-1))
	{
	return 255;
	}
	else
	{ 
		if(abs(x-lWidth+1)<=LIMIT)
		{
			if(abs(y-lHeight+1)<=LIMIT)
			{
		//		f1=*(lpDIBBits+lLineBytes*j1+i1);
				f1=((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j1))[i1];

				return (unsigned char) f1;
			}
			else
			{
			//	f1=*(lpDIBBits+lLineBytes*j1+i1);
				f1=((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j1))[i1];
		//		f3=*(lpDIBBits+lLineBytes*j1+i2);
				f3=((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j1))[i2];
				return (unsigned char) (f1+(y-j1)*(f3-f1));
			}
		}
		else if(abs(y-lHeight+1)<=LIMIT)
		{
			//f1=*(lpDIBBits+lLineBytes*j1+i1);
			f1=((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j1))[i1];
			//f2=*(lpDIBBits+lLineBytes*j2+i1);
			f2=((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j2))[i1];
			return (unsigned char) (f1+(x-i1)*(f2-f1));
		}
		else
		{
			//f1=(unsigned char )*(lpDIBBits+lLineBytes*j1+i1);
	          f1=((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j1))[i1];
			//f2=(unsigned char) *(lpDIBBits+lLineBytes*j2+i1);
	         f2=((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j2))[i1];
			//f3=(unsigned char) *(lpDIBBits+lLineBytes*j1+i2);
			 f3=((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j1))[i2];
			//f4=(unsigned char) *(lpDIBBits+lLineBytes*j2+i2);
			  f4=((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j2))[i2];
			f12=f1+(x-i1)*(f2-f1);
			f34=f3+(x-i1)*(f4-f3);
			return (unsigned char) (f12+(y-j1)*(f34-f12));
		}
	}
}




void TurnToRect(CIRCLE Inner, CIRCLE Outer,IplImage * lpDIB,IplImage * lpDst,long RectWidth,long RectHeight)
{
	long lWidth;
	long lHeight;
	//LPBITMAPINFOHEADER lpbmi;
	//LPSTR lpDIBBits,lpDst;
	//LPSTR lpNewDIBBits,lpNewDIB;IplImage *dst=cvCreateImage(cvSize(lWidth,lHeight),IPL_DEPTH_8U,1);
	//HDIB hDIB;
	float i,j;
	float i0,j0;
	//long lLineBytes;
	//long lNewLineBytes;
	//lpDIBBits=::FindDIBBits(lpDIB);
	//lWidth=::DIBWidth(lpDIB);
	//lHeight=::DIBHeight(lpDIB);
//	IplImage *lpNewDIB = cvCreateImage(cvSize(RectWidth,RectHeight),IPL_DEPTH_8U,1);  //需要释放！！！！
 //   IplImage * lpDst = cvCreateImage(cvSize(RectWidth,RectHeight),IPL_DEPTH_8U,1);


		
   // lLineBytes = WIDTHBYTES(lWidth * 8);
	//lNewLineBytes= WIDTHBYTES(RectWidth * 8);
	//hDIB=(HDIB)::GlobalAlloc(GHND,RectHeight*lNewLineBytes+*(LPDWORD)lpDIB+::PaletteSize(lpDIB));
	/*
	if(!hDIB)
	{ 
		AfxMessageBox("创建新内存失败!");
		return NULL;
	}
	lpNewDIB=(LPSTR)::GlobalLock((HGLOBAL)hDIB);
	memcpy(lpNewDIB,lpDIB,*(LPDWORD)lpDIB+::PaletteSize(lpDIB));
	lpbmi=(LPBITMAPINFOHEADER)lpNewDIB;
	lpbmi->biWidth=RectWidth;
	lpbmi->biHeight=RectHeight;
	::GlobalUnlock((HGLOBAL)hDIB);
	lpNewDIBBits=::FindDIBBits(lpNewDIB);
	*/
	if(Inner.x==Outer.x)//内圆圆心在x轴上；
	{ 
		for (i=0.0;i<RectWidth;i++)
		{
			int x1=Inner.x+Inner.r*cos((i/(RectWidth-1))*2*pi);
			int y1=Inner.y+Inner.r*sin((i/(RectWidth-1))*2*pi);
			float r0=sqrt(pow((double)(Inner.x-Outer.x),2)+pow((double)(Inner.y-Outer.y),2));
			float q1;
			if(i/RectWidth<=1/4)
				q1=(i/RectWidth)*2*pi+pi/2;
			else if(1/4<i/RectWidth<=1/2)
				q1=pi-(i/RectWidth)*2*pi+pi/2;
			else if(1/2<i/RectWidth<=3/4)
				q1=3/2*pi-(i/RectWidth)*2*pi;
			else 
				q1=(i/RectWidth)*2*pi-pi   ;
			float q2=asin((r0*sin(q1))/Outer.r);
			float Ia=sqrt(pow((double)r0,2)+pow((double)Outer.r,2)-2*r0*Outer.r*cos(pi-q1-q2));
			int x2=Inner.x+Ia*cos((i/(RectWidth-1))*2*pi);
			int y2=Inner.y+Ia*sin((i/(RectWidth-1))*2*pi);
            for(j=0.0;j<RectHeight;j++)
		    {      		 
				float lamuda=j/(RectHeight-1);
				i0=(1-lamuda)*x1+lamuda*x2;
				j0=(1-lamuda)*y1+lamuda*y2;
			//	lpDst=lpNewDIBBits+lNewLineBytes*((long)j)+(long)i;  
		
			//    ((unsigned char *)(lpDst->imageData+lpDst->widthStep*((long)j)))[(long)i]=((unsigned char *)(lpNewDIB->imageData+lpNewDIB->widthStep*((long)j)))[(long)i];
				((unsigned char *)(lpDst->imageData+lpDst->widthStep*((long)j)))[(long)i]=DLInsAlgorithm(lpDIB,i0,j0);
			}
		}
	}
	if(Inner.x<Outer.x&&Inner.y>=Outer.y)//内圆圆心在第二象限；
	{
		for (i=0.0;i<RectWidth;i++)
		{
			int x1=Inner.x+Inner.r*cos((i/(RectWidth-1))*2*pi);
			int y1=Inner.y+Inner.r*sin((i/(RectWidth-1))*2*pi);
			float q1=atan((double)abs((Inner.y-Outer.y)/(Inner.x-Outer.x)));
			float r0=sqrt(pow((double)(Inner.x-Outer.x),2)+pow((double)(Inner.y-Outer.y),2));
			float q=(i/(RectWidth-1))*2*pi+q1;
			if(q>pi)
				q=2*pi-q;
			if(q<0)
			q=-q;
			float q2=asin(r0*sin(q)/Outer.r);
			float Ia=sqrt(pow((double)r0,2)+pow((double)Outer.r,2)-2*r0*Outer.r*cos(pi-q-q2));
			int x2=Inner.x+Ia*cos((i/(RectWidth-1))*2*pi);
			int y2=Inner.y+Ia*sin((i/(RectWidth-1))*2*pi);
			for(j=0.0;j<RectHeight;j++) 
		    {   
				float lamuda=j/(RectHeight-1);
				i0=(1-lamuda)*x1+lamuda*x2;
				j0=(1-lamuda)*y1+lamuda*y2;

				//lpDst=lpNewDIBBits+lNewLineBytes*((long)j)+(long)i;
			//	((unsigned char *)(lpDst->imageData+lpDst->widthStep*((long)j)))[(long)i]=((unsigned char *)(lpNewDIB->imageData+lpNewDIB->widthStep*((long)j)))[(long)i];
			unsigned char Dli=DLInsAlgorithm(lpDIB,i0,j0);   //add!!!
				((unsigned char *)(lpDst->imageData+lpDst->widthStep*((long)j)))[(long)i]=DLInsAlgorithm(lpDIB,i0,j0);
				
			
			}
		}
	}
	if(Inner.x>Outer.x&&Inner.y>=Outer.y)//内圆圆心在第一象限；
	{  
		for (i=0.0;i<RectWidth;i++)
		{       
			int x1=Inner.x+Inner.r*cos((i/(RectWidth-1))*2*pi);
			int y1=Inner.y+Inner.r*sin((i/(RectWidth-1))*2*pi);
			float q1=atan((double)abs((Inner.y-Outer.y)/(Inner.x-Outer.x)));
			float r0=sqrt(pow((double)(Inner.x-Outer.x),2)+pow((double)(Inner.y-Outer.y),2));
			float q=(i/(RectWidth-1))*2*pi+q1;
			if(q<0)
				q=-q; 
			float q2=asin(r0*sin(q)/Outer.r);
			float Ia=sqrt(pow((double)r0,2)+pow((double)Outer.r,2)-2*r0*Outer.r*cos(pi-q2-q));
			int x2=Inner.x+Ia*cos((i/(RectWidth-1))*2*pi);
			int y2=Inner.y+Ia*sin((i/(RectWidth-1))*2*pi);
			for(j=0.0;j<RectHeight;j++)
			{      
				float lamuda=j/(RectHeight-1);
				i0=(1-lamuda)*x1+lamuda*x2;
				j0=(1-lamuda)*y1+lamuda*y2;

			   //lpDst=lpNewDIBBits+lNewLineBytes*((long)j)+(long)i;
			//	((unsigned char *)(lpDst->imageData+lpDst->widthStep*((long)j)))[(long)i]=((unsigned char *)(lpNewDIB->imageData+lpNewDIB->widthStep*((long)j)))[(long)i];
				((unsigned char *)(lpDst->imageData+lpDst->widthStep*((long)j)))[(long)i]=DLInsAlgorithm(lpDIB,i0,j0);
			}
		}
	}
	if(Inner.x<Outer.x&&Inner.y<=Outer.y)//内圆圆心在第三象限；
	{
		for (i=0.0;i<RectWidth;i++)
		{
			int x1=Inner.x+Inner.r*cos((i/(RectWidth-1))*2*pi);
			int y1=Inner.y+Inner.r*sin((i/(RectWidth-1))*2*pi);
			float q1=atan((double)abs((Inner.y-Outer.y)/(Inner.x-Outer.x)));
			float r0=sqrt(pow((double)(Inner.x-Outer.x),2)+pow((double)(Inner.y-Outer.y),2));
			float q=(i/(RectWidth-1))*2*pi-q1;
			if(q<0)
				q=-q;
			if(q>pi)
				q=2*pi-q;
			float q2=asin(r0*sin(q)/Outer.r);
			float Ia=sqrt(pow((double)r0,2)+pow((double)Outer.r,2)-2*r0*Outer.r*cos(pi-q-q2));
			int x2=Inner.x+Ia*cos((i/(RectWidth-1))*2*pi);
			int y2=Inner.y+Ia*sin((i/(RectWidth-1))*2*pi);
			for(j=0.0;j<RectHeight;j++) 
			{    
				float lamuda=j/(RectHeight-1);
				i0=(1-lamuda)*x1+lamuda*x2;
				j0=(1-lamuda)*y1+lamuda*y2;

				//lpDst=lpNewDIBBits+lNewLineBytes*((long)j)+(long)i;
			//	((unsigned char *)(lpDst->imageData+lpDst->widthStep*((long)j)))[(long)i] = ((unsigned char *)(lpNewDIB->imageData+lpNewDIB->widthStep*((long)j)))[(long)i];
				((unsigned char *)(lpDst->imageData+lpDst->widthStep*((long)j)))[(long)i] = DLInsAlgorithm(lpDIB,i0,j0);	       
			}
		}
	
	}
	if(Inner.x>Outer.x&&Inner.y<=Outer.y)//内圆圆心在第四象限；
	{
		for (i=0.0;i<RectWidth;i++)
		{
			int x1=Inner.x+Inner.r*cos((i/(RectWidth-1))*2*pi);
			int y1=Inner.y+Inner.r*sin((i/(RectWidth-1))*2*pi); 
			float q1=atan((double)abs((Inner.y-Outer.y)/(Inner.x-Outer.x)));
			float r0=sqrt(pow((double)(Inner.x-Outer.x),2)+pow((double)(Inner.y-Outer.y),2));
			float q=pi-( i/(RectWidth-1))*2*pi-q1;
			if(q<0)
				q=-q;
			float q2=asin(r0*sin(q)/Outer.r);
			float Ia=sqrt(pow((double)r0,2)+pow((double)Outer.r,2)-2*r0*Outer.r*cos(pi-q-q2));
			int x2=Inner.x+Ia*cos((i/(RectWidth-1))*2*pi);
			int y2=Inner.y+Ia*sin((i/(RectWidth-1))*2*pi);
		    for(j=0.0;j<RectHeight;j++)
		    {   
				float lamuda=j/(RectHeight-1);
				i0=(1-lamuda)*x1+lamuda*x2;
				j0=(1-lamuda)*y1+lamuda*y2;
			   //lpDst=lpNewDIBBits+lNewLineBytes*((long)j)+(long)i;
			//	((unsigned char *)(lpDst->imageData+lpDst->widthStep*((long)j)))[(long)i]=((unsigned char *)(lpNewDIB->imageData+lpNewDIB->widthStep*((long)j)))[(long)i];
				((unsigned char *)(lpDst->imageData+lpDst->widthStep*((long)j)))[(long)i]=DLInsAlgorithm(lpDIB,i0,j0);   
			}
		}
	}
	//return lpDst;
}


void LOGBin(IplImage * lpDIBBits)
{
	//LPSTR lpDIB=(LPSTR)::GlobalLock((HGLOBAL)hDIB);
	//LPSTR lpDIBBits=::FindDIBBits(lpDIB);
	//::GlobalUnlock((HGLOBAL)hDIB);
	const long lWidth=512;
	const long lHeight=128;
    //图像每行的字节数
	//LONG lLineBytes;
	//计算图像每行的字节数F
	//lLineBytes=WIDTHBYTES(lWidth*8);
	           //lpSrc=lpDIBBits+lLineBytes*i+j;
        //    ((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[0];
	float(*LoG)[lWidth]=new float[lHeight][lWidth];
	for(long j=0;j<lHeight;j++)
	{
		LoG[j][0]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[0])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[1])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[2])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[3])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[4])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[5])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[6])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[7])*(+0.0855);
		LoG[j][1]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[1])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[2])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[3])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[4])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[5])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[6])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[7])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[8])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[0])*(-0.7420);
		LoG[j][2]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[2])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[3])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[4])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[5])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[6])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[7])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[8])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[9])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[1])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[0])*(-0.1771);
		   LoG[j][3]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[3])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[4])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[5])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[6])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[7])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[8])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[9])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[10])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[2])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[1])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[0])*(0.2913);
		LoG[j][4]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[4])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[5])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[6])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[7])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[8])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[9])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[10])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[11])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[3])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[2])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[1])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[0])*(0.4462);
		LoG[j][5]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[5])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[6])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[7])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[8])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[9])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[10])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[11])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[12])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[4])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[3])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[2])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[1])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[0])*(0.3573);
		LoG[j][6]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[6])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[7])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[8])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[9])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[10])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[11])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[12])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[13])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[5])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[4])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[3])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[2])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[1])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[0])*(0.2007);
		for(long i=7;i<lWidth-7;i++)
		{
			LoG[j][i]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i+1])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i+2])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i+3])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i+4])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i+5])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i+6])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i+7])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i-1])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i-2])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i-3])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i-4])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i-5])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i-6])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i-7])*(0.0855);		
		}
		LoG[j][lWidth-1]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-1])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-2])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-3])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-4])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-5])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-6])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-7])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-8])*(+0.0855);
		LoG[j][lWidth-2]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-2])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-3])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-4])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-5])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-6])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-7])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-8])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-9])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-1])*(-0.7420);
		LoG[j][lWidth-3]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-3])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-4])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-5])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-6])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-7])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-8])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-9])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-10])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-2])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-1])*(-0.1771);
		LoG[j][lWidth-4]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-4])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-5])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-6])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-7])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-8])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-9])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-10])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-11])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-3])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-2])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-1])*(0.2913);
		LoG[j][lWidth-5]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-5])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-6])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-7])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-8])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-9])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-10])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-11])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-12])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-4])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-3])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-2])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-1])*(0.4462);
		LoG[j][lWidth-6]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-6])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-7])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-8])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-9])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-10])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-11])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-12])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-13])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-5])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-4])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-3])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-2])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-1])*(0.3573);
		LoG[j][lWidth-7]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-7])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-8])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-9])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-10])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-11])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-12])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-13])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-14])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-6])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-5])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-4])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-3])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-2])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[lWidth-1])*(0.2007);
	}
	float (*LoGG)[lWidth]=new float[lHeight][lWidth];
	long i;
    for( i=0;i<lWidth;i++)
	{
		LoGG[0][i]=LoG[0][i]+0.9113*LoG[1][i]+0.6896*LoG[2][i]+0.4334*LoG[3][i]+0.2262*LoG[4][i]+0.0980*LoG[5][i]+0.0353*LoG[6][i]+0.0105*LoG[7][i];
		LoGG[1][i]=LoG[1][i]+0.9113*LoG[2][i]+0.6896*LoG[3][i]+0.4334*LoG[4][i]+0.2262*LoG[5][i]+0.0980*LoG[6][i]+0.0353*LoG[7][i]+0.0105*LoG[8][i]
		           +0.9113*LoG[0][i];
		LoGG[2][i]=LoG[2][i]+0.9113*LoG[3][i]+0.6896*LoG[4][i]+0.4334*LoG[5][i]+0.2262*LoG[6][i]+0.0980*LoG[7][i]+0.0353*LoG[8][i]+0.0105*LoG[9][i]+
					0.9113*LoG[1][i]+0.6896*LoG[0][i];
		LoGG[3][i]=LoG[3][i]+0.9113*LoG[4][i]+0.6896*LoG[5][i]+0.4334*LoG[6][i]+0.2262*LoG[7][i]+0.0980*LoG[8][i]+0.0353*LoG[9][i]+0.0105*LoG[10][i]+
					0.9113*LoG[2][i]+0.6896*LoG[1][i]+0.4334*LoG[0][i];
		LoGG[4][i]=LoG[4][i]+0.9113*LoG[5][i]+0.6896*LoG[6][i]+0.4334*LoG[7][i]+0.2262*LoG[8][i]+0.0980*LoG[9][i]+0.0353*LoG[10][i]+0.0105*LoG[11][i]+
					0.9113*LoG[3][i]+0.6896*LoG[2][i]+0.4334*LoG[1][i]+0.2262*LoG[0][i];
		LoGG[5][i]=LoG[5][i]+0.9113*LoG[6][i]+0.6896*LoG[7][i]+0.4334*LoG[8][i]+0.2262*LoG[9][i]+0.0980*LoG[10][i]+0.0353*LoG[11][i]+0.0105*LoG[12][i]+
					0.9113*LoG[4][i]+0.6896*LoG[3][i]+0.4334*LoG[2][i]+0.2262*LoG[1][i]+0.0980*LoG[0][i];
	    LoGG[6][i]=LoG[6][i]+0.9113*LoG[7][i]+0.6896*LoG[8][i]+0.4334*LoG[9][i]+0.2262*LoG[10][i]+0.0980*LoG[11][i]+0.0353*LoG[12][i]+0.0105*LoG[13][i]+
					0.9113*LoG[5][i]+0.6896*LoG[4][i]+0.4334*LoG[3][i]+0.2262*LoG[2][i]+0.0980*LoG[1][i]+0.0353*LoG[0][i];
	    for(long j=7;j<lHeight-7;j++)
		{  
			LoGG[j][i]=LoG[j][i]+0.9113*LoG[j+1][i]+0.6896*LoG[j+2][i]+0.4334*LoG[j+3][i]+0.2262*LoG[j+4][i]+0.0980*LoG[j+5][i]+0.0353*LoG[j+6][i]+0.0105*LoG[j+7][i]+
		         0.9113*LoG[j-1][i]+0.6896*LoG[j-2][i]+0.4334*LoG[j-3][i]+0.2262*LoG[j-4][i]+0.0980*LoG[j-5][i]+0.0353*LoG[j-6][i]+0.0105*LoG[j-7][i];
		
		}
		LoGG[lHeight-1][i]=LoG[lHeight-1][i]+0.9113*LoG[lHeight-2][i]+0.6896*LoG[lHeight-3][i]+0.4334*LoG[lHeight-4][i]+0.2262*LoG[lHeight-5][i]+0.0980*LoG[lHeight-6][i]+0.0353*LoG[lHeight-7][i]+0.0105*LoG[lHeight-8][i];
		LoGG[lHeight-2][i]=LoG[lHeight-2][i]+0.9113*LoG[lHeight-3][i]+0.6896*LoG[lHeight-4][i]+0.4334*LoG[lHeight-5][i]+0.2262*LoG[lHeight-6][i]+0.0980*LoG[lHeight-7][i]+0.0353*LoG[lHeight-8][i]+0.0105*LoG[lHeight-9][i]
							+0.9113*LoG[lHeight-1][i];
		LoGG[lHeight-3][i]=LoG[lHeight-3][i]+0.9113*LoG[lHeight-4][i]+0.6896*LoG[lHeight-5][i]+0.4334*LoG[lHeight-6][i]+0.2262*LoG[lHeight-7][i]+0.0980*LoG[lHeight-8][i]+0.0353*LoG[lHeight-9][i]+0.0105*LoG[lHeight-10][i]+
							0.9113*LoG[lHeight-2][i]+0.6896*LoG[lHeight-1][i];
		LoGG[lHeight-4][i]=LoG[lHeight-4][i]+0.9113*LoG[lHeight-5][i]+0.6896*LoG[lHeight-6][i]+0.4334*LoG[lHeight-7][i]+0.2262*LoG[lHeight-8][i]+0.0980*LoG[lHeight-9][i]+0.0353*LoG[lHeight-10][i]+0.0105*LoG[lHeight-11][i]+
							0.9113*LoG[lHeight-3][i]+0.6896*LoG[lHeight-2][i]+0.4334*LoG[lHeight-1][i];
		LoGG[lHeight-5][i]=LoG[lHeight-5][i]+0.9113*LoG[lHeight-6][i]+0.6896*LoG[lHeight-7][i]+0.4334*LoG[lHeight-8][i]+0.2262*LoG[lHeight-9][i]+0.0980*LoG[lHeight-10][i]+0.0353*LoG[lHeight-11][i]+0.0105*LoG[lHeight-12][i]+
							0.9113*LoG[lHeight-4][i]+0.6896*LoG[lHeight-3][i]+0.4334*LoG[lHeight-2][i]+0.2262*LoG[lHeight-1][i];
		LoGG[lHeight-6][i]=LoG[lHeight-6][i]+0.9113*LoG[lHeight-7][i]+0.6896*LoG[lHeight-8][i]+0.4334*LoG[lHeight-9][i]+0.2262*LoG[lHeight-10][i]+0.0980*LoG[lHeight-11][i]+0.0353*LoG[lHeight-12][i]+0.0105*LoG[lHeight-13][i]+
							0.9113*LoG[lHeight-5][i]+0.6896*LoG[lHeight-4][i]+0.4334*LoG[lHeight-3][i]+0.2262*LoG[lHeight-2][i]+0.0980*LoG[lHeight-1][i];
	    LoGG[lHeight-7][i]=LoG[lHeight-7][i]+0.9113*LoG[lHeight-8][i]+0.6896*LoG[lHeight-9][i]+0.4334*LoG[lHeight-10][i]+0.2262*LoG[lHeight-11][i]+0.0980*LoG[lHeight-12][i]+0.0353*LoG[lHeight-13][i]+0.0105*LoG[lHeight-14][i]+
							0.9113*LoG[lHeight-6][i]+0.6896*LoG[lHeight-5][i]+0.4334*LoG[lHeight-4][i]+0.2262*LoG[lHeight-3][i]+0.0980*LoG[lHeight-2][i]+0.0353*LoG[lHeight-1][i];
	}
	float LoGy[lHeight][lWidth];
	for(long i=0;i<lWidth;i++)
	{
		LoGy[0][i]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*0))[i])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*1))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*2))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*3))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*4))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*5))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*6))[i])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*7))[i])*(+0.0855);
		LoGy[1][i]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*1))[i])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*2))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*3))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*4))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*5))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*6))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*7))[i])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*8))[i])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*0))[i])*(-0.7420);
		LoGy[2][i]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*2))[i])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*3))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*4))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*5))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*6))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*7))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*8))[i])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*9))[i])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*0))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*1))[i])*(-0.1771);
		LoGy[3][i]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*3))[i])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*4))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*5))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*6))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*7))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*8))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*9))[i])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*10))[i])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*0))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*1))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*2))[i])*(0.2913);
		LoGy[4][i]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*4))[i])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*5))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*6))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*7))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*8))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*9))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*10))[i])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*11))[i])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*1))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*1))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*2))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*3))[i])*(0.4462);
		LoGy[5][i]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*5))[i])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*6))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*7))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*8))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*9))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*10))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*11))[i])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*12))[i])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*1))[0])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*1))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*2))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*3))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*4))[i])*(0.3573);
		LoGy[6][i]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*6))[i])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*7))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*8))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*9))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*10))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*11))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*12))[i])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*13))[i])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*14))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*5))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*4))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*3))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*2))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*1))[i])*(0.2007);
		for(long j=7;j<lHeight-7;j++)
		{
			                  
           LoGy[j][i]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(j+1)))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(j+2)))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(j+3)))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(j+4)))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(j+5)))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(j+6)))[i])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(j+7)))[i])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(j-1)))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(j-2)))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(j-3)))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(j-4)))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(j-5)))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(j-6)))[i])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(j-7)))[i])*(0.0855);
		}
		LoGy[lHeight-1][i]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-1)))[i])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-2)))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-3)))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-4)))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-5)))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-6)))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-7)))[i])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-8)))[i])*(+0.0855);
		LoGy[lHeight-2][i]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-2)))[i])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-3)))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-4)))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-5)))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-6)))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-7)))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-8)))[i])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-9)))[i])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-1)))[i])*(-0.7420);
		LoGy[lHeight-3][i]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-3)))[i])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-4)))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-5)))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-6)))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-7)))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-8)))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-9)))[i])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-10)))[i])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-2)))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-1)))[i])*(-0.1771);
		LoGy[lHeight-4][i]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-4)))[i])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-5)))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-6)))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-7)))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-8)))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-9)))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-10)))[i])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-11)))[i])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-3)))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-2)))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-1)))[i])*(0.2913);
		LoGy[lHeight-5][i]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-5)))[i])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-6)))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-7)))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-8)))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-9)))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-10)))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-11)))[i])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-12)))[i])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-4)))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-3)))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-2)))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-1)))[i])*(0.4462);
		LoGy[lHeight-6][i]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-6)))[i])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-7)))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-8)))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-9)))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-10)))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-11)))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-12)))[i])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-13)))[i])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-5)))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-4)))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-3)))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-2)))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-1)))[i])*(0.3573);
		LoGy[lHeight-7][i]=(((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-7)))[i])*(-1)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-8)))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-9)))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-10)))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-11)))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-12)))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-13)))[i])*(0.2007)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-14)))[i])*(+0.0855)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-6)))[i])*(-0.7420)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-5)))[i])*(-0.1771)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-4)))[i])*(0.2913)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-3)))[i])*(0.4462)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-2)))[i])*(0.3573)+
		  (((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*(lHeight-1)))[i])*(0.2007);

	}
	float LoGGy[lHeight][lWidth];
    for(long j=0;j<lHeight;j++)
	{
		LoGGy[j][0]=LoGy[j][0]+0.9113*LoGy[j][1]+0.6896*LoGy[j][2]+0.4334*LoGy[j][3]+0.2262*LoGy[j][4]+0.0980*LoGy[j][5]+0.0353*LoGy[j][6]+0.0105*LoGy[j][7];
		LoGGy[j][1]=LoGy[j][1]+0.9113*LoGy[j][2]+0.6896*LoGy[j][3]+0.4334*LoGy[j][4]+0.2262*LoGy[j][5]+0.0980*LoGy[j][6]+0.0353*LoGy[j][7]+0.0105*LoGy[j][8]
					+0.9113*LoGy[j][0];
		LoGGy[j][2]=LoGy[j][2]+0.9113*LoGy[j][3]+0.6896*LoGy[j][4]+0.4334*LoGy[j][5]+0.2262*LoGy[j][6]+0.0980*LoGy[j][7]+0.0353*LoGy[j][8]+0.0105*LoGy[j][9]+
					0.9113*LoGy[j][1]+0.6896*LoGy[j][0];
		LoGGy[j][3]=LoGy[j][3]+0.9113*LoGy[j][4]+0.6896*LoGy[j][5]+0.4334*LoGy[j][6]+0.2262*LoGy[j][7]+0.0980*LoGy[j][8]+0.0353*LoGy[j][9]+0.0105*LoGy[j][10]+
					0.9113*LoGy[j][2]+0.6896*LoGy[j][1]+0.4334*LoGy[j][0];
		LoGGy[j][4]=LoGy[j][4]+0.9113*LoGy[j][5]+0.6896*LoGy[j][6]+0.4334*LoGy[j][7]+0.2262*LoGy[j][8]+0.0980*LoGy[j][9]+0.0353*LoGy[j][10]+0.0105*LoGy[j][11]+
					0.9113*LoGy[j][3]+0.6896*LoGy[j][2]+0.4334*LoGy[j][1]+0.2262*LoGy[j][0];
		LoGGy[j][5]=LoGy[j][5]+0.9113*LoGy[j][6]+0.6896*LoGy[j][7]+0.4334*LoGy[j][8]+0.2262*LoGy[j][9]+0.0980*LoGy[j][10]+0.0353*LoGy[j][11]+0.0105*LoGy[j][12]+
					0.9113*LoGy[j][4]+0.6896*LoGy[j][3]+0.4334*LoGy[j][2]+0.2262*LoGy[j][1]+0.0980*LoGy[j][0];
	    LoGGy[j][6]=LoGy[j][6]+0.9113*LoGy[j][7]+0.6896*LoGy[j][8]+0.4334*LoGy[j][9]+0.2262*LoGy[j][10]+0.0980*LoGy[j][11]+0.0353*LoGy[j][12]+0.0105*LoGy[j][13]+
					0.9113*LoGy[j][5]+0.6896*LoGy[j][4]+0.4334*LoGy[j][3]+0.2262*LoGy[j][2]+0.0980*LoGy[j][1]+0.0353*LoGy[j][0];
	    long i=7;
	    for(i=7;i<lWidth-7;i++)
		{  
			LoGGy[j][i]=LoGy[j][i]+0.9113*LoGy[j][i+1]+0.6896*LoGy[j][i+2]+0.4334*LoGy[j][i+3]+0.2262*LoGy[j][i+4]+0.0980*LoGy[j][i+5]+0.0353*LoGy[j][i+6]+0.0105*LoGy[j][i+7]+
		         0.9113*LoGy[j][i-1]+0.6896*LoGy[j][i-2]+0.4334*LoGy[j][i-3]+0.2262*LoGy[j][i-4]+0.0980*LoGy[j][i-5]+0.0353*LoGy[j][i-6]+0.0105*LoGy[j][i-7];
		}
		LoGGy[j][lWidth-1]=LoGy[j][lWidth-1]+0.9113*LoGy[j][lWidth-2]+0.6896*LoGy[j][lWidth-3]+0.4334*LoGy[j][lWidth-4]+0.2262*LoGy[j][lWidth-5]+0.0980*LoGy[j][lWidth-6]+0.0353*LoGy[j][lWidth-7]+0.0105*LoGy[j][lWidth-8];
		LoGGy[j][lWidth-2]=LoGy[j][lWidth-2]+0.9113*LoGy[j][lWidth-3]+0.6896*LoGy[j][lWidth-4]+0.4334*LoGy[j][lWidth-5]+0.2262*LoGy[j][lWidth-6]+0.0980*LoGy[j][lWidth-7]+0.0353*LoGy[j][lWidth-8]+0.0105*LoGy[j][lWidth-9]
							+0.9113*LoGy[j][lWidth-1];
		LoGGy[j][lWidth-3]=LoGy[j][lWidth-3]+0.9113*LoGy[j][lWidth-4]+0.6896*LoGy[j][lWidth-5]+0.4334*LoGy[j][lWidth-6]+0.2262*LoGy[j][lWidth-7]+0.0980*LoGy[j][lWidth-8]+0.0353*LoGy[j][lWidth-9]+0.0105*LoGy[j][lWidth-10]+
							0.9113*LoGy[j][lWidth-2]+0.6896*LoGy[j][lWidth-1];
		LoGGy[j][lWidth-4]=LoGy[j][lWidth-4]+0.9113*LoGy[j][lWidth-5]+0.6896*LoGy[j][lWidth-6]+0.4334*LoGy[j][lWidth-7]+0.2262*LoGy[j][lWidth-8]+0.0980*LoGy[j][lWidth-9]+0.0353*LoGy[j][lWidth-10]+0.0105*LoGy[j][lWidth-11]+
							0.9113*LoGy[j][lWidth-3]+0.6896*LoGy[j][lWidth-2]+0.4334*LoGy[j][lWidth-1];
		LoGGy[j][lWidth-5]=LoGy[j][lWidth-5]+0.9113*LoGy[j][lWidth-6]+0.6896*LoGy[j][lWidth-7]+0.4334*LoGy[j][lWidth-8]+0.2262*LoGy[j][lWidth-9]+0.0980*LoGy[j][lWidth-10]+0.0353*LoGy[j][lWidth-11]+0.0105*LoGy[j][lWidth-12]+
							0.9113*LoGy[j][lWidth-4]+0.6896*LoGy[j][lWidth-3]+0.4334*LoGy[j][lWidth-2]+0.2262*LoGy[j][lWidth-1];
        LoGGy[j][lWidth-6]=LoGy[j][lWidth-6]+0.9113*LoGy[j][lWidth-7]+0.6896*LoGy[j][lWidth-8]+0.4334*LoGy[j][lWidth-9]+0.2262*LoGy[lHeight-10][i]+0.0980*LoGy[lHeight-11][i]+0.0353*LoGy[lHeight-12][i]+0.0105*LoGy[lHeight-13][i]+
							0.9113*LoGy[j][lWidth-5]+0.6896*LoGy[j][lWidth-4]+0.4334*LoGy[j][lWidth-3]+0.2262*LoGy[j][lWidth-2]+0.0980*LoGy[j][lWidth-1];
		LoGGy[j][lWidth-7]=LoGy[j][lWidth-7]+0.9113*LoGy[j][lWidth-8]+0.6896*LoGy[j][lWidth-9]+0.4334*LoGy[j][lWidth-10]+0.2262*LoGy[j][lWidth-11]+0.0980*LoGy[j][lWidth-12]+0.0353*LoGy[j][lWidth-13]+0.0105*LoGy[j][lWidth-14]+
							0.9113*LoGy[j][lWidth-6]+0.6896*LoGy[j][lWidth-5]+0.4334*LoGy[j][lWidth-4]+0.2262*LoGy[j][lWidth-3]+0.0980*LoGy[j][lWidth-2]+0.0353*LoGy[j][lWidth-1];
	}
	for(long j=0;j<lHeight;j++)
	{
		for(long i=0;i<lWidth;i++)
		{
			LoGG[j][i]=LoGG[j][i]+LoGGy[j][i];
		}
	}
    for(long j=0;j<lHeight;j++)
	{
        int marker=0;
		for (long i=0;i<lWidth;i++)
		{
			if(LoGG[j][i]>0)
			{
				marker=1;
				((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i]=(unsigned char)255;
			}
			if(LoGG[j][i]<0)
			{
				marker=0;
				((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i]=(unsigned char)0;
			}
			if(LoGG[j][i]==0)
			{
				if(marker==1)
					((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i]=(unsigned char)255;
				else ((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i]=(unsigned char)0;
			}
		}
	}
	delete[] LoG;
	delete[] LoGG;
}




void Morphological(IplImage * lpDIBBits)
{
//	LPSTR lpDIB=(LPSTR)::GlobalLock((HGLOBAL)hDIB);
//	LPSTR lpDIBBits=::FindDIBBits(lpDIB);
//	::GlobalUnlock((HGLOBAL)hDIB);
	const long lWidth=512;
	const long lHeight=128;
    // 图像每行的字节数
	//LONG lLineBytes;
	// 计算图像每行的字节数
    int m=3;
    int n=3;
	int structure[3][3]={0,0,0,0,0,0,0,0,0};
	//循环变量
	long i;
	long j;
	//指向源图像的指针
	//LPSTR	lpSrc;
	char * lpSrc;
	// 指向缓存图像的指针
	//LPSTR	lpDst;
	char * lpDst;
	// 指向缓存DIB图像的指针
	//LPSTR	lpNewDIBBits;
	//HLOCAL	hNewDIBBits;
	//像素值
	unsigned char pixel;
	// 暂时分配内存，以保存新图像
//	hNewDIBBits=LocalAlloc(LHND,lWidth*lHeight);
	IplImage *lpNewDIBBits=cvCreateImage(cvSize(lWidth,lHeight),IPL_DEPTH_8U,1);
//	if(hNewDIBBits==NULL)
//	{
		//分配内存失败
	//	AfxMessageBox("内存分配失败！");
//		return ;
//	}
	// 锁定内存
//	lpNewDIBBits=(char *)LocalLock(hNewDIBBits);
	// 初始化新分配的内存，设定初始值为0
	lpDst=lpNewDIBBits->imageData;
	memset(lpNewDIBBits->imageData,0,lpNewDIBBits->imageSize);
	//memset(lpDst,(BYTE)0,lWidth*lHeight);
	//使用自定义的结构元素进行腐蚀
	for(j=1;j<lHeight-1;j++)
	{
		for(i=1;i<lWidth-1;i++)
		{
			//由于使用3×3的结构元素，为防止越界，所以不处理最左边和最右边的两列像素
			//和最上边和最下边的两列像素
			//指向源图像倒数第j行，第i个象素的指针			
		//	lpSrc=(char *)lpDIBBits+lWidth*j+i;
			lpSrc=lpDIBBits->imageData+lWidth*j+i;
			// 指向目标图像倒数第j行，第i个象素的指针			
		//	lpDst=(char *)lpNewDIBBits+lWidth*j + i;
			lpDst=lpNewDIBBits->imageData+lWidth*j + i;
			//取得当前指针处的像素值，注意要转换为unsigned char型
			pixel=(unsigned char)*lpSrc;
			
			//目标图像中含有0和255外的其它灰度值
			if(pixel!=255&&*lpSrc!=0)  
			{  
		//		AfxMessageBox("非二值图像!");
				std::cout<< "非二值图像！"<<std::endl;
				return ;
			}
			*lpDst=(unsigned char)255;
			//原图像中对应结构元素中为黑色的那些点中只要有一个是黑色，
			//则将目标图像中的当前点赋成黑色
			//注意在DIB图像中内容是上下倒置的
			for(m=0;m<3;m++ )
			{
				for(n=0;n<3;n++)
				{
					if(structure[m][n]==-1)
						continue;
					pixel=*(lpSrc+((2-m)-1)*lWidth+(n-1));
				//	pixel= ((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*((2-m)-1)))[(n-1)];
					if(pixel==0)
					{	
						*lpDst=(unsigned char)0;
					//	((unsigned char *)(lpDst->imageData+lpDst->widthStep*j))[i]=(unsigned char)0;
						break;
					}
				}
			}	
		}
	}
    //复制腐蚀后的图像
	//memcpy(lpDIBBits,lpNewDIBBits,lWidth*lHeight);
	memcpy(lpDIBBits->imageData,lpNewDIBBits->imageData,lpNewDIBBits->imageSize);

//	lpDst=(char *)lpNewDIBBits;
 //   memset(lpDst,(BYTE)0,lWidth*lHeight);
	lpDst=lpNewDIBBits->imageData;
	memset(lpNewDIBBits->imageData,0,lpNewDIBBits->imageSize);

	//使用自定义的结构元素进行膨胀
	for(j=1;j<lHeight-1;j++)
	{
		for(i=1;i<lWidth-1;i++)
		{
			//由于使用3×3的结构元素，为防止越界，所以不处理最左边和最右边的两列像素
			//和最上边和最下边的两列像素
			// 指向源图像倒数第j行，第i个象素的指针			
			lpSrc=lpDIBBits->imageData+lWidth*j+i;
			// 指向目标图像倒数第j行，第i个象素的指针			
		   lpDst=lpNewDIBBits->imageData+lWidth*j+i;
			//取得当前指针处的像素值，注意要转换为unsigned char型
			pixel=(unsigned char)*lpSrc;
		//	pixel= ((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*j))[i];
			//目标图像中含有0和255外的其它灰度值
			if(pixel!=255&&*lpSrc!=0)   //
			{
		//		AfxMessageBox("非二值图像！");
				std::cout<< "非二值图像！"<<std::endl;
				return ;
			}
			//目标图像中的当前点先赋成黑色
		//	((unsigned char *)(lpDst->imageData+lpDst->widthStep*j))[i]=(unsigned char)0;
			*lpDst=(unsigned char)0;
			//如果原图像中对应结构元素中为黑色的那些点中有一个不是黑色，
			//则将目标图像中的当前点赋成白色
			//注意在DIB图像中内容是上下倒置的
			for(m=0;m<3;m++ )
			{
				for(n=0;n<3;n++)
				{
					if(structure[m][n]==-1)
						continue;
					pixel=*(lpSrc+((2-m)-1)*lWidth+(n-1));
					//pixel= ((unsigned char *)(lpDIBBits->imageData+lpDIBBits->widthStep*((2-m)-1)))[(n-1)];
					if(pixel==255)
					{	
						*lpDst=(unsigned char)255;
					
					//	((unsigned char *)(lpDst->imageData+lpDst->widthStep*j))[i]=(unsigned char)255;
						break;
					}
				}
			}
		}
	}
	//复制膨胀后的图像
//	memcpy(lpDIBBits,lpNewDIBBits,lWidth*lHeight);	
	memcpy(lpDIBBits->imageData,lpNewDIBBits->imageData,lpNewDIBBits->imageSize);
	//释放内存
	//LocalUnlock(hNewDIBBits);
	//LocalFree(hNewDIBBits);

	 cvReleaseImage(&lpNewDIBBits);
}


void MarkthePoint(IplImage * lpDIBBits,PointInformation *PointInfor,int &tab)
{
	//LPSTR lpDIB=(LPSTR)::GlobalLock((HGLOBAL)hDIB);
	//LPSTR lpDIBBits=::FindDIBBits(lpDIB);
	//::GlobalUnlock((HGLOBAL)hDIB);
	const long lWidth=512;
	const long lHeight=128;
    // 图像每行的字节数
	//LONG lLineBytes;
	// 计算图像每行的字节数
	//lLineBytes=WIDTHBYTES(lWidth*8);
	int taber=0;//标记点个数
	//LPSTR src=lpDIBBits;
	char * src= lpDIBBits->imageData;
	unsigned char pre=*src;
	
	for(int i=0;i<255;i++)
	{
		PointInfor[i].useful=false;
	}
	if(pre==255)  //pre==255
	{//如果刚开始源地址为255情况标记为有用
		PointInfor[1].left=0;
		PointInfor[1].right=0;
		PointInfor[1].bottom=0;
		PointInfor[1].top=0;
		PointInfor[1].useful=true;
		PointInfor[1].xsum=0;
		PointInfor[1].ysum=0;
		PointInfor[1].size=1;
		*src=1;
		taber=taber+1;
	}
	//CList<long,long> unusedtab;	
	std::list<long> unusedtab;
	long i;	
	//处理第一行
	for(i=1;i<lWidth;i++)
	{
		src= lpDIBBits->imageData+i;
		//src=lpDIBBits+i;
		if((unsigned char)*src==255)
		{ 
			if(pre!=0)
			{
				*src=(unsigned char)pre;
				PointInfor[(unsigned long)*src].right=i;
				PointInfor[(unsigned long)*src].size+=1;
				PointInfor[(unsigned long)*src].xsum+=i;
			 }
			else
			{
				tab=tab+1;
				if(tab>=255)
				{
					tab=255;
					*src=0;//忽略该点，将该点涂黑，以下同
				}
				else//特征区域断开，新的特征区域
				{
				*src=(unsigned char)taber+1;
				PointInfor[taber].useful=true;
                PointInfor[taber].top=0;
				PointInfor[taber].bottom=0;
				PointInfor[taber].left=i;
				PointInfor[taber].right=i;
				PointInfor[taber].size=1;
				PointInfor[taber].xsum=i;
				PointInfor[taber].ysum=0;
				}
			 }
		 }
		 pre=*src;
	}

	for(long j=1;j<lHeight;j++)
	{
		//处理第一列
	//	LPSTR src_a=lpDIBBits+(j-1)*lLineBytes;
		char * src_a= lpDIBBits->imageData+(j-1)*lpDIBBits->widthStep;
		char * src  = lpDIBBits->imageData+    j*lpDIBBits->widthStep;
	//	LPSTR src=lpDIBBits+j*lLineBytes;
		if(*src=255)
		{ 
			if(*src_a!=0)
		    {
				*src=*src_a;
			    PointInfor[(unsigned long)*src_a].bottom=j;
			    PointInfor[(unsigned long)*src_a].size+=1;
			    PointInfor[(unsigned long)*src_a].ysum+=j;
		    }
			else
		    {   
				if(unusedtab.empty()!=true)
				{   
				//	long t=(unsigned char)unusedtab.GetHead();
					long t=(unsigned char)unusedtab.front();
					*src=t;
				//	unusedtab.RemoveHead();
					unusedtab.pop_front();
					PointInfor[t].useful=true;
					PointInfor[t].left=i;
				    PointInfor[t].right=i;
				    PointInfor[t].top=j;
				    PointInfor[t].bottom=j;
				    PointInfor[t].size=1;
					PointInfor[t].xsum=0;
					PointInfor[t].ysum=j;
				}
				else
		        {
					taber=taber+1;
					if(taber>=255)
					{
						taber=255;
						*src=0;
					}
					else
					{
						*src=(unsigned char)taber;
						PointInfor[taber].useful=true;
						PointInfor[taber].left=i;
						PointInfor[taber].right=i;
						PointInfor[taber].top=j;
						PointInfor[taber].bottom=j;
						PointInfor[taber].size=1;
						PointInfor[taber].xsum=i;
						PointInfor[taber].ysum=j;
					}
				}
				 
			}
			//处理全部区域
			for(long i=1;i<lWidth;i++)
			{
				char *  src_a=lpDIBBits->imageData+lpDIBBits->widthStep*(j-1)+i-1;
				char *  src_b=lpDIBBits->imageData+lpDIBBits->widthStep*(j-1)+i;
				char *  src_c=lpDIBBits->imageData+lpDIBBits->widthStep*j+i-1;
				char * src=lpDIBBits->imageData+lpDIBBits->widthStep*j+i;
				//LPSTR  src_a=lpDIBBits+lLineBytes*(j-1)+i-1;
				//LPSTR  src_b=lpDIBBits+lLineBytes*(j-1)+i;
				//LPSTR  src_c=lpDIBBits+lLineBytes*j+i-1;
				//LPSTR  src=lpDIBBits+lLineBytes*j+i;


				if(*src==0)
					continue;
				if(*src!=0)
				{
					//a没有被标记的情况
					if(*src_a!=0)
					{ 
						*src=*src_a;    //////////////////////////////////////////////
						long point=(unsigned char)*src;
						if(PointInfor[point].right<i)
							PointInfor[point].right=i;
						PointInfor[point].size+=1;
						if(PointInfor[point].bottom<j)
							PointInfor[point].bottom=j;
						PointInfor[point].xsum+=i;
						PointInfor[point].ysum+=j;
							continue;
					}
					//b或者c已经标记过的
					if(*src_b!=0&&*src_c==0)
					{
						*src=*src_b;
						long point=(unsigned char)*src;
						if(PointInfor[point].right<i)
							PointInfor[point].right=i;
						PointInfor[point].size+=1;
						if(PointInfor[point].bottom<j)
							PointInfor[point].bottom=j;
						PointInfor[point].xsum+=i;
						PointInfor[point].ysum+=j;
						continue;
					}
					if(*src_b==0&&*src_c!=0)
					{
						*src=*src_c;
						long point=(unsigned char)*src;
						if(PointInfor[point].right<i)
							PointInfor[point].right=i;
						PointInfor[point].size+=1;
						if(PointInfor[point].bottom<j)
							PointInfor[point].bottom=j;
						PointInfor[point].xsum+=i;
						PointInfor[point].ysum+=j;
						continue;
					}
					//bc已经标记过的且标记相同
					if(*src_b!=0&&*src_c!=0&&*src_b==*src_c)
					{
						*src=*src_c;
						long point=(unsigned char)*src;
						if(PointInfor[point].right<i)
							PointInfor[point].right=i;
						PointInfor[point].size+=1;
						if(PointInfor[point].bottom<j)
							PointInfor[point].bottom=j;
						PointInfor[point].xsum+=i;
						PointInfor[point].ysum+=j;
						continue;
					}
					//bc已经标记过的且标记不同
					if(*src_b!=0&&*src_c!=0&&*src_b!=*src_c)
					{
					*src=((unsigned char)*src_b<(unsigned char)*src_c)?(unsigned char)*src_b:(unsigned char) *src_c;
					long tmp=((unsigned char)*src_b<(unsigned char)*src_c)? (unsigned char)*src_c:(unsigned char)*src_b;
					if(PointInfor[(unsigned char)*src_c].left<PointInfor[(unsigned char)*src_b].left)
						PointInfor[(unsigned char)*src].left=PointInfor[(unsigned char)*src_c].left;
					else
						PointInfor[(unsigned char)*src].left=PointInfor[(unsigned char)*src_b].left;
					if(PointInfor[(unsigned char)*src_c].top<PointInfor[(unsigned char)*src_b].top)
						PointInfor[(unsigned char)*src].top=PointInfor[(unsigned char)*src_c].top;
					else 
						PointInfor[(unsigned char)*src].top=PointInfor[(unsigned char)*src_b].top;
					if(PointInfor[(unsigned char)*src_c].right>PointInfor[(unsigned char)*src_b].right)
						PointInfor[(unsigned char)*src].right=PointInfor[(unsigned char)*src_c].right;
					else
						PointInfor[(unsigned char)*src].right=PointInfor[(unsigned char)*src_b].right;
					if(PointInfor[(unsigned char)*src].right<i)
						PointInfor[(unsigned char)*src].right=i;
					PointInfor[(unsigned char)*src].bottom=j;
					PointInfor[(unsigned char)*src].size=PointInfor[(unsigned char)*src_b].size+PointInfor[(unsigned char)*src_c].size+1;
					PointInfor[(unsigned char)*src].xsum=PointInfor[(unsigned char)*src_b].xsum+PointInfor[(unsigned char)*src_c].xsum+i;
					PointInfor[(unsigned char)*src].ysum=PointInfor[(unsigned char)*src_b].ysum+PointInfor[(unsigned char)*src_c].ysum+j;
					for(long k=PointInfor[tmp].top;k<=PointInfor[tmp].bottom;k++)
					{  
						for(long l=PointInfor[tmp].left;l<=PointInfor[tmp].right;l++)
						{
							//LPSTR src1=lpDIBBits+lLineBytes*k+l;
							 char * src1=lpDIBBits->imageData+lpDIBBits->widthStep*k+l;

							if((unsigned char)*src1==(unsigned char)tmp)
							*src1=*src;
						}
					}
					PointInfor[tmp].useful=false;
					PointInfor[tmp].left=0;
					PointInfor[tmp].bottom=0;
					PointInfor[tmp].right=0;
					PointInfor[tmp].top=0;
					PointInfor[tmp].size=0;
					PointInfor[tmp].xsum=0;
					PointInfor[tmp].ysum=0;
				//	unusedtab.AddTail(tmp);
					unusedtab.push_back(tmp);
					continue;
					}
					//bc都没有被标记
					if(*src_b==0&&*src_c==0)
					{
						if(unusedtab.empty()!=true)
						{
						//	*src=(unsigned char)unusedtab.GetHead();
							*src=(unsigned char)unusedtab.front();
						  //unusedtab.RemoveHead();
											
					      unusedtab.pop_front();


							PointInfor[(unsigned char )*src].useful=true;
							PointInfor[(unsigned char )*src].top=j;
							PointInfor[(unsigned char )*src].bottom=j;
							PointInfor[(unsigned char )*src].left=i;
							PointInfor[(unsigned char )*src].right=i;
							PointInfor[(unsigned char )*src].size=1;
							PointInfor[(unsigned char )*src].xsum=i;
							PointInfor[(unsigned char )*src].ysum=j;
							continue;
						}
						else
						{
							taber=taber+1;
							if(taber>=255)
							{
								taber=255;
								*src=0;
							}else
							{
								*src=(unsigned char)taber;
								PointInfor[(unsigned char )*src].useful=true;
								PointInfor[(unsigned char )*src].top=j;
								PointInfor[(unsigned char )*src].bottom=j;
								PointInfor[(unsigned char )*src].left=i;
								PointInfor[(unsigned char )*src].right=i;
								PointInfor[(unsigned char )*src].size=1;
								PointInfor[(unsigned char )*src].xsum=i;
								PointInfor[(unsigned char )*src].ysum=j;
								continue;
							}
						}
				 
					}
				}
			}
		}
	}
	tab=taber;
 }
 void GetCharacterVector(PointInformation * PointInfor,int tab,CharacterVector *Character,int length)
{    
	const long lWidth=512;
	const long lHeight=128;
	//图像每行的字节数
	//LONG lLineBytes;
	//计算图像每行的字节数
	//lLineBytes=WIDTHBYTES(lWidth*8);
  //  HGLOBAL hGlobal=::GlobalAlloc(GHND,lHeight*lWidth);
//	LPSTR lpRecorder=(char*)::GlobalLock(hGlobal);
//	::GlobalUnlock(hGlobal);

	IplImage *lpRecorder=cvCreateImage(cvSize(lWidth,lHeight),IPL_DEPTH_8U,1);
	memset(lpRecorder->imageData,0,lpRecorder->imageSize);

	for(int i=1;i<=tab;i++)
	{  
		if(PointInfor[i].useful)
		{
			if(PointInfor[i].top<5&&((PointInfor[i].right-PointInfor[i].left)>80))
			{//去除下侧干扰点（虹膜内边缘部分）
				PointInfor[i].useful=false;
				PointInfor[i].top=0;
				PointInfor[i].bottom=0;
				PointInfor[i].right=0;
				PointInfor[i].bottom=0;
				PointInfor[i].size=0;
				PointInfor[i].xsum=0;
				PointInfor[i].ysum=0;
				 continue;
			}
			if(PointInfor[i].bottom>98&&((PointInfor[i].bottom-PointInfor[i].top)>45||(PointInfor[i].right-PointInfor[i].left)>80))	 
			{//去除眼皮和睫毛的干扰
				PointInfor[i].useful=false;
				PointInfor[i].top=0;
				PointInfor[i].bottom=0;
				PointInfor[i].right=0;
				PointInfor[i].bottom=0;
				PointInfor[i].size=0;
				PointInfor[i].xsum=0;
				PointInfor[i].ysum=0;
				continue;
			}
			PointInfor[i].xsum=(float)PointInfor[i].xsum/(float)PointInfor[i].size+0.5;
			PointInfor[i].ysum=(float)PointInfor[i].ysum/(float)PointInfor[i].size+0.5;
			  char * src2= lpRecorder->imageData+lpRecorder->widthStep*PointInfor[i].ysum+PointInfor[i].xsum;
			//LPSTR src2= lpRecorder+lLineBytes*PointInfor[i].ysum+PointInfor[i].xsum;
			*src2=i;
		}
	}
	for(int i=0;i<length;i++)
	{
		Character[i].type=0;
	}
	int counter=0;
	for(long j=0;j<lHeight;j++)
	{
		for(long i=0;i<lWidth;i++)
		{
			//LPSTR src3;
			char * src3=lpRecorder->imageData+lpRecorder->widthStep*j+i;
			int src_val=(unsigned char)*src3;
			if(src_val!=0)
			{   
				Character[counter].left=PointInfor[src_val].left;
				Character[counter].bottom=PointInfor[src_val].bottom;
				Character[counter].top=PointInfor[src_val].top;
				Character[counter].right=PointInfor[src_val].right;
				Character[counter].x=PointInfor[src_val].xsum;
				Character[counter].y=PointInfor[src_val].ysum;
				int dy=PointInfor[src_val].bottom-PointInfor[src_val].top;
				int dx=PointInfor[src_val].right-PointInfor[src_val].left;
				float dydx=(float)dy/(float)dx+0.5;
				float area_ratio=(float) PointInfor[src_val].size/(float)(dx*dy);
				if(area_ratio<=0.5)
				{ 
					if(PointInfor[src_val].size<100)
						Character[counter].type=9;
					else
						Character[counter].type=8;

				}
				else
				{ 
					if(dydx<2&&dydx>0.5)
					{
						if(PointInfor[src_val].size<40)
							Character[counter].type=1;
						if(PointInfor[src_val].size<160&&PointInfor[src_val].size>=40)
							Character[counter].type=2;
						if(PointInfor[src_val].size>=160)
							Character[counter].type=3;
					}
					if(dydx>=2)
					{   
						if(PointInfor[src_val].size<40)
							Character[counter].type=10;
						if(PointInfor[src_val].size<160&&PointInfor[src_val].size>=60)
							Character[counter].type=4;
						if(PointInfor[src_val].size>=160)
							Character[counter].type=5;
					}
					if(dydx<=0.5)
					{     
						if(PointInfor[src_val].size<40)
                          Character[counter].type=11;
					    if(PointInfor[src_val].size<160&&PointInfor[src_val].size>=60)
						  Character[counter].type=6;
					  if(PointInfor[src_val].size>=160)
						  Character[counter].type=7;
					}
				}
				counter++;
				if(counter>=length)
				{  
					//::GlobalFree(hGlobal);
					cvReleaseImage(&lpRecorder);
					return ;
				}
			}  
		}
	}
	//::GlobalFree(hGlobal);
	 cvReleaseImage(&lpRecorder);
}
float Compare(CharacterVector *chara1,CharacterVector* chara2,int Vect1,int Vect2)
{     
	const long lWidth=512;
	const long lHeight=128;
	//HGLOBAL	hGlobal=::GlobalAlloc(GHND,lHeight*lWidth);
	//LPSTR lpRecorder=(char*)::GlobalLock(hGlobal);
	//::GlobalUnlock(hGlobal);
	//LONG lLineBytes=WIDTHBYTES(lWidth*8);
	IplImage *lpRecorder=cvCreateImage(cvSize(lWidth,lHeight),IPL_DEPTH_8U,1);
	memset(lpRecorder->imageData,(unsigned char)255,lHeight*lWidth); 

	for(int i=0;i<Vect2;i++)
	{
		if(chara2[i].type!=0)
		{
			//	lpSrc=lpDIBBits->imageData+lWidth*j+i;  
			char * src=lpRecorder->imageData+chara2[i].y*lpRecorder->widthStep+chara2[i].x;
			*src=(unsigned char)i;
		}
	}
	int maxmatchednum=0;
	for(int angle=-42;angle<42;angle++)
	{
		for(int updown=-5;updown<5;updown++)
		{
          int matchednum=0;
	      bool matched=false;
	      for(int m=0;m<Vect1;m++)
	      {
			matched=false;
			if(chara1[m].type!=0)
			{  
				long t1=chara1[m].top-updown*0.02*chara1[m].top+0.5;
				long t2=chara1[m].bottom-updown*0.02*chara1[m].bottom+0.5;
                if(t1<0)
					t1=0;
				if(t2>lHeight-1)
					t2=lHeight-1;
	            for(long j=t1;j<=t2;j++)
	            {   
					if(matched)
	                   break;
					for(long i=chara1[m].left;i<chara1[m].right;i++)
					{   
						long t=(i+angle)%lWidth;
			         //   unsigned char  number=*(lpRecorder+lLineBytes*j+t);
						unsigned char  number=((unsigned char *)(lpRecorder->imageData+lpRecorder->widthStep*j))[t];
			            if(number!=255)
						{
							if(chara2[number].type==chara1[m].type)
							{     
								matched=true;
								matchednum+=1;
								break;
							}
						}
					}
				}
			}
		  }
		  if(matchednum>maxmatchednum)
			  maxmatchednum=matchednum;
		}
	} 
//	::GlobalFree(hGlobal);
	cvReleaseImage(&lpRecorder);
	int chara1num=0;
	int chara2num=0;
	for(int i=0;i<Vect1;i++)
	{
		if(chara1[i].type!=0)
			chara1num++;
	}
	for(int i=0;i<Vect2;i++)
	{
		if(chara2[i].type!=0)
			chara2num++;
	}
	float lamuda=(float)maxmatchednum/(float)(chara2num+chara1num); 
	
	
	return  2*lamuda;
}
void GetIrisVector(IplImage * src,CharacterVector * CharaVec,int Character_length)
{
	long lWidth = src->width;
    long lHeight= src->height;
	cvFlip(src,NULL,0);
	long threshold=Threshold(src,lWidth,lHeight);
	IplImage *dst=cvCreateImage(cvSize(lWidth,lHeight),IPL_DEPTH_8U,1);
	memset(dst->imageData,1,dst->imageSize);
	PupilContour(dst,lWidth,lHeight,threshold,src);
	std::list<PHTPoint>  PointListForPHT;
	FindPHTPoint(dst,&PointListForPHT,lHeight,lWidth);
	CIRCLE InnerCircle=PHT(&PointListForPHT);
	PointListForPHT.clear();
    CIRCLE Outer=OuterCircle(src,lWidth,lHeight,InnerCircle);
    const long RectWidth=512;
    const long RectHeight=128;
  	IplImage *hDIB=cvCreateImage(cvSize(RectWidth,RectHeight),IPL_DEPTH_8U,1);
    TurnToRect(InnerCircle,Outer,src,hDIB,RectWidth,RectHeight);
	LOGBin(hDIB);
	Morphological(hDIB);
	int tab=0;
	PointInformation pointInfor[255];
	MarkthePoint(hDIB,pointInfor,tab);
//	CharacterVector CharaV1[Character_length];
	GetCharacterVector(pointInfor,tab,CharaVec,Character_length);
	cvReleaseImage(&dst);
	cvReleaseImage(&hDIB);
}
void MySaveCharaV(FileName_Chara FileName_CharaVx,int Vect)
{
	char *filename=FileName_CharaVx.FileName;
	FILE *fp=fopen(filename,"w");
	if(fp==NULL)
	{
		return;
	}
	for(int i=0;i<Vect;i++)
	{
		fprintf(fp,"%4u %4u %4u %4u %4u %4u %4u\n",FileName_CharaVx.Chara[i].left,FileName_CharaVx.Chara[i].right,
			FileName_CharaVx.Chara[i].top,FileName_CharaVx.Chara[i].bottom,FileName_CharaVx.Chara[i].type,
			FileName_CharaVx.Chara[i].x,FileName_CharaVx.Chara[i].y);
	}
	fclose(fp);
}
void checkImage(char *a,bool * bFlag)    //,unsigned char * img
{
	* bFlag =0;
	IplImage *src=cvLoadImage(a,0); 
	if(src!=NULL)
	{
		if(src->depth != IPL_DEPTH_8U )
		{
			std::cout<< "Image format is not supportted!"<<std::endl;
			return;
		}
		else{
		//	img = (unsigned char * )src->imageData;
			std::cout<< "Image can generate feature code."<<std::endl;
			* bFlag = 1;
		}
	}
	else
	{
		std::cout<< "Load image failure!"<<std::endl;
		return;
	}
		
}
IplImage* GetSubImage(IplImage *pImg, CvRect roi)
{
	IplImage *result;
    // 设置 ROI 
    cvSetImageROI(pImg,roi);
    // 创建子图像
    result = cvCreateImage( cvSize(roi.width, roi.height), pImg->depth, pImg->nChannels );
    cvCopy(pImg,result);
    cvResetImageROI(pImg);	
    return result;
}
void DetectAndDraw(Mat& img, CascadeClassifier& cascade)
{
	int i = 0;
	double t = 0;
	vector<Rect> faces;
	const static Scalar colors[] = { CV_RGB(0, 0, 255),
		CV_RGB(0, 128, 255),
		CV_RGB(0, 255, 255),
		CV_RGB(0, 255, 0),
		CV_RGB(255, 128, 0),
		CV_RGB(255, 255, 0),
		CV_RGB(255, 0, 0),
		CV_RGB(255, 0, 255) };
	Mat gray;
	Mat frame(cvRound(img.rows), cvRound(img.cols), CV_8UC1);

	cvtColor(img, gray, CV_BGR2GRAY);
	resize(gray, frame, frame.size(), 0, 0, INTER_LINEAR);
	equalizeHist(frame, frame);

	t = (double)cvGetTickCount();
	cascade.detectMultiScale(frame, faces,
		1.1, 2, 0
		//|CV_HAAR_FIND_BIGGEST_OBJECT
		//|CV_HAAR_DO_ROUGH_SEARCH
		| CV_HAAR_SCALE_IMAGE
		,
		Size(300, 300));
	t = (double)cvGetTickCount() - t;
	printf("detection time = %g ms\n", t / ((double)cvGetTickFrequency()*1000.));

	
	for (vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++, i++)
	{
		Point center;
		Scalar color = colors[i % 8];
		CvPoint pt1, pt2;
		pt1.x = r->x;
		pt1.y = r->y;
		pt2.x = r->x + r->width;
		pt2.y = r->y + r->height;
		rectangle(img, pt1, pt2, color, 3, 8, 0);
		IplImage frame1=frame;
		IplImage *pFace = GetSubImage(&frame1, *r);
     	cvShowImage("subImage", pFace);
		
		char s[30]="D://R//";
		char d[10];
		itoa(num,d,3);
		strcat(s,d);
		strcat(s,".bmp");
		cvSaveImage(s,pFace);
		num++;
		/*
		int radius;
		center.x = cvRound(r->x + r->width*0.5);
		center.y = cvRound(r->y + r->height*0.5);
		radius = (int)(cvRound(r->width + r->height)*0.25);
		circle(img, center, radius, color, 3, 8, 0);
		*/
	}

	cv::imshow("result", img);
	
}
int detectEye1()
{
	Mat frame1;
	CascadeClassifier cascade;    ///////!!!!!
	if(!cascade.load(cascadeName)){
		std::cerr << "ERROR: Could not load cascade classifier \"" << cascadeName << "\"" <<std:: endl;
		return -1;
	}

	CvCapture* capture = 0;
	IplImage *frame;
	IplImage *frame_copy = 0;

	//capture = cvCaptureFromCAM(0);//0
	//capture =cvCreateCameraCapture(4);
	capture =cvCreateCameraCapture(0);
	int counter = 0;
	string strPreName = "", strName;
	if(capture)
	{
		num = 0;
		cvNamedWindow("result", 1);
//		cvNamedWindow("subImage", 1);  !
		for(;;)
		{
			frame = cvQueryFrame(capture);
			if(frame)
			{
				frame_copy = cvCreateImage( cvSize(frame->width,frame->height), IPL_DEPTH_8U, frame->nChannels);
				if( frame->origin == IPL_ORIGIN_TL )
					cvCopy( frame, frame_copy, 0 );
				else
					cvFlip( frame, frame_copy, 0 );
				//detect_and_recognize(frame_copy, m_pCascade);
			//	strName = detect_and_recognize(frame_copy, m_pCascade);
				//strName =DetectAndDraw(frame_copy, cascade);
			//	cvSaveImage("D://R//frame_copy.bmp",frame_copy);
				frame1=frame_copy;
				if (frame1.empty())
				break;
	         DetectAndDraw(frame1,cascade);

				
				if(strPreName == "")
				{
					strPreName = strName; 
				}
				else
				{
					string s;
					if(strPreName.compare( strName ) == 0)
					{
						counter++;
						if(counter >= 5)
						{
							//AfxMessageBox(_T(strName.c_str()));
							//AfxMessageBox(strName);
						}
					}
					else
					{
						counter = 0;
						strPreName = strName;
					}
				}
				cvShowImage("result", frame_copy);
				cvReleaseImage(&frame_copy);
				if( cvWaitKey(10) >= 0 )
				{
					cvDestroyWindow("result");
	//				cvDestroyWindow("subImage");
					cvReleaseCapture( &capture );
					break;
				}
			}
		}
	}
	return 1;
}



void detectEye(unsigned char * src,int width,int height,int depth, int channels, bool * bFlag)
{
	Mat frame1;
	CascadeClassifier cascade;    ///////!!!!!
	if(!cascade.load(cascadeName)){
		std::cerr << "ERROR: Could not load cascade classifier \"" << cascadeName << "\"" <<std:: endl;
		* bFlag=0;
		//return -1;
	}

	IplImage *frame = cvCreateImageHeader(cvSize(width,height), depth, channels); 
	//frame->origin=~frame->origin;
	cvSetData(frame,src,width*channels);  //8bit才是这样！！
//	IplImage *frame_copy = 0;


			if(frame)
			{
			//	frame_copy = cvCreateImage( cvSize(frame->width,frame->height), depth, frame->nChannels);
				/*
				if( frame->origin == IPL_ORIGIN_TL )
					cvCopy( frame, frame_copy, 0 );
				else
					cvFlip( frame, frame_copy, 0 );
					*/
				//detect_and_recognize(frame_copy, m_pCascade);
			//	strName = detect_and_recognize(frame_copy, m_pCascade);
				//strName =DetectAndDraw(frame_copy, cascade);
				frame1=frame;
				if (frame1.empty())
				{
				std::cerr << "ERROR: Could not create frame" << std:: endl;
		       * bFlag=0;
				}
				
	         DetectAndDraw(frame1,cascade);

				
	
	            cvNamedWindow("result", 1);
				cvShowImage("result", frame);
			//	cvReleaseImage(&frame_copy);
				if( cvWaitKey(10) >= 0 )
				{
					cvDestroyWindow("result");
	//				cvDestroyWindow("subImage");
				//	cvReleaseCapture( &capture );
				//	break;
				}
			}
		}
	
void transferCode(char *a,CharacterVector * CharaV1)
{
	 std::cout<< "Enter transferCode..."<<std::endl;
	 double scale = 1;
	 IplImage *src=cvLoadImage(a,0); //0011.jpg,1  "0011.bmp",0
	
	   IplImage* small_img = cvCreateImage( cvSize( cvRound (src->width/scale),
                         cvRound (src->height/scale)),
                     8, 1 );
	 if(src!=NULL)
	{
		if(src->depth != IPL_DEPTH_8U )
		{
			std::cout<< "Image format is not supportted!"<<std::endl;
			return;
		}
		else{
		//	img = (unsigned char * )src->imageData;
		}
	}
	else
	{
		std::cout<< "Load image failure!"<<std::endl;
		return;
	}
	

	
    cvResize( src, small_img, CV_INTER_LINEAR );
  //  cvEqualizeHist( small_img, small_img );


	//CharacterVector CharaV1[Character_length];  !!!!!NOOOO!!1
	GetIrisVector(small_img,CharaV1,Character_length);
	FileName_Chara FileCharaV1;
	FileCharaV1.FileName="./FeatureCode_1.txt";   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1need change!!!
	FileCharaV1.Chara=CharaV1;
	MySaveCharaV(FileCharaV1,Character_length);
	
    cvReleaseImage( &small_img );
	std::cout<< "Transfer code Finished."<<std::endl;
}


int findBaseLine(const cv::Mat &image)
{

    //cv::Rect roiRect(0,435,image.cols,20);
	cv::Rect roiRect(0,BASELINEPOS,image.cols,BASELINERANGE);
    cv::Mat roiImage=image(roiRect).clone();

    double gChannelSum=0.0;
    int baseLine;
    int lineIndex=0;

    for(int row=0;row<roiImage.rows;row++){
        if((row+3)>roiImage.rows)
            break;
        cv::Rect likelyArea(0,row,roiImage.cols,3);
        cv::Mat likelyImage=roiImage(likelyArea).clone();
        auto sum=cv::sum(likelyImage);
        if(sum[1]>gChannelSum){
            gChannelSum=sum[1];
            lineIndex=row+1;
        }
    }

    baseLine=BASELINEPOS+lineIndex;
    return baseLine;
}

void HoughLineX(char * txtName,char * fnametxt)
{
	IplImage* src;
	src=cvLoadImage(txtName,0);
	IplImage* dst = cvCreateImage( cvGetSize(src), 8, 1 );
	IplImage* color_dst = cvCreateImage( cvGetSize(src), 8, 3 );
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* lines = 0;
	IplImage* src1=cvCreateImage(cvSize(src->width,src->height),IPL_DEPTH_8U,1);
	//  cvCvtColor(src, src1, CV_BGR2GRAY);  
	cvCopy(src,src1);
	cvCanny( src1, dst, 50, 200, 3 );
	cvCvtColor( dst, color_dst, CV_GRAY2BGR );
	lines = cvHoughLines2( dst, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 10, 10, 10 );
	printf("%d\n",lines->total);
	int dis=0;
	int max=0;
	int j=0;
	CvPoint* line;
	CvPoint pointOne;
	CvPoint pointTwo;
	int a[1003]={0};
	for(int i = 0; i < lines->total; i++ )
	{
		line = (CvPoint*)cvGetSeqElem(lines,i);
		dis=(line[1].y-line[0].y)*(line[1].y-line[0].y)+(line[1].x-line[0].x)*(line[1].x-line[0].x);
		//  pointOne[i].x=line[0].x;
		//   pointOne[i].y=line[0].y;
		//    pointTwo[i].x=line[1].x;
		//pointTwo[i].y=line[1].y;
		a[4*i]=line[0].x;
		a[4*i+1]=line[0].y;
		a[4*i+2]=line[1].x;
		a[4*i+3]=line[1].y;
		if(dis>max)
		{
			max=dis;
			j=i;
		}
		cvLine( color_dst, line[0], line[1], CV_RGB(255,0,0), 3, 8 );
	}
	pointOne.x=a[4*j];
	pointOne.y=a[4*j+1];
	pointTwo.x=a[4*j+2];
	pointTwo.y=a[4*j+3];
	writeImgInfo(pointOne.x,pointOne.y,pointTwo.x,pointTwo.y,fnametxt);
	cvLine( color_dst, pointOne, pointTwo, CV_RGB(255,0,0), 3, 8 );    //画出最长的直线
	double Angle=0.0;
	Angle = atan2f(abs(pointTwo.y-pointOne.y),abs(pointTwo.x-pointOne.x));   //得到最长直线与水平夹角
	if(pointTwo.x>pointOne.x && pointTwo.y>pointOne.y)
	{
		Angle=-Angle;    //与常识一样
	}
	Angle=Angle*180/CV_PI;
	writeAngle((float)Angle,fnametxt);
	cvSaveImage(fnametxt,color_dst);	
	cvReleaseImage(&color_dst);
	cvReleaseImage(&dst);
	cvReleaseImage(&src1);
	cvReleaseImage(&src);
	cvReleaseMemStorage(&storage); 

}

int IterationThreshold(CvMat* gray)
{
	int width = gray->width;
	int height = gray->height;
	int histData[256] = {0};
	for(int j = 0; j < height; j ++)
	{
		uchar*data = (uchar*)(gray->data.ptr + j * gray->step);
		for (int i = 0; i < width; i ++)
		{
			histData[data[i]]++;
		}
	}

	//求图像的平均灰度值作为初始阈值
	int T0 = 0;
	for (int i = 0; i < 256; i ++)
	{
		T0 += i * histData[i];
	}
	T0 /= width * height;

	//迭代
	int T1 = 0, T2 = 0;
	int num1 = 0, num2 = 0;
	int T = 0;
	while (1)
	{
		for ( int i = 0; i < T0+1; i ++)
		{
			T1 += i * histData[i];
			num1 += histData[i];
		}
		if (num1 == 0)
			continue;
		for ( int i = T0 + 1; i < 256; i ++)
		{
			T2 += i * histData[i];
			num2 += histData[i];
		}
		if (num2 == 0)
			continue;
		T = (T1 / num1  + T2 / num2) / 2;
		if ( T == T0 )
			break;
		else
			T0 = T;
	}
	return T;
}

int clipIMG()
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char fnametxt[_MAX_PATH];
	char fnameExceptExt[_MAX_PATH];
	char * filePath = "C:\\2"; 
	vector<string> filesTxt;  
	getFiles(filePath, filesTxt);  
	for (int k = 0;k <  filesTxt.size();k++)  
	{  
		char * txtName=(char *) filesTxt[k].c_str();
		memset(drive,0,sizeof(drive)/sizeof(drive));
		memset(dir,0,sizeof(dir)/sizeof(dir));
		memset(fname,0,sizeof(fname)/sizeof(fname));
		memset(ext,0,sizeof(ext)/sizeof(ext));
		memset(fnametxt,0,sizeof(fnametxt)/sizeof(fnametxt));
		memset(fnameExceptExt,0,sizeof(fnameExceptExt)/sizeof(fnameExceptExt));
		_splitpath(txtName, drive, dir, fname, ext);
		strcat(fnametxt,"C:\\3\\");
		strcat(fnametxt,fname);
		strcat(fnametxt,ext);

		//////////////////////////////////////////////////////////////////////////
		Mat labImage = imread(txtName);
		cv::Mat labChannels[3];
		cv::split(labImage,labChannels);
		int baseLine=findBaseLine(labImage);
		cv::Rect roi4Rect(0,baseLine+850,labImage.cols,50);  
		cv::Mat roi4Image=labChannels[1](roi4Rect).clone();
		//   cv::imwrite(fnametxt,roi4Image);
		cv::Mat slidingWindow=roi4Image.clone();


		cv::Mat resultImage= Mat::zeros(roi4Rect.size(), CV_8UC3);
		cv::Mat resultImage2=labImage(roi4Rect).clone();


		vector<std::vector<cv::Point>> resultContours;


		//            cv::Mat printIamge=roi4Image.clone();
		//            cv::rectangle(printIamge,windowRect,cv::Scalar(0,0,255),2);
		//            cv::imshow("roi4",printIamge);
		int channel=0;
		cv::MatND dstHist;
		int histSize[]={256};
		float midRanges[]={0,255};
		const float *ranges[]={midRanges};
		cv::calcHist(&slidingWindow,1,&channel,cv::Mat(),dstHist,1,histSize,ranges,true,false);

		float sumPixel=0;
		int thresholdValue;
		for(int l=0;l<256;l++){
			sumPixel+=dstHist.at<float>(l);
			float ratio=sumPixel/(slidingWindow.rows*slidingWindow.cols);
			if(ratio>0.98){
				thresholdValue=l;
				break;
			}
		}


		//int thres = IterationThreshold(&(CvMat)slidingWindow);

		cv::Mat thresholdDst;
		cv::threshold(slidingWindow,thresholdDst,thresholdValue,255,CV_THRESH_BINARY);


		vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::Mat temp=thresholdDst.clone();
		cv::findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_NONE);
		cv::Mat printContoursIamge=thresholdDst.clone();

		for(int k=0;k<contours.size();k++){
			cv::RotatedRect rRc=cv::minAreaRect(contours.at(k));
			cv::Point2f vertices[4];
			rRc.points(vertices);

			double width = (vertices[0].x - vertices[1].x)* (vertices[0].x - vertices[1].x) + (vertices[0].y - vertices[1].y) * (vertices[0].y - vertices[1].y);
			width = sqrt(width);
			double height = (vertices[1].x - vertices[2].x)* (vertices[1].x - vertices[2].x) + (vertices[1].y - vertices[2].y) * (vertices[1].y - vertices[2].y);
			height = sqrt(height);
			double area = cv::contourArea(contours.at(k));


			if(area<=5 ||width<=0 ||height<=0){
				cv::drawContours(printContoursIamge,contours,k,cv::Scalar(0),CV_FILLED);
				continue;
			}

			double rRcArea=width*height;
			if(rRcArea<35){
				cv::drawContours(printContoursIamge,contours,k,cv::Scalar(0),CV_FILLED);
				continue;
			}


			double longSide = width<height ? height : width;
			double shortSide = height<width ? height : width;
			double longOverShortRatio=longSide/shortSide;
			if(longOverShortRatio<3.5){
				cv::drawContours(printContoursIamge,contours,k,cv::Scalar(0),CV_FILLED);
				continue;
			}




			auto meanCoord=cv::mean(contours.at(k));
			if(meanCoord[0]<=(slidingWindow.cols/6)){
				cv::drawContours(printContoursIamge,contours,k,cv::Scalar(0),CV_FILLED);
				continue;
			}
			if(meanCoord[0]>=(slidingWindow.cols*5/6)){
				cv::drawContours(printContoursIamge,contours,k,cv::Scalar(0),CV_FILLED);
				continue;
			}

			std::vector<cv::Point> tempContours=contours.at(k);
			resultContours.push_back(tempContours);
		}

		for(int j=0;j<resultContours.size();j++){
			cv::drawContours(resultImage2,resultContours,j,cv::Scalar(0,0,255),CV_FILLED);
		}

		cv::imwrite(fnametxt,resultImage2);

	}


	return 0;

}


void calAngle(char * txtName,char * fnametxt)
{
	Mat image = imread(txtName);
	vector<vector<cv::Point>> resultContours;
	cv::Mat resultImage=image.clone();
	cv::Mat labImage;
	cv::Mat labChannels[3];
	cv::cvtColor(resultImage,labImage,cv::COLOR_BGR2Lab);
	cv::split(labImage,labChannels);
	cv::threshold(labChannels[0],resultImage,100,255,CV_THRESH_BINARY);


	vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Mat temp=resultImage.clone();
	cv::findContours(temp,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	for(int j=0;j<contours.size();j++){

		cv::RotatedRect rRc=cv::minAreaRect(contours.at(j));
		cv::Point2f vertices[4];
		rRc.points(vertices);

		double width = (vertices[0].x - vertices[1].x)* (vertices[0].x - vertices[1].x) + (vertices[0].y - vertices[1].y) * (vertices[0].y - vertices[1].y);
		width = sqrt(width);

		double height = (vertices[3].x - vertices[0].x)* (vertices[3].x - vertices[0].x) + (vertices[3].y - vertices[0].y) * (vertices[3].y - vertices[0].y);
		height = sqrt(height);

		double area = cv::contourArea(contours.at(j));



		double longSide = width<height ? height : width;
		double shortSide = height <  width ? height : width;
		double longOverShortRatio=longSide/shortSide;

		double widthAngle = atan2f((vertices[1].y-vertices[0].y),(vertices[1].x-vertices[0].x));
		widthAngle = widthAngle * 180/CV_PI;

		double heightAngle = atan2f((vertices[3].y-vertices[0].y),(vertices[3].x-vertices[0].x));
		heightAngle = heightAngle * 180/CV_PI;

		double longSideAngle=height>width?heightAngle:widthAngle;
		double shortSideAngle=heightAngle<=-90?heightAngle+90:heightAngle-90;

		//if(abs(longSideAngle+110)<=10){
		//	continue;
		//}


		writeAngle((float)longSideAngle,fnametxt);
		resultContours.push_back(contours.at(j));

	}

	for(int j=0;j<resultContours.size();j++){
		cv::drawContours(image,resultContours,j,cv::Scalar(255,0,0),CV_FILLED);
	}
	cv::imwrite(fnametxt,image);
}

void calRAngle(char * txtName,char * fnametxt)
{
	Mat image = imread(txtName);
	vector<vector<cv::Point>> resultContours;
	cv::Mat resultImage=image.clone();
	cv::Mat rgbChannels[3];
	cv::split(image,rgbChannels);
	cv::threshold(rgbChannels[2],resultImage,100,255,CV_THRESH_BINARY);
	imwrite("c:\\1\\r.jpg",resultImage);

	vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Mat temp=resultImage.clone();
	cv::findContours(temp,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	for(int j=0;j<contours.size();j++){

		cv::RotatedRect rRc=cv::minAreaRect(contours.at(j));
		cv::Point2f vertices[4];
		rRc.points(vertices);

		double width = (vertices[0].x - vertices[1].x)* (vertices[0].x - vertices[1].x) + (vertices[0].y - vertices[1].y) * (vertices[0].y - vertices[1].y);
		width = sqrt(width);

		double height = (vertices[3].x - vertices[0].x)* (vertices[3].x - vertices[0].x) + (vertices[3].y - vertices[0].y) * (vertices[3].y - vertices[0].y);
		height = sqrt(height);

		double area = cv::contourArea(contours.at(j));



		double longSide = width<height ? height : width;
		double shortSide = height <  width ? height : width;
		double longOverShortRatio=longSide/shortSide;

		double widthAngle = atan2f((vertices[1].y-vertices[0].y),(vertices[1].x-vertices[0].x));
		widthAngle = widthAngle * 180/CV_PI;

		double heightAngle = atan2f((vertices[3].y-vertices[0].y),(vertices[3].x-vertices[0].x));
		heightAngle = heightAngle * 180/CV_PI;

		double longSideAngle=height>width?heightAngle:widthAngle;
		double shortSideAngle=heightAngle<=-90?heightAngle+90:heightAngle-90;

		if(abs(longSideAngle+145)<=3){
			continue;
		}


		writeAngle((float)longSideAngle,fnametxt);
		resultContours.push_back(contours.at(j));

	}


	for(int j=0;j<resultContours.size();j++){
		cv::drawContours(image,resultContours,j,cv::Scalar(255,0,0),CV_FILLED);
	}
	cv::imwrite(fnametxt,image);

}






void SobelRespectiveXY(const cv::Mat &image,vector<vector<cv::Point>> &resultContours, int clipx,int clipy)
{

	cv::Mat lightYSobel,greenYSobel;
	cv::Sobel(image,greenYSobel,CV_8U,0,1);
	//cv::Sobel(labChannels[0],lightYSobel,CV_8U,0,1);  bgrChannels[1]
	//cv::threshold(lightYSobel,lightYSobel,30,255,CV_THRESH_BINARY);
	cv::threshold(greenYSobel,greenYSobel,35,255,CV_THRESH_BINARY);
	//cv::Mat medianGreenImage;
	//cv::medianBlur(greenYSobel,medianGreenImage,5);

	vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Mat temp=greenYSobel.clone();
	cv::findContours(temp,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	for(int j=0;j<contours.size();j++){

		cv::RotatedRect rRc=cv::minAreaRect(contours.at(j));
		cv::Point2f vertices[4];
		rRc.points(vertices);

		double width = (vertices[0].x - vertices[1].x)* (vertices[0].x - vertices[1].x) + (vertices[0].y - vertices[1].y) * (vertices[0].y - vertices[1].y);
		width = sqrt(width);

		double height = (vertices[3].x - vertices[0].x)* (vertices[3].x - vertices[0].x) + (vertices[3].y - vertices[0].y) * (vertices[3].y - vertices[0].y);
		height = sqrt(height);

		double area = cv::contourArea(contours.at(j));

		if(area<=5 ||width<=1 ||height<=1){
			continue;
		}

		double c=cv::arcLength(contours.at(j),true);
		if(c<=5)
			continue;

		double longSide = width<height ? height : width;
		double shortSide = height <  width ? height : width;
		double longOverShortRatio=longSide/shortSide;

		if(longSide<=15)
		{
			continue;
		}


		double widthAngle = atan2f((vertices[1].y-vertices[0].y),(vertices[1].x-vertices[0].x));
		widthAngle = widthAngle * 180/CV_PI;

		double heightAngle = atan2f((vertices[3].y-vertices[0].y),(vertices[3].x-vertices[0].x));
		heightAngle = heightAngle * 180/CV_PI;

		double longSideAngle=height>width?heightAngle:widthAngle;
		double shortSideAngle=heightAngle<=-90?heightAngle+90:heightAngle-90;

		if(abs(longSideAngle+110)<=5){
			continue;
		}

		std::vector<cv::Point> tempContours=contours.at(j);
		for(int n=0;n<tempContours.size();n++){
			cv::Point temp=tempContours.at(n);
			tempContours.at(n).x=temp.x+clipx;
			tempContours.at(n).y=temp.y+clipy;
		}
		resultContours.push_back(tempContours);

	}



	contours.clear();
	hierarchy.clear();

	cv::Mat lightXSobel,greenXSobel;
	cv::Sobel(image,greenXSobel,CV_8U,1,0);
	//cv::Sobel(labChannels[0],lightXSobel,CV_8U,1,0);

	cv::threshold(greenXSobel,greenXSobel,45,255,CV_THRESH_BINARY);
	cv::threshold(lightXSobel,lightXSobel,35,255,CV_THRESH_BINARY);



	temp.release();
	temp=greenXSobel.clone();
	//resultImage.release();
	//resultImage=quenchPartImage.clone();
	cv::findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_NONE);
	for(int j=0;j<contours.size();j++){
		cv::RotatedRect rRc=cv::minAreaRect(contours.at(j));
		cv::Point2f vertices[4];
		rRc.points(vertices);

		double width = (vertices[0].x - vertices[1].x)* (vertices[0].x - vertices[1].x) + (vertices[0].y - vertices[1].y) * (vertices[0].y - vertices[1].y);
		width = sqrt(width);

		double height = (vertices[3].x - vertices[0].x)* (vertices[3].x - vertices[0].x) + (vertices[3].y - vertices[0].y) * (vertices[3].y - vertices[0].y);
		height = sqrt(height);

		double area = cv::contourArea(contours.at(j));

		if(area<=10 ||width<=1 ||height<=1){
			continue;
		}

		double c=cv::arcLength(contours.at(j),true);
		if(c<=5)
			continue;

		double longSide = width<height ? height : width;
		double shortSide = height <  width ? height : width;
		double longOverShortRatio=longSide/shortSide;

		if(longSide<=20)
		{
			continue;
		}


		//        if(longOverShortRatio<=1.5){
		//            cv::drawContours(resultImage,contours,j,cv::Scalar(255,255,0),CV_FILLED);
		//            continue;
		//        }

		double widthAngle = atan2f((vertices[1].y-vertices[0].y),(vertices[1].x-vertices[0].x));
		widthAngle = widthAngle * 180/CV_PI;

		double heightAngle = atan2f((vertices[3].y-vertices[0].y),(vertices[3].x-vertices[0].x));
		heightAngle = heightAngle * 180/CV_PI;

		double longSideAngle=height>width?heightAngle:widthAngle;
		double shortSideAngle=heightAngle<=-90?heightAngle+90:heightAngle-90;

		if(abs(longSideAngle+110)<=10){
			continue;
		}

		std::vector<cv::Point> tempContours=contours.at(j);
		for(int n=0;n<tempContours.size();n++){
			cv::Point temp=tempContours.at(n);
			tempContours.at(n).x=temp.x+clipx;
			tempContours.at(n).y=temp.y+clipy;
		}
		resultContours.push_back(tempContours);


	}

}




void ContourGeneralFilter(vector<vector<cv::Point>> &contours,vector<vector<cv::Point>> &resultContours,int clipx,int clipy)
{

	for(int j=0;j<contours.size();j++){

		cv::RotatedRect rRc=cv::minAreaRect(contours.at(j));
		cv::Point2f vertices[4];
		rRc.points(vertices);

		double width = (vertices[0].x - vertices[1].x)* (vertices[0].x - vertices[1].x) + (vertices[0].y - vertices[1].y) * (vertices[0].y - vertices[1].y);
		width = sqrt(width);

		double height = (vertices[3].x - vertices[0].x)* (vertices[3].x - vertices[0].x) + (vertices[3].y - vertices[0].y) * (vertices[3].y - vertices[0].y);
		height = sqrt(height);

		double area = cv::contourArea(contours.at(j));

		if(area<=5 ||width<=1 ||height<=1){
			continue;
		}

		double c=cv::arcLength(contours.at(j),true);
		if(c<=5)
			continue;

		double longSide = width<height ? height : width;
		double shortSide = height <  width ? height : width;
		double longOverShortRatio=longSide/shortSide;

		if(longSide<=15)
		{
			continue;
		}


		double widthAngle = atan2f((vertices[1].y-vertices[0].y),(vertices[1].x-vertices[0].x));
		widthAngle = widthAngle * 180/CV_PI;

		double heightAngle = atan2f((vertices[3].y-vertices[0].y),(vertices[3].x-vertices[0].x));
		heightAngle = heightAngle * 180/CV_PI;

		double longSideAngle=height>width?heightAngle:widthAngle;
		double shortSideAngle=heightAngle<=-90?heightAngle+90:heightAngle-90;

		if(abs(longSideAngle+110)<=5){
			continue;
		}

		std::vector<cv::Point> tempContours=contours.at(j);
		for(int n=0;n<tempContours.size();n++){
			cv::Point temp=tempContours.at(n);
			tempContours.at(n).x=temp.x+clipx;
			tempContours.at(n).y=temp.y+clipy;
		}
		resultContours.push_back(tempContours);

	}

}



void ContourPreciseFilter(vector<vector<cv::Point>> &contours,vector<vector<cv::Point>> &resultContours,int baseline)
{
	for(int j=0;j<contours.size();j++){

		cv::RotatedRect rRc=cv::minAreaRect(contours.at(j));
		cv::Point2f vertices[4];
		rRc.points(vertices);

		double width = (vertices[0].x - vertices[1].x)* (vertices[0].x - vertices[1].x) + (vertices[0].y - vertices[1].y) * (vertices[0].y - vertices[1].y);
		width = sqrt(width);

		double height = (vertices[3].x - vertices[0].x)* (vertices[3].x - vertices[0].x) + (vertices[3].y - vertices[0].y) * (vertices[3].y - vertices[0].y);
		height = sqrt(height);

		double area = cv::contourArea(contours.at(j));

		double longSide = width<height ? height : width;	
		double shortSide = height <  width ? height : width;
		double longOverShortRatio=longSide/shortSide;

		double widthAngle = atan2f((vertices[1].y-vertices[0].y),(vertices[1].x-vertices[0].x));
		widthAngle = widthAngle * 180/CV_PI;

		double heightAngle = atan2f((vertices[3].y-vertices[0].y),(vertices[3].x-vertices[0].x));
		heightAngle = heightAngle * 180/CV_PI;

		double longSideAngle=height>width?heightAngle:widthAngle;
		double shortSideAngle=heightAngle<=-90?heightAngle+90:heightAngle-90;


		auto meanCoord=cv::mean(contours.at(j));
		if(int(meanCoord[1]+0.5)>=(baseline-5) && int(meanCoord[1]+0.5)<=(baseline+5))
			continue;
		else if(int(meanCoord[1]+0.5)>=(baseline-SPECIAL0-5) && int(meanCoord[1]+0.5)<=(baseline-SPECIAL0+5))
			continue;
		else if(int(meanCoord[1])>=(baseline-SPECIAL1-5) && int(meanCoord[1])<=(baseline-SPECIAL1+5))
			continue;
		else if(int(meanCoord[1])>=(baseline+SPECIAL2-4) && int(meanCoord[1])<=(baseline+SPECIAL2+4))
		{
			if( abs(longSideAngle)<=15 || abs(abs(longSideAngle)-180)<=15)
				continue;
		}

		if(meanCoord[1]<=(baseline-SPECIAL1))
		{
			if(longSide<=VFLAWLEN)
				continue;
		}


		if(abs(longSideAngle+145)<=15){
			continue;
		}
		//writeAngle((float)longSideAngle,"c:\\1");



		resultContours.push_back(contours.at(j));

	}
}

void SobelBatch(char * txtName,char * fnametxt)
{
	Mat image = imread(txtName);
	vector<vector<cv::Point>> resultContours;
	vector<vector<cv::Point>> resultContours1;
	int baseLine=findBaseLine(image);
	cv::Mat bgrChannels[3];

	int upBaseLine=baseLine-UPPER0;

	int clipx=0;
	int clipy=upBaseLine;

	cv::Rect quenchRoi(clipx,clipy,image.cols,ROIH0);
	cv::Mat quenchPartImage=image(quenchRoi).clone();
	imwrite("c:\\1\\rlt.jpg",quenchPartImage);
	cv::Mat resultImage=image.clone();


	cv::split(quenchPartImage,bgrChannels);
	SobelRespectiveXY(bgrChannels[1],resultContours,clipx,clipy);
	ContourPreciseFilter(resultContours,resultContours1,baseLine);
	for(int j=0;j<resultContours1.size();j++){
		cv::drawContours(resultImage,resultContours1,j,cv::Scalar(0,0,255),CV_FILLED);
	}
	cv::imwrite(fnametxt,resultImage);
}
void SobelAmp(const cv::Mat &image,vector<vector<cv::Point>> &resultContours, int clipx,int clipy)
{
	cv::Mat greenXSobel,greenYSobel,sobelRlt;

	

	cv::Sobel(image,greenYSobel,CV_8U,0,1);
	cv::Sobel(image,greenXSobel,CV_8U,1,0);
	sobelRlt=abs(greenYSobel)+abs(greenXSobel);
	cv::threshold(sobelRlt,sobelRlt,35,255,CV_THRESH_BINARY);
	cv::Mat temp=sobelRlt.clone();

	vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(temp,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    ContourGeneralFilter(contours,resultContours,clipx,clipy);

//////////////////////////////////////////////////////////////////////////
	

}
void SobelAmpBatch(char * txtName,char * fnametxt)
{
	Mat image = imread(txtName);
	vector<vector<cv::Point>> resultContours;
	vector<vector<cv::Point>> resultContours1;
	int baseLine=findBaseLine(image);
	cv::Mat bgrChannels[3],sobelRlt;

	int upBaseLine=baseLine-UPPER0;

	int clipx=0;
	int clipy=upBaseLine;

	cv::Rect quenchRoi(clipx,clipy,image.cols,ROIH0);
	cv::Mat quenchPartImage=image(quenchRoi).clone();
	imwrite("c:\\1\\rlt.jpg",quenchPartImage);
	cv::Mat resultImage=image.clone();


	cv::split(quenchPartImage,bgrChannels);

	SobelAmp(bgrChannels[1],resultContours,clipx,clipy);
	ContourPreciseFilter(resultContours,resultContours1,baseLine);
	for(int j=0;j<resultContours1.size();j++){
		cv::drawContours(resultImage,resultContours1,j,cv::Scalar(0,0,255),CV_FILLED);
	}
	cv::imwrite(fnametxt,resultImage);


}





int _tmain(int argc, _TCHAR* argv[])
{

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char fnametxt[_MAX_PATH];
	char fnameExceptExt[_MAX_PATH];
	char * filePath = "C:\\2"; 
	vector<string> filesTxt;  
	getFiles(filePath, filesTxt);  
	for (int k = 0;k <  filesTxt.size();k++)  
	{  
		char * txtName=(char *) filesTxt[k].c_str();
		memset(drive,0,sizeof(drive)/sizeof(drive));
		memset(dir,0,sizeof(dir)/sizeof(dir));
		memset(fname,0,sizeof(fname)/sizeof(fname));
		memset(ext,0,sizeof(ext)/sizeof(ext));
		memset(fnametxt,0,sizeof(fnametxt)/sizeof(fnametxt));
		memset(fnameExceptExt,0,sizeof(fnameExceptExt)/sizeof(fnameExceptExt));
		_splitpath(txtName, drive, dir, fname, ext);
		strcat(fnametxt,"C:\\3\\");
		strcat(fnametxt,fname);
		strcat(fnametxt,ext);
		///////////////////////////////
		SobelBatch(txtName,fnametxt);
		//calRAngle(txtName,fnametxt);
		//SobelAmpBatch(txtName,fnametxt);
		//////////////////////////////
	
	}

//cvWaitKey(0);
return 0;
}




//////////////////////////////////BATCH//////////////////////////////////////
//char drive[_MAX_DRIVE];
//char dir[_MAX_DIR];
//char fname[_MAX_FNAME];
//char ext[_MAX_EXT];
//char fnametxt[_MAX_PATH];
//char fnameExceptExt[_MAX_PATH];
//char * filePath = "C:\\2"; 
//vector<string> filesTxt;  
//getFiles(filePath, filesTxt);  
//for (int k = 0;k <  filesTxt.size();k++)  
//{  
//	char * txtName=(char *) filesTxt[k].c_str();
//	memset(drive,0,sizeof(drive)/sizeof(drive));
//	memset(dir,0,sizeof(dir)/sizeof(dir));
//	memset(fname,0,sizeof(fname)/sizeof(fname));
//	memset(ext,0,sizeof(ext)/sizeof(ext));
//	memset(fnametxt,0,sizeof(fnametxt)/sizeof(fnametxt));
//	memset(fnameExceptExt,0,sizeof(fnameExceptExt)/sizeof(fnameExceptExt));
//	_splitpath(txtName, drive, dir, fname, ext);
//	strcat(fnametxt,"C:\\3\\");
//	strcat(fnametxt,fname);
//	strcat(fnametxt,ext);
//	///////////////////////////////
//	//SobelBatch(txtName,fnametxt);
//	//calRAngle(txtName,fnametxt);
//	//////////////////////////////
//
//}



/////////////////////////////////////////////////////////Store >255 Values
	//cv:Mat juzheng;
	//int fuzhi = 316;
	//juzheng.at<int>(0,0) = fuzhi;
	//int jieguo  = juzheng.at<int>(0,0);
	//int Rlt=111;
/////////////////////////////////////////////////////////Test RGB values
/*
Mat src, srcOfMerge,medImg;
Mat mv[3];
src = imread("c://1//1.jpg");
int cols = src.cols;
int rows = src.rows;
uchar *a = src.data;
int data = (int)a[1];  //
if (!src.data) { printf("Error loading src \n"); return -1; }
split(src, mv);
uchar *gPixel = mv[1].data;   //  
int gPixelData = (int)gPixel[0]; 
imwrite("c:\\1\\green.jpg",mv[1]);
*/
////////////////////////////////////////////////////////////////////////////////////
/*
 IplImage *src = cvLoadImage("C:\\1\\22222.jpg",0); 
 int m = src->height; 
 int n = src->width; 
 int i, j; 
 float *p = new float[m*n]; 
 unsigned char valb;
    for(i=0;i<m;i++)
    {
        for(j=0;j<n;j++)
        {
           valb =((unsigned char *)(src->imageData+src->widthStep*i))[j];
            // 计数加1
           *(p+n*i+j)=(float)valb;
        }
    }
	Mat mat=cv::Mat(m,n,CV_32FC1,p);
	Mat outmat1,outmat2;
	float k1[]={-1,0,1};  //水平方向的核
	float k2[3][1]={-1,0,1};  //垂直的核
	Mat Kore=Mat(1,3,CV_32FC1,k1);
	Mat Kore2=Mat(3,1,CV_32FC1,k2);
	filter2D(mat,outmat1,CV_32FC1,Kore);    //水平卷积运算
	filter2D(mat,outmat2,CV_32FC1,Kore2);  //垂直卷积运算
	Mat tidu;
	Mat jiaodu;
	cartToPolar(outmat1,outmat2,tidu,jiaodu,true);  //角度的结果在0-360之间
	FILE* file=fopen("C:\\1\\angle.dat","wb");
    fwrite(p,sizeof(float),m*n,file);
	fclose(file);
	delete [] p;
*/
////////////////////////////////////////////////////////////////////////////////////
//	char text[10];
//	RNG rng(12345);
//	Mat pSrcImage = imread("c:\\1\\Thresh.jpg", 0);
//	if (!pSrcImage.data)
//		return 0;
//	imshow("Image", pSrcImage);
////	threshold(pSrcImage, pSrcImage, 200, 255, cv::THRESH_BINARY_INV);
//	imwrite("binary.jpg", pSrcImage);
//	vector<std::vector<cv::Point>> contours;
//	Mat canny_output;
//	Canny(pSrcImage, canny_output, 80, 255, 3);
//	imwrite("C:\\1\\canny_output.jpg",canny_output);
//	vector<Vec4i> hierarchy;
//	findContours(pSrcImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
//	vector<RotatedRect> minRect(contours.size());
//	for (unsigned int i = 0; i < contours.size(); i++)
//	{
//		minRect[i] = minAreaRect(Mat(contours[i]));
//	}
//	/// 绘出轮廓及其可倾斜的边界框
//	CvBox2D  End_Rage2D;
//	CvPoint2D32f rectpoint[4];
//	Point2f rect_points[4];
//	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
//	for (unsigned int i = 0; i< contours.size(); i++)
//	{
//		End_Rage2D = minAreaRect(Mat(contours[i]));
//		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
//
//		// contour
//	//	drawContours(drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
//		// rotated rectangle
//		Point2f rect_points[4]; 
//		minRect[i].points(rect_points);
//		for (unsigned int j = 0; j < 4; j++)
//		{
//         line(drawing, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
//	//	 writeImgInfo(rect_points[j].x,rect_points[j].y,"coord");
//	//	 cout << "坐标点："<<rect_points[j] << "........" << rect_points[(j + 1) % 4] << endl;
//		}
//	
//		int a = 0, b = 0, k = 0;
//		a = (int)sqrt((pow((rect_points[0].x - rect_points[1].x), 2) + pow((rect_points[0].y - rect_points[1].y), 2)));  //width
//		b = (int)sqrt((pow((rect_points[0].x - rect_points[3].x), 2) + pow((rect_points[0].y - rect_points[3].y), 2)));  //heigh
//		if (a<b)
//		{
//				k = a;
//				a = b;
//				b = k;
//		}
//		double area = cv::contourArea(contours.at(i));
//		if (area<5 || a<5 || b<5)
//		{
//		        drawContours(drawing,contours,i,cv::Scalar(0),CV_FILLED);
//				itoa(i,text,10);
//	putText(drawing,text,rect_points[0],cv::FONT_HERSHEY_PLAIN,1,CV_RGB(255,255,255));
//
//		}
//	
//		 printf("最小外接矩形的长为：%d，宽为：%d。\n\n", a, b);	
//		 writeAngle((float)End_Rage2D.angle,i);
//	//cout << " angle:\n" << 90+(float)End_Rage2D.angle << std::endl;      //旋转角度 
//
//	}    
//	namedWindow("Contours1", CV_WINDOW_AUTOSIZE);
//	imshow("Contours1", drawing);
//	imwrite("C:\\1\\output.jpg",drawing);
//	waitKey(0);
//	return 0;
/////////////////////////////////////////////////////////////////////////////////////////////
/*
	Mat labImage = imread("c://1//831.jpg");
    cv::Mat labChannels[3];
    cv::split(labImage,labChannels);
    int baseLine=findBaseLine(labImage);

    //cv::Rect roi1Rect(0,baseLine-405,labImage.cols,30);
    //cv::Mat roi1Image=labChannels[0](roi1Rect).clone();

    //cv::Rect roi2Rect(0,baseLine-370,labImage.cols,325);
    //cv::Mat roi2Image=labChannels[0](roi2Rect).clone();

    //cv::Rect roi3Rect(0,baseLine-40,labImage.cols,37);
    //cv::Mat roi3Image=labChannels[0](roi3Rect).clone();

    cv::Rect roi4Rect(0,baseLine+5,labImage.cols,450);  //300
    cv::Mat roi4Image=labChannels[1](roi4Rect).clone();

	cv::Mat resultImage= Mat::zeros(roi4Rect.size(), CV_8UC3);
	cv::Mat resultImage2=labImage(roi4Rect).clone();
	cv::imwrite("C:\\1\\811g.jpg",roi4Image);
  
    vector<std::vector<cv::Point>> resultContours;
	vector<std::vector<cv::Point>> resultContours2;
    //60*30
    for(int i=40;i<(roi4Image.cols-60);){
        for(int j=0;j<(roi4Image.rows-30);){
            cv::Rect windowRect(i,j,60,30);
            cv::Mat slidingWindow=roi4Image(windowRect).clone();

//            cv::Mat printIamge=roi4Image.clone();
//            cv::rectangle(printIamge,windowRect,cv::Scalar(0,0,255),2);
//            cv::imshow("roi4",printIamge);
			int channel=0;
			cv::MatND dstHist;
			int histSize[]={256};
			float midRanges[]={0,255};
			const float *ranges[]={midRanges};
			cv::calcHist(&slidingWindow,1,&channel,cv::Mat(),dstHist,1,histSize,ranges,true,false);

			float sumPixel=0;
			int thresholdValue;
			for(int l=0;l<256;l++){
				sumPixel+=dstHist.at<float>(l);
				float ratio=sumPixel/(slidingWindow.rows*slidingWindow.cols);
				if(ratio>0.93){
					thresholdValue=l;
					break;
				}
			}


			//int thres = IterationThreshold(&(CvMat)slidingWindow);

            cv::Mat thresholdDst;
            cv::threshold(slidingWindow,thresholdDst,thres,255,CV_THRESH_BINARY);
			

			 vector<std::vector<cv::Point>> contours;
            std::vector<cv::Vec4i> hierarchy;
            cv::Mat temp=thresholdDst.clone();
            cv::findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_NONE);
            cv::Mat printContoursIamge=thresholdDst.clone();
			
            for(int k=0;k<contours.size();k++){
                cv::RotatedRect rRc=cv::minAreaRect(contours.at(k));
                cv::Point2f vertices[4];
                rRc.points(vertices);

                double width = (vertices[0].x - vertices[1].x)* (vertices[0].x - vertices[1].x) + (vertices[0].y - vertices[1].y) * (vertices[0].y - vertices[1].y);
                width = sqrt(width);
                double height = (vertices[1].x - vertices[2].x)* (vertices[1].x - vertices[2].x) + (vertices[1].y - vertices[2].y) * (vertices[1].y - vertices[2].y);
                height = sqrt(height);
                double area = cv::contourArea(contours.at(k));


				if(area<=5 ||width<=0 ||height<=0){
					cv::drawContours(printContoursIamge,contours,k,cv::Scalar(0),CV_FILLED);
					continue;
				}

                double rRcArea=width*height;
                if(rRcArea<35){
                    cv::drawContours(printContoursIamge,contours,k,cv::Scalar(0),CV_FILLED);
                    continue;
                }


                double longSide = width<height ? height : width;
                double shortSide = height<width ? height : width;
                double longOverShortRatio=longSide/shortSide;
                if(longOverShortRatio<3.5){
                    cv::drawContours(printContoursIamge,contours,k,cv::Scalar(0),CV_FILLED);
                    continue;
                }




                auto meanCoord=cv::mean(contours.at(k));
                if(meanCoord[0]<=(slidingWindow.cols/6)){
                    cv::drawContours(printContoursIamge,contours,k,cv::Scalar(0),CV_FILLED);
                    continue;
                }
                if(meanCoord[0]>=(slidingWindow.cols*5/6)){
                    cv::drawContours(printContoursIamge,contours,k,cv::Scalar(0),CV_FILLED);
                    continue;
                }

                std::vector<cv::Point> tempContours=contours.at(k);

                for(int n=0;n<tempContours.size();n++){
                    cv::Point temp=tempContours.at(n);
                    tempContours.at(n).x=temp.x+i;
                    tempContours.at(n).y=temp.y+j;
                }

                resultContours.push_back(tempContours);


            }

            j+=10;
        }
        i+=10;
    }

    for(int j=0;j<resultContours.size();j++){
        cv::drawContours(resultImage,resultContours,j,cv::Scalar(255,255,255),CV_FILLED);
    }
 

	std::string savePath="C:\\1\\811-result.jpg";
	cv::Mat labChannels2[3];
	cv::split(resultImage,labChannels2);
	vector<std::vector<cv::Point>> contoursAll;
	std::vector<cv::Vec4i> hierarchyAll;
	cv::findContours(labChannels2[1],contoursAll,hierarchyAll,CV_RETR_CCOMP,CV_CHAIN_APPROX_NONE);
	for(int k=0;k<contoursAll.size();k++){
		cv::RotatedRect rRcAll=cv::minAreaRect(contoursAll.at(k));
		cv::Point2f verticesAll[4];
		rRcAll.points(verticesAll);

		double widthAll = (verticesAll[0].x - verticesAll[1].x)* (verticesAll[0].x - verticesAll[1].x) + (verticesAll[0].y - verticesAll[1].y) * (verticesAll[0].y - verticesAll[1].y);
		widthAll = sqrt(widthAll);

		double heightAll = (verticesAll[1].x - verticesAll[2].x)* (verticesAll[1].x - verticesAll[2].x) + (verticesAll[1].y - verticesAll[2].y) * (verticesAll[1].y - verticesAll[2].y);
		heightAll = sqrt(heightAll);

		
		if (widthAll>heightAll)
		{
			if (rRcAll.angle>(-35) && rRcAll.angle<(-10))
			{
				cv::drawContours(resultImage,contoursAll,k,cv::Scalar(0,0,0),CV_FILLED);
				continue;
			}
		}


	std::vector<cv::Point> tempContours2=contoursAll.at(k);
	resultContours2.push_back(tempContours2);
	}

	for(int j=0;j<resultContours2.size();j++){
		cv::drawContours(resultImage2,resultContours2,j,cv::Scalar(0,0,255),CV_FILLED);
	}

  //  cv::imshow("result",resultImage);
    
    cv::imwrite(savePath,resultImage2);
    cv::waitKey(0);
*/
//////////////////////////////OSTU////////////////////////////////////////////////////
//Mat labImage = imread("C:\\1\\888.jpg");
//cv::Mat labChannels[3];
//cv::split(labImage,labChannels);
//cv::Mat slidingWindow=labChannels[1].clone();
//cv::Mat thresholdDst;
//cv::threshold(slidingWindow,thresholdDst,150,255,CV_THRESH_BINARY|CV_THRESH_OTSU);
//cv::imwrite("C:\\1\\otsu.jpg",thresholdDst);
//////////////////////////////Iteration
//Mat labImage = imread("C:\\1\\888.jpg");
//cv::Mat labChannels[3];
//cv::split(labImage,labChannels);
//cv::Mat slidingWindow=labChannels[1].clone();
//int thres = IterationThreshold(&(CvMat)slidingWindow);
//cv::Mat thresholdDst;
//cv::threshold(slidingWindow,thresholdDst,thres,255,CV_THRESH_BINARY);
//cv::imwrite("C:\\1\\Iteration.jpg",thresholdDst);
//////////////////////////////Adapt
//Mat labImage = imread(txtName);
//cv::Mat labChannels[3];
//cv::split(labImage,labChannels);
//cv::Mat slidingWindow=labChannels[1].clone();
//int channel=0;
//cv::MatND dstHist;
//int histSize[]={256};
//float midRanges[]={0,255};
//const float *ranges[]={midRanges};
//cv::calcHist(&slidingWindow,1,&channel,cv::Mat(),dstHist,1,histSize,ranges,true,false);
//float sumPixel=0;
//int thresholdValue;
//for(int l=0;l<256;l++){
//	sumPixel+=dstHist.at<float>(l);
//	float ratio=sumPixel/(slidingWindow.rows*slidingWindow.cols);
//	if(ratio>0.99){
//		thresholdValue=l;
//		break;
//	}
//}
//cv::Mat thresholdDst;
//cv::threshold(slidingWindow,thresholdDst,thresholdValue,255,CV_THRESH_BINARY);
//cv::imwrite(fnametxt,thresholdDst);
/////////////////////////////////////////////////////////////////////////////////////











		


//////Iris Processing!!!!//////////////////////////////////////////////////////////////
//bool bFlag=0;
//char a[1024],b[1024];
//CharacterVector CharaV1[Character_length];
//std::cout<<"Please input the path of first iris image:"<<std::endl;
//std::cin>>a;
//checkImage(a,&bFlag);  //校验某一个虹膜图片是否可以满足生成特征码的要求
//if(bFlag==1)
//   transferCode(a,CharaV1);
//checkImage(a,&bFlag);
//detectEye1(&bFlag);
///////////////////////////////////////////////////////////////////////////////



	

/*
	bool bFlag=0;
	IplImage *pSrc=cvLoadImage("c://1//vertical.jpg",1); //1是3波段
	IplImage *  gray=cvCreateImage(cvGetSize(pSrc),pSrc->depth,1);//分配图像空间
	IplImage *Gband =cvCreateImage(cvGetSize(pSrc),pSrc->depth,1);
    IplImage * sobel=cvCreateImage(cvGetSize(pSrc),pSrc->depth,1);
	unsigned char * img= (unsigned char *)pSrc->imageData;
	IplImage *p16SX = cvCreateImage(cvSize(pSrc->width,pSrc->height),IPL_DEPTH_16S,1);
	IplImage *p16SY = cvCreateImage(cvSize(pSrc->width,pSrc->height),IPL_DEPTH_16S,1);

	IplImage *sobel8u=cvCreateImage(cvGetSize(pSrc),IPL_DEPTH_8U,1);
	//int width=src->width;
	//int height=src->height;
	//int channels=src->nChannels;
    int depth=pSrc->depth;
	//int widthStep=src->widthStep;
	//detectEye(img,width,height,depth,channels,&bFlag);
	//detectEye1();
//	cvCvtColor(pSrc,gray,CV_BGR2GRAY);
	p16SX->origin = pSrc->origin; 
    p16SY->origin = pSrc->origin; 

//cvSobel(pSrc,p16SX,1,0,3);//x方向的边缘 
//cvSobel(pSrc,p16SY,0,1,3);//y方向的边缘 
cvSobel(gray,sobel,0,1,3);

cvNamedWindow("frame");

cvNamedWindow("gray");

cvNamedWindow("sobel");

cvShowImage("frame",pSrc);//揭示图像

cvShowImage("gray",gray);

cvShowImage("sobel",sobel);
	cvSaveImage("C://1//sx24.jpg",sobel);
cvWaitKey(0);//期待



cvMul(p16SX,p16SX,p16SX,1.0); 
cvMul(p16SY,p16SY,p16SY,1.0); 
cvAdd(p16SX,p16SY,p16SX,0); 


cvConvertScaleAbs(p16SX,sobel8u,1,0);





	cvReleaseImage(&pSrc);

	cvReleaseImage(&p16SX);
	cvReleaseImage(&p16SY);
    cvReleaseImage(&sobel8u);
	cvReleaseImage(&gray);
    cvReleaseImage(&sobel);
	cvReleaseImage(&Gband);





/*
	char a[1024],b[1024];
	std::cout<<"Please input the path of first iris image:"<<std::endl;
    std::cin>>a;
    std::cout<<"Please input the path of second iris image:"<<std::endl;
    std::cin>>b;

  //IplImage *src=cvLoadImage("0011.bmp",0); //0011.jpg,1  "0011.bmp",0
	IplImage *src=cvLoadImage(a,0);
	CharacterVector CharaV1[Character_length];
	GetIrisVector(src,CharaV1,Character_length);
  //IplImage *src2=cvLoadImage("003.bmp",0); 
	IplImage *src2=cvLoadImage(b,0); 
	CharacterVector CharaV2[Character_length];
	GetIrisVector(src2,CharaV2,Character_length);
	float lamda=Compare(CharaV1,CharaV2,Character_length,Character_length);

/////
		FileName_Chara FileCharaV1,FileCharaV2;
	FileCharaV1.FileName="D:/DLL/对比_1.txt";
	FileCharaV2.FileName="D:/DLL/对比_2.txt";
	FileCharaV1.Chara=CharaV1;
	FileCharaV2.Chara=CharaV2;
	MySaveCharaV(FileCharaV1,Character_length);
	MySaveCharaV(FileCharaV2,Character_length);
//////



	if(lamda>0.11)
		std::cout<< "Iris matched!!!"<<std::endl;
	else
		std::cout<< "NOT MATCH"<<std::endl;
	/*
    long lWidth = src->width;
    long lHeight= src->height;
	cvFlip(src,NULL,0);
    long threshold=Threshold(src,lWidth,lHeight);
    std::cout<< threshold<<std::endl;
	IplImage *dst=cvCreateImage(cvSize(lWidth,lHeight),IPL_DEPTH_8U,1);
	memset(dst->imageData,1,dst->imageSize);
    PupilContour(dst,lWidth,lHeight,threshold,src);
    std::list<PHTPoint>  PointListForPHT;
    FindPHTPoint(dst,&PointListForPHT,lHeight,lWidth);
    CIRCLE InnerCircle=PHT(&PointListForPHT);
    PointListForPHT.clear();
    CIRCLE Outer=OuterCircle(src,lWidth,lHeight,InnerCircle);
    const long RectWidth=512;
    const long RectHeight=128;
  	IplImage *hDIB=cvCreateImage(cvSize(RectWidth,RectHeight),IPL_DEPTH_8U,1);
    TurnToRect(InnerCircle,Outer,src,hDIB,RectWidth,RectHeight);
	LOGBin(hDIB);
	Morphological(hDIB);
	int tab=0;
	PointInformation pointInfor[255];
	MarkthePoint(hDIB,pointInfor,tab);
	CharacterVector CharaV1[Character_length];
	GetCharacterVector(pointInfor,tab,CharaV1,Character_length);
	*/

	/*
	src=cvLoadImage("003.bmp",0); 
	lWidth = src->width;
    lHeight= src->height;
	cvFlip(src,NULL,0);
    threshold=Threshold(src,lWidth,lHeight);
    std::cout<< threshold<<std::endl;
	IplImage *lpRecorder=cvCreateImage(cvSize(lWidth,lHeight),IPL_DEPTH_8U,1);
    memset(lpRecorder->imageData,1,lpRecorder->imageSize);
	PupilContour(lpRecorder,lWidth,lHeight,threshold,src);
	FindPHTPoint(lpRecorder,&PointListForPHT,lHeight,lWidth);
	InnerCircle=PHT(&PointListForPHT);
	PointListForPHT.clear();
	Outer=OuterCircle(src,lWidth,lHeight,InnerCircle);
	TurnToRect(InnerCircle,Outer,src,hDIB,RectWidth,RectHeight);
	LOGBin(hDIB);
	Morphological(hDIB);
	tab=0;
	MarkthePoint(hDIB,pointInfor,tab);
	CharacterVector CharaV2[Character_length];
	GetCharacterVector(pointInfor,tab,CharaV2,Character_length);
	float lamda=Compare(CharaV1,CharaV2,Character_length,Character_length);
	if(lamda>0.11)
		std::cout<< "是同一虹膜！"<<std::endl;
	else
		std::cout<< "不是同一虹膜！"<<std::endl;

*/


