/* Include libraries */
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
/* Begin defines */
#define PIN_IR_SENSOR           2         // Chân out của cảm biến hồng ngoại
#define NUMBER_OF_BLADES_FAN    3         // Số lượng cánh quạt
#define STEPS_PER_REVOLUTION    200       // Số bước để hoàn thành 1 vòng quay của step motor
#define FAN_RADIUS_CENTIMETER   100       // Bán kính của cánh quạt (đơn vị: cm)
#define FAN_DIAMETER_CENTIMETER 2*PI*FAN_RADIUS_CENTIMETER  // Đường kính của cánh quạt
#define STEP_ANGLE    360/STEPS_PER_REVOLUTION  // 1 bước quay được 1 góc STEP_ANGLE độ
#define MIN_SPEED     0
#define MIN_STEP      0
#define MAX_SPEED     255
#define MAX_STEP      MAX_SPEED/STEP_ANGLE
/* End defines*/
/* Begin global variables */
LiquidCrystal_I2C lcd(0x27, 16, 2);
Stepper stepper(STEPS_PER_REVOLUTION, 8, 9, 10, 11);
volatile int revCount = 0;
unsigned int RPM = 0;
unsigned int SPEED  = 0;
unsigned int STEP   = 0;
unsigned int preStep   = 0;
unsigned long timeGetRPM  = 0;
bool flagLCD = false;
/* End global variables */
/* Begin private functions */
void countRev();
unsigned int calculateRPM();
unsigned int calculateSpeed();
void displayLCD();
void turnStepMotor();
/* End private functions*/
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin();
  lcd.clear();  // Xóa màn hình LCD
  lcd.setCursor(0, 3);  // Đặt con trỏ tại ô thứ 3 hàng 0 (trên)
  lcd.print("Wellcome!");
  lcd.setCursor(1, 2);  // Đặt con trỏ tại ô thứ 2 hàng 1 (dưới)
  lcd.print("Speedometer");
  delay(1500);  // Dừng màn hình LCD 1.5s
  lcd.clear();
  // convert meter to kilo metor
  attachInterrupt(0, countRev, RISING); // Chọn interrupt 0, ngắt cạnh lênh, countRPM là hàm thực thi khi gặp ngắt
  timeGetRPM = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  RPM = calculateRPM();
  SPEED = calculateSpeed();
  displayLCD();

}
/* Begin private functions */
/*
  Tăng giá trị của biến revCount thêm 1 khi mà cảm biến IR sensor phát hiện cánh quạt
*/
void countRev()
{
  revCount++;
}
/*
  rpm = (revCount / Số lượng cánh quạt) / phút
  Cụ thể ở đây ta lấy 0.5s = 0.5/60 phút (1phút = 60s)
  -> rpm = (revCount / Số lượng cánh quạt) / (0.5/60)
*/
unsigned int calculateRPM()
{
  if ((unsigned long)(timeGetRPM - millis() >= 500)) // Tính RPM sau 0.5s
  {
    unsigned int rpm = 0;
    rpm = (unsigned int)((revCount / NUMBER_OF_BLADES_FAN) / (0.5 / 60)); // rpm: vòng/phút
    revCount = 0;  // Đặt lại biến rev = 0
    flagLCD = true;
    timeGetRPM = millis();
    return rpm;
  }
  return RPM;
}
/*
  Tốc độ(km/h) = Đường kính quạt(cm) * RPM * 0.001885
*/
unsigned int calculateSpeed()
{
  return (unsigned int)(FAN_DIAMETER_CENTIMETER * RPM * 0.001885);
}
void turnStepMotor()
{
  unsigned int tempStep = map(SPEED, MIN_SPEED, MAX_SPEED, MIN_STEP, MAX_STEP);
  STEP = tempStep - preStep;
  preStep = tempStep;
  stepper.step(STEP);
}
void displayLCD()
{
  if (flagLCD)  // Cập nhật LCD cứ sau 0.5s
  {
    lcd.setCursor(0, 0);
    lcd.print("RPM: ");
    lcd.setCursor(0, 5);
    lcd.print(RPM);
    lcd.setCursor(1, 0);
    lcd.print("Speed: ");
    lcd.setCursor(1, 7);
    lcd.print(SPEED);
    flagLCD = false;
  }
}
/* End private functions*/
