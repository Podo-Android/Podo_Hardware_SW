/*
사용 센서 
1. UNO R3 : http://kit128.com/goods/view?no=337
2. HX711 : http://kit128.com/goods/view?no=592 
3. 로드셀 (5Kg) : http://kit128.com/goods/view?no=595
*/

// serial communication
#include <SoftwareSerial.h>
#define PIN_RX 3
#define PIN_TX 2
// weight sensor
#include <HX711.h>

HX711 scale(8, 9);

// 영점 설정을 위한 보정계수
float calibration_factor = 462;

int En0 = 7; //  Low enabled
int En1 = 6; //  Low enabled

int S0 = 5;
int S1 = 4;
int S2 = 3;
int S3 = 2;

float avg_min = 0;
int leg_min = 0;

int leg[4] = {1, 2, 3, 4};
// 강아지 다리 별 압력 평균
float R1_avg = 0;
int R1_total = 0;
float R2_avg = 0;
int R2_total = 0;
float L1_avg = 0;
int L1_total = 0;
float L2_avg = 0;
int L2_total = 0;
// 평균이 담기는 배열
float avg[4];

int SIG_pin = A3;

void setup()
{
    // 확인을 위한 Serial
    Serial.begin(9600);
    // 블루투스 통신에 보내지는 데이터를 쓰는 Serial
    Serial1.begin(38400);
    scale.set_scale(calibration_factor); //Adjust to this calibration factor(영점 설정)
    scale.get_units(10);
    scale.tare();

    pinMode(En0, OUTPUT);
    pinMode(En1, OUTPUT);

    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
}

void loop()
{
    //압력센서
    for (int i = 0; i < 32; i++)
    {
        switch (i)
        {
        //R1
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            R1_total += readMux(i);
            break;
        //R2
        case 10:
        case 12:
        case 14:
            R2_total += readMux(i);
            break;
        //L1
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
            L1_total += readMux(i);
            break;
        //L2
        case 16:
        case 18:
        case 20:
            L2_total += readMux(i);
            break;

        default:
            break;
        }
    }

    R1_avg = R1_total / 5.0;
    R2_avg = R2_total / 3.0;
    L1_avg = L1_total / 5.0;
    L2_avg = L2_total / 3.0;

    avg[0] = R1_avg;
    avg[1] = R2_avg;
    avg[2] = L1_avg;
    avg[3] = L2_avg;

    avg_min = avg[0];
    leg_min = leg[0];
    for (int i = 1; i < 4; i++)
    {
        if (avg[i] < avg_min)
        {
            avg_min = avg[i];
            leg_min = leg[i];
        }
    }

    if (R1_avg < 60 && R2_avg < 70 && L1_avg < 60 && L2_avg < 60)
    {
        /**
         * 강아지가 내려가있을 때 아무 데이터도 보내지 않습니다.
         */
    }
    else
    {
        //무게센서
        Serial1.print(scale.get_units() / 1000);
        Serial1.print("/");

        Serial.print(scale.get_units() / 1000);
        Serial.print("/");

        Serial.print("R1 평균: ");
        Serial.print(R1_avg);
        Serial.println(" ");

        Serial.print("R2 평균: ");
        Serial.print(R2_avg);
        Serial.println(" ");

        Serial.print("L1 평균: ");
        Serial.print(L1_avg);
        Serial.println(" ");

        Serial.print("L2 평균: ");
        Serial.print(L2_avg);
        Serial.println(" ");

        /**
         * 모든 다리 건강할 때
         */
        if (avg_min > 80)
        {
            Serial.println(5);
            Serial1.println(5);
        }
        else if (avg_min < 70)
        {
            /**
        * 한 쪽 다리만 아플 때
        */
            Serial.println(leg_min);
            Serial1.println(leg_min);
        }
    }

    R1_avg = 0;
    R2_avg = 0;
    L1_avg = 0;
    L1_avg = 0;

    R1_total = 0;
    R2_total = 0;
    L1_total = 0;
    L2_total = 0;
    delay(3000);

    if (Serial1.available() > 0)
    {
        Serial.write(Serial1.read());
    }
}

int readMux(int channel)
{
    int controlPin[] = {S0, S1, S2, S3, En0, En1};

    int muxChannel[32][6] = {
        {0, 0, 0, 0, 0, 1}, //channel 0
        {0, 0, 0, 1, 0, 1}, //channel 1
        {0, 0, 1, 0, 0, 1}, //channel 2
        {0, 0, 1, 1, 0, 1}, //channel 3
        {0, 1, 0, 0, 0, 1}, //channel 4
        {0, 1, 0, 1, 0, 1}, //channel 5
        {0, 1, 1, 0, 0, 1}, //channel 6
        {0, 1, 1, 1, 0, 1}, //channel 7
        {1, 0, 0, 0, 0, 1}, //channel 8
        {1, 0, 0, 1, 0, 1}, //channel 9
        {1, 0, 1, 0, 0, 1}, //channel 10
        {1, 0, 1, 1, 0, 1}, //channel 11
        {1, 1, 0, 0, 0, 1}, //channel 12
        {1, 1, 0, 1, 0, 1}, //channel 13
        {1, 1, 1, 0, 0, 1}, //channel 14
        {1, 1, 1, 1, 0, 1}, //channel 15
        {0, 0, 0, 0, 1, 0}, //channel 16
        {0, 0, 0, 1, 1, 0}, //channel 17
        {0, 0, 1, 0, 1, 0}, //channel 18
        {0, 0, 1, 1, 1, 0}, //channel 19
        {0, 1, 0, 0, 1, 0}, //channel 20
        {0, 1, 0, 1, 1, 0}, //channel 21
        {0, 1, 1, 0, 1, 0}, //channel 22
        {0, 1, 1, 1, 1, 0}, //channel 23
        {1, 0, 0, 0, 1, 0}, //channel 24
        {1, 0, 0, 1, 1, 0}, //channel 25
        {1, 0, 1, 0, 1, 0}, //channel 26
        {1, 0, 1, 1, 1, 0}, //channel 27
        {1, 1, 0, 0, 1, 0}, //channel 28
        {1, 1, 0, 1, 1, 0}, //channel 29
        {1, 1, 1, 0, 1, 0}, //channel 30
        {1, 1, 1, 1, 1, 0}  //channel 31
    };

    //loop through the 6 sig
    for (int i = 0; i < 6; i++)
    {
        digitalWrite(controlPin[i], muxChannel[channel][i]);
    }

    //read the value at the SIG pin
    int val = analogRead(SIG_pin);

    //return the value
    return val;
}
