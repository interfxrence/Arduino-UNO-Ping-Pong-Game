#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define start 13
#define P1U 6
#define P1D 7
#define P2U 8
#define P2D 9
#define piezo 10

byte paddle1[16], paddle2[16], ball[16];
byte player11[8], player12[8];
byte player21[8], player22[8];
byte ball1[8], ball2[8];

int x = 0, bounce = 0;
char direction = 'L';
int score1 = 0, score2 = 0;
bool game = false;
int a = 0;

void setup() {
  pinMode(P1U, INPUT);
  pinMode(P1D, INPUT);
  pinMode(P2U, INPUT);
  pinMode(P2D, INPUT);
  pinMode(start, INPUT);
  pinMode(piezo, OUTPUT);

  piezoSound(50);
  lcd.init(); 
  lcd.backlight(); 
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Arduino UNO");
  lcd.setCursor(1, 1);
  lcd.print("Ping Pong Game");
  delay(1500);
}

void loop() {
  while (!game) {
    if (a == 0) {
      initObjects();
      x = 8;
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Press start");
      lcd.setCursor(3, 1);
      lcd.print("button   ->");
      a = 1;
    }

    if (digitalRead(start) == HIGH) {
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("Player");
      lcd.setCursor(0, 1);
      lcd.print("1");
      lcd.setCursor(15, 1);
      lcd.print("2");
      delay(2000);

      for (int i = 3; i > 0; i--) {
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("Be ready!");
        lcd.setCursor(8, 1);
        lcd.print(i);
        if (i == 1) piezoSound(100);
        delay(1000);
      }

      a = 0;
      game = true;
    }
    delay(100);
  }

  readInputs();

  splitArray(paddle1, player11, player12);
  splitArray(paddle2, player21, player22);
  splitArray(ball, ball1, ball2);
  createCustomChars();

  render();

  if (x == 3 && detectCollision(paddle1)) {
    direction = 'R';
    score1++;
    piezoSound(10);
  } else if (x == 3) {
    gameOver("P2 won!");
  }

  if (x == 15 && detectCollision(paddle2)) {
    direction = 'L';
    score2++;
    piezoSound(10);
  } else if (x == 15) {
    gameOver("P1 won!");
  }

  updateBallPosition();
}

void piezoSound(int duration) {
  tone(piezo, 1000);
  delay(duration);
  noTone(piezo);
}

void paddle1Up() {
  for (int i = 0; i < 15; i++) {
    paddle1[i] = paddle1[i + 1];
  }
  paddle1[15] = 0;
}

void paddle1Down() {
  for (int i = 15; i > 0; i--) {
    paddle1[i] = paddle1[i - 1];
  }
  paddle1[0] = 0;
}

void paddle2Up() {
  for (int i = 0; i < 15; i++) {
    paddle2[i] = paddle2[i + 1];
  }
  paddle2[15] = 0;
}

void paddle2Down() {
  for (int i = 15; i > 0; i--) {
    paddle2[i] = paddle2[i - 1];
  }
  paddle2[0] = 0;
}

void ballLeftDown() {
  for (int i = 15; i > 0; i--) {
    ball[i] = ball[i - 1];
  }
  ball[0] = 0;
  bounce = 0;
}

void ballLeftUp() {
  for (int i = 15; i > 0; i--) {
    ball[i] = ball[i - 1];
  }
  ball[0] = 0;
  bounce = 1;
}

void ballRightDown() {
  for (int i = 0; i < 15; i++) {
    ball[i] = ball[i + 1];
  }
  ball[15] = 0;
  bounce = 0;
}

void ballRightUp() {
  for (int i = 0; i < 15; i++) {
    ball[i] = ball[i + 1];
  }
  ball[15] = 0;
  bounce = 1;
}

void initObjects() {
  for (int i = 0; i < 16; i++) {
    paddle1[i] = (i >= 6 && i <= 9) ? 1 : 0;
    paddle2[i] = (i >= 6 && i <= 9) ? 16 : 0;
    ball[i] = (i == 7) ? 1 : 0;
  }
}

void readInputs() {
  if (digitalRead(P1U) == HIGH) { paddle1Up(); delay(50); }
  if (digitalRead(P1D) == HIGH) { paddle1Down(); delay(50); }
  if (digitalRead(P2U) == HIGH) { paddle2Up(); delay(50); }
  if (digitalRead(P2D) == HIGH) { paddle2Down(); delay(50); }
}

void render() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(score1);
  lcd.setCursor(0, 1); lcd.print(score2);
  lcd.setCursor(3, 0); lcd.write(byte(0));
  lcd.setCursor(3, 1); lcd.write(byte(1));
  lcd.setCursor(15, 0); lcd.write(byte(2));
  lcd.setCursor(15, 1); lcd.write(byte(3));
  lcd.setCursor(x, 0); lcd.write(byte(4));
  lcd.setCursor(x, 1); lcd.write(byte(5));
}

bool detectCollision(byte* paddle) {
  for (int i = 0; i < 16; i++) {
    if (ball[i] != 0 && ball[i] == paddle[i]) return true;
  }
  return false;
}

void gameOver(const char* winner) {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Game over");
  lcd.setCursor(4, 1);
  lcd.print(winner);
  for (int i = 0; i < 3; i++) {
    piezoSound(500);
    delay(1000);
  }
  score1 = score2 = 0;
  delay(1000);
  game = false;
}

void updateBallPosition() {
  bool moved = false;

  if (direction == 'L') {
    if (bounce == 0) { ballLeftDown(); x--; }
    else { ballLeftUp(); x--; }
  } else {
    if (bounce == 0) { ballRightDown(); x++; }
    else { ballRightUp(); x++; }
  }
}

void splitArray(byte* src, byte* top, byte* bottom) {
  for (int i = 0; i < 8; i++) {
    top[i] = src[i];
    bottom[i] = src[i + 8];
  }
}

void createCustomChars() {
  lcd.createChar(0, player11);
  lcd.createChar(1, player12);
  lcd.createChar(2, player21);
  lcd.createChar(3, player22);
  lcd.createChar(4, ball1);
  lcd.createChar(5, ball2);
}
