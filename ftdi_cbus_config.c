/* ftdi_cbus_config.c
 * tool used to change FT232 CBUS pins configuration
 * 
 * (C) 2015 by Gwenhael Goavec-Merou <gwen@trabucayre.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
*/

#include <stdio.h>
#include <ftdi.h>
#include <libusb.h>
#include <stdlib.h>

int main(void)
{
	int ret;
	struct ftdi_context *ftdi;
	int vendor_id = 0x403, product_id = 0x6001;
	int decode_verbose = 1;

	if ((ftdi = ftdi_new()) == NULL) {
		printf("FTDI context allocation failed\n");
		return EXIT_FAILURE;
	}

	if (ftdi_usb_open(ftdi, vendor_id, product_id) != 0) {
		printf("FTDI open failed: %s\n", ftdi_get_error_string(ftdi));
		goto cleanup;
	}

	ret = ftdi_read_eeprom(ftdi);
	if (ret != 0) {
		printf("FTDI read eeprom failed: %s\n",
		       ftdi_get_error_string(ftdi));
		goto cleanup;
	}

	ret = ftdi_eeprom_decode(ftdi, decode_verbose);
	if (ret != 0) {
		printf("FTDI decode eeprom failed: %s\n",
		       ftdi_get_error_string(ftdi));
		goto cleanup;
	}

	/* some change */
	ret = ftdi_set_eeprom_value(ftdi, VENDOR_ID, 0x403);
	ret = ftdi_set_eeprom_value(ftdi, PRODUCT_ID, 0x6001);
	ret = ftdi_set_eeprom_value(ftdi, CBUS_FUNCTION_0, CBUS_IOMODE);
	if (ret != 0) {
		printf("FTDI set CBUS0 failed: %s\n",
		       ftdi_get_error_string(ftdi));
		goto cleanup;
	}

	ret = ftdi_set_eeprom_value(ftdi, CBUS_FUNCTION_1, CBUS_IOMODE);
	if (ret != 0) {
		printf("FTDI set CBUS1 failed: %s\n",
		       ftdi_get_error_string(ftdi));
		goto cleanup;
	}

	ret = ftdi_set_eeprom_value(ftdi, CBUS_FUNCTION_2, CBUS_IOMODE);
	if (ret != 0) {
		printf("FTDI set CBUS2 failed: %s\n",
		       ftdi_get_error_string(ftdi));
		goto cleanup;
	}

	ret = ftdi_set_eeprom_value(ftdi, CBUS_FUNCTION_3, CBUS_IOMODE);
	if (ret != 0) {
		printf("FTDI set CBUS3 failed: %s\n",
		       ftdi_get_error_string(ftdi));
		goto cleanup;
	}

	/* generate new eeprom */
	ret = ftdi_eeprom_build(ftdi);
	if (ret < 0) {
		printf("FTDI eeprom_build: %d %s\n", ret,
		       ftdi_get_error_string(ftdi));
		goto cleanup;
	}

	if (decode_verbose)
		ftdi_eeprom_decode(ftdi, 1);

	/* flash */
	ret = ftdi_write_eeprom(ftdi);
	if (ret != 0) {
		printf("FTDI write eeprom: %d %s\n", ret,
		       ftdi_get_error_string(ftdi));
		goto cleanup;
	}

	libusb_reset_device(ftdi->usb_dev);
	printf("EEPROM updated\n");
cleanup:
	printf("FTDI close: %d\n", ftdi_usb_close(ftdi));

	ftdi_deinit(ftdi);
	ftdi_free(ftdi);

	return EXIT_SUCCESS;
}
