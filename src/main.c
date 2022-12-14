/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */




#define DT_DRV_COMPAT st_iis2dh


// - SECTION - includes

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>

// - DEV 1028 - studying and debugging Zephyr device tree macro:
#if DT_ANY_INST_ON_BUS_STATUS_OKAY(i2c)
#warning "- DEV 1028 - app side macro test for I2C device instances returns 1, \"one or more found\""
#else
#warning "- DEV 1028 - app side macro test for I2C device instances returns 0, \"none found\""
#endif

#include <iis2dh.h>



// - SECTION - defines

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

#define THREAD_ID__THREAD_MAIN "main"



// - SECTION - file scoped

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);



#define ROUTINE_OK 0

#define DT_DRV_COMPAT st_iis2dh

const struct device *sensor = DEVICE_DT_GET_ANY(st_iis2dh);



//----------------------------------------------------------------------
// Note, following two code snippets show driver change from Zephyr
//   release 3.1.0 to Zephyr 3.2.0:
//
//    struct iis2dh_data *data_struc_ptr = (struct iis2dh_data *)dev->data;
//        data_struc_ptr->bus,   // data_struc_ptr->i2c_dev,  . . . suits Zephyr 3.1.0 STMicro driver API
//
//    struct iis2dh_device_config *config_struct_ptr = (struct iis2dh_device_config *)dev->config;
//        config_struct_ptr->i2c.bus,                         . . . suits Zephyr 3.2.0 STMicro driver API
//----------------------------------------------------------------------



#if 1
static uint32_t read_iis2dh_whoami_register(const struct device *dev, struct sensor_value value)
{
#define RNAME_WHOAMI_QUERY "read_whoami"

    int rstatus = ROUTINE_OK;
    uint8_t cmd[] = { 0x0F };
//    struct iis2dh_data *data_struc_ptr = (struct iis2dh_data *)dev->data;
    struct iis2dh_device_config *config_struct_ptr = (struct iis2dh_device_config *)dev->config;
    uint8_t scratch_pad_byte = 0;

#if 1
#if DT_ANY_INST_ON_BUS_STATUS_OKAY(i2c)
#warning "- DEV 1027 - DT_ANY_INST_ON_BUS_STATUS_OKAY(i2c) returns 'true', one or more devices found 'okay' on I2C bus"
    rstatus = i2c_write_read(
#if 0
                             config_struct_ptr->i2c.bus,
                             DT_INST_REG_ADDR(0),
                             cmd, sizeof(cmd),
                             &scratch_pad_byte, sizeof(scratch_pad_byte));
#else
                             config_struct_ptr->i2c.bus,
                             0x18,
                             cmd,
                             1,
                             &scratch_pad_byte,
                             1
#endif
                            );
    value.val1 = scratch_pad_byte;

    printk("- %s - Zephyr I2C API write_read routine returns %d,\n",
      "whoami_query_routine", rstatus);
    printk("- %s - read back 'whoami' value of 0x%02x from iis2dh sensor,\n\n",
      "whoami_query_routine", value.val1);



#else
    printk("- %s - WARNING:  iis2dh driver did not compile with I2C bus awareness!\n",
      "whoami_query_routine");

#endif // check for I2C device instance on bus
#endif // 0, or 1

//    printk("- %s - DEV 1027 - at compile time DT_ANY_INST_ON_BUS_STATUS_OKAY(i2c) returns '%u'\n",
//      RNAME_WHOAMI_QUERY, DT_ANY_INST_ON_BUS_STATUS_OKAY(i2c));
//    printk("- %s - DEV 1027 - during build of sample app sources.\n", RNAME_WHOAMI_QUERY);

    return rstatus;
}
#endif



void main(void)
{
    int ret;
    struct sensor_value value;
    uint32_t rstatus = 0;


// --- BLOCK IIS2DH DRIVER API TEST BEGIN ---

    if ( sensor == NULL )
        { printk("- %s - No iis2dh device found!\n", THREAD_ID__THREAD_MAIN); }
    else
        { printk("- %s - Success finding iis2dh device,\n", THREAD_ID__THREAD_MAIN); }

    if (!device_is_ready(sensor)) {
        printk("- %s - NOTICE!  Device %s is not ready\n", THREAD_ID__THREAD_MAIN, sensor->name);
#if 0
        printk("- %s - Returning from this thread's entry point . . .\n\n\n", THREAD_ID__THREAD_MAIN);
        return;
#else
        printk("- %s - continuing on to blinky sample app code: . . .\n", THREAD_ID__THREAD_MAIN);
#endif
    }
    else
    {
        printk("- %s - Call to device_is_ready() says %s sensor is ready,\n",
          THREAD_ID__THREAD_MAIN, sensor->name);
    }

// --- BLOCK IIS2DH DRIVER API TEST END ---




    if (!device_is_ready(led.port)) {
        printk("- %s - LED device reported not ready, returning early!\n",
          THREAD_ID__THREAD_MAIN);
        return;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        printk("- %s - GPIO pin configuration failed, returning early!\n",
          THREAD_ID__THREAD_MAIN);
        return;
    }

    while (1) {
        ret = gpio_pin_toggle_dt(&led);
        if (ret < 0) {
            printk("- %s - call to toggle GPIO pin failed, leaving main loop and returning!\n",
              THREAD_ID__THREAD_MAIN);
            return;
        }
        k_msleep(SLEEP_TIME_MS);

        printk("- %s - calling routine to query IIS2DH sensor for 'whoami' value . . .\n",
          THREAD_ID__THREAD_MAIN);
        rstatus = read_iis2dh_whoami_register(sensor, value);
    }

}




