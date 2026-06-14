#define F_CPU 16000000UL // 16 MHz Clock Speed
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

// I2C/TWI Configuration
#define SCL_CLK 100000UL // 100 kHz I2C clock speed
#define BITRATE_REG ((F_CPU / SCL_CLK) - 16) / 2

// Device I2C Addresses 
#define OLED_ADDR 0x3C
#define BMP280_ADDR 0x76 

// BMP280 Registers
#define BMP280_REG_DIG_T1 0x88
#define BMP280_REG_CONTROL 0xF4
#define BMP280_REG_TEMP_MSB 0xFA

// 1. LOW-LEVEL I2C (TWI) FUNCTIONS

void i2c_init(void) {
	TWBR = BITRATE_REG; // Set bit rate register
	TWSR = 0x00;        // Set prescaler to 1
}

void i2c_start(void) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))); // Wait for transmission to complete
}

void i2c_stop(void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	while (TWCR & (1 << TWSTO));    // Wait for stop condition to execute
}

void i2c_write(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

uint8_t i2c_read_ack(void) {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

uint8_t i2c_read_nack(void) {
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}


// 2. SSD1306 OLED FUNCTIONS

void oled_command(uint8_t cmd) {
	i2c_start();
	i2c_write(OLED_ADDR << 1);
	i2c_write(0x00); // Co = 0, D/C# = 0 (Control byte for command)
	i2c_write(cmd);
	i2c_stop();
}

void oled_init(void) {
	_delay_ms(100); // Wait for screen to power up
	oled_command(0xAE); // Turn off display
	oled_command(0x20); // Set Memory Addressing Mode
	oled_command(0x02); // Page Addressing Mode
	oled_command(0x8D); // Enable charge pump
	oled_command(0x14);
	oled_command(0xAF); // Turn on display
}

void oled_clear(void) {
	for (uint8_t page = 0; page < 8; page++) {
		oled_command(0xB0 + page); // Set page address
		oled_command(0x00);        // Set lower column start address
		oled_command(0x10);        // Set higher column start address
		
		i2c_start();
		i2c_write(OLED_ADDR << 1);
		i2c_write(0x40); // Control byte for Data
		for (uint8_t i = 0; i < 128; i++) {
			i2c_write(0x00); // Clear every pixel
		}
		i2c_stop();
	}
}

// A basic 5x7 Font map for numbers, spaces, letters, and symbols
const uint8_t font[][5] = {
	[0]   = {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
	['1'-'0'] = {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
	['2'-'0'] = {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
	['3'-'0'] = {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
	['4'-'0'] = {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
	['5'-'0'] = {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
	['6'-'0'] = {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
	['7'-'0'] = {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
	['8'-'0'] = {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
	['9'-'0'] = {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
	['.'-'0'] = {0x00, 0x60, 0x60, 0x00, 0x00}, // .
	['-'-'0'] = {0x08, 0x08, 0x08, 0x08, 0x08}, // -
	[' '-'0'] = {0x00, 0x00, 0x00, 0x00, 0x00}, // Space
	['C'-'0'] = {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
	['T'-'0'] = {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
	['e'-'0'] = {0x38, 0x54, 0x54, 0x54, 0x18}, // e
	['m'-'0'] = {0x7C, 0x04, 0x18, 0x04, 0x7C}, // m
	['p'-'0'] = {0x7C, 0x14, 0x14, 0x14, 0x08}, // p
	[':'-'0'] = {0x00, 0x36, 0x36, 0x00, 0x00}, // :
};

void oled_putchar(char c, uint8_t page, uint8_t col) {
	if (c < ' ' || c > 'p') return;
	
	oled_command(0xB0 + page);
	oled_command(col & 0x0F);
	oled_command(0x10 | (col >> 4));
	
	i2c_start();
	i2c_write(OLED_ADDR << 1);
	i2c_write(0x40);
	for (uint8_t i = 0; i < 5; i++) {
		i2c_write(font[c - '0'][i]);
	}
	i2c_write(0x00); // Space between characters
	i2c_stop();
}

void oled_print(const char* str, uint8_t page, uint8_t col) {
	while (*str) {
		oled_putchar(*str++, page, col);
		col += 6; // Move right by 6 pixels
	}
}

// 3. BMP280 SENSOR FUNCTIONS

uint16_t dig_T1;
int16_t  dig_T2, dig_T3;

void bmp280_init(void) {
	// Read Calibration Parameters for Temperature
	i2c_start();
	i2c_write(BMP280_ADDR << 1);
	i2c_write(BMP280_REG_DIG_T1);
	i2c_start();
	i2c_write((BMP280_ADDR << 1) | 1);
	
	dig_T1 = i2c_read_ack() | (i2c_read_ack() << 8);
	dig_T2 = i2c_read_ack() | (i2c_read_ack() << 8);
	dig_T3 = i2c_read_ack() | (i2c_read_nack() << 8);
	i2c_stop();

	// Set Sensor Mode: Normal, Temp Oversampling x1
	i2c_start();
	i2c_write(BMP280_ADDR << 1);
	i2c_write(BMP280_REG_CONTROL);
	i2c_write(0x23); // 001(Oversampling x1) 000(No press) 11(Normal Mode)
	i2c_stop();
}

float bmp280_get_temperature(void) {
	i2c_start();
	i2c_write(BMP280_ADDR << 1);
	i2c_write(BMP280_REG_TEMP_MSB);
	i2c_start();
	i2c_write((BMP280_ADDR << 1) | 1);
	
	uint8_t msb = i2c_read_ack();
	uint8_t lsb = i2c_read_ack();
	uint8_t xlsb = i2c_read_nack();
	i2c_stop();

	// Combine raw data bits into a single 32-bit signed integer
	int32_t adc_T = ((int32_t)msb << 12) | ((int32_t)lsb << 4) | ((int32_t)xlsb >> 4);

	// Apply compensation formulas straight from the BMP280 Datasheet
	int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
	int32_t var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
	
	int32_t t_fine = var1 + var2;
	float temperature = (t_fine * 5 + 128) >> 8;
	
	return temperature / 100.0; // Return converted Celsius
}


// 4. MAIN LOOP EXECUTION

int main(void) {
	char temp_string[16];
	float current_temp;

	// Initialize Peripherals
	i2c_init();
	oled_init();
	bmp280_init();
	
	oled_clear();

	while (1) {
		// Read raw data and parse into Celsius
		current_temp = bmp280_get_temperature();

		// Clear display text locations
		oled_clear();

		// Format raw float data into a dynamic display string
		dtostrf(current_temp, 4, 1, temp_string);
		strcat(temp_string, " C");

		// Render formatted text to screen
		oled_print("Temp:", 2, 10);
		oled_print(temp_string, 4, 10);

		// Required 2-second interval delay
		_delay_ms(2000);
	}
}