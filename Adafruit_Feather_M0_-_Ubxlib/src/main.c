#include <string.h>
#include <stdio.h>
#include <time.h>

#include "ubxlib.h"

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>



// The devicetree node identifier for the "led0" alias.
#define GPS_NODE DT_ALIAS(gps)


#if DT_NODE_HAS_STATUS(GPS_NODE, okay)
#define GPS	DT_GPIO_LABEL(GPS_NODE, gpios)
#define PIN	    DT_GPIO_PIN(GPS_NODE, gpios)
#define FLAGS	DT_GPIO_FLAGS(LED0GPS_NODE_NODE, gpios)
#else
// A build error here means your board isn't set up to blink an LED.
#error "Unsupported board: led0 devicetree alias is not defined"
#define GPS	""
#define PIN	0
#define FLAGS	0
#endif


// Change all -1 values below to appropriate pin and settings values
// appropriate for your module connection.
static const uDeviceCfg_t gDeviceCfg = {
    .deviceType = U_DEVICE_TYPE_GNSS,
    .deviceCfg = {
        .cfgGnss = {
            .moduleType = U_GNSS_MODULE_TYPE_M8,
            .pinEnablePower = 18,
            .pinDataReady = -1,
            .i2cAddress = 0x42
        },
    },
    .transportType = U_DEVICE_TRANSPORT_TYPE_I2C,
    .transportCfg = {
        .cfgI2c = {
            .i2c = 3,
            .pinSda = -1,
            .pinScl = -1,
            .clockHertz = -1
        },
    },
};

static const uNetworkCfgGnss_t gNetworkCfg = {
    .type = U_NETWORK_TYPE_GNSS,
    .moduleType = U_GNSS_MODULE_TYPE_M8,
    .devicePinPwr = 18,
    .devicePinDataReady = -1
};




void main()
{



    const struct device *dev;
	int ret;

	dev = device_get_binding(GPS);
	if (dev == NULL) {
		return;
	}

	ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
	if (ret < 0) {
		return;
	}

	gpio_pin_set(dev, PIN, true);


    // Remove the line below if you want the log printouts from ubxlib
    uPortLogOff();
    // Initiate ubxlib
    
    printf("errorCode %d\n", uPortInit());
    printf("errorCode %d\n", uPortI2cInit());
    printf("errorCode %d\n", uDeviceInit());

    // And the U-blox GNSS module
    int32_t errorCode;
    uDeviceHandle_t deviceHandle;
    printf("\nInitiating the module...\n");
    errorCode = uDeviceOpen(&gDeviceCfg, &deviceHandle);

    if (errorCode == 0) {
        // Bring up the GNSS
        errorCode = uNetworkInterfaceUp(deviceHandle, U_NETWORK_TYPE_GNSS, &gNetworkCfg);
        if (errorCode == 0) {
            printf("Waiting for position.");
            uLocation_t location;
            int tries = 0;
            int64_t startTime = uPortGetTickTimeMs();
            do {
                printf(".");
                errorCode = uLocationGet(deviceHandle, U_LOCATION_TYPE_GNSS,
                                         NULL, NULL, &location, NULL);
            } while (errorCode == U_ERROR_COMMON_TIMEOUT && tries++ < 4);
            printf("\nWaited: %lld s\n", (uPortGetTickTimeMs() - startTime) / 1000);
            if (errorCode == 0) {
                printf("Position: https://maps.google.com/?q=%d.%07d,%d.%07d\n",
                       location.latitudeX1e7 / 10000000, location.latitudeX1e7 % 10000000,
                       location.longitudeX1e7 / 10000000, location.longitudeX1e7 % 10000000);
                printf("Radius: %d m\n", location.radiusMillimetres / 1000);
                struct tm *t = gmtime(&location.timeUtc);
                printf("UTC Time: %4d-%02d-%02d %02d:%02d:%02d\n",
                       t->tm_year + 1900, t->tm_mon, t->tm_mday,
                       t->tm_hour, t->tm_min, t->tm_sec);
            } else if (errorCode == U_ERROR_COMMON_TIMEOUT) {
                printf("* Timeout\n");
            } else {
                printf("* Failed to get position: %d\n", errorCode);
            }
            uNetworkInterfaceDown(deviceHandle, U_NETWORK_TYPE_GNSS);
        } else {
            printf("* Failed to bring up the GNSS: %d", errorCode);
        }
        uDeviceClose(deviceHandle, true);
    } else {
        printf("* Failed to initiate the module: %d", errorCode);
    }

    printf("\n== All done ==\n");

}


