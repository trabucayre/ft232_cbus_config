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

int main(int argc, char **argv)
{
	int ret, i;
	struct ftdi_context *ftdi;
	int vendor_id = 0x403, product_id = 0x6001;
	int iomode;

	if (argc < 3) {
		printf("%s -v vid -p pid\n", argv[0]);
		return EXIT_FAILURE;
	}

	for (i = 1; i < argc; i+=2) {
		if (argv[i][1] == 'v')
			sscanf(argv[i+1], "%x", &vendor_id);
		if (argv[i][1] == 'p')
			sscanf(argv[i+1], "%x", &product_id);
	}

	printf("vendor %x product %x\n", vendor_id, product_id);

	int decode_verbose = 1;

	if (product_id == 0x6015) // CBUSX
		iomode = CBUSX_IOMODE;
	else // CBUS
		iomode = CBUS_IOMODE;

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
	/* example changes */
	/*ret = ftdi_set_eeprom_value(ftdi, VENDOR_ID, 0x403);
	ret = ftdi_set_eeprom_value(ftdi, PRODUCT_ID, 0x6001);*/
	/*ret = ftdi_set_eeprom_value(ftdi, CHANNEL_A_TYPE, 1);
	ret = ftdi_set_eeprom_value(ftdi, CHANNEL_A_DRIVER, 0);
	ret = ftdi_set_eeprom_value(ftdi, SELF_POWERED, 0);
	ret = ftdi_set_eeprom_value(ftdi, SELF_POWERED, 0);
	ret = ftdi_set_eeprom_value(ftdi, MAX_POWER, 90);
	ret = ftdi_set_eeprom_value(ftdi, HIGH_CURRENT_A, 1);*/
	ret = ftdi_set_eeprom_value(ftdi, CBUS_FUNCTION_0, iomode);
	if (ret != 0) {
		printf("FTDI set CBUS0 failed: %s\n",
		       ftdi_get_error_string(ftdi));
		goto cleanup;
	}

	ret = ftdi_set_eeprom_value(ftdi, CBUS_FUNCTION_1, iomode);
	if (ret != 0) {
		printf("FTDI set CBUS1 failed: %s\n",
		       ftdi_get_error_string(ftdi));
		goto cleanup;
	}

	ret = ftdi_set_eeprom_value(ftdi, CBUS_FUNCTION_2, iomode);
	if (ret != 0) {
		printf("FTDI set CBUS2 failed: %s\n",
		       ftdi_get_error_string(ftdi));
		goto cleanup;
	}

	ret = ftdi_set_eeprom_value(ftdi, CBUS_FUNCTION_3, iomode);
	if (ret != 0) {
		printf("FTDI set CBUS3 failed: %s\n",
		       ftdi_get_error_string(ftdi));
		goto cleanup;
	}

	if (product_id != 0x6015) {
		ret = ftdi_set_eeprom_value(ftdi, CBUS_FUNCTION_4, CBUS_CLK6);
		if (ret != 0) {
			printf("FTDI set CBUS3 failed: %s\n",
			       ftdi_get_error_string(ftdi));
			goto cleanup;
		}
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
