#include <Arduino.h>
// #include <myArduino.hpp>

const int digitalPin[5] = {32, 34, 36, 38, 40}; // sensor
int instruction[] = {3, 2, 1, 2, 4, 2, 1, 2, 3, 2, 1, 2, 4, 2, 1, 2}; // 1 = straight, 2 = reverse, 3 = turn right, 4 = turn left
// can be enum
int ite = 0;
int white[5] = {};
int sum_white = 0;
int weight[5] = {5, 2, 0, -2, -5};

const int Kp = 25;
const int Tp = 50;
double error;

enum PIN
{
    //right motor
    PWMA = 12,
    AIN1 = 2,
    AIN2 = 3,

    //left motor
    PWMB = 13,
    BIN1 = 5,
    BIN2 = 6,
};

enum DIRECTION
{
    GO = 1,
    BACK = 2,
    RIGHT = 3,
    LEFT = 4,
};

enum COLOR{
    BLACK = 0,
    WHITE = 1,
};

void Tracking();
void MotorWriting(double, double);
void Takeinstruct();
void countwhite();

void MotorWriting(double vL, double vR)
{
    if (vR >= 0)
    {
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        // 這邊的Motor第幾個對應到的High/Low是助教的車對應到的，請自己測試自己車該怎麼填！
    }
    else if (vR < 0)
    {
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        vR = -vR; // 因為analogWrite只吃正數，所以如果本來是負數，就要乘-1
    }
    if (vL >= 0)
    {

        digitalWrite(BIN1, HIGH);
        digitalWrite(BIN2, LOW);
        // 這邊的Motor第幾個對應到的High/Low是助教的車對應到的，請自己測試自己車該怎麼填！
    }
    else if (vL < 0)
    {
        digitalWrite(BIN1, LOW);
        digitalWrite(BIN2, HIGH);
        vL = -vL; // 因為analogWrite只吃正數，所以如果本來是負數，就要乘-1
    }
    analogWrite(PWMB, vL * 1.1);
    analogWrite(PWMA, vR);
}

void Tracking()
{
    error = 0;
    for (int i = 0; i < 5; i++)
    {
        error += white[i] * weight[i];
    }
    if (sum_white != 0)
    {
        error /= sum_white;
    }

    int powerCorrection = Kp * error; // ex. Kp = 100, 也與w2 & w3有關
    int vR = Tp - powerCorrection;    // ex. Tp = 150, 也與w2 & w3有關
    int vL = Tp + powerCorrection;

    MotorWriting(vL, vR); // Feedback to motors
    
    // Serial.println(vL);
    // Serial.println(vR);
}

void Takeinstruct()
{
    switch (instruction[ite])
    {
    case 1:
        MotorWriting(Tp, Tp);
        delay(1300);
        break;
    case 2:
        MotorWriting(-Tp, Tp);
        delay(1000);
        while (!digitalRead(digitalPin[1]))
        {
            MotorWriting(-Tp, Tp);
        }
        break;
    case 3:
        MotorWriting(1.2 * Tp, Tp / 2);
        delay(1000);
        while (!digitalRead(digitalPin[3]))
        {
            MotorWriting(1.2 * Tp, Tp / 2);
        }
        break;
    case 4:
        MotorWriting(Tp / 2, 1.2 * Tp);
        delay(1000);
        while (!digitalRead(digitalPin[1]))
        {
            MotorWriting(Tp / 2, 1.2 * Tp);
        }
        break;
    }
    ite++;
    // Serial.println(ite);
}

void countwhite()
{
    sum_white = 0;
    for (int i = 0; i < 5; i++)
    {
        white[i] = !digitalRead(digitalPin[i]);
        sum_white += white[i];
    }
}

void setup()
{
    pinMode(PWMA, OUTPUT);
    pinMode(PWMB, OUTPUT);
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    for (int i = 0; i < 5; i++)
    {
        pinMode(digitalPin[i], INPUT);
    }
    Serial.begin(9600);
}

void loop()
{
    digitalWrite(PWMA, HIGH);
    digitalWrite(PWMB, HIGH);
    countwhite();
    if (sum_white >= 3)
        Tracking();
    else
    {
        MotorWriting(0, 0);
        // delay(500);
        Takeinstruct();
    }
    Serial.println(sum_white);
}

/*
1. identify enter and leave the block in series
2. once leave the block stop
3. take the instruction
*/