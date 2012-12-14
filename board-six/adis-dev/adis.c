/*! \file adis.c
 *
 */

#include "lpc23xx-spi.h"
#include "ringbuffer.h"
#include "printf-lpc.h"
#include "gfe2368-util.h"


#include "adis.h"

Ringbuffer                adis_spi_done_q;

spi_master_xact_data      adis_read_id_xact;
spi_master_xact_data      adis_read_smpl_prd_xact;

spi_master_xact_data      adis_read_brst_mode_xact;

adis_cache                adis_data_cache;

spi_ctl                   adis_spi_ctl;

/*! \brief Initialize IO and Queue for ADIS
 *
 */
void adis_init() {

	rb_initialize(&adis_spi_done_q);

	FIO_ENABLE;

	FIO_ADIS_RESET_OUTPUT;
	PINSEL_ADIS_RESET;
	PINMODE_ADIS_RESET_PULLUP;

	PINSEL_ADIS_EINT0_DIO1;
	PINMODE_ADIS_EINT0_DIO1_NOPULL;

	PINSEL_ADIS_EINT1_DIO2;
	PINMODE_ADIS_EINT1_DIO2_NOPULL;

	PINSEL_ADIS_DIO3;
	PINMODE_ADIS_DIO3_PULLUP;

	PINSEL_ADIS_DIO4;
	PINMODE_ADIS_DIO4_PULLUP;

    ADIS_RESET_HIGH;
}


/*! \brief Reset the ADIS
 *
 */
void adis_reset() {

	ADIS_RESET_LOW;

	util_wait_msecs(ADIS_RESET_MSECS);

	ADIS_RESET_HIGH;

	util_wait_msecs(ADIS_RESET_MSECS);
}

bool adis_read_cache(adis_regaddr adis_recent, uint16_t* adis_data) {

	adis_cache*       c;
	c               = &adis_data_cache;

	if((adis_recent== ADIS_PRODUCT_ID)) {
		adis_data[0] = ( (c->adis_prod_id.data_high << 8) |  c->adis_prod_id.data_low);
		return true;
	}
	if((adis_recent== ADIS_SMPL_PRD)) {
			adis_data[0] = ( (c->adis_sampl_per.data_high << 8) |  c->adis_sampl_per.data_low);
			return true;
		}
	return false;
}

void adis_get_data() {

	bool            ok            = false;

	adis_regaddr    adis_recent  = 0;

	uint16_t        adis_data[ADIS_MAX_DATA_BUFFER];

	ok = rb_get_elem((uint8_t *) &adis_recent, &adis_spi_done_q);
	if(ok) {
		ok = adis_read_cache(adis_recent, adis_data);
		if(ok) {
			switch(adis_recent) {

			case ADIS_PRODUCT_ID:
				printf_lpc(UART0, "ID 0x%x\r\n",      adis_data[0]) ;
				break;
			case ADIS_SMPL_PRD:
				printf_lpc(UART0, "SMPL_PER 0x%x\r\n",      adis_data[0]) ;
				break;
			default:
				break;

			}
		}
	}
}

void adis_process_done_q() {
	while(!rb_is_empty(&adis_spi_done_q)) {
		adis_get_data();
	}
}


void adis_read_cb(spi_master_xact_data* caller, spi_master_xact_data* spi_xact, void* data) {
	   // data is NULL in this cb.
		// if (data != NULL) {}

		uint16_t i             = 0;
	    adis_regaddr  regaddr  = 0;

	    // copy out read buffer.
	    for(i=0; i<spi_xact->read_numbytes; ++i) {
	        caller->readbuf[i] = spi_xact->readbuf[i];
	    }
	    // copy out write buffer.
	    for(i=0; i<spi_xact->write_numbytes; ++i) {
	        caller->writebuf[i] = spi_xact->writebuf[i];
	    }

	    // The register address is always the first byte of the write buffer.
	    regaddr = caller->writebuf[0];

	    switch(regaddr) {
	        case ADIS_PRODUCT_ID:
	        	adis_data_cache.adis_prod_id.data_high   = caller->readbuf[1];
	        	adis_data_cache.adis_prod_id.data_low    = caller->readbuf[2];

	        	break;
	        case ADIS_SMPL_PRD:
	        	 adis_data_cache.adis_sampl_per.data_high  = caller->readbuf[1];
	        	 adis_data_cache.adis_sampl_per.data_low   = caller->readbuf[2];
	           	break;
	        default:
	            break;
	    }
	    if(!rb_is_full(&adis_spi_done_q)) {
	        rb_put_elem((char) regaddr, &adis_spi_done_q);
	    }       // now check queue not empty in main loop to see if there is fresh data.

	//printf_lpc(UART0, "%s: Called\n", __func__);

}

/*! \brief Create a read address
 *
 * @param s
 * @return  formatted read address for adis
 */
static adis_regaddr adis_create_read_addr(adis_regaddr s) {
	return (s & 0b01111111);
}


void adis_read_brst_mode(SPI_XACT_FnCallback cb) {

	spi_init_master_xact_data(&adis_read_brst_mode_xact);

	adis_read_brst_mode_xact.spi_cpha_val    = SPI_SCK_SECOND_CLK;
	adis_read_brst_mode_xact.spi_cpol_val    = SPI_SCK_ACTIVE_HIGH;
	adis_read_brst_mode_xact.spi_lsbf_val    = SPI_DATA_MSB_FIRST;

	adis_read_brst_mode_xact.writebuf[0]     = adis_create_read_addr(ADIS_GLOB_CMD);
	adis_read_brst_mode_xact.writebuf[1]     = 0x0;
	adis_read_brst_mode_xact.write_numbytes  = 2;
	adis_read_brst_mode_xact.read_numbytes   = 38;
	adis_read_brst_mode_xact.dummy_value     = 0x7f;

	// Start the transaction
	start_spi_master_xact_intr(&adis_read_brst_mode_xact, cb) ;

}


void adis_read_smpl_prd() {

	spi_init_master_xact_data(&adis_read_smpl_prd_xact);

	adis_read_smpl_prd_xact.spi_cpha_val    = SPI_SCK_SECOND_CLK;
	adis_read_smpl_prd_xact.spi_cpol_val    = SPI_SCK_ACTIVE_HIGH;
	adis_read_smpl_prd_xact.spi_lsbf_val    = SPI_DATA_MSB_FIRST;

	adis_read_smpl_prd_xact.writebuf[0]     = adis_create_read_addr(ADIS_SMPL_PRD);
	adis_read_smpl_prd_xact.writebuf[1]     = 0x0;
	adis_read_smpl_prd_xact.write_numbytes  = 2;
	adis_read_smpl_prd_xact.read_numbytes   = 3;
	adis_read_smpl_prd_xact.dummy_value     = 0x7f;

	// Start the transaction
	start_spi_master_xact_intr(&adis_read_smpl_prd_xact, adis_read_cb) ;

}

void adis_read_id() {

	spi_init_master_xact_data(&adis_read_id_xact);

	adis_read_id_xact.spi_cpha_val    = SPI_SCK_SECOND_CLK;
	adis_read_id_xact.spi_cpol_val    = SPI_SCK_ACTIVE_HIGH;
	adis_read_id_xact.spi_lsbf_val    = SPI_DATA_MSB_FIRST;

	adis_read_id_xact.writebuf[0]     = adis_create_read_addr(ADIS_PRODUCT_ID);
	adis_read_id_xact.writebuf[1]     = 0x0;
	adis_read_id_xact.write_numbytes  = 2;
	adis_read_id_xact.read_numbytes   = 3;
	adis_read_id_xact.dummy_value     = 0x7f;

	// Start the transaction
	start_spi_master_xact_intr(&adis_read_id_xact, adis_read_cb) ;
}

