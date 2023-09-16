# Aircraft-Battle (嵌入式课程项目设计)
- 开发平台：嵌入式开发板
- 编程语言：C语言
- 成员： 舒天野，卫焱滨，李树青，朱星宇
- 时间: 2019年12月
## 效果图：
![Loading](https://github.com/SliverySky/Aircraft-Battle/blob/master/Fig.png)
### 我方飞机（hero）：
- 停留在屏幕最下方，可以通过按键控制其左右移动。
- 通过按键控制是否发射子弹，以及子弹发射速度和弹道。（子弹为黑色）
- 由于我方飞机是大战入侵者的英雄，机首、机翼和机尾都是无敌状态，只有红色核心区域被击中时生命值才会减少。
- 被敌方子弹击中时黄色LED灯会亮。
- 生命值小于0时该关卡失败。
- 击中敌机得到对应分数，分数达到一定值该关卡通过。
### 敌方飞机（enemy）：
- 通过一定的频率（带有随机性）在屏幕最上方左、中、右三个区域随机出现。
- 敌方飞机的任意一处被子弹打到都会减少该飞机的生命值。
- 生命值小于0时敌机坠毁。
- 被我方子弹击中时红色LED灯会亮。
### 状态显示
- Lv: 目前关卡 (共四关)
- G: 得到的分数
- Hp: 剩余生命值
- D-S: 道具种类（子弹弹道和威力不同）   




