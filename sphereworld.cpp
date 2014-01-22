/*
20140122更新部分
完成邊界判斷
增加死亡動畫
遊戲重新開始後會重置設定
更新血調
增加combo系統
調整遊戲難度
*/
//==========
#include <stdlib.h>
#include <time.h>
#include <string>
#include "weapon.h"
#include "actor.h"
#include "shared/gltools.h"        // OpenGL toolkit
#include "shared/math3d.h"    // 3D Math Library
#include "shared/glframe.h"   // Frame class
#include <math.h>
#include "glm.h"
#include <stdio.h>
using namespace std;
//==========2D用
#include <windows.h>
#include <GL/glut.h>
void DrawFrame()
{
    // 紅 X
    glBegin(GL_LINES);
    glColor3ub(255,0,0);
    glVertex3f(0,0,0);
    glVertex3f(1,0,0);
    glEnd();
    // 黃 Y
    glBegin(GL_LINES);
    glColor3ub(255,255,0);
    glVertex3f(0,0,0);
    glVertex3f(0,1,0);
    glEnd();
    // 綠 Z
    glBegin(GL_LINES);
    glColor3ub(0,255,0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,1);
    glEnd();

    glColor3ub(255,255,255);
}
//============視角切換==========
int view = 4; //4為45度視角, 2為90度視角
//============視窗==========
int screenWidth = 800 , screenHeight = 600; // 視窗大小
//============遊戲分數==========
bool gameover = 0;        // 目前是否遊戲結束 20140113
int highscore = 0;        // 目前最高分數 20140113
int gamescore = 0;        // 目前遊戲分數
//============鍵盤================
const unsigned int MAX_KEY_STATE = 256;
const unsigned int MAX_SPECIAL_KEY_STATE = 256;
bool keyState[MAX_KEY_STATE];
bool specialKeyState[MAX_SPECIAL_KEY_STATE];
//============鍵盤================
#define NUM_MAP_OBJS    5  // 有幾種場景物體
#define NUM_BODYPARTS    10
#define NUM_WEAPONS    1
#define BUG_NUM      15
//===========子彈用
GLMmodel *BULL;
//===========
float obstaclex[BUG_NUM]= {0};
float obstacley[BUG_NUM]= {0};
int casualty=-1;
int hurt=0;
int bdamage=10;  //蟲傷害係數
float targetx=1;
float targety=1;
float targetz=1.75;
int face=0;
int combo=0;
int combotime=0;
int combolimit;
int achieve=0;
int num=5;
/////bullet class
int check_bound(float x,float y);
class bullet
{
public:
    bullet();
    ~bullet();
    void draw();
    void action();
    void setb();
    int hit();
    float bx;
    float by;
    float bz;
    int drawbullet;
    int vec;
    int damage;
};
bullet::bullet()
{
    drawbullet=0;
    damage=25; //====子彈傷害係數
    bx=targetx+cos(3.1415926/4*(vec-2));
    by=targety+sin(3.1415923/4*(vec-2));
    bz=-0.25;
    //=========讀子彈obj
    BULL=glmReadOBJ("weapon/bullet.obj");
    glmUnitize(BULL);
    glmFacetNormals(BULL);
    glmVertexNormals(BULL,60);
}
bullet::~bullet() {}
void bullet::draw()
{
    glTranslatef(bx,by,bz);
    //glutSolidSphere(0.1,10,10);
    glColor3f(1.0,1.0,1.0);
    glScalef(0.15,0.15,0.15);
    glRotatef(90,0,1,0);
    glRotatef(-45*(vec-2),1,0,0);
    glmDraw(BULL, GLM_SMOOTH|GLM_TEXTURE);
}

void bullet::action()
{
    if(drawbullet==1)
    {
        draw();
    }
    bx+=0.3*cos(3.1415926/4*(vec-2));
    by+=0.3*sin(3.1415923/4*(vec-2));
    if(pow(bx-targetx,2)+pow(by-targety,2)>100)
    {
        drawbullet=0;
    }
}
void bullet::setb()
{
    vec=face;
    bx=targetx+0.5*cos(3.1415926/4*(vec-2));
    by=targety+0.5*sin(3.1415923/4*(vec-2));
    drawbullet=1;
}
int bullet::hit()
{
    for(int i=0; i<num; i++)
    {
        if(pow(obstaclex[i]-bx,2)+pow(obstacley[i]-by,2)<0.5)
        {
            drawbullet=0;
            by=-100;
            bx=-100;
            return i;
        }
    }
    return -1;
}

/////bug class
class bug
{
public:
    bug();
    ~bug();
    void healthdamage(int d);
    void action(float x,float y);
    void attack(int i);
    void drawbug(int i);
    void attacked(int i);
    void crawl(int i);
    void die(int i);
    void moving(double targetx,double targety);
    void setlocation(float x,float y,float z);
    void setnumber(int i);
    float getx();
    float gety();
    void behit();
    int sattack;
    int sattacked;
    int scrawl;
    int sdie;
    int health;
    float speed;


private:
    float angle[6][5];
    float flip;
    double turn;
    float movex;
    float movey;
    float movez;
    int head;
    int antenna;
    int attacktimer;
    int attackedtimer;
    int dietimer;
    int crawltimer;

    int number;
    int damaged;


};
bug::bug()
{
    flip=0;
    sdie=0;
    scrawl=0;
    dietimer=0;
    crawltimer=0;
    attacktimer=0;
    attackedtimer=0;
    movex=0;
    movey=0;
    movez=0;
    head=0;
    antenna=-30;
    sattack=0;
    health=100;
    speed=0.02; //====蟲的移動速度
    damaged=10;
    for(int i=0; i<6; i++)
    {
        if(i==0||i==3)
        {
            angle[i][0]=-30;
        }
        else if(i==1||i==4)
        {
            angle[i][0]=0;
        }
        else
        {
            angle[i][0]=30;
        }
        angle[i][1]=-20;
        angle[i][2]=60;
        angle[i][3]=50;
        angle[i][4]=0;
    }
}
bug::~bug() {}
float bug::getx()
{
    return movex;
}
float bug::gety()
{
    return movey;
}
void bug::setnumber(int i)
{
    number=i;
}
void bug::drawbug(int color)
{

    float x,y,z,x2,y2,y3,y4,z2,z3,z4;
    float rad=3.14159/180.0;

    /*if(color == 0)
        glColor3ub(128, 128, 128);
    else
        glColor3ub(0,0,0);*/

    M3DVector3f vNormal;

    glLineWidth(1.5);



    glTranslatef(movex,movey,movez);

    //glutSolidSphere(0.5,10,10);
    glRotatef(turn,0,0,1);
    glRotatef(180,0,0,1);
    glRotatef(flip,0,1,0);


    glBegin(GL_QUAD_STRIP);//body

    for(int i=30; i<=120; i+=30)
    {

        x=0.4*cos(rad*i);
        x2=0.4*cos(rad*(i-30));
        for(int j=30; j<=360; j+=30)
        {

            if(j<=180)
            {
                y=0.25*sin(rad*i)*cos(rad*j);
                y2=0.25*sin(rad*(i-30))*cos(rad*j);
                y3=0.25*sin(rad*(i))*cos(rad*(j-30));
                y4=0.25*sin(rad*(i-30))*cos(rad*(j-30));
                z=0.25*sin(rad*i)*0.8*sin(rad*j);
                z2=0.25*sin(rad*(i-30))*0.8*sin(rad*j);
                z3=0.25*sin(rad*(i))*0.8*sin(rad*(j-30));
                z4=0.25*sin(rad*(i-30))*0.8*sin(rad*(j-30));

            }
            else
            {
                y=0.25*sin(rad*i)*0.8*cos(rad*j);
                y2=0.25*sin(rad*(i-30))*0.8*cos(rad*j);
                y3=0.25*sin(rad*(i))*0.8*cos(rad*(j-30));
                y4=0.25*sin(rad*(i-30))*0.8*cos(rad*(j-30));
                z=0.25*sin(rad*i)*0.8*0.6*sin(rad*j);
                z2=0.25*sin(rad*(i-30))*0.8*0.6*sin(rad*j);
                z3=0.25*sin(rad*(i))*0.8*0.6*sin(rad*(j-30));
                z4=0.25*sin(rad*(i-30))*0.8*0.6*sin(rad*(j-30));
            }

            M3DVector3f vPoints[3] = {{ x, y, z },
                { x2, y4, z4 },
                { x, y3, z3 }
            };
            m3dFindNormal(vNormal, vPoints[0], vPoints[1], vPoints[2]);
            glNormal3fv(vNormal);
            glVertex3f(x,y,z);
            glVertex3f(x,y3,z3);
            glVertex3f(x2,y2,z2);
            glVertex3f(x2,y4,z4);
        }

    }

    glEnd();

    glPushMatrix();//head

    glTranslatef(0.4*cos(rad*135)-0.1*cos(rad*60),0,0);
    glRotatef(head,1,0,0);
    glutSolidSphere(0.125,13,26);
    glPushMatrix();
    glRotatef(-30,0,0,1);
    glRotatef(antenna,0,1,0);
    glTranslatef(-0.35,0.05,0.1);

    glBegin(GL_LINE_STRIP);
    for(int i=0; i<90; i+=1)
    {
        x=cos(rad*i)*0.3;
        z=sin(rad*i)*0.3;
        glVertex3f(x,0,z);
    }
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glRotatef(30,0,0,1);
    glRotatef(antenna,0,1,0);
    glTranslatef(-0.35,-0.05,0.1);
    glBegin(GL_LINE_STRIP);
    for(int i=0; i<90; i+=1)
    {
        x=cos(rad*i)*0.3;
        z=sin(rad*i)*0.3;
        glVertex3f(x,0,z);
    }
    glEnd();
    glPopMatrix();

    glPopMatrix();

    //legs

    glPushMatrix();
    glTranslatef(0.4*cos(rad*120)+0.07,0.25*0.8*sin(rad*120)*cos(rad*225)+0.03,0.25*0.8*0.6*sin(rad*120)*cos(rad*225)+0.02);
    glRotatef(angle[0][0],0,0,1);
    glRotatef(angle[0][1],1,0,0);
    glRotatef(angle[0][4],0,1,0);
    glTranslatef(0,-0.1,0);
    glPushMatrix();
    glScalef(0.05,0.2,0.05);
    glutSolidCube(1);
    glPopMatrix();
    glTranslatef(0,-0.1,0);
    glRotatef(angle[0][2],1,0,0);
    glTranslatef(0,-0.1,0);
    glPushMatrix();
    glScalef(0.05,0.2,0.05);
    glutSolidCube(1);
    glPopMatrix();
    glTranslatef(0,-0.1,0);
    glRotatef(angle[0][3],1,0,0);
    glRotatef(90,1,0,0);
    glutSolidCone(0.04,0.2,10,10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.4*cos(rad*90)+0.03,0.25*0.8*sin(rad*90)*cos(rad*225)+0.03,0.25*0.8*0.6*sin(rad*90)*cos(rad*225)+0.02);
    glRotatef(angle[1][0],0,0,1);
    glRotatef(angle[1][1],1,0,0);
    glRotatef(angle[1][4],0,1,0);
    glTranslatef(0,-0.1,0);
    glPushMatrix();
    glScalef(0.05,0.2,0.05);
    glutSolidCube(1);
    glPopMatrix();
    glTranslatef(0,-0.1,0);
    glRotatef(angle[1][2],1,0,0);
    glTranslatef(0,-0.1,0);
    glPushMatrix();
    glScalef(0.05,0.2,0.05);
    glutSolidCube(1);
    glPopMatrix();
    glTranslatef(0,-0.1,0);
    glRotatef(angle[1][3],1,0,0);
    glRotatef(90,1,0,0);
    glutSolidCone(0.04,0.2,10,10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.4*cos(rad*60)+0.03,0.25*0.8*sin(rad*90)*cos(rad*225)+0.03,0.25*0.8*0.6*sin(rad*90)*cos(rad*225)+0.02);
    glRotatef(angle[2][0],0,0,1);
    glRotatef(angle[2][1],1,0,0);
    glRotatef(angle[2][4],0,1,0);
    glTranslatef(0,-0.1,0);
    glPushMatrix();
    glScalef(0.05,0.2,0.05);
    glutSolidCube(1);
    glPopMatrix();
    glTranslatef(0,-0.1,0);
    glRotatef(angle[2][2],1,0,0);
    glTranslatef(0,-0.1,0);
    glPushMatrix();
    glScalef(0.05,0.2,0.05);
    glutSolidCube(1);
    glPopMatrix();
    glTranslatef(0,-0.1,0);
    glRotatef(angle[2][3],1,0,0);
    glRotatef(90,1,0,0);
    glutSolidCone(0.04,0.2,10,10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.4*cos(rad*60)+0.03,-(0.25*0.8*sin(rad*90)*cos(rad*225)+0.03),0.25*0.8*0.6*sin(rad*90)*cos(rad*225)+0.02);
    glRotatef(angle[3][0],0,0,1);
    glRotatef(-angle[3][1],1,0,0);
    glRotatef(angle[3][4],0,1,0);
    glTranslatef(0,0.1,0);
    glPushMatrix();
    glScalef(0.05,-0.2,0.05);
    glutSolidCube(1);
    glPopMatrix();
    glTranslatef(0,0.1,0);
    glRotatef(-angle[3][2],1,0,0);
    glTranslatef(0,0.1,0);
    glPushMatrix();
    glScalef(0.05,0.2,0.05);
    glutSolidCube(1);
    glPopMatrix();
    glTranslatef(0,0.1,0);
    glRotatef(-angle[3][3],1,0,0);
    glRotatef(-90,1,0,0);
    glutSolidCone(0.04,0.2,10,10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.4*cos(rad*90)+0.03,-(0.25*0.8*sin(rad*90)*cos(rad*225)+0.03),0.25*0.8*0.6*sin(rad*90)*cos(rad*225)+0.02);
    glRotatef(angle[4][0],0,0,1);
    glRotatef(-angle[4][1],1,0,0);
    glRotatef(angle[4][4],0,1,0);
    glTranslatef(0,0.1,0);
    glPushMatrix();
    glScalef(0.05,-0.2,0.05);
    glutSolidCube(1);
    glPopMatrix();
    glTranslatef(0,0.1,0);
    glRotatef(-angle[4][2],1,0,0);
    glTranslatef(0,0.1,0);
    glPushMatrix();
    glScalef(0.05,0.2,0.05);
    glutSolidCube(1);
    glPopMatrix();
    glTranslatef(0,0.1,0);
    glRotatef(-angle[4][3],1,0,0);
    glRotatef(-90,1,0,0);
    glutSolidCone(0.04,0.2,10,10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.4*cos(rad*120)+0.07,-(0.25*0.8*sin(rad*90)*cos(rad*225)+0.03),0.25*0.8*0.6*sin(rad*90)*cos(rad*225)+0.02);
    glRotatef(angle[5][0],0,0,1);
    glRotatef(-angle[5][1],1,0,0);
    glRotatef(angle[5][4],0,1,0);
    glTranslatef(0,0.1,0);
    glPushMatrix();
    glScalef(0.05,-0.2,0.05);
    glutSolidCube(1);
    glPopMatrix();
    glTranslatef(0,0.1,0);
    glRotatef(-angle[5][2],1,0,0);
    glTranslatef(0,0.1,0);
    glPushMatrix();
    glScalef(0.05,0.2,0.05);
    glutSolidCube(1);
    glPopMatrix();
    glTranslatef(0,0.1,0);
    glRotatef(-angle[5][3],1,0,0);
    glRotatef(-90,1,0,0);
    glutSolidCone(0.04,0.2,10,10);
    glPopMatrix();


}
void bug::action(float x,float y)
{


    if(sdie==1)
    {
        die(dietimer);
    }
    else if(sattacked==1)
    {
        attacked(attackedtimer);
    }
    else if(sattack==1)
    {
        attack(attacktimer);
    }
    else
    {
        crawl(crawltimer);
        moving(x,y);
    }
}
void bug::attack(int i)
{

    if(i<100)
    {

        if(i<25)
        {
            movex-=0.006*cos(turn/180*3.1415926);
            movey-=0.006*sin(turn/180*3.1415926);
            for(int j=0; j<6; j++)
            {

                angle[j][4]+=0.6;

            }
        }
        else if(i<75)
        {
            movex+=0.006*cos(turn/180*3.1415926);
            movey+=0.006*sin(turn/180*3.1415926);
            for(int j=0; j<6; j++)
            {

                angle[j][4]-=0.6;

            }
            if(i==50&&pow(movex-targetx,2)-pow(movey-targety,2)<0.5)
            {
                hurt++;
            }
        }
        else
        {
            movex-=0.006*cos(turn/180*3.1415926);
            movey-=0.006*sin(turn/180*3.1415926);
            for(int j=0; j<6; j++)
            {

                angle[j][4]+=0.6;

            }
        }
        attacktimer++;

    }
    else
    {
        attacktimer=0;
        sattack=0;
    }
    glutPostRedisplay();
}
void bug::attacked(int i)
{

    if(i<100)
    {

        if(i<25)
        {
            movex-=0.04*cos(turn/180*3.1415926);
            movey-=0.04*sin(turn/180*3.1415926);
            for(int j=0; j<6; j++)
            {

                angle[j][4]+=1;

            }
        }
        else if(i<75)
        {

        }
        else
        {
            movex+=0.006*cos(turn/180*3.1415926);
            movey+=0.006*sin(turn/180*3.1415926);
            for(int j=0; j<6; j++)
            {

                angle[j][4]-=1;

            }
        }
        attackedtimer++;

    }
    else
    {
        sattacked=0;
        attackedtimer=0;
    }
    glutPostRedisplay();
}
void bug::die(int i)
{
    if(i==0)
    {
        combotime=0;
        combo++;
        achieve=1;
        gamescore += 1000; // 成功殺死一隻蟲
        if(gamescore/1000>20&&speed<0.05)
        {
            speed+=0.01;
        }
    }
    if(i<200)
    {
        if(i<50)
        {
            flip=3.6*i;
        }
        else
        {
            flip=180;
            for(int j=0; j<6; j++)
            {
                if(j%2==0)
                {
                    angle[j][2]=60+30*sin(0.314159*2/5*i);
                    angle[j][3]=50+30*sin(0.314159*2/5*i);
                }
                else
                {
                    angle[j][2]=60-30*sin(0.314159*2/5*i);
                    angle[j][3]=50-30*sin(0.314159*2/5*i);
                }
            }
        }
        dietimer++;
        //shadow.RotateLocalZ(3.14159/20.0);
    }
    else
    {
        sdie=0;
        flip=0;
        dietimer=0;
        health=100;
        if(rand()%2==0)
        {
            setlocation(-1.7-rand()%6,-16-rand()%5,-1);
        }
        else
        {
            setlocation(13+rand()%5,3.5+rand()%6,-1);
        }

        for(int j=0; j<num; j++)
        {
            if(obstaclex[j]==movex)
            {
                if(obstacley[j]==movey)
                {
                    if(rand()%2==0)
                    {
                        setlocation(-1.7-rand()%6,-16-rand()%5,-1);
                    }
                    else
                    {
                        setlocation(13+rand()%5,3.5+rand()%6,-1);
                    }
                    j=-1;
                }
            }
        }
        for(int i=0; i<6; i++)
        {
            if(i==0||i==3)
            {
                angle[i][0]=-30;
            }
            else if(i==1||i==4)
            {
                angle[i][0]=0;
            }
            else
            {
                angle[i][0]=30;
            }
            angle[i][1]=-20;
            angle[i][2]=60;
            angle[i][3]=50;
            angle[i][4]=0;
        }
    }
    glutPostRedisplay();
}
void bug::crawl(int i)
{

    float t=1;

    if(i<40)
    {

        t=2*sin(3.14159/20*i);


        angle[0][0]=-30+10*t;
        angle[2][0]=60+10*t;
        angle[3][0]=-60+10*t;
        angle[5][0]=30+10*t;
        angle[1][0]=10-8*t;
        angle[4][0]=-10-8*t;
        angle[5][1]=-20-10*t;
        angle[5][2]=60+5*t;
        angle[5][3]=50+5*t;
        angle[5][4]=4*t;
        angle[4][1]=-20+5*t;
        //angle[4][2]=60+5*t;
        //angle[4][3]=50+5*t;
        angle[4][4]=-10*t;
        angle[3][1]=-20-10*t;
        angle[3][2]=60+5*t;
        angle[3][3]=50+5*t;
        angle[3][4]=2*t;
        angle[0][1]=-20+10*t;
        angle[0][2]=60+5*t;
        angle[0][3]=50+5*t;
        angle[0][4]=-4*t;
        angle[1][1]=-20-5*t;
        //angle[1][2]=60+5*t;
        //angle[1][3]=50+5*t;
        angle[1][4]=10*t;
        angle[2][1]=-20+10*t;
        angle[2][2]=60+5*t;
        angle[2][3]=50+5*t;
        angle[2][4]=-2*t;

        crawltimer++;

    }
    else
    {
        scrawl=0;
        crawltimer=0;
    }
    glutPostRedisplay();
}
void bug::setlocation(float x,float y,float z)
{
    movex=x;
    movey=y;
    movez=z;
}
void bug::moving(double targetx, double targety)
{

    double futurex=targetx-movex;
    double futurey=targety-movey;
    double otherx;
    double othery;
    double otherturn;
    int areaclear;
    int cyclecount=0;
    float originx=movex;
    float originy=movey;
    float faketurn;

    turn=atan(futurey/futurex)/3.1415926*180;

    if(futurex<0)
    {
        turn+=180;
    }
    if(turn<0)
    {
        turn+=360;
    }

    faketurn=turn;

    if(pow(futurex,2)+pow(futurey,2)>1)
    {
        while(1)
        {
            movex+=cos(faketurn/180*3.1415926)*speed;
            movey+=sin(faketurn/180*3.1415926)*speed;
            /*if (cyclecount>100)
            {
                movex=movex+cos((turn-180)/180*3.1415926-3.1415926/10)*speed;
                movey=movey+sin((turn-180)/180*3.1415926-3.1415926/10)*speed;
                /*if(cyclecount%2==0)
                {
                    movex=movex-cos(turn/180*3.1415926-3.1415926/18*(cyclecount%100+2));
                    movey=movey-sin(turn/180*3.1415926-3.1415926/18*(cyclecount%100+2));
                }
                else
                {
                    movex=movex-cos(turn/180*3.1415926+3.1415926/18*(cyclecount%100+2));
                    movey=movey-sin(turn/180*3.1415926+3.1415926/18*(cyclecount%100+2));
                }
            }*/

            areaclear=1;
            cyclecount++;
            if(check_bound(movex,movey)==0)
            {
                areaclear=0;
                if(faketurn>180)
                {
                    faketurn-=45;
                }
                else
                {
                    faketurn+=45;
                }
                if (cyclecount>50)
                {
                    movex=originx-otherx*0.2*(cyclecount%100+1);
                    movey=originy-othery*0.2*(cyclecount%100+1);
                }
                if(areaclear==1||cyclecount>200)
                {
                    movex=originx;
                    movey=originy;
                    if(check_bound(originx,originy)==0)
                    {
                        if(rand()%2==0)
                        {
                            setlocation(-1.7-rand()%6,-16-rand()%5,-1);
                        }
                        else
                        {
                            setlocation(13+rand()%5,3.5+rand()%6,-1);
                        }

                        for(int j=0; j<num; j++)
                        {
                            if(obstaclex[j]==movex)
                            {
                                if(obstacley[j]==movey)
                                {
                                    if(rand()%2==0)
                                    {
                                        setlocation(-1.7-rand()%6,-16-rand()%5,-1);
                                    }
                                    else
                                    {
                                        setlocation(13+rand()%5,3.5+rand()%6,-1);
                                    }
                                    j=-1;
                                }
                            }
                        }

                    }
                    break;
                }
                //printf("%d\n",cyclecount);
                continue;
            }
            for(int i=0; i<num; i++)
            {


                if(((pow(obstaclex[i]-movex,2)+pow(obstacley[i]-movey,2)<1)&&i!=number))
                {


                    //printf("%lf\n",pow(obstaclex[i]-movex,2)+pow(obstacley[i]-movey,2));

                    otherx=obstaclex[i]-movex;
                    othery=obstacley[i]-movey;

                    otherturn=atan(othery/otherx)/3.1415926*180;


                    if(otherx<0)
                    {
                        otherturn+=180;
                    }
                    if(otherturn<0)
                    {
                        otherturn+=360;
                    }
                    //if(i==6||i==0) {
                    //printf("%d,%lf,%lf,%lf,%lf\n",number,obstaclex[i],obstacley[i],movex,movey);
                    //}

                    if((otherturn-turn>100&&otherturn-turn<260)||(otherturn-turn<-100&&otherturn-turn>-260))
                        continue;
                    areaclear=0;

                    //printf("%d\n",cyclecount);

                    if (cyclecount>100)
                    {
                        movex=originx-otherx*0.2*(cyclecount%100+1);
                        movey=originy-othery*0.2*(cyclecount%100+1);
                        //continue;
                        /*if(otherturn<faketurn){
                        movex=obstaclex[i]+0.5;
                        movey=obstacley[i]+0.5;
                        }
                        else {
                        movex=obstaclex[i]+0.5;
                        movey=obstacley[i]+0.5;
                        }*/
                        /*if(cyclecount%2==0)
                        {
                            movex=movex-cos(turn/180*3.1415926-3.1415926/18*(cyclecount%100+2));
                            movey=movey-sin(turn/180*3.1415926-3.1415926/18*(cyclecount%100+2));
                        }
                        else
                        {
                            movex=movex-cos(turn/180*3.1415926+3.1415926/18*(cyclecount%100+2));
                            movey=movey-sin(turn/180*3.1415926+3.1415926/18*(cyclecount%100+2));
                        }*/
                    }
                    if(otherturn==0)
                    {
                        if(faketurn>180)
                        {
                            faketurn-=45;
                        }
                        else
                        {
                            faketurn+=45;
                        }

                    }
                    else if(otherturn>=faketurn)
                    {
                        faketurn-=45;
                    }
                    else
                    {
                        faketurn+=45;
                    }

                    //break;

                    /*if((turn<180&&turn>=0&&obstaclex[i]-targetx=>0)||((turn>=180||turn<0)&&obstaclex[i]-targetx<=0))
                        turn+=10;
                    else {
                        turn-=10;
                    }
                    break;
                    }*/


                }


            }
            /*for(int j; j<BUG_NUM; j++)
            {
                if(obstaclex[j]==movex)
                    if(obstacley[j]==movey)
                    {
                        areaclear=0;
                    }
            }*/
            if(areaclear==1||cyclecount>5000)
            {
                cyclecount=0;
                break;
            }
            /*else if (cyclecount>100) {
                while()
                movex=originx;
                movey=originy;
                cyclecount=0;
                break;
            }*/
        }
    }

    else
    {
        sattack=1;
    }
}
void bug::healthdamage(int d)
{
    health-=d;
    sattacked=1;
    if(health<=0)   //20140113
    {
        sdie=1;

    }
}

//////

GLMmodel *MODEL;
GLMmodel *MODEL_weapons[NUM_WEAPONS];
GLMmodel *MODEL_bodyparts[NUM_BODYPARTS];
//GLMmodel *MODEL_TREE[NUM_TREE];
GLMmodel *MODEL_SCENE[NUM_MAP_OBJS];

//GLFrame    spheres[NUM_SPHERES];
GLFrame    frameCamera;
GLFrame    Actorframe;
GLFrame    world;

// Light and material Data
GLfloat fLightPos[4]   = { -4.0f, -2.0f, 2.0f, 1.0f };  // Point source
GLfloat fNoLight[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat fLowLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat fBrightLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

M3DMatrix44f mShadowMatrix;

#define GROUND_TEXTURE  0
#define HOUSE_TEXTURE  1
#define ACTOR_TEXTURE  0
#define FACE_TEXTURE  1
#define NUM_TEXTURES    2  //======人的tga數
#define NUM_BODYPARTS 10   //======人的obj數
GLuint  textureObjects[NUM_TEXTURES];
GLuint  map_textureObjects[NUM_MAP_OBJS];
GLuint  weapon_textureObjects[NUM_WEAPONS];
char *weapon_texture[] = {"weapon/handgun.tga"};
char *map_texture[] = {"old fashion town/Maps/df2.tga","old fashion town/Maps/na1.tga","old fashion town/Maps/wf1c.tga","old fashion town/Maps/rf2b.tga","old fashion town/Maps/dr1.tga"};
const char *szTextureFiles[] = { "main_actor/body.tga","main_actor/face.tga"};
char *bodyparts[] = {"main_actor/head.obj","main_actor/body.obj","main_actor/upper_left_arm.obj","main_actor/lower_left_arm.obj",
                     "main_actor/upper_right_arm.obj","main_actor/lower_right_arm.obj","main_actor/upper_left_foot.obj","main_actor/lower_left_foot.obj","main_actor/upper_right_foot.obj",
                     "main_actor/lower_right_foot.obj"
                    };

char *map_scene[] = {"old fashion town/electric_pole.obj","old fashion town/ground.obj","old fashion town/wall.obj","old fashion town/roof.obj","old fashion town/door.obj"};

char *weapons[] = {"weapon/handgun.obj"};
//主角變數
float bound[8][6]= {{-1.4,-13.4,-1.405,-5.1,1.66,0.001},{-1.405,-5.1,0.8,0.95,2.74,-1},{0.8,0.95,19.4,4.325,0.18,-1},{13.8,3.2,12.8,14,2.7,1},{13,8.6,-7.43,6.5,0.99,10},{-4.4,6.8,-10.8,-13.2,-3.125,1},{-7.6,-3.2,-8,-24.8,-5.4,0.1},{-7.8,-14,-1.4,-13.4,0.94,-10}};
static GLfloat zTra = 0.0f;
static GLfloat xRot = 0.0f;
static GLfloat r = 0.0f;
int health = 100;  //==========人的血量
//bug
float delta=0;
float angle[6][5];
int state=0;
int cond=0;
int head=0;
int antenna=-30;
float speed=0.5f;
float xro=0.5;
float yro=0;
//static GLfloat xRot = 0.0f;
//static GLfloat yRot = 0.0f;
float flip=0;
int bulletcount=0;
int timer=0;
float headx=0;
float heady=-1.32;
float headz=0;
float headr=0;
float bodyy=0;
float bodyz=0;
float bodyr=0;


bug test[BUG_NUM];
bullet bb[100];
//////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering
// context.

Actor::Actor()
{
    id = 0.0;
    weapon_type = "noname weapon";
}

Actor::Actor(int id,const char *name)
{
    id = id;
    weapon_type = name;
}


void Actor::drawActor(/*int gun_id*/)
{
    //Actorframe.ApplyActorTransform();
    //world.ApplyActorTransform();
//畫人
//for(int i=0;i<2;i++){

//glScalef(1.6, 1.2, 1.2);
//頭


    glTranslatef(targetx,targety,targetz);
    glTranslatef(0.0f, 0.2f, 0.0f);
    glRotatef(90,1,0,0);

    glRotatef(45*face,0,1,0);
    glScalef(1.5,1.5,1.5);


/////20140113 手槍

    glPushMatrix();



    glBindTexture(GL_TEXTURE_2D, textureObjects[FACE_TEXTURE]);
    glScalef(1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(headx,heady,headz);
    glRotatef(headr,1,0,0);


    glmDraw(MODEL_bodyparts[0], GLM_SMOOTH | GLM_TEXTURE);//GLM_FLAT
    glPopMatrix();
    glTranslatef(0.0,-1.32,0.0);
    //DrawFrame();
    Weapon handgun;
    if(gameover==1)
    {
        glScalef(0,0,0);
    }
    handgun.drawWeapon();

    glPopMatrix();

//}
    //glMaterialfv(GL_FRONT, GL_SPECULAR, fNoLight);
    //身體
    glTranslatef(0,bodyy,bodyz);
    glRotatef(bodyr,1,0,0);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureObjects[ACTOR_TEXTURE]);
    glScalef(0.5, 0.5, 0.5);
    glTranslatef(0.0f,-3.0f,0.0f);
    glmDraw(MODEL_bodyparts[1], GLM_SMOOTH | GLM_TEXTURE);//GLM_FLAT
    glPopMatrix();
//左臂
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureObjects[ACTOR_TEXTURE]);
    glScalef(0.6, 0.6, 0.6);
    glTranslatef(0.15f,-2.4f,0.1f);
    glRotatef(100.0f,0.0f,-1.0f,-1.0f);
    glmDraw(MODEL_bodyparts[2], GLM_SMOOTH | GLM_TEXTURE);//GLM_FLAT

    glPushMatrix();

    glBindTexture(GL_TEXTURE_2D, textureObjects[ACTOR_TEXTURE]);
    //glScalef(0.6, 0.6, 0.6);
    glRotatef(70.0f,1.0f,-1.0f,1.0f);
    glTranslatef(0.24f,-0.1f,0.0f);
    glmDraw(MODEL_bodyparts[3], GLM_SMOOTH | GLM_TEXTURE);//GLM_FLAT

    glPopMatrix();

    glPopMatrix();
//右臂
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureObjects[ACTOR_TEXTURE]);
    glScalef(0.6, 0.6, 0.6);
    glTranslatef(-0.15f,-2.4f,0.1f);
    glRotatef(60.0f,0.0f,1.0f,1.0f);
    glmDraw(MODEL_bodyparts[4], GLM_SMOOTH | GLM_TEXTURE);//GLM_FLAT


    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureObjects[ACTOR_TEXTURE]);
    //glScalef(0.6, 0.6, 0.6);
    glRotatef(80.0f,0.0f,1.0f,0.0f);
    glTranslatef(-0.2f,-0.0f,0.0f);
    //glTranslatef(-0.46f,-2.35f,0.0f);
    glmDraw(MODEL_bodyparts[5], GLM_SMOOTH | GLM_TEXTURE);//GLM_FLAT
    glPopMatrix();



    glPopMatrix();
//左腳

    glPushMatrix();


    glBindTexture(GL_TEXTURE_2D, textureObjects[ACTOR_TEXTURE]);

    glScalef(0.3, 0.3, 0.3);
    //glTranslatef(0.19f,-5.8f,0.1f);

    glTranslatef(0.19f,-5.8f,zTra);

    glRotatef(2*xRot,1.0f,0.0f,0.0f);
    //glRotatef(20.0f,1.0f,0.0f,0.0f);
    glPushMatrix();
    glRotatef(10.0f,0.0f,1.0f,0.0f);
    glmDraw(MODEL_bodyparts[6], GLM_SMOOTH | GLM_TEXTURE);//GLM_FLAT
    glPopMatrix();

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureObjects[ACTOR_TEXTURE]);
    glRotatef(-10*xRot,1.0f,0.0f,0.0f);
    glScalef(0.35, 0.35, 0.35);
    glTranslatef(0.2f,-1.5f,0.2f);
    glmDraw(MODEL_bodyparts[7], GLM_SMOOTH | GLM_TEXTURE);//GLM_FLAT
    glPopMatrix();

    glPopMatrix();
//右腳
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureObjects[ACTOR_TEXTURE]);
    glScalef(0.3, 0.3, 0.3);
    //glTranslatef(-0.18f,-5.8f,0.0f);
    glTranslatef(-0.15f,-5.8f,-zTra);

    glRotatef(2*xRot,-1.0f,0.0f,0.0f);
    glPushMatrix();
    glRotatef(10.0f,0.0f,-1.0f,0.0f);
    glmDraw(MODEL_bodyparts[8], GLM_SMOOTH | GLM_TEXTURE);//GLM_FLAT
    glPopMatrix();

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureObjects[ACTOR_TEXTURE]);
    glRotatef(-10*xRot,-1.0f,0.0f,0.0f);
    glScalef(0.35, 0.35, 0.35);
    glTranslatef(-0.2f,-1.5f,0.0f);
    glmDraw(MODEL_bodyparts[9], GLM_SMOOTH | GLM_TEXTURE);//GLM_FLAT
    glPopMatrix();
    glPopMatrix();


}

Weapon::Weapon()
{
    id = 0.0;
    weapon_type = "noname weapon";
}

Weapon::Weapon(int id,const char *name)
{
    id = id;
    weapon_type = name;
}


void Weapon::drawWeapon(/*int gun_id*/)
{
    glBindTexture(GL_TEXTURE_2D, weapon_textureObjects[0]);
    glScalef(0.1f,0.1f,0.1f);
    glRotatef(270.0f,1.0f,0.0f,0.0f);
    glRotatef(90.0f,0.0f,1.0f,0.0f);
    glTranslatef(0.0f,-3.0f,0.0f);
    glmDraw(MODEL_weapons[0], GLM_SMOOTH | GLM_TEXTURE);

}

bool check(float x, float y)
{
    for(int i=0; i<num; i++)
    {
        if(pow(obstaclex[i]-x,2)+pow(obstacley[i]-y,2)<0.5)
            return false;
    }
    return true;
}


void SetupRC()
{
    M3DVector3f vPoints[3] = {{ 0.0f, -0.4f, 0.0f },
        { 10.0f, -0.4f, 0.0f },
        { 5.0f, -0.4f, -5.0f }
    };
    int iSphere;
    int i;

    srand(time(NULL));


    world.SetOrigin(0.0f, 0.34f, -2.5);
    frameCamera.SetOrigin(-1,12.7,-2.5);
    frameCamera.RotateLocalX(3.14159/2);
    world.RotateLocalX(-3.14159/view);

//讀主角OBJ
    for(int j=0; j<NUM_BODYPARTS; j++)
    {
        MODEL_bodyparts[j] = glmReadOBJ(bodyparts[j]);
        glmUnitize(MODEL_bodyparts[j]);
    }
//讀武器OBJ
    for(int j=0; j<NUM_WEAPONS; j++)
    {
        MODEL_weapons[j] = glmReadOBJ(weapons[j]);
        glmUnitize(MODEL_weapons[j]);
    }
    //glmFacetNormals(MODEL);
    //glmVertexNormals(MODEL, 60);
    /*for(int j=0;j<=)
    MODEL = glmReadOBJ("VDV/vdv.obj");
    glmUnitize(MODEL);*/

    // 場景的初始化
    for(int j=0; j<NUM_MAP_OBJS; j++)
    {
        MODEL_SCENE[j] = glmReadOBJ(map_scene[j]);
        glmUnitize(MODEL_SCENE[j]);
    }


    //MODEL_SCENE = glmReadOBJ("old fashion town/old_town_block.obj");
    //glmUnitize(MODEL_SCENE);
    //glmFacetNormals(MODEL_SCENE);
    //glmVertexNormals(MODEL_SCENE, 60);

    glEnable(GL_NORMALIZE);

    // Grayish background
    glClearColor(fNoLight[0], fNoLight[1], fNoLight[2], fNoLight[3]);

    // Clear stencil buffer with zero, increment by one whenever anybody
    // draws into it. When stencil function is enabled, only write where
    // stencil value is zero. This prevents the transparent shadow from drawing
    // over itself
    glStencilOp(GL_INCR, GL_INCR, GL_INCR);
    glClearStencil(0);
    glStencilFunc(GL_EQUAL, 0x0, 0x01);

    // Cull backs of polygons
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE_ARB);

    // Setup light parameters

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fNoLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, fLowLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, fBrightLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, fBrightLight);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);



    glLightfv(GL_LIGHT1, GL_AMBIENT, fBrightLight);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, fBrightLight);
    glLightfv(GL_LIGHT1, GL_SPECULAR, fBrightLight);

    glLightfv(GL_LIGHT2, GL_AMBIENT, fBrightLight);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, fBrightLight);
    glLightfv(GL_LIGHT2, GL_SPECULAR, fBrightLight);

    // Calculate shadow matrix
    glPushMatrix();
    M3DVector4f pPlane;
    world.ApplyActorTransform();
    m3dGetPlaneEquation(pPlane, vPoints[0], vPoints[1], vPoints[2]);
    m3dMakePlanarShadowMatrix(mShadowMatrix, pPlane, fLightPos);
    glPopMatrix();

    // Mostly use material tracking
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, fBrightLight);
    glMateriali(GL_FRONT, GL_SHININESS, 128);


    // Randomly place the sphere inhabitants
    /* for(iSphere = 0; iSphere < NUM_SPHERES; iSphere++)
     {
         // Pick a random location between -20 and 20 at .1 increments
         spheres[iSphere].SetOrigin(((float)((rand() % 400) - 200) * 0.1f), 0.0, (float)((rand() % 400) - 200) * 0.1f);
     }*/

    // Set up texture maps
    glEnable(GL_TEXTURE_2D);
    glGenTextures(NUM_TEXTURES, textureObjects);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


    for(i = 0; i < NUM_TEXTURES; i++)
    {
        GLbyte *pBytes;
        GLint iWidth, iHeight, iComponents;
        GLenum eFormat;

        glBindTexture(GL_TEXTURE_2D, textureObjects[i]);

        // Load this texture map
        pBytes = gltLoadTGA(szTextureFiles[i], &iWidth, &iHeight, &iComponents, &eFormat);
        gluBuild2DMipmaps(GL_TEXTURE_2D, iComponents, iWidth, iHeight, eFormat, GL_UNSIGNED_BYTE, pBytes);
        free(pBytes);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glGenTextures(NUM_WEAPONS, weapon_textureObjects);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    //武器貼圖
    for(i = 0; i < NUM_WEAPONS; i++)
    {
        GLbyte *pBytes;
        GLint iWidth, iHeight, iComponents;
        GLenum eFormat;

        glBindTexture(GL_TEXTURE_2D, weapon_textureObjects[i]);

        // Load this texture map
        pBytes = gltLoadTGA(weapon_texture[i], &iWidth, &iHeight, &iComponents, &eFormat);
        gluBuild2DMipmaps(GL_TEXTURE_2D, iComponents, iWidth, iHeight, eFormat, GL_UNSIGNED_BYTE, pBytes);
        free(pBytes);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glGenTextures(NUM_MAP_OBJS, map_textureObjects);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    //場景貼圖
    for(i = 0; i < NUM_MAP_OBJS; i++)
    {
        GLbyte *pBytes;
        GLint iWidth, iHeight, iComponents;
        GLenum eFormat;

        glBindTexture(GL_TEXTURE_2D, map_textureObjects[i]);

        // Load this texture map
        pBytes = gltLoadTGA(map_texture[i], &iWidth, &iHeight, &iComponents, &eFormat);
        gluBuild2DMipmaps(GL_TEXTURE_2D, iComponents, iWidth, iHeight, eFormat, GL_UNSIGNED_BYTE, pBytes);
        free(pBytes);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    for(int i=0; i<BUG_NUM; i++)
    {
        if(rand()%2==0)
        {
            test[i].setlocation(-1.7-rand()%6,-16-rand()%5,-1);
        }
        else
        {
            test[i].setlocation(13+rand()%5,3.5+rand()%6,-1);
        }

        for(int j=0; j<i; j++)
        {
            if(test[j].getx()==test[i].getx())
            {
                if(test[j].gety()==test[i].gety())
                {
                    if(rand()%2==0)
                    {
                        test[i].setlocation(-1.7-rand()%6,-16-rand()%5,-1);
                    }
                    else
                    {
                        test[i].setlocation(13+rand()%5,3.5+rand()%6,-1);
                    }
                    j=-1;
                }
            }
        }
        test[i].setnumber(i);
    }

}

////////////////////////////////////////////////////////////////////////
// Do shutdown for the rendering context
void ShutdownRC(void)
{
    // Delete the textures
    glDeleteTextures(NUM_TEXTURES, textureObjects);
}

///////////////////////////////////////////////////////////
// Draw the ground as a series of triangle strips
GLfloat	 lightPos2[] = { frameCamera.GetOriginX()-3,frameCamera.GetOriginY(),frameCamera.GetOriginZ()-2.5, 0.0f };
GLfloat  lightPos3[] = { frameCamera.GetOriginX()+1.7,frameCamera.GetOriginY(),frameCamera.GetOriginZ()-1.5, 0.0f };

void DrawGround(void)
{
    /* GLfloat fExtent = 20.0f;
     GLfloat fStep = 1.0f;
     GLfloat y = -0.4f;
     GLint iStrip, iRun;
     GLfloat s = 0.0f;
     GLfloat t = 0.0f;
     GLfloat texStep = 1.0f / (fExtent * .075f);

     glBindTexture(GL_TEXTURE_2D, textureObjects[GROUND_TEXTURE]);
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

     for(iStrip = -fExtent; iStrip <= fExtent; iStrip += fStep)
     {
         t = 0.0f;
         glBegin(GL_TRIANGLE_STRIP);

         for(iRun = fExtent; iRun >= -fExtent; iRun -= fStep)
         {
             glTexCoord2f(s, t);
             glNormal3f(0.0f, 1.0f, 0.0f);   // All Point up
             glVertex3f(iStrip, y, iRun);

             glTexCoord2f(s + texStep, t);
             glNormal3f(0.0f, 1.0f, 0.0f);   // All Point up
             glVertex3f(iStrip + fStep, y, iRun);

             t += texStep;
         }
         glEnd();
         s += texStep;
     }*/
}
void healthdamage()
{
    if ( health > 0 )   //20140113
    {
        health-=hurt*bdamage;
    }
    else
    {
        gameover = 1; // 人的血量歸零,遊戲結束
    }
    hurt=0;
}

void headroll(int i)
{

    static float t=0.68;
    state=1;

    if(i<60)
    {
        headz+=-0.04;
        if(i%24==12)
        {
            t=t/2;
        }
        if(i>=12&&i<=36)
        {
            heady=-2-t*cos(3.1415926/24*i);
        }
        else
        {
            heady=-2+t*cos(3.1415926/24*i);
        }

        headr=i*60;

        if(i<40)
        {
            bodyr=i*-90/40;

            bodyy=-2+2*cos(3.1415926/80*i);
            bodyz=-2*sin(3.1415926/80*i);
        }

        printf("%f\n",t);

        if(gameover!=1)
        {
            i+=60;
            headx=0;
            heady=-1.32;
            headz=0;
            headr=0;
            state=0;
            bodyy=0;
            bodyz=0;
            bodyr=0;

        }

        i++;

        glutTimerFunc(33,headroll,i);

    }
    else
    {

        t=0.68;

    }
    glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////
// Draw random inhabitants and the rotating torus/sphere duo
void DrawInhabitants(GLint nShadow)
{
    //主角變數
    //static GLfloat yRot = 0.0f;

    GLint i;

    if(nShadow == 0)
    {
        //  yRot += 0.5f;
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else
        glColor4f(0.00f, 0.00f, 0.00f, .6f);  // Shadow color


    // Draw the randomly located spheres
    // glBindTexture(GL_TEXTURE_2D, textureObjects[SPHERE_TEXTURE]);
    /*  for(i = 0; i < NUM_SPHERES; i++)
      {
          glPushMatrix();
          spheres[i].ApplyActorTransform();
          gltDrawSphere(0.3f, 21, 11);
          glPopMatrix();
      }*/
    zTra = 0.05*sin(r);
    xRot = 2*sin(r);
//glMaterialfv(GL_FRONT, GL_SPECULAR, fNoLight);
    // glPushMatrix();
    //glTranslatef(0.0f, 0.1f, -2.5f);
    /*
        glPushMatrix();
        glRotatef(-yRot * 2.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(1.0f, 0.0f, 0.0f);
        gltDrawSphere(0.1f,21, 11);
        glPopMatrix();*/

    /* if(nShadow == 0)
     {
         // Torus alone will be specular
         glMaterialfv(GL_FRONT, GL_SPECULAR, fBrightLight);
     }*/



    glPushMatrix();

    Actor actor;
    actor.drawActor();
    glPopMatrix();
//畫蟲


//畫武器
    glPushMatrix();
    Weapon handgun;
    //handgun.drawWeapon();
    glPopMatrix();

//畫場景

    //gltDrawTorus(0.35, 0.15, 61, 37);

//    for(i = 0; i < NUM_MAP_OBJS; i++)
    //  {
    /* glPushMatrix();
     glRotatef(90,1,0,0);
     //glBindTexture(GL_TEXTURE_2D, textureObjects[FACE_TEXTURE]);
     glBindTexture(GL_TEXTURE_2D, map_textureObjects[0]);
     glScalef(21.0, 21.0, 21.0);
     glTranslatef(0.0f,0.023f,0.0f);
     glmDraw(MODEL_SCENE[0], GLM_SMOOTH | GLM_TEXTURE);//GLM_FLAT
     glPopMatrix();*/

    glPushMatrix();
    glRotatef(90,1,0,0);
    //glBindTexture(GL_TEXTURE_2D, textureObjects[FACE_TEXTURE]);
    glBindTexture(GL_TEXTURE_2D, map_textureObjects[0]);
    glScalef(25.0, 25.0, 25.0);
    glmDraw(MODEL_SCENE[0], GLM_SMOOTH | GLM_TEXTURE);//GLM_FLAT
    glPopMatrix();

    glPushMatrix();
    glRotatef(90,1,0,0);
    //glBindTexture(GL_TEXTURE_2D, textureObjects[FACE_TEXTURE]);
    glBindTexture(GL_TEXTURE_2D, map_textureObjects[1]);
    glScalef(15.0, 15.0, 15.0);
    glmDraw(MODEL_SCENE[1], GLM_SMOOTH | GLM_TEXTURE);//GLM_FLAT
    glPopMatrix();


    glPushMatrix();
    glRotatef(90,1,0,0);
    //glBindTexture(GL_TEXTURE_2D, textureObjects[FACE_TEXTURE]);
    //DrawFrame();
    glBindTexture(GL_TEXTURE_2D, map_textureObjects[2]);
    glScalef(19.0, 19.0, 19.0);
    //glTranslatef(0.0f,0.023f,0.0f);
    glmDraw(MODEL_SCENE[2], GLM_SMOOTH | GLM_TEXTURE);//GLM_FLAT
    glPopMatrix();

    glPushMatrix();
    glRotatef(90,1,0,0);
    //glBindTexture(GL_TEXTURE_2D, textureObjects[FACE_TEXTURE]);
    glBindTexture(GL_TEXTURE_2D, map_textureObjects[3]);
    glScalef(25.0, 25.0, 25.0);
    glTranslatef(0.0f,0.02f,0.0f);
    glmDraw(MODEL_SCENE[3], GLM_SMOOTH | GLM_TEXTURE);//GLM_FLAT
    glPopMatrix();

    glPushMatrix();
    glRotatef(90,1,0,0);
    //glBindTexture(GL_TEXTURE_2D, textureObjects[FACE_TEXTURE]);
    glBindTexture(GL_TEXTURE_2D, map_textureObjects[4]);
    glScalef(13.0, 13.0, 13.0);
    glTranslatef(0.0f,0.0f,-0.05f);
    glmDraw(MODEL_SCENE[4], GLM_SMOOTH | GLM_TEXTURE);//GLM_FLAT
    glPopMatrix();
//   }

    //glMaterialfv(GL_FRONT, GL_SPECULAR, fNoLight);
    //glPopMatrix();
}


void combocount()
{

    if(combo>=10&&combo%10==0&&health<100&&achieve==1)
    {
        health+=10;
        achieve=0;
    }


    if(combo<=10)
    {
        combolimit=1000-combo*50;
    }
    else
    {
        combolimit=500-(combo-10)*25;
        if(combolimit<50)
        {
            combolimit=50;
        }
    }

    if(combotime>combolimit)
    {
        combo=0;
        combotime=0;
    }

}

// Called to draw scene
void RenderScene(void)
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glPushMatrix();
    frameCamera.ApplyCameraTransform();

    // Position light before any other transformations
    glPushMatrix();
    world.ApplyActorTransform();
    glLightfv(GL_LIGHT0, GL_POSITION, fLightPos);
    glTranslatef(fLightPos[0],fLightPos[1],fLightPos[2]);
    //glutSolidSphere(1.0,10,10);
    glPopMatrix();

    // Draw the ground
    //glColor3f(1.0f, 1.0f, 1.0f);
    //DrawGround();

    // Draw shadows first
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_STENCIL_TEST);
    glPushMatrix();
    glMultMatrixf(mShadowMatrix);
    //DrawInhabitants(1);
    glPopMatrix();
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    // Draw inhabitants normally
    glPushMatrix();
    world.ApplyActorTransform();
    DrawInhabitants(0);
    healthdamage();
    //printf("%d\n",health);
    glPopMatrix();

    if(gamescore/1000==10)
    {
        num=10;
    }

    else if(gamescore/1000==20)
    {
        num=15;
    }

    //printf("%f, %f\n",test[5].getx(),test[5].gety());


    for(int i=0; i<num; i++)
    {
        glPushMatrix();
        world.ApplyActorTransform();
        //glTranslatef(1,0,0);
        glColor3ub(128-i*10, i*10, 128);
        test[i].drawbug(0);
        test[i].action(targetx,targety);
        obstaclex[i]=test[i].getx();
        obstacley[i]=test[i].gety();
        glPopMatrix();
    }

    for(int i=0; i<100; i++)
    {
        glPushMatrix();
        world.ApplyActorTransform();
        bb[i].action();
        casualty=bb[i].hit();
        if(casualty!=-1)
        {
            test[casualty].healthdamage(bb[i].damage+combo*2);
        }
        glPopMatrix();
    }

    glPushMatrix();
    glLightfv(GL_LIGHT1,GL_POSITION,lightPos2);
    glLightfv(GL_LIGHT2,GL_POSITION,lightPos3);

    glTranslatef(frameCamera.GetOriginX()-2.1,frameCamera.GetOriginY()-7,frameCamera.GetOriginZ()-1.9);
    glEnable(GL_LIGHT1);

    glPushMatrix();
    glScalef(0.15,0.2,0.2);
    for(int i=0; i<10; i++)
    {
        if(i<health/10)
        {
            glColor3f(0.0,1.0,0.0);
        }
        else
        {
            glColor3f(1.0,0.0,0.0);
        }
        glutSolidCube(1);
        glTranslatef(1,0,0);

    }
    glPopMatrix();
    glDisable(GL_LIGHT1);
    combocount();
    glEnable(GL_LIGHT2);
    glTranslatef(4.7,0,0.32);
    glPushMatrix();
    glScalef(0.03,0.2,0.2);
    glColor3f(0.0,0.0,1.0);
    int limit=(combolimit-combotime)/(combolimit/50);
    if(combo!=0)
    {
        for(int i=0; i<limit; i++)
        {
            glutSolidCube(1);
            glTranslatef(-1,0,0);
        }
    }

    glPopMatrix();

    glDisable(GL_LIGHT2);
    glPopMatrix();



    glPushMatrix();
    char Health[30], Score[100], HighScore[100],Combo[100], GameOver[] = "Game Over!!!! Press R to try again, Esc to quit...";
    //===========血量部分
    if ( health < 0 )
    {
        health = 0;    // 避免有負數血量的事情發生 20140113
    }
    sprintf(Health, "Health");
    glColor3f(1.0, 0.0, 0.0);  // 字形顏色紅
    glWindowPos2i(20, screenHeight-50);    // 字形左下角位置 原點為視窗左下角
    for( int i=0 ; i < strlen( Health ) ; i++ )
    {
        //glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, Health[i]);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, Health[i]);
    }
    //===========分數部分
    sprintf(Score, "Score  %d", gamescore );
    glColor3f(1.0, 0.0, 0.0);  // 字形顏色紅
    glWindowPos2i(20, screenHeight-100);    // 字形左下角位置 原點為視窗左下角
    for( int i=0 ; i < strlen( Score ) ; i++ )
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, Score[i]);
    }
    //===========高分紀錄部分 20140113
    sprintf(HighScore, "High Score  %d", highscore );
    glColor3f(1.0, 0.0, 0.0);  // 字形顏色紅
    glWindowPos2i( screenWidth/2, screenHeight-25);    // 字形左下角位置 原點為視窗左下角
    for( int i=0 ; i < strlen( HighScore ) ; i++ )
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, HighScore[i]);
    }

    sprintf(Combo, "Combo  x%d", combo );
    if(targetx<0.12)
    {
        glColor3f(1.0, 1.0, 1.0);  // 字形顏色紅
    }
    else
    {
        glColor3f(1.0,0.0,0.0);
    }
    glWindowPos2i( screenWidth-160, screenHeight-50);    // 字形左下角位置 原點為視窗左下角
    for( int i=0 ; i < strlen( Combo ) ; i++ )
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, Combo[i]);
    }

    //===========遊戲結束 20140113
    if ( gameover == 1 )
    {
        if(state==0)
        {
            headroll(0);
        }
        glColor3f(1.0, 0.0, 0.0);  // 字形顏色紅
        glWindowPos2i( screenWidth/2 - 100, screenHeight/2 );    // 字形左下角位置 原點為視窗左下角
        for( int i=0 ; i < strlen( GameOver ) ; i++ )
        {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, GameOver[i]);
        }
    }
    glPopMatrix();

    glPopMatrix();

    // Do the buffer Swap
    glutSwapBuffers();
}

// Respond to arrow keys by moving the camera frame of reference
void SpecialKeys(int key, int x, int y) // 會lag 目前不要用
{
    if(key == GLUT_KEY_UP)
        frameCamera.MoveForward(0.2f);

    if(key == GLUT_KEY_DOWN)
        frameCamera.MoveForward(-0.2f);

    if(key == GLUT_KEY_LEFT)
        frameCamera.RotateLocalY(0.2f);

    if(key == GLUT_KEY_RIGHT)
        frameCamera.RotateLocalY(-0.2f);

    // Refresh the Window
    glutPostRedisplay();
}


void Keys(unsigned char key, int x, int y) // 會lag 目前不要用
{
    printf("%ud\n",key);
    switch(key)
    {

    case 'w':
        //Actorframe.MoveForward(0.025f);
        //Actor_position_z+=0.2f;
        r+=0.8f;
        face=0;

        if(check(targetx,targety-0.2))
        {
            frameCamera.MoveUp(0.2);
            targety-=0.2;

        }
        break;

    case 's':
        //Actorframe.MoveForward(-0.025f);
        //Actor_position_z-=0.2f;
        r-=0.8f;
        face=2;
        if(check(targetx,targety+0.2))
        {
            frameCamera.MoveUp(-0.2);
            targety+=0.2;

        }
        break;

    case 'a':
        //Actorframe.RotateLocalY(0.1f);
        //Actor_yRot+=2.0f;
        r+=0.8f;
        face=1;
        if(check(targetx+0.2,targety))
        {
            frameCamera.MoveRight(0.2);
            targetx+=0.2;

        }

        break;

    case 'd':
        //Actorframe.RotateLocalY(-0.1f);
        r+=0.8f;
        face=3;
        if(check(targetx-0.2,targety))
        {
            frameCamera.MoveRight(-0.2);
            targetx-=0.2;

        }
        //Actor_yRot-=2.0f;
        break;
    case ' ':

        if(timer>=0)
        {
            bb[bulletcount].setb();
            bulletcount++;
            if(bulletcount>=100)
            {
                bulletcount=0;
            }
            timer=-30;
        }



        break;
    }

    // Refresh the Window
    //glutPostRedisplay();
}
///////////////////////////////////////////////////////////
// Called by GLUT library when idle (window not being
// resized or moved)
void TimerFunction(int value)
{
    // Redraw the scene with new coordinates
    timer++;
    combotime++;
    glutPostRedisplay();
    glutTimerFunc(3,TimerFunction, 1);
}

void ChangeSize(int w, int h)
{
    GLfloat fAspect;
    /* Save the new width and height */
    screenWidth  = w;
    screenHeight = h;
    //============================
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;

    glViewport(0, 0, w, h);

    fAspect = (GLfloat)w / (GLfloat)h;

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set the clipping volume
    gluPerspective(35.0f, fAspect, 1.0f, 50.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
//============鍵盤================(不會lag用)
void init_KeyEvent( void )   // 初始化這些按鍵
{
    for( unsigned int i = 0 ; i < MAX_KEY_STATE; ++i )
        keyState[i] = false;
    for( unsigned int i = 0 ; i < MAX_SPECIAL_KEY_STATE; ++i )
        specialKeyState[i] = false;
}
void setKeyStateUp( const unsigned char k )   // 代表按下某鍵, true
{
    keyState[k] = false;
}
void setSpecialKeyStateUp( const int k )   // 代表按下某鍵(F1~F12,上下左右等), true
{
    specialKeyState[k] = false;
}
void setKeyStateDown( const unsigned char k )   // 代表放開某鍵, false
{
    keyState[k] = true;
}
void setSpecialKeyStateDown( const int k )   // 代表放開某鍵(F1~F12,上下左右等), true
{
    specialKeyState[k] = true;
}
const bool isKeyStateDown( const unsigned char k )   // 回傳是否有壓下某鍵
{
    return keyState[k];
}
const bool isSpecialKeyStateDown( const int k )   // 回傳是否有壓下某鍵(F1~F12,上下左右等)
{
    return specialKeyState[k];
}
//============鍵盤所有按鍵是否按下==========(此方法順暢)
void SpecialKeyUP( int key, int x, int y )
{
    setSpecialKeyStateUp(key);
}
void SpecialKeyDOWN( int key, int x, int y )
{
    setSpecialKeyStateDown(key);
}
void keyUP( unsigned char key, int x, int y )
{
    setKeyStateUp(key);
}
void keyDOWN( unsigned char key, int x, int y )
{
    setKeyStateDown(key);
}
//============update_game==========
int check_bound(float x,float y) //==true為在可活動範圍內
{
//===========有在可活動範為內會進到if內


    if(y<-5.1)
    {
        if((bound[0][1]-bound[1][1])/(bound[0][0]-bound[1][0])*x-y+(bound[0][1]-(bound[0][1]-bound[1][1])/(bound[0][0]-bound[1][0])*bound[0][0])<0)
        {
            return 0;
        }
    }
    else if(y<0.95)
    {
        if((bound[1][1]-bound[2][1])/(bound[1][0]-bound[2][0])*x-y+(bound[1][1]-(bound[1][1]-bound[2][1])/(bound[1][0]-bound[2][0])*bound[1][0])>0)
        {
            return 0;
        }
    }
    else if(y<3.2)
    {
        if((bound[2][1]-bound[3][1])/(bound[2][0]-bound[3][0])*x-y+(bound[2][1]-(bound[2][1]-bound[3][1])/(bound[2][0]-bound[3][0])*bound[2][0])>0)
        {
            return 0;
        }
    }
    if(y<-3.2)
    {
        if((bound[7][1]-bound[6][1])/(bound[7][0]-bound[6][0])*x-y+(bound[7][1]-(bound[7][1]-bound[6][1])/(bound[7][0]-bound[6][0])*bound[7][0])<0)
        {
            return 0;
        }
    }
    else if(y<6.8)
    {
        if((bound[6][1]-bound[5][1])/(bound[6][0]-bound[5][0])*x-y+(bound[6][1]-(bound[6][1]-bound[5][1])/(bound[6][0]-bound[5][0])*bound[6][0])<0)
        {
            return 0;
        }
    }
    else
    {
        if((bound[5][1]-bound[4][1])/(bound[5][0]-bound[4][0])*x-y+(bound[5][1]-(bound[5][1]-bound[4][1])/(bound[5][0]-bound[4][0])*bound[5][0])<0)
        {
            return 0;
        }
    }
    /*for(int i=0;i<8;i++) {
        if(!AABBtest(x,y,x,y,bound[i][0],bound[i][1],bound[i][2]+bound[i][4],bound[i][3]+bound[i][5])) {
            return 0;
        }
    }*/

    return 1;
    /*if( (1.8/17.4)*x - y + 7.2552 > 0 )
    {//======在CD兩點連線下方OK (X向右為正, Y向上為正)
            if( 7.875*x - y + (-2.375) < 0 )
            {//======在AF兩點連線左側OK (X向右為正, Y向上為正)
                    if( (0.6 / 6.2)*x - y + (-13.2645) < 0 )
                    {//======在AB兩點連線上方OK (X向右為正, Y向上為正)
                            if( 6.5*x - y + 35.4 > 0 )
                            {//======在BC兩點連線右側OK (X向右為正, Y向上為正)
                                    return 1; // 玩家在"甲"區域內
                            }else{
                                    return 0; // 玩家脫離可移動範圍!!!!!
                            }
                    }else{
                            return 0; // 玩家脫離可移動範圍!!!!!
                    }
            }else{
            //======在AF兩點連線右側&線上OK (X向右為正, Y向上為正)
                    if( (4/13)*x - y + (-0.8615) < 0 )
                    {//======在EF兩點連線上方OK (X向右為正, Y向上為正)
                            if( -27*x - y + 359.6 > 0 )
                            {//======在DE兩點連線左側OK (X向右為正, Y向上為正)
                                    return 1; // 玩家在"乙"區域內
                            }else{
                                    return 0; // 玩家脫離可移動範圍!!!!!
                            }
                    }else{
                            return 0; // 玩家脫離可移動範圍!!!!!
                    }
            }
    }else{
            return 0; // 玩家脫離可移動範圍!!!!!
    }*/
}
//=================================
void update_game( int value )
{
    //=====處理一般key的部分
    if ( isKeyStateDown(27) )   // Esc
    {
        exit(0); // 結束遊戲
    }
    //===================
    if ( isSpecialKeyStateDown( GLUT_KEY_UP ) )
    {
        //Actorframe.MoveForward(0.025f);
        //Actor_position_z+=0.2f;
        if ( gameover == 0 )   //如果gameover則不能移動
        {
            float turnangle=1;
            r+=0.8f;
            if(isSpecialKeyStateDown( GLUT_KEY_RIGHT )||isSpecialKeyStateDown( GLUT_KEY_LEFT ))
            {
                r-=0.4f;
                turnangle=cos(3.1415926/4);

            }
            face=0;
            if(isSpecialKeyStateDown( GLUT_KEY_RIGHT ))
            {
                face+=7;
            }
            else if(isSpecialKeyStateDown( GLUT_KEY_LEFT ))
            {
                face+=1;
            }
            if( check(targetx,targety-0.2) ) //偵測人和蟲是否碰撞
            {
                if(targety>=-1.2)
                {
                    frameCamera.MoveUp(0.2*turnangle);
                }
                else
                {
                    frameCamera.MoveForward(-0.2*turnangle);
                }
                targety-=0.2*turnangle;
                if ( check_bound(targetx,targety) == 0||(targetx>13||targety<-13.8) ) // 如果下一步超出邊界, 回來!!
                {
                    if(targety>=-1.2)
                    {
                        frameCamera.MoveUp(-0.2*turnangle);
                    }
                    else
                    {
                        frameCamera.MoveForward(0.2*turnangle);
                    }
                    targety+=0.2*turnangle;
                }
            }
        }
        //printf("%d\n",check_bound(targetx,targety)); //邊件判定
        printf("x = %f, y = %f\n",targetx,targety); //邊件判定
    }
    //===================
    if ( isSpecialKeyStateDown( GLUT_KEY_DOWN ) )
    {
        //Actorframe.MoveForward(-0.025f);
        //Actor_position_z-=0.2f;
        if ( gameover == 0 )   //如果gameover則不能移動
        {
            float turnangle=1;
            r+=0.8f;
            if(isSpecialKeyStateDown( GLUT_KEY_RIGHT )||isSpecialKeyStateDown( GLUT_KEY_LEFT ))
            {
                r-=0.4f;
                turnangle=cos(3.1415926/4);
            }
            face=4;
            if(isSpecialKeyStateDown( GLUT_KEY_RIGHT ))
            {
                face+=1;
            }
            else if(isSpecialKeyStateDown( GLUT_KEY_LEFT ))
            {
                face-=1;
            }
            if( check(targetx,targety+0.2) ) //偵測人和蟲是否碰撞
            {
                if(targety>=-1.2)
                {
                    frameCamera.MoveUp(-0.2*turnangle);
                }
                else
                {
                    frameCamera.MoveForward(0.2*turnangle);
                } //====畫面動
                targety+=0.2*turnangle;                         //====人動
                if ( check_bound(targetx,targety) == 0 ||(targetx>13||targety<-13.8)) // 如果下一步超出邊界, 回來!!
                {
                    if(targety>=-1.2)
                    {
                        frameCamera.MoveUp(0.2*turnangle);
                    }
                    else
                    {
                        frameCamera.MoveForward(-0.2*turnangle);
                    }
                    targety-=0.2*turnangle;
                }
            }
        }
        //printf("%d\n",check_bound(targetx,targety)); //邊件判定
        printf("x = %f, y = %f\n",targetx,targety); //邊件判定
    }
    //===================
    if ( isSpecialKeyStateDown( GLUT_KEY_LEFT ) )
    {
        //Actorframe.RotateLocalY(0.1f);
        //Actor_yRot+=2.0f;
        if ( gameover == 0 )   //如果gameover則不能移動
        {
            float turnangle=1;
            r+=0.8f;
            if(isSpecialKeyStateDown( GLUT_KEY_UP )||isSpecialKeyStateDown( GLUT_KEY_DOWN ))
            {
                r-=0.4f;
                turnangle=cos(3.1415926/4);
            }
            face=2;
            if(isSpecialKeyStateDown( GLUT_KEY_UP ))
            {
                face-=1;
            }
            else if(isSpecialKeyStateDown( GLUT_KEY_DOWN ))
            {
                face+=1;
            }
            if( check(targetx+0.2,targety) ) //偵測人和蟲是否碰撞
            {
                frameCamera.MoveRight(0.2*turnangle);
                targetx+=0.2*turnangle;
                if ( check_bound(targetx,targety) == 0 ||(targetx>13||targety<-13.8)) // 如果下一步超出邊界, 回來!!
                {
                    frameCamera.MoveRight(-0.2*turnangle);
                    targetx-=0.2*turnangle;
                }
            }
        }
        //printf("%f\n",check_bound(targetx,targety)); //邊件判定
        printf("x = %f, y = %f\n",targetx,targety); //邊件判定
    }
    //===================
    if ( isSpecialKeyStateDown( GLUT_KEY_RIGHT ) )
    {
        //Actorframe.RotateLocalY(-0.1f);
        if ( gameover == 0 )   //如果gameover則不能移動
        {
            r+=0.8f;
            float turnangle=1;
            if(isSpecialKeyStateDown( GLUT_KEY_UP )||isSpecialKeyStateDown( GLUT_KEY_DOWN ))
            {
                r-=0.4f;
                turnangle=cos(3.1415926/4);
            }
            face=6;
            if(isSpecialKeyStateDown( GLUT_KEY_UP ))
            {
                face+=1;
            }
            else if(isSpecialKeyStateDown( GLUT_KEY_DOWN ))
            {
                face-=1;
            }
            if( check(targetx-0.2,targety) ) //偵測人和蟲是否碰撞
            {
                frameCamera.MoveRight(-0.2*turnangle);
                targetx-=0.2*turnangle;
                if ( check_bound(targetx,targety) == 0 ||(targetx>13||targety<-13.8)) // 如果下一步超出邊界, 回來!!
                {
                    frameCamera.MoveRight(0.2*turnangle);
                    targetx+=0.2*turnangle;
                }
            }
        }
        //printf("%d\n",check_bound(targetx,targety)); //邊件判定
        printf("x = %f, y = %f\n",targetx,targety); //邊件判定
        //Actor_yRot-=2.0f;
    }
    //===================
    if ( isKeyStateDown(' ') )
    {
        if ( gameover == 0 )   //20140113
        {
            if(timer>=0)
            {
                bb[bulletcount].setb();
                bulletcount++;
                if(bulletcount>=100)
                {
                    bulletcount=0;
                }
                timer=-30;
            }
        }
    }
    //=====處理SpecialKey的部分
    if( isKeyStateDown('v') || isKeyStateDown('V') )
    {
        if ( gameover == 0 )   //20140113
        {
            if ( view == 2 )   //90度
            {
                world.RotateLocalX(3.14159/view);
                view = 4;
                world.RotateLocalX(-3.14159/view);
            }
            else if ( view == 4 )   //45度
            {
                world.RotateLocalX(3.14159/view);
                view = 2;
                world.RotateLocalX(-3.14159/view);
            }
        }
    }
    if( isKeyStateDown('x') || isKeyStateDown('X') )
    {
        if ( gameover == 0 )   //20140113
        {
            frameCamera.MoveForward(0.2f);
        }
    }
    //===================
    if( isKeyStateDown('z') || isKeyStateDown('Z') )
    {
        if ( gameover == 0 )   //20140113
        {
            frameCamera.MoveForward(-0.2f);
        }
    }
    //===================
    if( isKeyStateDown('r') || isKeyStateDown('R') )   // 重新開始遊戲 20140113
    {
        if ( gameover == 1 )   //20140113
        {
            if ( gamescore >= highscore )   // 刷新最高分數
            {
                highscore = gamescore;
            }
            gameover = 0;  //======重新開始遊戲!!!
            gamescore = 0; //======重新開始計分!!!

            headx=0;
            heady=-1.32;
            headz=0;
            headr=0;
            state=0;
            bodyy=0;
            bodyz=0;
            bodyr=0;
            num=5;
            combo=0;
            for(int i=0; i<num; i++)
            {
                test[i].sattack=0;
                test[i].sattacked=0;
                test[i].speed=0.02;
                if(rand()%2==0)
                {
                    test[i].setlocation(-1.7-rand()%6,-16-rand()%5,-1);
                }
                else
                {
                    test[i].setlocation(13+rand()%5,3.5+rand()%6,-1);
                }

                for(int j=0; j<i; j++)
                {
                    if(test[j].getx()==test[i].getx())
                    {
                        if(test[j].gety()==test[i].gety())
                        {
                            if(rand()%2==0)
                            {
                                test[i].setlocation(-1.7-rand()%6,-16-rand()%5,-1);
                            }
                            else
                            {
                                test[i].setlocation(13+rand()%5,3.5+rand()%6,-1);
                            }
                            j=-1;
                        }
                    }
                }
                test[i].setnumber(i);
            }
            health = 100;  //======血量重新補滿!!!
        }
    }
    // 呼叫myDisplay重畫
    glutPostRedisplay();
    glutTimerFunc( 33, update_game, 0);
}
//============主程式============
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("蟲蟲生存戰");
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    //========================================
    init_KeyEvent(); // 初始化所有鍵盤上的按鍵
    glutKeyboardUpFunc( keyUP );
    glutKeyboardFunc( keyDOWN ); // 壓著鍵盤
    glutSpecialUpFunc( SpecialKeyUP );
    glutSpecialFunc( SpecialKeyDOWN ); // 壓著鍵盤
    //========================================
    //glutKeyboardFunc(Keys);      // (會lag)
    //glutSpecialFunc(SpecialKeys);// F1~F12,上下左右等(會lag)

    SetupRC();
    glutTimerFunc(33, TimerFunction, 1);
    //========================================
    update_game(0); // 主要為鍵盤事件
    //========================================
    glutMainLoop(); // 訊息迴圈

    ShutdownRC();

    return 0;
}

