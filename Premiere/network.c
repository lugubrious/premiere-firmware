//
//  network.c
//  Premiere
//
//  Created by Samuel Dewan on 2015-11-07.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#include "network.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

#include "strings.h"
#include "global.h"
#include "dmx.h"
#include "buttons.h"

#include "shift.h"
#include "lcd.h"

#include "./libethernet/libethernet.h"

//MARK: Constants
#define ETHERNET_INIT_TIMEOUT       100

#define DEFAULT_IP                  MAKE_IP(192,168,2,10)   // A random address
#define DEFAULT_NETMASK             MAKE_IP(255,255,255,0)
#define DEFAULT_ROUTER              MAKE_IP(192,168,1,1)
#define DEFAULT_DNS                 MAKE_IP(192,168,1,1)
#define DEFAULT_NTP                 MAKE_UP(132.246.11.227) // time.nrc.ca
#define GMT_OFFSET                  +2

static const uint8_t sample_MACAddress[MAC_ADDRESS_LENGTH] = {0x00,0x44,0x65,0x77,0x61,0x6E};

//MARK: Variables
#ifdef IMPLEMENT_DHCP
static bool dhcp_avaliable = true;
#endif // IMPLEMENT_DHCP

static uint32_t lastEthernetTick;
static TCPSocket connection;

// MARK: Function declarations
static void _process_get(TCPSocket socket, uint8_t id, uint16_t qualifier);
static void _process_set (TCPSocket socket, const uint8_t* buffer, size_t Length);
static void _send_ping_response (TCPSocket socket, uint8_t byte);

//MARK: Internal Function Definitions
void network_handle_UDP_connect (UDPSocket Socket, uint32_t RemoteIP) {
    
}

void network_handle_UDP_disconnect (UDPSocket Socket) {
    
}

void network_handle_UDP_packet (UDPSocket Socket, const uint8_t* Buffer, size_t Length) {
    
}


void network_handle_TCP_connect (TCPSocket Socket, uint32_t RemoteIP) {
    connection = Socket;
    shift_out_buffer |= (1<<LED_EXCEPT_ID);
    shift_out();
}

void network_handle_TCP_disconnect (TCPSocket Socket) {
    connection = INVALID_TCP_SOCKET;
    shift_out_buffer &= !(1<<LED_EXCEPT_ID);
    shift_out();
}

void network_handle_TCP_packet (TCPSocket Socket, const uint8_t* Buffer, size_t Length) {
    lcd_write_string((char*)Buffer, LCD_LINE_TWO_START);
    lcd_write_int(Length, 2, LCD_LINE_TWO_START+14);
    
    if (Length < 2) {
        return;
    }

    switch (Buffer[0]) {
        case 0b00000000: //ping
            _send_ping_response(Socket, Buffer[1]);
            break;
        case 0b00000001: //ping response
            break;
        case 0b00000010: //set
            _process_set(Socket, Buffer, Length);
            break;
        case 0b00000011: { //get
            uint16_t qualifier = 0;
            if (Length > 4) {
                qualifier = Buffer[3];
                qualifier |= Buffer[2] << 8;
            } else if (Length > 3) {
                qualifier = Buffer[2];
            }
            _process_get(Socket, Buffer[1], qualifier);
            break;
        }
        case 0b00000100: //event
            break;
    }
}

static void _send_ping_response (TCPSocket socket, uint8_t byte) {
    uint8_t* buf;
    size_t buf_size, packet_length = 2;
    tcp_start_packet(socket, &buf, &buf_size);
    
    buf[0] = 0b00000001;
    buf[1] = byte;
    
    tcp_send(packet_length);
}

static void _process_get (TCPSocket socket, uint8_t id, uint16_t qualifier) {
    uint8_t* buf;
    size_t buf_size, packet_length = 0;
    tcp_start_packet(socket, &buf, &buf_size);
    
    buf[0] = 2; //set
    buf[1] = id;
    
    switch (id) {
        case 0b00000000: //info string
            packet_length = sprintf((char*)buf + 2, "Premiere Console Version 0.001");
            break;
        case 0b00000001: { //dmx value
            volatile uint8_t* universe = (qualifier < 512) ? dmx_universe_one : dmx_universe_two;
            uint8_t dimmer = (qualifier < 512) ? qualifier : qualifier - 512;
            
            buf[2] = universe[dimmer];
            packet_length = 3;
            break;
        }
        case 0b00000010: // ADC value
            memcpy(buf + 2, adc_values, 8);
            packet_length = 10;
            break;
        case 0b00000011: // Button events
            buf[2] = buttons_bump_dirty;
            buf[3] = (buttons_keypad_dirty >> 16) & 0xFF;
            buf[4] = (buttons_keypad_dirty >> 8)  & 0xFF;
            buf[5] = buttons_keypad_dirty & 0xFF;
            
            buttons_bump_dirty = 0;
            buttons_keypad_dirty = 0;
            
            packet_length = 6;
            break;
    }
    
    tcp_send(packet_length);
}

static void _process_set (TCPSocket socket, const uint8_t* buffer, size_t length) {
    switch (buffer[1]) {
        case 0b00000000: //info string
            break;
        case 0b00000001: { //dmx value
            uint16_t address = (buffer[2] << 8) | buffer[3];
            
            volatile uint8_t* universe = (address < 512) ? dmx_universe_one : dmx_universe_two;
            uint8_t dimmer = (address < 512) ? address : address - 512;
            
            memcpy(universe, buffer + 4, fmin(length - 4, 511 - dimmer));
            
            break;
        }
        case 0b00000010: // ADC value
            break;
        case 0b00000011: // Button events
            break;
    }
}

//MARK: External Function Definitions
void network_init(void) {
    spi_initialise(&PORTB, &DDRB, PB7, PB6, PB5, PB4);
    enc28j60_initialise(sample_MACAddress, true);
    
    lcd_write_char('_', LCD_LINE_ONE_START + 15);
    
    // Initialise all enabled modules of the ethernet stack
    #ifdef IMPLEMENT_DHCP
        ethernet_initialise_dhcp(STRING_HOSTNAME, ETHERNET_INIT_TIMEOUT);
//        ethernet_initialise_dhcp(NULL, ETHERNET_INIT_TIMEOUT);
    #else
        ethernet_initialise(DEFAULT_IP, DEFAULT_NETMASK, DEFAULT_ROUTER);
        ethernet_wait_for_link_status(0);
    #endif // IMPLEMENT_DHCP
    
    // Initialise the DNS resolver module
    #ifdef IMPLEMENT_DNS
    #	ifdef IMPLEMENT_DHCP
        dns_initialise(dhcp_get_dns_server_ip());
    #	else
        dns_initialise(DEFAULT_DNS);
    #	endif //IMPLEMENT_DHCP
    #endif //IMPLEMENT_DNS
    
    // Initialise the NTP client module
    #ifdef IMPLEMENT_NTP
    #	ifdef IMPLEMENT_DHCP
        ntp_initialise(dhcp_get_ntp_server_ip(), GMT_OFFSET);
    #	else
        ntp_initialise(DEFAULT_NTP, GMT_OFFSET);
    #	endif //IMPLEMENT_DHCP
    #endif //IMPLEMENT_DNS
    
//    udp_open_port(6001, 5000, &network_handle_UDP_connect, &network_handle_UDP_disconnect, &network_handle_UDP_packet);
    tcp_open_port(6562, 5000, &network_handle_TCP_connect, &network_handle_TCP_disconnect, &network_handle_TCP_packet);
}

void network_deinit (void) {
    ethernet_deinitialise();
    enc28j60_deinitialise();
}

void network_service(void) {
    ethernet_update();
    
    if ((millis - lastEthernetTick) > 1000) {
        lastEthernetTick = millis;
        ethernet_second_tick();
    }
    
#ifdef IMPLEMENT_DHCP
    if(dhcp_avaliable && !dhcp_has_valid_configuration()){
        if(!_ethernet_configure_via_dhcp(STRING_HOSTNAME,5000)){
            dhcp_avaliable = false;
            _ethernet_set_ip_netmask_router(DEFAULT_IP,DEFAULT_NETMASK,DEFAULT_ROUTER);
        }
    }
#endif
}

void network_get_ip_addr(uint8_t *address) {
    uint32_t ip = ethernet_get_ip();
    
    address[0] = ip & 0xFF;
    address[1] = ip >> 8;
    address[2] = ip >> 16;
    address[3] = ip >> 24;
}