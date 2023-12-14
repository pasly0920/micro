#include <avr/io.h>
#include <stdio.h>
#include <avr/iom128.h>

#define F_CPU 16000000UL // 버저 소리를 원래대로 할려면 주석 처리
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

#define CDS_VALUE 871

#define aTS75_CONFIG_REG        1
#define aTS75_TEMP_REG        0

void buzzer() {
    for(int loop = 0; loop < 2730; loop++) {
        PORTB = 0x10;
        _delay_us(183);
        PORTB = 0x00;
        _delay_us(183);
    }
}

void display_FND(int value)
{
    char digit[12] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67, 0x40, 0x00};
    char fnd_sel[4] = {0x01, 0x02, 0x04, 0x08};
    char value_int, value_deci, num[4];
    int i;
    if((value & 0x8000) != 0x8000)        // Sign 비트 체크
    num[3] = 11;
    else
    {    num[3] = 10;
        value = (~value)-1;            // 2's Compliment
    }
    value_int = (char)((value & 0x7f00) >> 8);
    value_deci = (char)(value & 0x00ff);
    num[2] = (value_int / 10) % 10;
    num[1] = value_int % 10;
    num[0] = ((value_deci & 0x80) == 0x80) * 5;
    for(i=0; i<4; i++)
    {
        PORTC = digit[num[i]];
        PORTG = fnd_sel[i];
        if(i==1)
        PORTC |= 0x80;
        _delay_ms(2);
    }
}


void I2C_Init(void) {
    TWBR = 12;                            // 16000000 / (16 + (2x12)) x prescaler = 400000Hz = 400kHz
    TWSR = (0<<TWPS1) | (0<<TWPS0);        // prescaler = 1
    TWCR = 0x04;
}

void I2C_start(void) {
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
}

void I2C_write(unsigned char data) {
    TWDR = data;
    TWCR = (1<<TWINT) | (1<<TWEN);
    while(!(TWCR & (1<<TWINT)));
}

unsigned char I2C_read(unsigned char ackVal) {
    TWCR = (1<<TWINT) | (1<<TWEN) | (ackVal<<TWEA);
    while(!(TWCR & (1<<TWINT)));
    return TWDR;
}

void I2c_stop(void) {
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
    for(int k=0; k<100; k++);                    //wait
}


void show_adc(unsigned short value) {
  if (value < CDS_VALUE) // 기준값 미만이면
    PORTD |= (0x01 << 3);
  else // 기준값 이상이면
    PORTD &= ~(0x01 << 3);
}

void temp_init(void) {
    I2C_Init();
    I2C_start();
    I2C_write(0b10011000);
    I2C_write(aTS75_CONFIG_REG);        // Configuration Register P1 = 0, P0 = 1
    I2C_write(0x00);
    I2c_stop();
}

int temp_read(void) {
    char high_byte, low_byte;
    I2C_start();
    I2C_write(0b10011000);
    I2C_write(aTS75_TEMP_REG);            // Temperature Register P1, P0 = 0

    I2C_start();
    I2C_write(0b10011000 | 1);            // Address + write (not read)
    high_byte = I2C_read(1);
    low_byte = I2C_read(0);
    I2c_stop();

    return ((high_byte<<8) | low_byte);
}

// 조도 센서 값 읽기
unsigned short read_adc() {
  unsigned char adc_low, adc_high;
  unsigned short value;
  ADCSRA |= 0x40; // ADC start conversion, ADSC = '1'
  while ((ADCSRA & 0x10) != 0x10) // ADC 변환 완료 검사
  ;
  adc_low = ADCL; // 변환된 Low 값 읽어오기
  adc_high = ADCH; // 변환된 High 값 읽어오기
  value = (adc_high << 8) | adc_low;
  // value 는 High 및 Low 연결한 16비트값
  return value;
}

void read_gas() {
    if (!((PIND & 0x10) == 0x10)) {
        // buzzer
        buzzer();
    }
}

void warn_temperature(int temperature) {
//    if(temperature > 40) {
//        buzzer();
//        _delay_ms(1000);
//    }
}

void setup() {
    // Buzzer init
    DDRB = 0x10;
    // 조도 센서 init
    DDRA = 0xff;
    ADMUX = 0x00;
    ADCSRA = 0x87;
    // 릴레이
    DDRD |= (0x01 << 3);
    DDRC = 0xff; DDRG = 0xff;
    PORTD = 3;
    temp_init();

    // 가스
    DDRD &= ~(0x01 << 4);
}

void loop() {
    unsigned short value;
    int temperature = 23;
    value = read_adc();
    show_adc(value);
    temp_read();
    temperature = temp_read();
    _delay_ms(10);
    if(temperature == 0) {
        temperature = temp_read();
    }
    warn_temperature(temperature);
    read_gas();
    display_FND(temperature);
}

int main() {
    setup();
    while(1) {
        loop();
    }
    return 0;
}
