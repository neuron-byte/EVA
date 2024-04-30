#pragma once

// ESP CAM pinout
#define IN1_A 12 
#define IN2_A 13
#define IN3_B 14
#define IN4_B 15

void move_forward() {
    debugln("Movendo para frente.");
    digitalWrite(IN1_A, HIGH);
    digitalWrite(IN2_A, LOW);
    digitalWrite(IN3_B, HIGH);
    digitalWrite(IN4_B, LOW);
}

void move_back(){
    debugln("Movendo para trás.");
    digitalWrite(IN1_A, LOW);
    digitalWrite(IN2_A, HIGH);
    digitalWrite(IN3_B, LOW);
    digitalWrite(IN4_B, HIGH);
}

void turn_right(){
    debugln("Virando para direita.");
    digitalWrite(IN1_A, LOW);
    digitalWrite(IN2_A, HIGH);
    digitalWrite(IN3_B, HIGH);
    digitalWrite(IN4_B, LOW);
}

void turn_left(){
    debugln("Virando para esquerda.");
    digitalWrite(IN1_A, HIGH);
    digitalWrite(IN2_A, LOW);
    digitalWrite(IN3_B, LOW);
    digitalWrite(IN4_B, HIGH);
}

void move_stop() {
    debugln("Parando.");
    digitalWrite(IN1_A, HIGH);
    digitalWrite(IN2_A, HIGH);
    digitalWrite(IN3_B, HIGH);
    digitalWrite(IN4_B, HIGH);
}

void reset_move() {
    debugln("Resetando saidas.");
    digitalWrite(IN1_A, LOW);
    digitalWrite(IN2_A, LOW);
    digitalWrite(IN3_B, LOW);
    digitalWrite(IN4_B, LOW);
}