/*
 ESP31BWiFi.h - ESP31B Wifi support.
 Based on WiFi.h from Ardiono WiFi shield library.
 Copyright (c) 2011-2014 Arduino.  All right reserved.
 Modified by Ivan Grokhotkov, December 2014

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
 */

#ifndef WiFi_h
#define WiFi_h

#include <stdint.h>

extern "C" {
#include "include/wl_definitions.h"
}

#include "IPAddress.h"

#include "ESP31BWiFiType.h"
#include "ESP31BWiFiSTA.h"
#include "ESP31BWiFiAP.h"
#include "ESP31BWiFiScan.h"
#include "ESP31BWiFiGeneric.h"

#include "WiFiClient.h"
#include "WiFiServer.h"

class ESP31BWiFiClass : public ESP31BWiFiGenericClass, public ESP31BWiFiSTAClass, public ESP31BWiFiScanClass, public ESP31BWiFiAPClass {
    public:

        // workaround same function name with different signature
        using ESP31BWiFiGenericClass::channel;

        using ESP31BWiFiSTAClass::SSID;
        using ESP31BWiFiSTAClass::RSSI;
        using ESP31BWiFiSTAClass::BSSID;
        using ESP31BWiFiSTAClass::BSSIDstr;

        using ESP31BWiFiScanClass::SSID;
        using ESP31BWiFiScanClass::encryptionType;
        using ESP31BWiFiScanClass::RSSI;
        using ESP31BWiFiScanClass::BSSID;
        using ESP31BWiFiScanClass::BSSIDstr;
        using ESP31BWiFiScanClass::channel;
        using ESP31BWiFiScanClass::isHidden;

        // ----------------------------------------------------------------------------------------------
        // ------------------------------------------- Debug --------------------------------------------
        // ----------------------------------------------------------------------------------------------

    public:

        void printDiag(Print& dest);

        friend class WiFiClient;
        friend class WiFiServer;

};

extern ESP31BWiFiClass WiFi;

#endif
