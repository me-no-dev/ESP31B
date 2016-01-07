/*
 ESP31BWiFiScan.cpp - WiFi library for ESP31B

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
#include "ESP31BWiFiScan.h"

extern "C" {
#include "c_types.h"
#include "esp_common.h"
}
// -----------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------- Private functions ------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------




// -----------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------- scan function ---------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

bool ESP31BWiFiScanClass::_scanAsync = false;
bool ESP31BWiFiScanClass::_scanStarted = false;
bool ESP31BWiFiScanClass::_scanComplete = false;

size_t ESP31BWiFiScanClass::_scanCount = 0;
void* ESP31BWiFiScanClass::_scanResult = 0;

/**
 * Start scan WiFi networks available
 * @param async         run in async mode
 * @param show_hidden   show hidden networks
 * @return Number of discovered networks
 */
int8_t ESP31BWiFiScanClass::scanNetworks(bool async, bool show_hidden) {
    if(ESP31BWiFiScanClass::_scanStarted) {
        return WIFI_SCAN_RUNNING;
    }

    ESP31BWiFiScanClass::_scanAsync = async;

    WiFi.enableSTA(true);

    int status = wifi_station_get_connect_status();
    if(status != STATION_GOT_IP && status != STATION_IDLE) {
        WiFi.disconnect(false);
    }

    scanDelete();

    struct scan_config config;
    config.ssid = 0;
    config.bssid = 0;
    config.channel = 0;
    config.show_hidden = show_hidden;
    if(wifi_station_scan(&config, reinterpret_cast<scan_done_cb_t>(&ESP31BWiFiScanClass::_scanDone))) {
        ESP31BWiFiScanClass::_scanComplete = false;
        ESP31BWiFiScanClass::_scanStarted = true;

        if(ESP31BWiFiScanClass::_scanAsync) {
            delay(0); // time for the OS to trigger the scan
            return WIFI_SCAN_RUNNING;
        }
        return ESP31BWiFiScanClass::_scanCount;
    } else {
        return WIFI_SCAN_FAILED;
    }

}


/**
 * called to get the scan state in Async mode
 * @return scan result or status
 *          -1 if scan not fin
 *          -2 if scan not triggered
 */
int8_t ESP31BWiFiScanClass::scanComplete() {

    if(_scanStarted) {
        return WIFI_SCAN_RUNNING;
    }

    if(_scanComplete) {
        return ESP31BWiFiScanClass::_scanCount;
    }

    return WIFI_SCAN_FAILED;
}

/**
 * delete last scan result from RAM
 */
void ESP31BWiFiScanClass::scanDelete() {
    if(ESP31BWiFiScanClass::_scanResult) {
        delete[] reinterpret_cast<bss_info*>(ESP31BWiFiScanClass::_scanResult);
        ESP31BWiFiScanClass::_scanResult = 0;
        ESP31BWiFiScanClass::_scanCount = 0;
    }
    _scanComplete = false;
}


/**
 * loads all infos from a scanned wifi in to the ptr parameters
 * @param networkItem uint8_t
 * @param ssid  const char**
 * @param encryptionType uint8_t *
 * @param RSSI int32_t *
 * @param BSSID uint8_t **
 * @param channel int32_t *
 * @param isHidden bool *
 * @return (true if ok)
 */
bool ESP31BWiFiScanClass::getNetworkInfo(uint8_t i, String &ssid, uint8_t &encType, int32_t &rssi, uint8_t* &bssid, int32_t &channel, bool &isHidden) {
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if(!it) {
        return false;
    }

    ssid = (const char*) it->ssid;
    encType = encryptionType(i);
    rssi = it->rssi;
    bssid = it->bssid; // move ptr
    channel = it->channel;
    isHidden = (it->is_hidden != 0);

    return true;
}


/**
 * Return the SSID discovered during the network scan.
 * @param i     specify from which network item want to get the information
 * @return       ssid string of the specified item on the networks scanned list
 */
String ESP31BWiFiScanClass::SSID(uint8_t i) {
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if(!it) {
        return "";
    }

    return String(reinterpret_cast<const char*>(it->ssid));
}


/**
 * Return the encryption type of the networks discovered during the scanNetworks
 * @param i specify from which network item want to get the information
 * @return  encryption type (enum wl_enc_type) of the specified item on the networks scanned list
 */
uint8_t ESP31BWiFiScanClass::encryptionType(uint8_t i) {
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if(!it) {
        return -1;
    }

    switch(it->authmode) {
        case AUTH_OPEN:
            return ENC_TYPE_NONE;
        case AUTH_WEP:
            return ENC_TYPE_WEP;
        case AUTH_WPA_PSK:
            return ENC_TYPE_TKIP;
        case AUTH_WPA2_PSK:
            return ENC_TYPE_CCMP;
        case AUTH_WPA_WPA2_PSK:
            return ENC_TYPE_AUTO;
        default:
            return -1;
    }
}

/**
 * Return the RSSI of the networks discovered during the scanNetworks
 * @param i specify from which network item want to get the information
 * @return  signed value of RSSI of the specified item on the networks scanned list
 */
int32_t ESP31BWiFiScanClass::RSSI(uint8_t i) {
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if(!it) {
        return 0;
    }
    return it->rssi;
}


/**
 * return MAC / BSSID of scanned wifi
 * @param i specify from which network item want to get the information
 * @return uint8_t * MAC / BSSID of scanned wifi
 */
uint8_t * ESP31BWiFiScanClass::BSSID(uint8_t i) {
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if(!it) {
        return 0;
    }
    return it->bssid;
}

/**
 * return MAC / BSSID of scanned wifi
 * @param i specify from which network item want to get the information
 * @return String MAC / BSSID of scanned wifi
 */
String ESP31BWiFiScanClass::BSSIDstr(uint8_t i) {
    char mac[18] = { 0 };
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if(!it) {
        return String("");
    }
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", it->bssid[0], it->bssid[1], it->bssid[2], it->bssid[3], it->bssid[4], it->bssid[5]);
    return String(mac);
}

int32_t ESP31BWiFiScanClass::channel(uint8_t i) {
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if(!it) {
        return 0;
    }
    return it->channel;
}

/**
 * return if the scanned wifi is Hidden (no SSID)
 * @param networkItem specify from which network item want to get the information
 * @return bool (true == hidden)
 */
bool ESP31BWiFiScanClass::isHidden(uint8_t i) {
    struct bss_info* it = reinterpret_cast<struct bss_info*>(_getScanInfoByIndex(i));
    if(!it) {
        return false;
    }
    return (it->is_hidden != 0);
}

/**
 * private
 * scan callback
 * @param result  void *arg
 * @param status STATUS
 */
void ESP31BWiFiScanClass::_scanDone(void* result, int status) {
    if(status != OK) {
        ESP31BWiFiScanClass::_scanCount = 0;
        ESP31BWiFiScanClass::_scanResult = 0;
    } else {

        int i = 0;
        bss_info_head_t* head = reinterpret_cast<bss_info_head_t*>(result);

        for(bss_info* it = STAILQ_FIRST(head); it; it = STAILQ_NEXT(it, next), ++i)
            ;
        ESP31BWiFiScanClass::_scanCount = i;
        if(i == 0) {
            ESP31BWiFiScanClass::_scanResult = 0;
        } else {
            bss_info* copied_info = new bss_info[i];
            i = 0;
            for(bss_info* it = STAILQ_FIRST(head); it; it = STAILQ_NEXT(it, next), ++i) {
                memcpy(copied_info + i, it, sizeof(bss_info));
            }

            ESP31BWiFiScanClass::_scanResult = copied_info;
        }

    }

    ESP31BWiFiScanClass::_scanStarted = false;
    ESP31BWiFiScanClass::_scanComplete = true;
}

/**
 *
 * @param i specify from which network item want to get the information
 * @return bss_info *
 */
void * ESP31BWiFiScanClass::_getScanInfoByIndex(int i) {
    if(!ESP31BWiFiScanClass::_scanResult || (size_t) i > ESP31BWiFiScanClass::_scanCount) {
        return 0;
    }
    return reinterpret_cast<bss_info*>(ESP31BWiFiScanClass::_scanResult) + i;
}

