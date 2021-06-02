// AT28C EEPROM Programmer for Arduino Mega
// Colin Maykish (github.com/crmaykish)
// 04/17/2016

// Control pins
#define CHIP_ENABLE 7
#define OUTPUT_ENABLE 8
#define WRITE_ENABLE 9

// I/O pins
const int I[] = {46, 47, 48, 49, 50, 51, 52, 53};

// Address pins
const int A[] = {26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40};

void setup()
{
	// Define control pins and turn them all off
	pinMode(OUTPUT_ENABLE, OUTPUT);
	pinMode(WRITE_ENABLE, OUTPUT);
	pinMode(CHIP_ENABLE, OUTPUT);
	digitalWrite(OUTPUT_ENABLE, HIGH);
	digitalWrite(WRITE_ENABLE, HIGH);
	digitalWrite(CHIP_ENABLE, HIGH);

	// Set address bus to outputs
	for (int i = 0; i < (sizeof(A) / sizeof(int) ); i++)
	{
		pinMode(A[i], OUTPUT);
	}

	Serial.begin(115200);
}

void loop()
{
	while (Serial.available() > 0)
	{
    // Note: Reading and writing works, but only with decimal arguments
  
    String input = Serial.readStringUntil('\n');
    String command = input.substring(0, 2);
    uint16_t addr = (input.substring(2, 7).toInt() & 0xFFFF);

    if (command.equals("RD"))
    {
      Serial.println(readByte(addr), HEX);
    }

    else if (command.equals("WR")){
      byte data = (input.substring(7, 10).toInt() & 0xFF);
//      Serial.println(data, HEX);
      writeByte(addr, data);
      Serial.println("DONE");
    }
    else
    {
      Serial.println("Bad input: " + input);
    }
	}
}

// Set data bus to INPUT or OUTPUT
void setDataBusMode(int mode)
{
	if (mode == INPUT || mode == OUTPUT)
	{
		for (int i = 0; i < 8; i++)
		{
			pinMode(I[i], mode);
		}
	}
}

// Write an address to the address bus
void setAddress(int addr)
{
	for (int i = 0; i < (sizeof(A) / sizeof(int) ); i++)
	{
		int a = (addr & (1 << i)) > 0;
		digitalWrite(A[i], a);
	}
}

void writeByte(int addr, byte val)
{
	digitalWrite(OUTPUT_ENABLE, HIGH);
	setDataBusMode(OUTPUT);
	setAddress(addr);

	// Send data value to data bus
	for (int i = 0; i < 8; i++)
	{
		int a = (val & (1 << i)) > 0;
		digitalWrite(I[i], a);
	}

	// Commit data write
	digitalWrite(CHIP_ENABLE, LOW);
	delay(10);
	digitalWrite(WRITE_ENABLE, LOW);
	delay(10);
	digitalWrite(WRITE_ENABLE, HIGH);
	delay(10);
	digitalWrite(CHIP_ENABLE, HIGH);
	delay(10);
}

byte readByte(int addr)
{
	byte data = 0;

	setDataBusMode(INPUT);

	// Write the addr
	for (int i = 0; i < (sizeof(A) / sizeof(int) ); i++)
	{
		int a = (addr & (1 << i)) > 0;
		digitalWrite(A[i], a);
	}

	digitalWrite(CHIP_ENABLE, LOW);
	digitalWrite(OUTPUT_ENABLE, LOW);
	delay(10);

	// Read data bus
	for (int i = 0; i < 8; i++)
	{
		int d = digitalRead(I[i]);
		data += (d << i);
	}

	digitalWrite(OUTPUT_ENABLE, HIGH);
	digitalWrite(CHIP_ENABLE, HIGH);
	delay(10);

	return data;
}
