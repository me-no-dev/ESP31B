/*
 ESP31BWiFiSTA.cpp - WiFi library for ESP31B

 Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
 This file is part of the ESP31B core for Arduino environment.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 Reworked on 28 Dec 2015 by Markus Sattler

 */

#include "ESP31BWiFi.h"
#include "ESP31BWiFiGeneric.h"
#include "ESP31BWiFiSTA.h"

extern "C" {
#include "c_types.h"
#include "esp_common.h"
#include "smartconfig.h"
#include "lwip/err.h"
#include "lwip/dns.h"
}

extern "C" void esp_schedule();
extern "C" void esp_yield();

// -----------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------- Private functions ------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

static bool sta_config_equal(const station_config& lhs, const station_config& rhs);


/**
 * compare two STA configurations
 * @param lhs station_config
 * @param rhs station_config
 * @return equal
 */
static bool sta_config_equal(const station_config& lhs, const station_config& rhs) {
    if(strcmp(reinterpret_cast<const char*>(lhs.ssid), reinterpret_cast<const char*>(rhs.ssid)) != 0) {
        return false;
    }

    if(strcmp(reinterpret_cast<const char*>(lhs.password), reinterpret_cast<const char*>(rhs.password)) != 0) {
        return false;
    }

    if(lhs.bssid_set != rhs.bssid_set) {
        return false;
    }

    if(lhs.bssid_set) {
        if(memcmp(lhs.bssid, rhs.bssid, 6) != 0) {
            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------- STA function -----------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

bool ESP31BWiFiSTAClass::_useStaticIp = false;

/**
 * Start Wifi connection
 * if passphrase is set the most secure supported mode will be automatically selected
 * @param ssid const char*          Pointer to the SSID string.
 * @param passphrase const char *   Optional. Passphrase. Valid characters in a passphrase must be between ASCII 32-126 (decimal).
 * @param bssid uint8_t[6]          Optional. BSSID / MAC of AP
 * @param channel                   Optional. Channel of AP
 * @param connect                   Optional. call connect
 * @return
 */
wl_status_t ESP31BWiFiSTAClass::begin(const char* ssid, const char *passphrase, int32_t channel, const uint8_t* bssid, bool connect) {

    if(!WiFi.enableSTA(true)) {
        // enable STA failed
        return WL_CONNECT_FAILED;
    }

    if(!ssid || *ssid == 0x00 || strlen(ssid) > 31) {
        // fail SSID too long or missing!
        return WL_CONNECT_FAILED;
    }

    if(passphrase && strlen(passphrase) > 63) {
        // fail passphrase too long!
        return WL_CONNECT_FAILED;
    }

    struct station_config conf;
    strcpy(reinterpret_cast<char*>(conf.ssid), ssid);

    if(passphrase) {
        strcpy(reinterpret_cast<char*>(conf.password), passphrase);
    } else {
        *conf.password = 0;
    }

    if(bssid) {
        conf.bssid_set = 1;
        memcpy((void *) &conf.bssid[0], (void *) bssid, 6);
    } else {
        conf.bssid_set = 0;
    }

    struct station_config current_conf;
    wifi_station_get_config(&current_conf);
    if(sta_config_equal(current_conf, conf)) {
        DEBUGV("sta config unchanged");
        return status();
    }


    if(WiFi._persistent) {
        wifi_station_set_config(&conf);
    } else {
        wifi_station_set_config_current(&conf);
    }

    if(connect) {
        wifi_station_connect();
    }

    if(channel > 0 && channel <= 13) {
        wifi_set_channel(channel);
    }

    if(!_useStaticIp) {
        wifi_station_dhcpc_start();
    }

    return status();
}

wl_status_t ESP31BWiFiSTAClass::begin(char* ssid, char *passphrase, int32_t channel, const uint8_t* bssid, bool connect) {
    return begin((const char*) ssid, (const char*) passphrase, channel, bssid, connect);
}

/**
 * Use to connect to SDK config.
 * @return wl_status_t
 */
wl_status_t ESP31BWiFiSTAClass::begin() {

    if(!WiFi.enableSTA(true)) {
        // enable STA failed
        return WL_CONNECT_FAILED;
    }

    wifi_station_connect();

    if(!_useStaticIp) {
        wifi_station_dhcpc_start();
    }
    return status();
}


/**
 * Change IP configuration settings disabling the dhcp client
 * @param local_ip   Static ip configuration
 * @param gateway    Static gateway configuration
 * @param subnet     Static Subnet mask
 * @param dns1       Static DNS server 1
 * @param dns2       Static DNS server 2
 */
bool ESP31BWiFiSTAClass::config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1, IPAddress dns2) {

    if(!WiFi.enableSTA(true)) {
        return false;
    }

    struct ip_info info;
    info.ip.addr = static_cast<uint32_t>(local_ip);
    info.gw.addr = static_cast<uint32_t>(gateway);
    info.netmask.addr = static_cast<uint32_t>(subnet);

    wifi_station_dhcpc_stop();
    if(wifi_set_ip_info(STATION_IF, &info)) {
        _useStaticIp = true;
    } else {
        return false;
    }
    ip_addr_t d;

    if(dns1 != (uint32_t)0x00000000) {
        // Set DNS1-Server
        d.addr = static_cast<uint32_t>(dns1);
        dns_setserver(0, &d);
    }

    if(dns2 != (uint32_t)0x00000000) {
        // Set DNS2-Server
        d.addr = static_cast<uint32_t>(dns2);
        dns_setserver(1, &d);
    }

    return true;
}

/**
 * will force a disconnect an then start reconnecting to AP
 * @return ok
 */
bool ESP31BWiFiSTAClass::reconnect() {
    if((WiFi.getMode() & WIFI_STA) != 0) {
        if(wifi_station_disconnect()) {
            return wifi_station_connect();
        }
    }
    return false;
}

/**
 * Disconnect from the network
 * @param wifioff
 * @return  one value of wl_status_t enum
 */
bool ESP31BWiFiSTAClass::disconnect(bool wifioff) {
    bool ret;
    struct station_config conf;
    *conf.ssid = 0;
    *conf.password = 0;

    if(WiFi._persistent) {
        wifi_station_set_config(&conf);
    } else {
        wifi_station_set_config_current(&conf);
    }
    ret = wifi_station_disconnect();

    if(wifioff) {
        WiFi.enableSTA(false);
    }

    return ret;
}

/**
 * is STA interface connected?
 * @return true if STA is connected to an AD
 */
bool ESP31BWiFiSTAClass::isConnected() {
    return (status() == WL_CONNECTED);
}


/**
 * Setting the ESP31B station to connect to the AP (which is recorded)
 * automatically or not when powered on. Enable auto-connect by default.
 * @param autoConnect bool
 * @return if saved
 */
bool ESP31BWiFiSTAClass::setAutoConnect(bool autoConnect) {
    bool ret;
    ret = wifi_station_set_auto_connect(autoConnect);
    return ret;
}

/**
 * Checks if ESP31B station mode will connect to AP
 * automatically or not when it is powered on.
 * @return auto connect
 */
bool ESP31BWiFiSTAClass::getAutoConnect() {
    return (wifi_station_get_auto_connect() != 0);
}

/**
 * Set whether reconnect or not when the ESP31B station is disconnected from AP.
 * @param autoReconnect
 * @return
 */
bool ESP31BWiFiSTAClass::setAutoReconnect(bool autoReconnect) {
    return wifi_station_set_reconnect_policy(autoReconnect);
}

/**
 * Wait for WiFi connection to reach a result
 * returns the status reached or disconnect if STA is off
 * @return wl_status_t
 */
uint8_t ESP31BWiFiSTAClass::waitForConnectResult() {
    //1 and 3 have STA enabled
    if((wifi_get_opmode() & 1) == 0) {
        return WL_DISCONNECTED;
    }
    while(status() == WL_DISCONNECTED) {
        delay(100);
    }
    return status();
}

/**
 * Get the station interface IP address.
 * @return IPAddress station IP
 */
IPAddress ESP31BWiFiSTAClass::localIP() {
    struct ip_info ip;
    wifi_get_ip_info(STATION_IF, &ip);
    return IPAddress(ip.ip.addr);
}


/**
 * Get the station interface MAC address.
 * @param mac   pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
 * @return      pointer to uint8_t *
 */
uint8_t* ESP31BWiFiSTAClass::macAddress(uint8_t* mac) {
    wifi_get_macaddr(STATION_IF, mac);
    return mac;
}

/**
 * Get the station interface MAC address.
 * @return String mac
 */
String ESP31BWiFiSTAClass::macAddress(void) {
    uint8_t mac[6];
    char macStr[18] = { 0 };
    wifi_get_macaddr(STATION_IF, mac);

    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(macStr);
}

/**
 * Get the interface subnet mask address.
 * @return IPAddress subnetMask
 */
IPAddress ESP31BWiFiSTAClass::subnetMask() {
    struct ip_info ip;
    wifi_get_ip_info(STATION_IF, &ip);
    return IPAddress(ip.netmask.addr);
}

/**
 * Get the gateway ip address.
 * @return IPAddress gatewayIP
 */
IPAddress ESP31BWiFiSTAClass::gatewayIP() {
    struct ip_info ip;
    wifi_get_ip_info(STATION_IF, &ip);
    return IPAddress(ip.gw.addr);
}

/**
 * Get the DNS ip address.
 * @param dns_no
 * @return IPAddress DNS Server IP
 */
IPAddress ESP31BWiFiSTAClass::dnsIP(uint8_t dns_no) {
    ip_addr_t dns_ip = dns_getserver(dns_no);
    return IPAddress(dns_ip.addr);
}

/**
 * Return Connection status.
 * @return one of the value defined in wl_status_t
 *
 */
wl_status_t ESP31BWiFiSTAClass::status() {
    STATION_STATUS status = wifi_station_get_connect_status();

    switch(status) {
        case STATION_GOT_IP:
            return WL_CONNECTED;
        case STATION_NO_AP_FOUND:
            return WL_NO_SSID_AVAIL;
        case STATION_CONNECT_FAIL:
        case STATION_WRONG_PASSWORD:
            return WL_CONNECT_FAILED;
        case STATION_IDLE:
            return WL_IDLE_STATUS;
        default:
            return WL_DISCONNECTED;
    }
}

/**
 * Return the current SSID associated with the network
 * @return SSID
 */
String ESP31BWiFiSTAClass::SSID() const {
    struct station_config conf;
    wifi_station_get_config(&conf);
    return String(reinterpret_cast<char*>(conf.ssid));
}

/**
 * Return the current pre shared key associated with the network
 * @return  psk string
 */
String ESP31BWiFiSTAClass::psk() const {
    struct station_config conf;
    wifi_station_get_config(&conf);
    return String(reinterpret_cast<char*>(conf.password));
}

/**
 * Return the current bssid / mac associated with the network if configured
 * @return bssid uint8_t *
 */
uint8_t* ESP31BWiFiSTAClass::BSSID(void) {
    static struct station_config conf;
    wifi_station_get_config(&conf);
    return reinterpret_cast<uint8_t*>(conf.bssid);
}

/**
 * Return the current bssid / mac associated with the network if configured
 * @return String bssid mac
 */
String ESP31BWiFiSTAClass::BSSIDstr(void) {
    struct station_config conf;
    char mac[18] = { 0 };
    wifi_station_get_config(&conf);
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", conf.bssid[0], conf.bssid[1], conf.bssid[2], conf.bssid[3], conf.bssid[4], conf.bssid[5]);
    return String(mac);
}

/**
 * Return the current network RSSI.
 * @return  RSSI value
 */
int32_t ESP31BWiFiSTAClass::RSSI(void) {
    return wifi_station_get_rssi();
}
