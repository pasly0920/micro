#include <avr/io.h>
#include <avr/iom128.h>

int main() {
    DDRC = 0xFF;
    DDRG = 0x0F;
    PORTC = 0xFF;
    PORTG = 0x0F;
}
