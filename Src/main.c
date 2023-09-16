/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct Bullet{
    int x,y,vx,vy;
    int power;
    int cnt,type,isMir;
}Bullet;
typedef struct Fire{
	int cnt,fire_time,fire_num,sleep_time,move_type,enable,power,shape,biMir;
}Fire;
typedef struct Fly{
    int x,y;
    int hp;
    int cnt;
    int vx,vy;
    int fire_num;
    Fire fires[12];
    int score;
    int move_type;
}Fly;
typedef struct Hero{
	int fire_level;
    int v;
    int fire_name[10];
    int fire_state;
    char bul_name[5];
}Hero;
typedef struct Level{
	int maxTime;
	int minTime;
	int lastTime;
	int randTime;
	int maxHP;
	int minHP;
	int enemy_type_num;
	int enemy_type[10];
	int goal;
	int pos;
}LevelInfo;
typedef struct Boss{
	int max_hp,hp,T;
	int cnt;
	int x,y;
    int fire_num;

    Fire fires[10];
}Boss;

Boss boss;
typedef struct Paint{
	int draw_ori_x,draw_ori_y,draw_new_x,draw_new_y;
	float draw_size_x,draw_size_y;
	uint16_t draw_color;
}Paint;
Paint paint;


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define MAX_BULLET_ENEMY 220
#define MAX_BULLET_HERO 100
#define MAX_FLY 20
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define SCREEN_TOP 30
#define MAX_FLY_IN_SCREEN 4


#define BULLET_MOVE_LINE 1
#define BULLET_MOVE_XIE60 2
#define BULLET_MOVE_XIE45 3
#define BULLET_MOVE_XIE30 4
#define BULLET_MOVE_SEMICIRCLE 5

#define FLY_MOVE_QUICK 1
#define FLY_MOVE_NORMAL 2
#define FLY_MOVE_SHIFT 3
#define FLY_MOVE_CIRCLE 4

#define FIRE_SHAPE_NORMAL 0
#define FIRE_SHAPE_HEART 1


#define FIRE_SLOW_LINE 1
#define FIRE_HIGH_LINE 2
#define FIRE_MULTI_LINE60 3
#define FIRE_MULTI_LINE45 4
#define FIRE_MULTI_LINE30 5
#define FIRE_HEART 6
#define FIRE_SNAKE 7
#define FIRE_SPIRAL 8
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
Bullet enemy_bul[MAX_BULLET_ENEMY];
Bullet hero_bul[MAX_BULLET_HERO];
char msg[30];
Fly flys[MAX_FLY];//flys[0] is hero
int level_num;
int state; // 1 win 2 lose 3 be hit
int time;
int bul_num_e;
int bul_num_h;
int fly_num;
int move_state;
LevelInfo level_info;
Hero hero;
int key_a=0,key_b=0,key_c=0;
int score;
int myhp;
int game_cnt;
uint16_t bg_color;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int det(int x1, int y1, int x2, int y2){
    return x1 * y2 - x2 * y1;
}

int isCollision(int x,int y,int i){
    int fly_x = flys[i].x;
    int fly_y = flys[i].y;

    if (i == 0) {
        if (x <= fly_x+4 && x >= fly_x-4 && y <= fly_y+4 && y >= fly_y-4) {
            return 1;
        }
    } else {
        int x1 = fly_x;
        int y1 = fly_y + (int) 79*0.2f;
        int x2 = fly_x - (int) 75*0.2f;
        int y2 = fly_y - (int) 46*0.2f;
        int x3 = fly_x + (int) 75*0.2f;
        int y3 = fly_y - (int) 46*0.2f;
        if (det(x - x1, y - y1, x2 - x1, y2 - y1) <= 0
            && det(x - x2, y - y2, x3 - x2, y3 - y2) <= 0
            && det(x - x3, y - y3, x1 - x3, y1 - y3) <= 0) {
            return 1;
        }
    }

	return 0;
}
int isOutOfRange(int x,int y){
    int gap=2;
    if(y>=SCREEN_HEIGHT-gap || y<=SCREEN_TOP+gap ||x<=gap||x>=SCREEN_WIDTH-gap){
        return 1;
    }else{
        return 0;
    }
}
int changeBulletPos(int i,int isHero){
    /*int gap=10;
    if(isHero){
        if(hero_bul[i].x<=gap){
            hero_bul[i].x=SCREEN_WIDTH-gap;
        }
        if(hero_bul[i].x>=SCREEN_WIDTH-gap){
            hero_bul[i].x=gap;
        }
    }else{
        if(enemy_bul[i].x<=gap){
            enemy_bul[i].x=SCREEN_WIDTH-gap;
        }
        if(enemy_bul[i].x>=SCREEN_WIDTH-gap){
            enemy_bul[i].x=gap;
        }
    }*/
}
int changeFlyPos(int i){
    int gap=10;
    if(flys[i].x<=gap){
        flys[i].x=gap;
    }
    if(flys[i].x>=SCREEN_WIDTH-gap){
        flys[i].x=SCREEN_WIDTH-gap;
    }
    if(flys[i].y<=SCREEN_TOP+gap){
    	flys[i].y=SCREEN_TOP+gap;
    }
}

void setHeroFire(){
	for(int i=0;i<flys[0].fire_num;i++){
		flys[0].fires[i].enable=0;
		flys[0].fires[i].cnt=0;
	}
	if(hero.fire_state==0){
		flys[0].fires[0].enable=1;
		strcpy(hero.bul_name,"D");
	}else if(hero.fire_state==1){
		flys[0].fires[1].enable=1;
		strcpy(hero.bul_name,"C");
	}else if(hero.fire_state==2){
		if(hero.fire_name[2]==1){
			flys[0].fires[2].enable=1;
			strcpy(hero.bul_name,"B1");
		}else{
			flys[0].fires[6].enable=1;
			strcpy(hero.bul_name,"B2");
		}
	}else if(hero.fire_state==3){
		if(hero.fire_name[3]==1){
			flys[0].fires[0].enable=1;
			flys[0].fires[2].enable=1;
			strcpy(hero.bul_name,"A1");
		}else{
			flys[0].fires[7].enable=1;
			strcpy(hero.bul_name,"A2");
		}
	}else if(hero.fire_state==4){
		if(hero.fire_name[3]==1){
			flys[0].fires[5].enable=1;
			strcpy(hero.bul_name,"S3");
		}else if(hero.fire_name[3]==2){
			flys[0].fires[4].enable=1;
			flys[0].fires[7].enable=1;
			strcpy(hero.bul_name,"S2");
		}else{
			flys[0].fires[1].enable=1;
			flys[0].fires[2].enable=1;
			flys[0].fires[3].enable=1;
			flys[0].fires[4].enable=1;
			strcpy(hero.bul_name,"S1");
		}
	}
}

void enemy_LED() {
	HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
	HAL_Delay(10);
	HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
}

void hero_LED() {
	HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	HAL_Delay(10);
	HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
}



//cal all bullet whether collision or out of screen
void estimate(){
	int level_flag;
    Bullet copy1[MAX_BULLET_ENEMY];
    int flag1[MAX_BULLET_ENEMY];
    Bullet copy2[MAX_BULLET_HERO];
    int flag2[MAX_BULLET_ENEMY];
    Fly copy3[MAX_FLY];
    int flag3[MAX_FLY];
    int num1=0,num2=0,num3=0;
    for(int i=0;i<bul_num_e;i++){
        flag1[i]=1;
        copy1[i]=enemy_bul[i];
        int x=enemy_bul[i].x;
        int y=enemy_bul[i].y;
        if(isCollision(x,y,0)){
        	hero_LED();
        	if(enemy_bul[i].power==0)myhp-=1;
        	else myhp-=enemy_bul[i].power;
            key_a=0; key_b=0; key_c=0;
            flag1[i]=0;
            updateInfo();
        }
        if(isOutOfRange(x,y)){
            flag1[i]=0;
        }
    }
    for(int i=0;i<bul_num_h;i++){
        flag2[i]=1;
        copy2[i]=hero_bul[i];
        int x=hero_bul[i].x;
        int y=hero_bul[i].y;
        for(int j=1;j<fly_num;j++){
            if(isCollision(x,y,j)){
            	enemy_LED();
                flys[j].hp-=1;
                flag2[i]=0;
            }
        }
        if(isOutOfRange(x,y)){
            flag2[i]=0;
        }
    }
    for(int i=1;i<fly_num;i++){
        flag3[i]=1;
        copy3[i]=flys[i];
        if(flys[i].hp<=0){
        	enemy_LED();
            flag3[i]=0;
            level_flag=0;
            srand(HAL_GetTick());
            if(score<=2 && score+flys[i].score>2){
            	hero.fire_level=2;
            	myhp+=5;
            	level_flag=1;
            }else if(score<=7 && score+flys[i].score>7){
            	hero.fire_level=3;
            	hero.fire_name[2]=rand()%2;
            	myhp+=5;
            	level_flag=1;

            }else if(score<=25 && score+flys[i].score>25){
            	hero.fire_level=4;
            	hero.fire_name[3]=rand()%2;
            	myhp+=5;
            	level_flag=1;
            }else if(score<=80 && score+flys[i].score>80){
            	hero.fire_level=5;
            	hero.fire_name[4]=rand()%3;
            	myhp+=5;
            	level_flag=1;

            }

            score+=flys[i].score;
            updateInfo();
            if(level_flag){
            	hero.fire_state=hero.fire_level-1;
            	setHeroFire();
            	sprintf(msg,"Level Up!Get%s,Hp+5",hero.bul_name);
            	LCD_ShowString(20,160, 220, 200, 24, (uint8_t*)msg);
            	HAL_Delay(1000);
            	LCD_Clear(bg_color);
                updateInfo();
            }
        }
        if(isOutOfRange(flys[i].x,flys[i].y)){
            flag3[i]=0;
        }
    }
    for(int i=0;i<bul_num_e;i++){
        if(flag1[i])enemy_bul[num1++]=copy1[i];
    }
    bul_num_e=num1;
    for(int i=0;i<bul_num_h;i++){
        if(flag2[i])hero_bul[num2++]=copy2[i];
    }
    bul_num_h=num2;
    num3=1;
    for(int i=1;i<fly_num;i++){
        if(flag3[i])flys[num3++]=copy3[i];
    }
    fly_num=num3;
}
//move all bullet
void move(int move_state){
    if(move_state==1){
        flys[0].x+=flys[0].vx;
    }else if(move_state==2){
        flys[0].x+=flys[0].vx;
    }
    for(int i=1;i<fly_num;i++){
    	move_fly(i);
    }
    for(int i=0;i<fly_num;i++){
        changeFlyPos(i);
    }
    for(int i=0;i<bul_num_e;i++){
        bullet_move(i,0);
        changeBulletPos(i,0);
    }
    for(int i=0;i<bul_num_h;i++){
        bullet_move(i,1);
        changeBulletPos(i,1);
    }
}
void bullet_move(int i,int isHero){
    int type;
    int cnt,dx=0,dy=0,dir_y;
    int dir_x=1;
    if(isHero){
        cnt=hero_bul[i].cnt;
        type=hero_bul[i].type;
        dir_y=-1;
        hero_bul[i].cnt++;
        if(hero_bul[i].isMir)dir_x=-1;
        else dir_x=1;
    }else{
        cnt=enemy_bul[i].cnt;
        type=enemy_bul[i].type;
        dir_y=1;
        enemy_bul[i].cnt++;
        if(enemy_bul[i].isMir)dir_x=-1;
        else dir_x=1;
    }

    if(type==BULLET_MOVE_LINE){
        dx=0; dy=6;
    }else if(type==BULLET_MOVE_SEMICIRCLE){
    	double angle=(cnt*20-90)*3.1415/180;
    	dx=-sin(angle)*6;
    	dy=-cos(angle)*6;
    	if((cnt*20)%360<180){
    		dy=-dy;
    	}
    }else if(type==BULLET_MOVE_XIE60){
    	dx=1; dy=6;
    }else if(type==BULLET_MOVE_XIE45){
    	dx=3; dy=6;
    }else if(type==BULLET_MOVE_XIE30){
    	dx=4; dy=6;
    }else{
    	dx=0; dy=6;
    }
    if(isHero){
        hero_bul[i].x+=dx*dir_x;
        hero_bul[i].y+=dy*dir_y;
    }else{
        enemy_bul[i].x+=dx*dir_x;
        enemy_bul[i].y+=dy*dir_y;
    }
}

void move_fly(int i){

	int t = flys[i].cnt;
	if(flys[i].move_type==FLY_MOVE_SHIFT){
		if(flys[i].cnt%30<15){
			flys[i].x += 3; flys[i].vx =3;
		}else{
			flys[i].x += -3; flys[i].vx = -3;
		}
		flys[i].y +=1; flys[i].vy= 1;
	}else if(flys[i].move_type==FLY_MOVE_CIRCLE){
		double angle=t*5*3.1415/180;
		flys[i].x+=cos(angle)*2; flys[i].vx=cos(angle)*2;
		flys[i].y+=-sin(angle)*2+1; flys[i].vy=-sin(angle)*2+1;
	}else if(flys[i].move_type==FLY_MOVE_QUICK){
		flys[i].x += 0; flys[i].vx = 0;
		flys[i].y +=3; flys[i].vy= 3;
	}else if(flys[i].move_type==FLY_MOVE_NORMAL){
		flys[i].x += 0; flys[i].vx = 0;
		flys[i].y +=1; flys[i].vy= 1;
	}else{
		flys[i].x += 0; flys[i].vx = 0;
		flys[i].y +=1; flys[i].vy= 1;
	}
	flys[i].cnt++;
}


int is_fire(Fire *fire){
	if(!fire->enable){
		return 0;
	}
	int cnt = fire->cnt%(fire->sleep_time+fire->fire_time*fire->fire_num);
	if(cnt>=fire->fire_time*fire->fire_num){
		return 0;
	}
	if(cnt%(fire->fire_time)==0){
		return 1;
	}
	return 0;
}
Bullet bullet_factory(int i,int j){
	Bullet bul;
	bul.vx = flys[i].vx;bul.vy=flys[i].vy;
	bul.cnt=0;
	bul.type=flys[i].fires[j].move_type;
	bul.power=flys[i].fires[j].power;
	return bul;
}
Fire fire_factory(int type){
	Fire fire;
	fire.cnt=0;
	fire.enable=1;
	fire.shape=FIRE_SHAPE_NORMAL;
	fire.move_type=BULLET_MOVE_LINE;
	fire.power=1;
	if(type==FIRE_SLOW_LINE){
		fire.fire_time=7;
		fire.fire_num=3;
		fire.sleep_time=20;
	}else if(type==FIRE_HIGH_LINE){
		fire.fire_time=6;
		fire.fire_num=6;
		fire.sleep_time=12;
	}else if(type==FIRE_MULTI_LINE60){
		fire.fire_time=7;
		fire.fire_num=4;
		fire.sleep_time=8;
		fire.move_type=BULLET_MOVE_XIE60;
		fire.biMir=1;
	}else if(type==FIRE_MULTI_LINE45){
		fire.fire_time=7;
		fire.fire_num=4;
		fire.sleep_time=8;
		fire.move_type=BULLET_MOVE_XIE45;
		fire.biMir=1;
	}else if(type==FIRE_MULTI_LINE30){
		fire.fire_time=7;
		fire.fire_num=4;
		fire.sleep_time=8;
		fire.move_type=BULLET_MOVE_XIE30;
		fire.biMir=1;
	}else if(type==FIRE_HEART){
		fire.fire_time=1;
		fire.fire_num=1;
		fire.sleep_time=50;
		fire.power=3;
		fire.shape=FIRE_SHAPE_HEART;
	}else if(type==FIRE_SNAKE){
		fire.fire_time=3;
		fire.fire_num=5;
		fire.sleep_time=0;
		fire.move_type=BULLET_MOVE_SEMICIRCLE;
		fire.biMir=0;
	}else if(type==FIRE_SPIRAL){
		fire.fire_time=4;
		fire.fire_num=5;
		fire.sleep_time=0;
		fire.move_type=BULLET_MOVE_SEMICIRCLE;
		fire.biMir=1;
	}else{
		fire.fire_time=5;
		fire.fire_num=3;
		fire.sleep_time=30;
	}
	return fire;
}
#define FLY_CHONGER_A 1
#define FLY_CHONGER_B 2
#define FLY_CHONGER_C 3
#define FLY_MOVER_A 4
#define FLY_MOVER_B 5
#define FLY_MOVER_C 6
#define FLY_STORYER 7
Fly fly_factory(int type){
	Fly fly;
	fly.x=0; fly.y=0;
	fly.vx=0;fly.vy=0;fly.cnt=0;
	fly.hp=1;
	if(type==FLY_CHONGER_A){
        fly.score=1; fly.move_type=FLY_MOVE_NORMAL;
		fly.fire_num=1; fly.fires[0]=fire_factory(FIRE_SLOW_LINE);
	}else if(type==FLY_CHONGER_B){
        fly.score=2; fly.move_type=FLY_MOVE_NORMAL;
		fly.fire_num=1; fly.fires[0]=fire_factory(FIRE_MULTI_LINE60);
	}else if(type==FLY_CHONGER_C){
        fly.score=3; fly.move_type=FLY_MOVE_QUICK;
		fly.fire_num=2; fly.fires[0]=fire_factory(FIRE_MULTI_LINE45);
		fly.fires[1]=fire_factory(FIRE_MULTI_LINE60);
		fly.fires[0].power=2;
		fly.fires[1].power=2;
	}else if(type==FLY_MOVER_A){
        fly.score=1; fly.move_type=FLY_MOVE_SHIFT;
		fly.fire_num=1; fly.fires[0]=fire_factory(FIRE_SLOW_LINE);
	}else if(type==FLY_MOVER_B){
        fly.score=2; fly.move_type=FLY_MOVE_SHIFT;
		fly.fire_num=1; fly.fires[0]=fire_factory(FIRE_HIGH_LINE);
	}else if(type==FLY_MOVER_C){
	    fly.score=3; fly.move_type=FLY_MOVE_SHIFT;
		fly.fire_num=1; fly.fires[0]=fire_factory(FIRE_SNAKE);
		fly.fires[0].power=2;
	}else if(type==FLY_STORYER){
	    fly.score=4; fly.move_type=FLY_MOVE_CIRCLE;
		fly.fire_num=1; fly.fires[0]=fire_factory(FIRE_SPIRAL);
		fly.fires[0].power=3;
	}else{
		fly.hp=1; fly.score=1; fly.move_type=FLY_MOVE_NORMAL;
		fly.fire_num=1; fly.fires[0]=fire_factory(FIRE_SLOW_LINE);

	}
	return fly;

}

void add_heart_bul(int index,Bullet bul){
	int xv[50]={36,31,25,17,8,5,4,7,13,19,27,32,37,41,47,55,64,67,68,65,59,53,45,40};
	int yv[50]={20,13,9,8,14,22,30,39,44,49,54,59,66,13,9,8,14,22,30,39,44,49,54,59};
	for(int i=0;i<24;i++){
		bul.x=xv[i]-35;
		bul.y=yv[i]-35;
		if(index==0){
			bul.x = bul.x+flys[index].x;
			bul.y = bul.y+flys[index].y-35;
			hero_bul[bul_num_h++]=bul;
		}else{
			bul.x = bul.x+flys[index].x;
			bul.y = -bul.y+flys[index].y+35;
			enemy_bul[bul_num_e++]=bul;
		}
	}
}
void generate_bullet(){
    for(int i=0;i<fly_num;i++){
    	for(int j=0;j<flys[i].fire_num;j++){

    		if(is_fire(&flys[i].fires[j])){
    			Bullet bul,bul2;
    			if(flys[i].fires[j].shape==0){

            		bul = bullet_factory(i,j);
            		bul.x=flys[i].x; bul.y=flys[i].y;
            		bul.isMir=0;
            		if(i==0){
            			hero_bul[bul_num_h++]=bul;
            		}else{
            			enemy_bul[bul_num_e++]=bul;
            		}
            		if(flys[i].fires[j].biMir){
            			bul2 = bullet_factory(i,j);
            			bul2.x=flys[i].x; bul2.y=flys[i].y;
            			bul2.isMir=1;
            			if(i==0){
							hero_bul[bul_num_h++]=bul2;
						}else{
							enemy_bul[bul_num_e++]=bul2;
						}
            		}
    			}else if(flys[i].fires[j].shape==FIRE_SHAPE_HEART){
    				bul_num_h=0;
    				bul_num_e=0;
    				bul = bullet_factory(i,j);
    				add_heart_bul(i,bul);
    			}

    		}
    		if(flys[i].fires[j].enable){
    			flys[i].fires[j].cnt++;
    		}
    	}
    }
}

void generate_fly(){
	int hp,type;
	if((game_cnt-level_info.lastTime)>level_info.randTime){
		if(fly_num>MAX_FLY_IN_SCREEN)return;
		srand(HAL_GetTick());
		level_info.pos%=3;
		hp=rand()%(level_info.maxHP-level_info.minHP+1)+level_info.minHP;
		type=level_info.enemy_type[rand()%level_info.enemy_type_num];
		Fly fly=fly_factory(type); fly.hp=hp; fly.score*=fly.hp;
		fly.x=rand()%80+level_info.pos*80;fly.y = SCREEN_TOP+10;
		flys[fly_num++]=fly;
		level_info.randTime=level_info.minTime+rand()%(level_info.maxTime-level_info.minTime);
		level_info.lastTime=game_cnt;
		level_info.pos++;
	}
}
void DrawMyLine(int x1,int y1,int x2,int y2){
	x1=(int)((x1-paint.draw_ori_x)*paint.draw_size_x)+paint.draw_new_x;
	y1=(int)((y1-paint.draw_ori_y)*paint.draw_size_y)+paint.draw_new_y;
	x2=(int)((x2-paint.draw_ori_x)*paint.draw_size_x)+paint.draw_new_x;
	y2=(int)((y2-paint.draw_ori_y)*paint.draw_size_y)+paint.draw_new_y;
	if(x1<=0)x1=0;else if(x1>=SCREEN_WIDTH)x1=SCREEN_WIDTH;
	if(x2<=0)x2=0;else if(x2>=SCREEN_WIDTH)x2=SCREEN_WIDTH;
	if(y1<=0)y1=0;else if(y1>=SCREEN_HEIGHT)y1=SCREEN_HEIGHT;
	if(y2<=0)y2=0;else if(y2>=SCREEN_HEIGHT)y2=SCREEN_HEIGHT;
	LCD_DrawLine(x1,y1,x2,y2,paint.draw_color);
}

void draw_fly(int x,int y,float size_x,float size_y,uint16_t color){
	paint.draw_ori_x=120; paint.draw_ori_y=185;paint.draw_new_x=x;paint.draw_new_y=y;
	paint.draw_size_x=size_x; paint.draw_size_y=size_y;
	paint.draw_color=color;
	DrawMyLine(120, 80, 115, 85);
	DrawMyLine(115, 85, 110, 140);

	DrawMyLine(120, 80, 125, 85);
	DrawMyLine(125, 85, 130, 140);

	DrawMyLine(110, 140, 80, 155);
	DrawMyLine(130, 140, 160, 155);

	DrawMyLine(80, 155, 105, 150);
	DrawMyLine(160, 155, 135, 150);

	DrawMyLine(105, 150, 50, 185);
	DrawMyLine(135, 150, 190, 185);

	DrawMyLine(50, 185, 45, 205);
	DrawMyLine(190, 185, 195, 205);

	DrawMyLine(45, 205, 105, 190);
	DrawMyLine(195, 205, 135, 190);

	DrawMyLine(105, 190, 105, 200);
	DrawMyLine(135, 190, 135, 200);

	DrawMyLine(105, 200, 70, 220);
	DrawMyLine(135, 200, 170, 220);

	DrawMyLine(70, 220, 75, 230);
	DrawMyLine(170, 220, 165, 230);

	DrawMyLine(75, 230, 105, 225);
	DrawMyLine(165, 230, 135, 225);

	DrawMyLine(107, 185, 107, 235);
	DrawMyLine(133, 185, 133, 235);

	DrawMyLine(115, 185, 115, 235);
	DrawMyLine(125, 185, 125, 235);

	DrawMyLine(107, 185, 111, 175);
	DrawMyLine(111, 175, 115, 185);
	DrawMyLine(133, 185, 129, 175);
	DrawMyLine(129, 175, 125, 185);

	DrawMyLine(107, 235, 111, 245);
	DrawMyLine(111, 245, 115, 235);
	DrawMyLine(133, 235, 129, 245);
	DrawMyLine(129, 245, 125, 235);
}
void draw_fly_simple(int x,int y,float size_x,float size_y,uint16_t color){
	paint.draw_ori_x=120; paint.draw_ori_y=185;paint.draw_new_x=x;paint.draw_new_y=y;
	paint.draw_size_x=size_x; paint.draw_size_y=size_y;
	paint.draw_color=color;
	DrawMyLine(120,106,45,231);
	DrawMyLine(120,106,195,231);
	DrawMyLine(45,231,195,231);
	/*DrawMyLine(115,85,125,85);
	DrawMyLine(115, 85, 110, 140);
	DrawMyLine(125, 85, 130, 140);


	DrawMyLine(110, 140, 50, 185);
	DrawMyLine(130, 140, 190, 185);

	DrawMyLine(50, 185, 45, 205);
	DrawMyLine(190, 185, 195, 205);

	DrawMyLine(45, 205, 105, 190);
	DrawMyLine(195, 205, 135, 190);

	DrawMyLine(105, 190, 105, 200);
	DrawMyLine(135, 190, 135, 200);

	DrawMyLine(105, 200, 70, 220);
	DrawMyLine(135, 200, 170, 220);

	DrawMyLine(70, 220, 75, 230);
	DrawMyLine(170, 220, 165, 230);


	DrawMyLine(75, 230,165,230);*/

}
void draw_bullet(int x,int y,uint16_t color){
	LCD_Fill(x-2, y-2, x+2, y+2,color);
}
void draw_screen(){
	draw_fly(flys[0].x,flys[0].y,0.22f,0.22f,BLACK);
	LCD_Fill(flys[0].x-4,flys[0].y-4,flys[0].x+4,flys[0].y+4,RED);
	for(int i=1;i<fly_num;i++){
		uint16_t color =GRAY;
		if(flys[i].hp==5)color=RED;
		else if(flys[i].hp==4)color=MAGENTA;
		else if(flys[i].hp==3)color=BLUE;
		else if(flys[i].hp==2)color=LIGHTBLUE;
		else if(flys[i].hp==1)color=GRAY;
        draw_fly(flys[i].x,flys[i].y,0.2f,-0.2f,color);
		//draw_fly_simple(flys[i].x,flys[i].y,0.2f,-0.2f,color);
	}
	for(int i=0;i<bul_num_e;i++){
		uint16_t color=BLACK;
		if(enemy_bul[i].power==1)color =GRAYBLUE;
		else if(enemy_bul[i].power==2)color =BLUE;
		else if(enemy_bul[i].power==3)color =RED;
        draw_bullet(enemy_bul[i].x,enemy_bul[i].y,color);
	}
	for(int i=0;i<bul_num_h;i++){
		uint16_t color=BLACK;
		if(hero_bul[i].power==3){
			color=RED;
		}
        draw_bullet(hero_bul[i].x,hero_bul[i].y,color);
	}
}
void clear_screen(){
    for(int i=0;i<bul_num_e;i++){
        draw_bullet(enemy_bul[i].x,enemy_bul[i].y,bg_color);
	}
	for(int i=0;i<bul_num_h;i++){
        draw_bullet(hero_bul[i].x,hero_bul[i].y,bg_color);
	}

	draw_fly(flys[0].x,flys[0].y,0.22f,0.22f,bg_color);
	LCD_Fill(flys[0].x-4,flys[0].y-4,flys[0].x+4,flys[0].y+4,bg_color);
	for(int i=1;i<fly_num;i++){
        draw_fly(flys[i].x,flys[i].y,0.2f,-0.2f,bg_color);
        //draw_fly_simple(flys[i].x,flys[i].y,0.2f,-0.2f,bg_color);
	}
}

void control(){
    if(key_a==1 && key_c==0)move_state=1;// 1 left ,2 right
    else if(key_c==1 && key_a==0)move_state=2;
    else move_state=0;
    generate_fly();
    generate_bullet();
    move(move_state);
    estimate();
    change_level();
}
void BossControl(){
    if(key_a==1 && key_c==0)move_state=1;// 1 left ,2 right
    else if(key_c==1 && key_a==0)move_state=2;
    else move_state=0;
    BossAction();
    generate_bullet();
    move(move_state);
    estimate();
    estimateBoss();
}
int isHitBoss(int x,int y){
	int x1=boss.x,x2=boss.x-60,x3=boss.x,x4=boss.x+60;
	int y1=boss.y-30,y2=boss.y,y3=boss.y+30,y4=boss.y;
	if((boss.x-x)*(boss.x-x)+(boss.y-y)*(boss.y-y)<15*15){
		return 2;
	}else if(det(x1-x,y1-y,x2-x,y2-y)<0 &&det(x2-x,y2-y,x3-x,y3-y)<0&&det(x3-x,y3-y,x4-x,y4-y)<0&&det(x4-x,y4-y,x1-x,y1-y)<0){
		return 1;
	}
	return 0;
return 0;
}
void estimateBoss(){
	    Bullet copy2[MAX_BULLET_HERO];
	    int flag2[MAX_BULLET_ENEMY];
	    int num2=0;
	    for(int i=0;i<bul_num_h;i++){
	        flag2[i]=1;
	        copy2[i]=hero_bul[i];
	        int x=hero_bul[i].x;
	        int y=hero_bul[i].y;
			if(isHitBoss(x,y)==1){
				boss.hp-=1;
				updateBossHp();
				flag2[i]=0;
			}else if(isHitBoss(x,y)==2){
				boss.hp-=5;
				updateBossHp();
				flag2[i]=0;
			}
	        if(isOutOfRange(x,y)){
	            flag2[i]=0;
	        }
	    }
	    for(int i=0;i<bul_num_h;i++){
	        if(flag2[i])hero_bul[num2++]=copy2[i];
	    }
	    bul_num_h=num2;
}


void BossAction(){
	int T=boss.T;
	int r=boss.cnt%(4*T);
	int r2=boss.cnt%(48*T);
	if(r2==16*T){
		LCD_Clear(BLACK);
		bg_color=BLACK;
		updateInfo();
	}else if(r2==0){
		LCD_Clear(WHITE);
		bg_color=WHITE;
		updateInfo();
	}
	if(r2>=16*T){
		if(r2%5==0&&r2<32*T){
			for(int i=10;i<SCREEN_WIDTH;i+=40){
				Bullet bul;
				bul.cnt=0; bul.power=3; bul.type=BULLET_MOVE_SEMICIRCLE;
				bul.vx=0;bul.vy=0; bul.x=i; bul.y=SCREEN_TOP+10;
				enemy_bul[bul_num_e++]=bul;
			}
			if(r2%10==0){
				Bullet bul;bul.cnt=0; bul.power=3; bul.type=BULLET_MOVE_LINE;
				bul.vx=0;bul.vy=0; bul.x=6; bul.y=SCREEN_TOP+10;
				enemy_bul[bul_num_e++]=bul;
				bul.vx=0;bul.vy=0; bul.x=15; bul.y=SCREEN_TOP+10;
				enemy_bul[bul_num_e++]=bul;
				bul.vx=0;bul.vy=0; bul.x=24; bul.y=SCREEN_TOP+10;
				enemy_bul[bul_num_e++]=bul;
				bul.vx=0;bul.vy=0; bul.x=SCREEN_WIDTH-5; bul.y=SCREEN_TOP+10;
				enemy_bul[bul_num_e++]=bul;
				bul.vx=0;bul.vy=0; bul.x=SCREEN_WIDTH-11; bul.y=SCREEN_TOP+10;
				enemy_bul[bul_num_e++]=bul;
			}
		}
	}else if(r2<12*T){
		if(r2%6==0){
			Bullet bul;
			bul.cnt=0; bul.power=3;bul.type=BULLET_MOVE_XIE60;
			bul.vx=0;bul.vy=0; bul.x=boss.x; bul.y=boss.y; bul.isMir=0;
			enemy_bul[bul_num_e++]=bul;
			bul.isMir=1;
			enemy_bul[bul_num_e++]=bul;
		}

	}
	if(r<T){
		boss.x-=8;
		boss.y+=5;
	}else if(r<2*T){
		boss.x+=8;
		boss.y+=5;
	}else if(r<3*T){
		boss.x+=8;
		boss.y-=5;
	}else{
		boss.x-=8;
		boss.y-=5;
	}

	boss.cnt++;
}

void drawBoss(){
	int x=boss.x;
	int y=boss.y;
	paint.draw_ori_x=0;paint.draw_ori_y=0;paint.draw_new_x=0;paint.draw_new_y=0;
	paint.draw_size_x=1.0f;paint.draw_size_y=1.0f;
	paint.draw_color=BLACK;
	DrawMyLine(x,y-30,x+60,y);
	DrawMyLine(x,y-30,x-60,y);
	DrawMyLine(x,y+30,x-60,y);
	DrawMyLine(x,y+30,x+60,y);

	if(boss.hp*2>boss.max_hp){
		LCD_Draw_Circle(x,y,15,BLACK);
		LCD_Draw_Circle(x,y,11,BLACK);
		LCD_Draw_Circle(x,y,7,BLACK);
		LCD_Draw_Circle(x,y,3,BLACK);
	}else{
		LCD_Draw_Circle(x,y,15,RED);
		LCD_Draw_Circle(x,y,11,RED);
		LCD_Draw_Circle(x,y,7,RED);
		LCD_Draw_Circle(x,y,3,RED);
	}

}
void clearBoss(){
	int x=boss.x;
	int y=boss.y;
	paint.draw_ori_x=0;paint.draw_ori_y=0;paint.draw_new_x=0;paint.draw_new_y=0;
	paint.draw_size_x=1.0f;paint.draw_size_y=1.0f;
	paint.draw_color=bg_color;
	DrawMyLine(x,y-30,x+60,y);
	DrawMyLine(x,y-30,x-60,y);
	DrawMyLine(x,y+30,x-60,y);
	DrawMyLine(x,y+30,x+60,y);
	LCD_Draw_Circle(x,y,15,bg_color);
	LCD_Draw_Circle(x,y,11,bg_color);
	LCD_Draw_Circle(x,y,7,bg_color);
	LCD_Draw_Circle(x,y,3,bg_color);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	switch (GPIO_Pin) {
		case KEY_WK_Pin:
			key_a = 1; key_b = key_c = 0;
			flys[0].vx=-hero.v;
			break;
		case KEY0_Pin:
			if (HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == GPIO_PIN_RESET) {
				key_c = 1; key_a = key_b = 0;
				flys[0].vx=hero.v;
			}
			break;
		case KEY1_Pin:
			if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET) {
				hero.fire_state++;
				hero.fire_state%=hero.fire_level;
				setHeroFire();
				key_b = 1; key_a = key_c = 0;
				flys[0].vx=0;
				updateInfo();
				HAL_Delay(50);
			}
			break;
		default:
			key_a=0; key_b=0; key_c=0;
	}
//	sprintf(msg,"%d,%d,%d",key_a,key_b,key_c);
//		LCD_ShowString(30, 260, 200, 24, 24, (uint8_t*) msg);

}
void updateBossHp(){
	LCD_Fill(0,SCREEN_TOP-5,SCREEN_WIDTH,SCREEN_TOP+5,bg_color);
	if(boss.hp>=0&&boss.max_hp>0){
		LCD_Fill(0,SCREEN_TOP-5,boss.hp*SCREEN_WIDTH/boss.max_hp,SCREEN_TOP+5,RED);
	}
}
void updateInfo(){
	sprintf(msg,"Lv%d G:%d Hp:%d %s",level_num,score,myhp,hero.bul_name);
	LCD_ShowString(0, 0, 200, SCREEN_TOP, 24, (uint8_t*)msg);
	updateBossHp();

}
void init_game(){
	hero.fire_level=1;
	hero.fire_name[0]=0;
	hero.fire_state=0;
	hero.v=3;
	myhp=2;
	level_num=1;
	score=0;
	boss.hp=0;
	bg_color=WHITE;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);

}
void init_boss(){
	boss.cnt=0;
	boss.max_hp=400;
	boss.hp=boss.max_hp;
	boss.x=120;
	boss.y=SCREEN_TOP+45;
	boss.T=8;

}
void init_level(int level_num){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);

    bul_num_e=0;
    bul_num_h=0;
    fly_num=0;


    level_info.maxTime=80;
    level_info.minTime=50;
    level_info.randTime=10;
    level_info.lastTime=0;

    if(level_num==1){
    	level_info.maxHP=2;
    	level_info.minHP=1;
    	level_info.enemy_type_num=2;
    	level_info.enemy_type[0]=FLY_CHONGER_A;
    	level_info.enemy_type[1]=FLY_MOVER_A;
    	level_info.goal=8;
    }else if(level_num==2){
    	level_info.maxHP=4;
    	level_info.minHP=2;
    	level_info.enemy_type_num=4;
    	level_info.enemy_type[0]=FLY_CHONGER_A;
    	level_info.enemy_type[1]=FLY_MOVER_A;
    	level_info.enemy_type[2]=FLY_CHONGER_B;
    	level_info.enemy_type[3]=FLY_MOVER_B;
    	level_info.goal=40;
    }else if(level_num==3){
    	level_info.maxHP=5;
    	level_info.minHP=3;
    	level_info.enemy_type_num=5;
    	level_info.enemy_type[0]=FLY_CHONGER_B;
    	level_info.enemy_type[1]=FLY_MOVER_B;
    	level_info.enemy_type[2]=FLY_CHONGER_C;
    	level_info.enemy_type[3]=FLY_MOVER_C;
    	level_info.enemy_type[4]=FLY_STORYER;
    	level_info.goal=150;
    }else if(level_num==4){
    	state=5;
    	init_boss();
    }else{
    	level_info.maxHP=2;
    	level_info.minHP=1;
    	level_info.enemy_type_num=2;
    	level_info.enemy_type[0]=FLY_CHONGER_A;
    	level_info.enemy_type[1]=FLY_MOVER_A;
    	level_info.goal=10;
    }
	Fly fly = fly_factory(1);
	fly.x=120; fly.y=SCREEN_HEIGHT-30;
	fly.fire_num=8;
	fly.fires[0]=fire_factory(FIRE_SLOW_LINE);
	fly.fires[1]=fire_factory(FIRE_HIGH_LINE);
	fly.fires[2]=fire_factory(FIRE_MULTI_LINE60);
	fly.fires[3]=fire_factory(FIRE_MULTI_LINE45);
	fly.fires[4]=fire_factory(FIRE_MULTI_LINE30);
	fly.fires[5]=fire_factory(FIRE_HEART);
	fly.fires[6]=fire_factory(FIRE_SNAKE);
	fly.fires[7]=fire_factory(FIRE_SPIRAL);
	flys[0]=fly;
	fly_num=1;
	setHeroFire();
    game_cnt=0;
    updateInfo();
}

void change_level(){
	if(game_cnt%10==0){
		if(level_info.maxTime>30)level_info.maxTime--;
		if(level_info.minTime>20)level_info.minTime--;
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  LCD_Init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
//  HAL_UART_Receive_IT(&huart1, (uint8_t *)rxBuffer, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  state=1;key_a=0; key_b=0; key_c=0;
  char msg[30];
  init_game();
  while (1)
  {//bul_num_e,bul_num_h,fly_num,
	if(state==1){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
		draw_fly(120,185,1,1,RED);
		LCD_ShowString(30, 40, 200, 24, 24, (uint8_t*) "Aircraft Battle");
		LCD_ShowString(30, 260, 200, 24, 24, (uint8_t*) "Start");
		LCD_ShowString(150, 260, 200, 24, 24, (uint8_t*) "Exit");
		if(key_a==1){
			state=2;
			key_a=0; key_b=0; key_c=0;
            LCD_Clear(WHITE);
            init_level(level_num);
		}
	}else if(state==2){
		game_cnt++;
		control();
		draw_screen();
		HAL_Delay(80);
		clear_screen();
		if(score>level_info.goal){
			state=4;
			key_a=0; key_b=0; key_c=0;
			LCD_Clear(WHITE);
		}else if(myhp<=0){
			state=3;
			key_a=0; key_b=0; key_c=0;
			LCD_Clear(WHITE);
		}
	}else if(state==3){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
		draw_fly(120,185,1,1,GRAY);
		LCD_ShowString(30, 40, 200, 24, 24, (uint8_t*) "You are loser!");
		LCD_ShowString(30, 260, 200, 24, 24, (uint8_t*) "Restart");
		LCD_ShowString(150, 260, 200, 24, 24, (uint8_t*) "Exit");
		play_mus();
		if(key_a==1){
			init_game();
			level_num=1;
			state=2;
			key_a=0; key_b=0; key_c=0;
            LCD_Clear(WHITE);
            init_level(level_num);
		}else if(key_b==1){
			state=1;
			key_a=0; key_b=0; key_c=0;
            LCD_Clear(WHITE);

		}
	}else if(state==4){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
		draw_fly(120,185,1,1,RED);
		LCD_ShowString(30, 40, 200, 24, 24, (uint8_t*) "Aircraft Battle");
		LCD_ShowString(30, 260, 200, 24, 24, (uint8_t*) "Next Lv ");
		LCD_ShowString(150, 260, 200, 24, 24, (uint8_t*) "Exit");
			if(key_a==1){
				level_num++;
				state=2;
				key_a=0; key_b=0; key_c=0;
	            LCD_Clear(WHITE);
	            init_level(level_num);

			}else if(key_b==1){
				state=1;
				key_a=0; key_b=0; key_c=0;
	            LCD_Clear(WHITE);
			}
	}else if(state==5){
		draw_screen();
		drawBoss();
		HAL_Delay(80);
		clear_screen();
		clearBoss();
		game_cnt++;
		BossControl();
		if(boss.hp<=0){
			state=6;
			key_a=0; key_b=0; key_c=0;
			LCD_Clear(WHITE);
		}else if(myhp<=0){
			state=3;
			key_a=0; key_b=0; key_c=0;
			LCD_Clear(WHITE);
		}
	}else if(state==6){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
		draw_fly(120,185,1,1,RED);
		LCD_ShowString(30, 40, 200, 24, 24, (uint8_t*) "You are winner!");
		LCD_ShowString(30, 260, 200, 24, 24, (uint8_t*) "Again");
		LCD_ShowString(150, 260, 200, 24, 24, (uint8_t*) "Exit");
		if(key_a==1){
			init_game();
			level_num=1;
			state=2;
			key_a=0; key_b=0; key_c=0;
            LCD_Clear(WHITE);
            init_level(level_num);
		}else if(key_b==1){
			state=1;
			init_game();
			key_a=0; key_b=0; key_c=0;
            LCD_Clear(WHITE);

		}
	}


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
