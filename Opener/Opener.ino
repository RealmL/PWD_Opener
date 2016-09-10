

#include <avr/sleep.h>
#include <Servo.h>

#define CHECK_LAST_TIME 10000  //外部监测的持续时间(ms)
#define INNER_DELAY 200    //内部按钮按下要求时间范围(ms)
#define PWD_LEN 4   //密码长度
#define LED_DELAY 150   //LED闪烁间隔
#define TRY_LIMIT 3   //密码尝试次数限制

//************数据设置****************
bool WaitForCheck = 0;  //外部持续监测标志位
unsigned InnerDownTimes =0; //按钮按下的时间
unsigned t1;    //外部计时开始
unsigned t2;    //外部计时结束
unsigned InnerStart; //内部按钮计时开始
unsigned InnerStop; //内部按钮计时结束
bool ForceSweep =0;   //内部动作标志位

int PWD[PWD_LEN]={2,1,3,4};   //密码数组
int INPT[PWD_LEN]={0};    //输入数组
int TryTimes = 0;   //尝试次数
//************************************

//********引脚设置****************
int InnerButton = 2;//内部开关
int EXTERN_GND1 = 7;//额外的GND脚                         
int btA = 4;                                                   
int btB = 3;
int btC = 5;
int btD = 11;
int count = 0;    //密码输入计数
int LED = 6;//指示灯引脚
//********************************

//**************类实例**************************
  Servo myservo;
//**********************************************


//*********************舵机设置*************************
int pos = 0;    // variable to store the servo position
int FrontRange=45;
int BackRange=100;
int readData=0;
//******************************************************

void InnerClickDown()
{
  InnerDownTimes++;
  if(InnerDownTimes>=INNER_DELAY)
    ForceSweep =1;
}


//执行一次Sweep即可开门
void Sweep()
{
  for(pos = FrontRange; pos <= BackRange; pos += 1) // goes from 0 degrees to 180 degrees
  {                                  // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }   
}

//闪烁,delay:间隔时间,times:闪烁次数
void Blink(int times)
{
  int i=0;
  for(i=0;i<times;i++)
  {
  digitalWrite(LED,HIGH);
  delay(LED_DELAY);
  digitalWrite(LED,LOW);
  delay(LED_DELAY);
  }

}

void EnableCheck()
{
  t1 = millis();//开始计时
  WaitForCheck =1;  //开始监测
}


void Check()
{

  digitalWrite(LED,LOW);
  digitalWrite(EXTERN_GND1,LOW);


  //*********读取各按钮状态***************
  int A = digitalRead(btA);
  int B = digitalRead(btB);
  int C = digitalRead(btC);
  int D = digitalRead(btD);

  //***********密码判断****************
  if(count==3)
  {
    if(INPT[0]==PWD[0]&&INPT[1]==PWD[1]&&INPT[2]==PWD[2]&&INPT[3]==PWD[3])   
      {
        //密码正确
        Sweep(); //开门
        TryTimes = 0;   //错误计数器清零
      }
     else
     {
      //密码错误
      TryTimes++;//错误计数增加

      Blink(4); //闪烁提示错误
     }
     count=-1;
  }

  if(A==LOW||B==LOW||C==LOW||D==LOW)
   {
    digitalWrite(LED,HIGH);
    int v;
   if(A==LOW)
     v=1;
   else if(B==LOW)
      v=2;
      else if(C==LOW)
        v=3;
        else if(D==LOW)
          v=4;

    count++;
    INPT[count]=v;
    delay(300);
   }
}



void setup() {

  // 设定使用“Power-down”睡眠模式
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  // 允许启用睡眠模式
  sleep_enable();


  pinMode(EXTERN_GND1,OUTPUT);

  pinMode(LED,OUTPUT);
  pinMode(btA,INPUT_PULLUP);
  pinMode(btB,INPUT_PULLUP);
  pinMode(btC,INPUT_PULLUP);
  pinMode(btD,INPUT_PULLUP);
  pinMode(InnerButton,INPUT_PULLUP);//内部按钮上拉
  myservo.attach(9); //舵机引脚
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  delay(1000);
  Serial.println("Goodnight moon!");
  count =-1;

//中断函数注册
  attachInterrupt(0,InnerClickDown,LOW); //内部按钮按下,pin2
  attachInterrupt(1,EnableCheck,FALLING); //当第一个按键被按下时开始监测,pin3

}

void loop()
{ // run over and over

  if(ForceSweep==1)
    {
      Sweep();
      ForceSweep = 0;
      InnerDownTimes = 0;
    }

 if(WaitForCheck&&TryTimes<=TRY_LIMIT)
 {
  Check();
  t2 = millis();
  if(t2 - t1>CHECK_LAST_TIME)
    WaitForCheck = 0;   //停止监测
 }
  else{
    sleep_cpu();
  }
}

