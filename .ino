#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>

#define I2C_ADDR 0x27
#define Esquerdo   PB1  // pino 9
#define Meio       PB2  // pino 10
#define Direito    PB3  // pino 11
#define Reset      PB4  // pino 12
#define Boozer     PB0  // pino 8
#define Sensor     PD2  // pino 2 (INT0)

volatile uint16_t gotas = 0;
uint16_t meta = 0;
volatile bool detectando = false;
volatile bool ativo = false;
volatile bool nova_gota = false;

void i2c_init() {
    TWSR = 0x00;
    TWBR = 32;
}

void i2c_start() {
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
}

void i2c_stop() {
    TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
    _delay_ms(1);
}

void i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
}

// LCD
void lcd_send_cmd(uint8_t cmd) {
    uint8_t u = cmd & 0xF0;
    uint8_t l = (cmd << 4) & 0xF0;

    i2c_start();
    i2c_write(I2C_ADDR << 1);
    i2c_write(u | 0x0C);
    i2c_write(u | 0x08);
    i2c_write(l | 0x0C);
    i2c_write(l | 0x08);
    i2c_stop();
}

void lcd_send_data(uint8_t data) {
    uint8_t u = data & 0xF0;
    uint8_t l = (data << 4) & 0xF0;

    i2c_start();
    i2c_write(I2C_ADDR << 1);
    i2c_write(u | 0x0D);
    i2c_write(u | 0x09);
    i2c_write(l | 0x0D);
    i2c_write(l | 0x09);
    i2c_stop();
}

void lcd_init() {
    _delay_ms(50);
    lcd_send_cmd(0x33);
    lcd_send_cmd(0x32);
    lcd_send_cmd(0x28);
    lcd_send_cmd(0x0C);
    lcd_send_cmd(0x06);
    lcd_send_cmd(0x01);
    _delay_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    lcd_send_cmd(0x80 | (col + (row ? 0x40 : 0x00)));
}

void lcd_print(char* str) {
    while (*str) {
        lcd_send_data(*str++);
    }
}

void lcd_clear() {
    lcd_send_cmd(0x01);
    _delay_ms(2);
}

void atualizar_lcd() {
    char linha1[17];
    char linha2[17];
    lcd_clear();
    lcd_set_cursor(0, 0);
    snprintf(linha1, 17, "Meta: %4d", meta);
    lcd_print(linha1);
    lcd_set_cursor(1, 0);
    snprintf(linha2, 17, "Cont: %4d", gotas);
    lcd_print(linha2);
}

// boozer
void beep_Esquerdo() {
    PORTB |= (1 << Boozer);
    _delay_ms(60);
    PORTB &= ~(1 << Boozer);
}

void beep_Meio() {
    for (int i = 0; i < 2; i++) {
        PORTB |= (1 << Boozer);
        _delay_ms(40);
        PORTB &= ~(1 << Boozer);
        _delay_ms(40);
    }
}

void beep_Direito() {
    PORTB |= (1 << Boozer);
    _delay_ms(120);
    PORTB &= ~(1 << Boozer);
}

void beep_reset() {
    for (int i = 0; i < 3; i++) {
        PORTB |= (1 << Boozer);
        _delay_ms(70);
        PORTB &= ~(1 << Boozer);
        _delay_ms(30);
    }
}

void beep_meta() {
    for (int i = 0; i < 3; i++) {
        PORTB |= (1 << Boozer);
        _delay_ms(150);
        PORTB &= ~(1 << Boozer);
        _delay_ms(100);
    }
}

void beep_gota_normal() {
    PORTB |= (1 << Boozer);
    _delay_ms(80);
    PORTB &= ~(1 << Boozer);
}

void beep_gota_excedente() {
    for (int i = 0; i < 2; i++) {
        PORTB |= (1 << Boozer);
        _delay_ms(120);
        PORTB &= ~(1 << Boozer);
        _delay_ms(60);
    }
}

ISR(INT0_vect) {
    if (!ativo) return;

    if (!detectando) {
        gotas++;
        detectando = true;
        nova_gota = true;

        if (gotas == meta) {
            beep_meta();
        } else if (gotas > meta) {
            beep_gota_excedente();
        } else {
            beep_gota_normal();
        }
    }
}

int main(void) {

    i2c_init();
    lcd_init();
    atualizar_lcd();

    DDRB |= (1 << Boozer);
    DDRD &= ~(1 << Sensor);
    PORTD |= (1 << Sensor);

    DDRB &= ~((1 << Esquerdo) | (1 << Meio) | (1 << Direito) | (1 << Reset));
    PORTB &= ~((1 << Esquerdo) | (1 << Meio) | (1 << Direito) | (1 << Reset));

    EICRA |= (1 << ISC01);
    EIMSK |= (1 << INT0);
    sei();

    while (1) {
        _delay_ms(50);

        if (PIND & (1 << Sensor)) {
            detectando = false;
        }

        if (nova_gota) {
            nova_gota = false;
            atualizar_lcd();
        }

        if (PINB & (1 << Esquerdo)) {
            if (meta > 1) meta--;
            atualizar_lcd();
            beep_Esquerdo();
            _delay_ms(300);
        }

        if (PINB & (1 << Direito)) {
            if (meta < 9999) meta++;
            atualizar_lcd();
            beep_Direito();
            _delay_ms(300);
        }

        if (PINB & (1 << Meio)) {
            if (gotas == 0) {
                ativo = true;
            } else {
                gotas = 0;
                ativo = false;
            }
            atualizar_lcd();
            beep_Meio();
            _delay_ms(300);
        }

        if (PINB & (1 << Reset)) {
            gotas = 0;
            ativo = false;
            atualizar_lcd();
            beep_reset();
            _delay_ms(300);
        }
    }
}
