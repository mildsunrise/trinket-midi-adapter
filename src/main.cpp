/**
 * Copyright (c) 2017 Alba Mendez
 * This file is part of trinket-midi-adapter.
 *
 * trinket-midi-adapter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * trinket-midi-adapter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with trinket-midi-adapter.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <avr/power.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "SoftwareSerial.h" // Serial (MIDI) signal receiving
#include "trinketusb.h" // Trinket oscillator calibration & USB init


/* Build USB-MIDI event packets */

uint8_t packet [4]; // where packet bytes are stored
enum {
    UNKNOWN = 0,
    ONE_PARAMETER = 1,
    TWO_PARAMETER = 2,
    SYS_EX = 3,
} command_kind = UNKNOWN; // kind of the last command we saw
int parameters_read = 0; // how many data bytes have been read for current command

char build_packet(uint8_t next) {
    if (next & 0x80) {
        // New command is started!
        switch (next >> 4) {
        case 0x8: // Note OFF
        case 0x9: // Note ON
        case 0xA: // Poly Key Pressure
        case 0xB: // Control message
        case 0xE: // Pitch Bend
            command_kind = TWO_PARAMETER; break;
        case 0xC: // Program Change
        case 0xD: // Mono Key Pressure
            command_kind = ONE_PARAMETER; break;
        case 0xF:
            switch (next & 0xF) {
            case 0x0: // SysEx start
                command_kind = SYS_EX; break;
            case 0x7: // SysEx end
                {uint8_t byte_to_write = (1 + (parameters_read++)) % 3;
                packet[1 + byte_to_write] = next;
                packet[0] = 0x5 + byte_to_write;
                command_kind = UNKNOWN;}
                return 1;
            // TODO: support rest of messages
            default:
                command_kind = UNKNOWN;
            } break;
        default:
            command_kind = UNKNOWN;
        }
        packet[0] = next >> 4;
        packet[1] = next;
        packet[2] = packet[3] = 0;
        parameters_read = 0;
        return 0;
    }

    if (command_kind == ONE_PARAMETER || command_kind == TWO_PARAMETER) {
        // Data byte is command parameter, store in packet
        packet[2 + (parameters_read++)] = next;
        if (parameters_read == (int) command_kind) {
            // All parameters in! Finish this command, start new one
            parameters_read = 0;
            return 1;
        }
    }

    if (command_kind == SYS_EX) {
        // Store data byte in packet
        uint8_t byte_to_write = (1 + (parameters_read++)) % 3;
        packet[1 + byte_to_write] = next;
        if (byte_to_write == 0) packet[2] = packet[3] = 0;
        if (byte_to_write == 2) {
            packet[0] = 0x4;
            return 1;
        }
    }

    return 0;
}


/* Main loop. */

SoftwareSerial serial (0,0); // pins are ignored

int counter = 0;
void work(void) {
    int next; // FIXME: send many together, if possible(?)
    while (usbInterruptIsReady() && (next = serial.read()) != -1) {
        if (build_packet((uint8_t) next))
            usbSetInterrupt(packet, 4);
    }
}

int main(void) {
    // run at full speed, because Trinket defaults to 8MHz for low voltage compatibility reasons
    clock_prescale_set(clock_div_1);

    serial.begin(31250);
    trinketUsbBegin();

    while(1) {
        work();
        usbPoll();
    }
}
