# Python CLI for Parallel EEPROM Programmer
# Colin Maykish
# May 2021

import serial
import time


def main():

    # A Small Test
    addr = 0
    limit = 64
    filename = "rom.bin"

    # Open serial port
    ser = serial.Serial("COM8", 115200)

    time.sleep(1)

    if not ser.is_open:
        print("Failed to open " + ser.name)
        exit(1)

    ser.flushInput()

    print("Connected to " + ser.name + " at " + str(ser.baudrate))

    print("Reading EEPROM")

    for x in range(limit):
        command = "RD" + hex(addr)[2:].zfill(4).upper() + '\n'
        b = command.encode()
        print("Command: ", b)
        ser.write(b)

        # Wait for response
        response = ser.readline().decode().strip()
        print(hex(addr)[2:].zfill(4).upper() + " : " + response.zfill(2))
        addr += 1

    print("Writing EEPROM")

    print("Writing file " + filename + " to EEPROM")

    addr = 0

    # Open binary file
    with open(filename, mode='rb') as file:
        contents = file.read()

        print("Input file size: " + str(len(contents)))

        print("Limiting to first " + str(limit) + " bytes")

        for b in contents:
            command = "WR" + \
                hex(addr)[2:].zfill(4).upper() + \
                hex(b)[2:].zfill(2).upper() + '\n'
            b = command.encode()
            print("Command: ", b)            
            ser.write(b)
            addr += 1

            # Wait for response
            response = ser.readline().decode().strip()

            if response != "DONE":
                print(response)
                ser.close()
                print("Closed " + ser.name)
                exit(1)
            else:
                print(
                    str(addr) + " / " + str(len(contents)))

            if limit is not None and addr >= limit:
                break

    ser.close()
    print("Closed " + ser.name)


main()
