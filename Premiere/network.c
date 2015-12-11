//
//  network.c
//  Premiere
//
//  Created by Samuel Dewan on 2015-11-07.
//  Copyright © 2015 Samuel Dewan. All rights reserved.
//

#include "network.h"

#include "strings.h"
#include "global.h"

#include "./libethernet/libethernet.h"

//MARK: Constants
#define ETHERNET_INIT_TIMEOUT       60000

#define DEFAULT_IP                  MAKE_IP(192,168,6,60)   // A random address
#define DEFAULT_NETMASK             MAKE_IP(255,255,255,0)
#define DEFAULT_ROUTER              MAKE_IP(192,168,6,1)
#define DEFAULT_DNS                 MAKE_IP(192,168,6,1)
#define DEFAULT_NTP                 MAKE_UP(132.246.11.227) // time.nrc.ca
#define GMT_OFFSET                  +2

//MARK: Variables
#ifdef IMPLEMENT_DHCP
static bool dhcp_avaliable = true;
#endif // IMPLEMENT_DHCP

static uint32_t lastEthernetTick;

//MARK: Internal Function Definitions
void network_handle_UDP_connect (UDPSocket Socket, uint32_t RemoteIP) {
    
}

void network_handle_UDP_disconnect (UDPSocket Socket) {
    
}

void network_handle_UDP_packet (UDPSocket Socket, const uint8_t* Buffer, size_t Length) {
    
}


void network_handle_TCP_connect (TCPSocket Socket, uint32_t RemoteIP) {
    
}

void network_handle_TCP_disconnect (TCPSocket Socket) {
    
}

void network_handle_TCP_packet (TCPSocket Socket, const uint8_t* Buffer, size_t Length) {
    
}

//MARK: External Function Definitions
void network_init(void) {
    spi_initialise(&PORTB, &DDRB, PB7, PB6, PB5, PB4);
    
    // Initialise all enabled modules of the ethernet stack
    #ifdef IMPLEMENT_DHCP
        ethernet_initialise_dhcp(STRING_HOSTNAME, ETHERNET_INIT_TIMEOUT);
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
    
    udp_open_port(6001, 5000, &network_handle_UDP_connect, &network_handle_UDP_disconnect, &network_handle_UDP_packet);
}

void network_deinit (void) {
    ethernet_deinitialise();
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