/*
 ESP31BWiFi.cpp - WiFi library for ESP31B

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

extern "C" {
#include "c_types.h"
#include "esp_common.h"
#include "smartconfig.h"
#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/dns.h"
}


// -----------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------- Debug ------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------


/**
 * Output WiFi settings to an object derived from Print interface (like Serial).
 * @param p Print interface
 */
void ESP31BWiFiClass::printDiag(Print& p) {
    const char* modes[] = { "NULL", "STA", "AP", "STA+AP" };
    p.print("Mode: ");
    p.println(modes[wifi_get_opmode()]);

    p.print("Channel: ");
    p.println(wifi_get_channel());

    p.print("AP id: ");
    p.println(wifi_station_get_current_ap_id());

    p.print("Status: ");
    p.println(wifi_station_get_connect_status());

    p.print("Auto connect: ");
    p.println(wifi_station_get_auto_connect());

    struct station_config conf;
    wifi_station_get_config(&conf);

    const char* ssid = reinterpret_cast<const char*>(conf.ssid);
    p.print("SSID (");
    p.print(strlen(ssid));
    p.print("): ");
    p.println(ssid);

    const char* passphrase = reinterpret_cast<const char*>(conf.password);
    p.print("Passphrase (");
    p.print(strlen(passphrase));
    p.print("): ");
    p.println(passphrase);

    p.print("BSSID set: ");
    p.println(conf.bssid_set);

}

ESP31BWiFiClass WiFi;
