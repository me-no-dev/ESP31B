/*
 ESP31BWiFiGeneric.cpp - WiFi library for ESP31B

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

extern "C" {
#include "c_types.h"
#include "esp_common.h"

#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/dns.h"
}

#include "WiFiClient.h"
#include "WiFiUdp.h"

#undef min
#undef max
#include <vector>

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------- Generic WiFi function -----------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

// arduino dont like std::vectors move static here
static std::vector<WiFiEventCbList_t> cbEventList;

bool ESP31BWiFiGenericClass::_persistent = true;
WiFiMode_t ESP31BWiFiGenericClass::_forceSleepLastMode = WIFI_OFF;

ESP31BWiFiGenericClass::ESP31BWiFiGenericClass()  {
    wifi_set_event_handler_cb((wifi_event_handler_cb_t) &ESP31BWiFiGenericClass::_eventCallback);
}

/**
 * set callback function
 * @param cbEvent WiFiEventCb
 * @param event optional filter (WIFI_EVENT_MAX is all events)
 */
void ESP31BWiFiGenericClass::onEvent(WiFiEventCb cbEvent, WiFiEvent_t event) {
    if(!cbEvent) {
        return;
    }
    WiFiEventCbList_t newEventHandler;
    newEventHandler.cb = cbEvent;
    newEventHandler.event = event;
    cbEventList.push_back(newEventHandler);
}

/**
 * removes a callback form event handler
 * @param cbEvent WiFiEventCb
 * @param event optional filter (WIFI_EVENT_MAX is all events)
 */
void ESP31BWiFiGenericClass::removeEvent(WiFiEventCb cbEvent, WiFiEvent_t event) {
    if(!cbEvent) {
        return;
    }

    for(uint32_t i = 0; i < cbEventList.size(); i++) {
        WiFiEventCbList_t entry = cbEventList[i];
        if(entry.cb == cbEvent && entry.event == event) {
            cbEventList.erase(cbEventList.begin() + i);
        }
    }
}

/**
 * callback for WiFi events
 * @param arg
 */
void ESP31BWiFiGenericClass::_eventCallback(void* arg) {
    System_Event_t* event = reinterpret_cast<System_Event_t*>(arg);
    DEBUGV("wifi evt: %d\n", event->event);

    if(event->event_id == EVENT_STAMODE_DISCONNECTED) {
        DEBUGV("STA disconnect: %d\n", event->event_info.disconnected.reason);
        WiFiClient::stopAll();
    }

    for(uint32_t i = 0; i < cbEventList.size(); i++) {
        WiFiEventCbList_t entry = cbEventList[i];
        if(entry.cb) {
            if(entry.event == (WiFiEvent_t) event->event_id || entry.event == WIFI_EVENT_MAX) {
                entry.cb((WiFiEvent_t) event->event_id);
            }
        }
    }
}

/**
 * Return the current channel associated with the network
 * @return channel (1-13)
 */
int32_t ESP31BWiFiGenericClass::channel(void) {
    return wifi_get_channel();
}


/**
 * store WiFi config in SDK flash area
 * @param persistent
 */
void ESP31BWiFiGenericClass::persistent(bool persistent) {
    _persistent = persistent;
}


/**
 * set new mode
 * @param m WiFiMode_t
 */
bool ESP31BWiFiGenericClass::mode(WiFiMode_t m) {
    if(wifi_get_opmode() == (uint8) m) {
        return true;
    }

    bool ret = false;

    if(_persistent) {
        ret = wifi_set_opmode((WIFI_MODE)m);
    } else {
        ret = wifi_set_opmode_current((WIFI_MODE)m);
    }

    return ret;
}

/**
 * get WiFi mode
 * @return WiFiMode
 */
WiFiMode_t ESP31BWiFiGenericClass::getMode() {
    return (WiFiMode_t) wifi_get_opmode();
}

/**
 * control STA mode
 * @param enable bool
 * @return ok
 */
bool ESP31BWiFiGenericClass::enableSTA(bool enable) {

    WiFiMode_t currentMode = getMode();
    bool isEnabled = ((currentMode & WIFI_STA) != 0);

    if(isEnabled != enable) {
        if(enable) {
            return mode((WiFiMode_t)(currentMode | WIFI_STA));
        } else {
            return mode((WiFiMode_t)(currentMode & (~WIFI_STA)));
        }
    } else {
        return true;
    }
}

/**
 * control AP mode
 * @param enable bool
 * @return ok
 */
bool ESP31BWiFiGenericClass::enableAP(bool enable){

    WiFiMode_t currentMode = getMode();
    bool isEnabled = ((currentMode & WIFI_AP) != 0);

    if(isEnabled != enable) {
        if(enable) {
            return mode((WiFiMode_t)(currentMode | WIFI_AP));
        } else {
            return mode((WiFiMode_t)(currentMode & (~WIFI_AP)));
        }
    } else {
        return true;
    }
}


// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------ Generic Network function ---------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

void wifi_dns_found_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg);

/**
 * Resolve the given hostname to an IP address.
 * @param aHostname     Name to be resolved
 * @param aResult       IPAddress structure to store the returned IP address
 * @return 1 if aIPAddrString was successfully converted to an IP address,
 *          else error code
 */
static bool _dns_busy = false;

int ESP31BWiFiGenericClass::hostByName(const char* aHostname, IPAddress& aResult) {
    ip_addr_t addr;
    aResult = static_cast<uint32_t>(0);
    err_t err = dns_gethostbyname(aHostname, &addr, &wifi_dns_found_callback, &aResult);
    _dns_busy = err == ERR_INPROGRESS;
    while(_dns_busy) delayMicroseconds(1);
    if(err == ERR_INPROGRESS && aResult){
      //found by search
    } else if(err == ERR_OK && addr.u_addr.ip4.addr) {
      aResult = addr.u_addr.ip4.addr;
    } else {
      return 0;
    }
    return 1;
}

/**
 * DNS callback
 * @param name
 * @param ipaddr
 * @param callback_arg
 */
void wifi_dns_found_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    if(ipaddr) {
        (*reinterpret_cast<IPAddress*>(callback_arg)) = ipaddr->u_addr.ip4.addr;
    }
    _dns_busy = false;
}


