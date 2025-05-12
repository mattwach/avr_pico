/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "nRF24L01.h"
#include "rf24.h"
#include <avr/pgmspace.h>
#include <misc/pin.h>
#include <spi/spi.h>
#include <string.h>
#include <util/delay.h>

#define LOW 0
#define HIGH 1
#define rf24_min(X,Y) ((X)<(Y) ? (X) : (Y))
#define rf24_max(X,Y) ((X)>(Y) ? (X) : (Y))


void delay_us(uint32_t delay_us) {
  // _delay_us can only take a compile-time constant
  // thus we approximate it via power-of-ten loops
  // Note this will waste extra us...
  for (; delay_us > 1000; delay_us -= 1000) {
    _delay_us(1000);
  }
  for (; delay_us > 100; delay_us -= 100) {
    _delay_us(100);
  }
  for (; delay_us > 10; delay_us -= 10) {
    _delay_us(10);
  }
  for (; delay_us > 0; --delay_us) {
    _delay_us(1);
  }
}


void ce(const RF24* rf24, bool_t level) {
  //Allow for 3-pin use on ATTiny
  if (rf24->ce_pin == rf24->csn_pin) {
    return;
  }
  
  if (level) {
    set_pin(rf24->ce_pin);
  } else {
    clr_pin(rf24->ce_pin);
  }
}


static inline void begin_spi(const RF24* rf24) {
#if defined (RF24_TINY)
  if (rf24->ce_pin != rf24->csn_pin) {
	clr_pin(rf24->csn_pin);
  } else {
	PORTB &= ~(1<<PINB2);	// SCK->CSN LOW
	_delay_us(11);  // allow csn to settle
  }
#else
  clr_pin(rf24->csn_pin);
  _delay_us(5);
#endif
}


static inline void end_spi(const RF24* rf24) {
#if defined (RF24_TINY)
  if (rf24->ce_pin != rf24->csn_pin) {
	set_pin(rf24->csn_pin);
  } else {
	// ATTiny
	PORTB |= (1<<PINB2);  	// SCK->CSN HIGH
	_delay_us(100); // allow csn to settle.
  }
#else
  set_pin(rf24->csn_pin);
  _delay_us(5);
#endif
}


static uint8_t send_spi_cmd(const RF24* rf24, uint8_t cmd) {
  begin_spi(rf24);
  uint8_t status = spi_syncTransact(cmd);
  end_spi(rf24);
  return status;
}


static inline uint8_t get_status(const RF24* rf24) {
  return send_spi_cmd(rf24, RF24_NOP);
}


void toggle_features(const RF24* rf24) {
  begin_spi(rf24);
  spi_syncWrite(ACTIVATE);
  spi_syncWrite(0x73);
  end_spi(rf24);
}


uint8_t read_reg_buf(const RF24* rf24, uint8_t reg, uint8_t* buf, uint8_t len) {
  begin_spi(rf24);
  uint8_t status = spi_syncTransact(R_REGISTER | (REGISTER_MASK & reg));
  while (len--) {
    *buf++ = spi_syncTransact(0xFF);
  }

  end_spi(rf24);
  return status;
}


uint8_t read_reg(const RF24* rf24, uint8_t reg) {
  begin_spi(rf24);
  spi_syncWrite(R_REGISTER | (REGISTER_MASK & reg));
  uint8_t result = spi_syncTransact(0xFF);
  end_spi(rf24);
  return result;
}


uint8_t write_reg_buf(
	const RF24* rf24, uint8_t reg, const uint8_t* buf, uint8_t len) {
  begin_spi(rf24);
  uint8_t status = spi_syncTransact(W_REGISTER | (REGISTER_MASK & reg));
  while (len--) {
    spi_syncWrite(*buf++);
  }

  end_spi(rf24);
  return status;
}


uint8_t write_reg(const RF24* rf24, uint8_t reg, uint8_t value) {
  begin_spi(rf24);
  uint8_t status = spi_syncTransact(W_REGISTER | (REGISTER_MASK & reg));
  spi_syncWrite(value);
  end_spi(rf24);
  return status;
}


uint8_t write_payload(
	const RF24* rf24,
	const void* buf,
	uint8_t data_len,
	const uint8_t writeType) {
  const uint8_t* current = (const uint8_t*)buf;
  data_len = rf24_min(data_len, rf24->payload_size);
  uint8_t blank_len = rf24->dynamic_payloads_enabled ?
      0 : rf24->payload_size - data_len;

  begin_spi(rf24);
  uint8_t status = spi_syncTransact( writeType );
  while (data_len--) {
    spi_syncWrite(*current++);
  }

  while (blank_len--) {
    spi_syncWrite(0);
  }  

  end_spi(rf24);
  return status;
}


uint8_t read_payload(const RF24* rf24, void* buf, uint8_t data_len) {
  uint8_t status;
  uint8_t* current = (uint8_t*)buf;

  if (data_len > rf24->payload_size) {
    data_len = rf24->payload_size;
  }

  uint8_t blank_len =
      rf24->dynamic_payloads_enabled ?
          0 : rf24->payload_size - data_len;
  
  begin_spi(rf24);
  status = spi_syncTransact( R_RX_PAYLOAD );
  while ( data_len-- ) {
    *current++ = spi_syncTransact(0xFF);
  }

  while ( blank_len-- ) {
    spi_syncWrite(0xff);
  }

  end_spi(rf24);
  return status;
}


uint8_t rf24_flush_rx(const RF24* rf24) {
  return send_spi_cmd(rf24 , FLUSH_RX);
}


uint8_t rf24_flush_tx(const RF24* rf24) {
  return send_spi_cmd(rf24 , FLUSH_TX);
}

//#if !defined (MINIMAL)
//void rf24_print_status(const RF24* rf24, uint8_t status)
//{
//  printf_P(PSTR("STATUS\t\t = 0x%02x RX_DR=%x TX_DS=%x MAX_RT=%x RX_P_NO=%x TX_FULL=%x\r\n"),
//           status,
//           (status & _BV(RX_DR))?1:0,
//           (status & _BV(TX_DS))?1:0,
//           (status & _BV(MAX_RT))?1:0,
//           ((status >> RX_P_NO) & 0x07),
//           (status & _BV(TX_FULL))?1:0
//          );
//}
//
//
//void print_observe_tx(uint8_t value)
//{
//  printf_P(PSTR("OBSERVE_TX=%02x: POLS_CNT=%x ARC_CNT=%x\r\n"),
//           value,
//           (value >> PLOS_CNT) & 0x0F,
//           (value >> ARC_CNT) & 0x0F
//          );
//}
//
//
//void rf24_print_byte_register(const RF24* rf24, const char* name, uint8_t reg, uint8_t qty)
//{
//    printf_P(PSTR(PRIPSTR"\t ="),name);
//  while (qty--)
//    printf_P(PSTR(" 0x%02x"),read_reg(rf24, reg++));
//  printf_P(PSTR("\r\n"));
//}
//
//
//void rf24_print_address_register(const RF24* rf24, const char* name, uint8_t reg, uint8_t qty)
//{
//
//    printf_P(PSTR(PRIPSTR"\t ="),name);
//  while (qty--)
//  {
//    uint8_t buffer[rf24->addr_width];
//    read_reg(rf24, reg++,buffer,sizeof buffer);
//
//    printf_P(PSTR(" 0x"));
//    uint8_t* bufptr = buffer + sizeof buffer;
//    while( --bufptr >= buffer )
//      printf_P(PSTR("%02x"),*bufptr);
//  }
//
//  printf_P(PSTR("\r\n"));
//}
//#endif


void rf24_init(RF24* rf24, uint16_t _cepin, uint16_t _cspin) {
  rf24->ce_pin = _cepin;
  rf24->csn_pin = _cspin;
  rf24->p_variant = FALSE;
  rf24->payload_size = 32;
  rf24->dynamic_payloads_enabled = FALSE;
  rf24->addr_width = 5;
  rf24->pipe0_reading_address[0]=0;
}


void rf24_set_channel(const RF24* rf24, uint8_t channel) {
  const uint8_t max_channel = 125;
  write_reg(rf24, RF_CH, rf24_min(channel,max_channel));
}

void rf24_set_payload_size(RF24* rf24, uint8_t size)
{
    // payload size must be in range [1, 32]
    rf24->payload_size = rf24_max(1, rf24_min(32, size));

    // write static payload size setting for all pipes
    for (uint8_t i = 0; i < 6; ++i)
        write_reg(rf24, RX_PW_P0 + i, rf24->payload_size);
}

/****************************************************************************/

uint8_t rf24_get_payload_size(const RF24* rf24)
{
    return rf24->payload_size;
}

//uint8_t rf24_get_channel(const RF24* rf24)
//{
//  
//  return read_reg(rf24, RF_CH);
//}
//
//#if !defined (MINIMAL)
//
//static const char rf24_datarate_e_str_0[] PROGMEM = "1MBPS";
//static const char rf24_datarate_e_str_1[] PROGMEM = "2MBPS";
//static const char rf24_datarate_e_str_2[] PROGMEM = "250KBPS";
//static const char * const rf24_datarate_e_str_P[] PROGMEM = {
//  rf24_datarate_e_str_0,
//  rf24_datarate_e_str_1,
//  rf24_datarate_e_str_2,
//};
//static const char rf24_model_e_str_0[] PROGMEM = "nRF24L01";
//static const char rf24_model_e_str_1[] PROGMEM = "nRF24L01+";
//static const char * const rf24_model_e_str_P[] PROGMEM = {
//  rf24_model_e_str_0,
//  rf24_model_e_str_1,
//};
//static const char rf24_crclength_e_str_0[] PROGMEM = "Disabled";
//static const char rf24_crclength_e_str_1[] PROGMEM = "8 bits";
//static const char rf24_crclength_e_str_2[] PROGMEM = "16 bits" ;
//static const char * const rf24_crclength_e_str_P[] PROGMEM = {
//  rf24_crclength_e_str_0,
//  rf24_crclength_e_str_1,
//  rf24_crclength_e_str_2,
//};
//static const char rf24_pa_dbm_e_str_0[] PROGMEM = "PA_MIN";
//static const char rf24_pa_dbm_e_str_1[] PROGMEM = "PA_LOW";
//static const char rf24_pa_dbm_e_str_2[] PROGMEM = "PA_HIGH";
//static const char rf24_pa_dbm_e_str_3[] PROGMEM = "PA_MAX";
//static const char * const rf24_pa_dbm_e_str_P[] PROGMEM = {
//  rf24_pa_dbm_e_str_0,
//  rf24_pa_dbm_e_str_1,
//  rf24_pa_dbm_e_str_2,
//  rf24_pa_dbm_e_str_3,
//};
//
//void rf24_print_details(const RF24* rf24)
//{
//
//  rf24_print_status(rf24, get_status(rf24));
//
//  rf24_print_address_register(rf24, PSTR("RX_ADDR_P0-1"),RX_ADDR_P0,2);
//  rf24_print_byte_register(rf24, PSTR("RX_ADDR_P2-5"),RX_ADDR_P2,4);
//  rf24_print_address_register(rf24, PSTR("TX_ADDR\t"),TX_ADDR);
//
//  rf24_print_byte_register(rf24, PSTR("RX_PW_P0-6"),RX_PW_P0,6);
//  rf24_print_byte_register(rf24, PSTR("EN_AA\t"),EN_AA);
//  rf24_print_byte_register(rf24, PSTR("EN_RXADDR"),EN_RXADDR);
//  rf24_print_byte_register(rf24, PSTR("RF_CH\t"),RF_CH);
//  rf24_print_byte_register(rf24, PSTR("RF_SETUP"),RF_SETUP);
//  rf24_print_byte_register(rf24, PSTR("CONFIG\t"),NRF_CONFIG);
//  rf24_print_byte_register(rf24, PSTR("DYNPD/FEATURE"),DYNPD,2);
//
//  printf_P(PSTR("Data Rate\t = " PRIPSTR "\r\n"),rf24_pgm_read_word(rf24, &rf24_datarate_e_str_P[rf24_get_data_rate(rf24)]));
//  printf_P(PSTR("Model\t\t = " PRIPSTR "\r\n"),rf24_pgm_read_word(rf24, &rf24_model_e_str_P[rf24_is_rf24->p_variant(rf24)]));
//  printf_P(PSTR("CRC Length\t = " PRIPSTR "\r\n"),rf24_pgm_read_word(rf24, &rf24_crclength_e_str_P[rf24_get_crc_length(rf24)]));
//  printf_P(PSTR("PA Power\t = " PRIPSTR "\r\n"),  rf24_pgm_read_word(rf24, &rf24_pa_dbm_e_str_P[rf24_get_pa_level(rf24)]));
//
//}
//
//#endif


bool_t rf24_begin(RF24* rf24) {
  uint8_t setup=0;

  // Initialize pins
  if (rf24->ce_pin != rf24->csn_pin) {
    set_pin_as_output(rf24->ce_pin);  
    set_pin_as_output(rf24->csn_pin);
  } else {
    set_pin_as_output(rf24->csn_pin);
  }

  spi_initMasterFreq(SPI_USE_MOSI | SPI_USE_MISO, 1000000);
  ce(rf24, LOW);
  end_spi(rf24);

  // Must allow the radio time to settle else configuration bits will not
  // necessarily stick.  This is actually only required following power up but
  // some settling time also appears to be required after resets too. For full
  // coverage, we'll always assume the worst.  Enabling 16b CRC is by far the
  // most obvious case if the wrong timing is used - or skipped.  Technically
  // we require 4.5ms + 14us as a worst case. We'll just call it 5ms for good
  // measure.  WARNING: Delay is based on P-variant whereby non-P *may* require
  // different timing.
  _delay_us(5) ;

  // Reset NRF_CONFIG and enable 16-bit CRC.
  write_reg(rf24 , NRF_CONFIG, 0x0C) ;

  // Set 1500uS (minimum for 32B payload in ESB@250KBPS) timeouts, to make
  // testing a little easier WARNING: If this is ever lowered, either 250KBS
  // mode with AA is broken or maximum packet sizes must never be used. See
  // documentation for a more complete explanation.
  rf24_set_retries(rf24, 5,15);

  // check for connected module and if this is a p nRF24l01 variant
  //
  if (rf24_set_data_rate(rf24 , RF24_250KBPS)) {
    rf24->p_variant = TRUE ;
  }

  setup = read_reg(rf24, RF_SETUP);

  // Then set the data rate to the slowest (and most reliable) speed supported
  // by all hardware.
  rf24_set_data_rate(rf24 , RF24_1MBPS) ;

  // Disable dynamic payloads, to match rf24->dynamic_payloads_enabled setting
  // - Reset value is 0
  toggle_features(rf24);
  write_reg(rf24, FEATURE,0 );
  write_reg(rf24, DYNPD,0);
  rf24->dynamic_payloads_enabled = FALSE;

  // Reset current status
  // Notice reset and flush is the last thing we do
  write_reg(rf24, NRF_STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );

  // Set up default configuration.  Callers can always change it later.
  // This channel should be universally safe and not bleed over into adjacent
  // spectrum.
  rf24_set_channel(rf24, 76);

  // Flush buffers
  rf24_flush_rx(rf24);
  rf24_flush_tx(rf24);
  rf24_power_up(rf24); //Power up by default when rf24_begin(rf24) is called

  // Enable PTX, do not write CE high so radio will remain in standby I mode (
  // 130us max to transition to RX or TX instead of 1500us from powerUp ) PTX
  // should use only 22uA of power
  write_reg(rf24, NRF_CONFIG, ( read_reg(rf24, NRF_CONFIG) ) & ~_BV(PRIM_RX) );

  // if setup is 0 or ff then there was no response from module
  return setup != 0 && setup != 0xff;
}

//
//bool_t rf24_is_chip_connected(const RF24* rf24)
//{
//  uint8_t setup = read_reg(rf24, SETUP_AW);
//  if(setup >= 1 && setup <= 3)
//  {
//    return TRUE;
//  }
//
//  return FALSE;
//}


void rf24_start_listening(const RF24* rf24) {
#if !defined (RF24_TINY) && ! defined(LITTLEWIRE)
  // mattwach: Commented this out.  Is it really needed?
  //rf24_power_up(rf24);
#endif
  write_reg(rf24, NRF_CONFIG, read_reg(rf24, NRF_CONFIG) | _BV(PRIM_RX));
  write_reg(rf24, NRF_STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
  ce(rf24, HIGH);
  // Restore the pipe0 adddress, if exists
  if (rf24->pipe0_reading_address[0] > 0) {
    write_reg_buf(rf24, RX_ADDR_P0, rf24->pipe0_reading_address, rf24->addr_width);	
  } else {
    rf24_close_reading_pipe(rf24, 0);
  }

  // Flush buffers
  // mattwach: confirmed commented-out
  //rf24_flush_rx(rf24);
  if (read_reg(rf24, FEATURE) & _BV(EN_ACK_PAY)) {
    rf24_flush_tx(rf24);
  }

  // Go!
  // mattwach: confirmed commented-out
  //_delay_us(100);
}

static const uint8_t child_pipe_enable[] PROGMEM =
{
  ERX_P0, ERX_P1, ERX_P2, ERX_P3, ERX_P4, ERX_P5
};

void rf24_stop_listening(const RF24* rf24) {  
  ce(rf24, LOW);
  delay_us(rf24->txDelay);
  if (read_reg(rf24, FEATURE) & _BV(EN_ACK_PAY)) {
    delay_us(rf24->txDelay); //200
    rf24_flush_tx(rf24);
  }

  write_reg(rf24, NRF_CONFIG, ( read_reg(rf24, NRF_CONFIG) ) & ~_BV(PRIM_RX) );

/*
#if defined (RF24_TINY) || defined (LITTLEWIRE)
  // for 3 pins solution TX mode is only left with additonal powerDown/powerUp
  // cycle
  if (rf24->ce_pin == rf24->csn_pin) {
    rf24_power_down(rf24);
    rf24_power_up(rf24);
  }
#endif
*/

 // Enable RX on pipe0
  write_reg(
	  rf24,
	  EN_RXADDR,
	  read_reg(rf24, EN_RXADDR) |
	                 _BV(pgm_read_byte(&child_pipe_enable[0])));
}


void rf24_power_down(const RF24* rf24) {
  ce(rf24, LOW); // Guarantee CE is low on powerDown
  write_reg(rf24, NRF_CONFIG,read_reg(rf24, NRF_CONFIG) & ~_BV(PWR_UP));
}


//Power up now. Radio will not power down unless instructed by MCU for config
//changes etc.
void rf24_power_up(const RF24* rf24) {
  uint8_t cfg = read_reg(rf24, NRF_CONFIG);

  // if not powered up then power up and wait for the radio to initialize
  if (!(cfg & _BV(PWR_UP))) {
    write_reg(rf24, NRF_CONFIG, cfg | _BV(PWR_UP));

	// For nRF24L01+ to go from power down mode to TX or RX mode it must first
	// pass through stand-by mode.  There must be a delay of Tpd2stby (see
	// Table 16.) after the nRF24L01+ leaves power down mode before the CEis
	// set high. - Tpd2stby can be up to 5ms per the 1.0 datasheet
    _delay_us(5);
  }
}

//#if defined (FAILURE_HANDLING)
//void rf24_err_notify(const RF24* rf24){
//	#if defined (SERIAL_DEBUG)
//	  printf_P(PSTR("RF24 HARDWARE FAIL: Radio not responding, verify pin connections, wiring, etc.\r\n"));
//	#endif
//	#if defined (FAILURE_HANDLING)
//	rf24->failureDetected = 1;
//	#else
//	_delay_us(5000);
//	#endif
//}
//#endif

//Similar to the previous write, clears the interrupt flags
bool_t rf24_write(
	const RF24* rf24 , const void* buf, uint8_t len, const bool_t multicast) {
  rf24_start_fast_write(rf24, buf,len,multicast, TRUE);

  //Wait until complete or failed
#if defined (FAILURE_HANDLING)
  uint32_t timer = rf24_millis(rf24);
#endif
  while (!(get_status(rf24) & (_BV(TX_DS) | _BV(MAX_RT)))) { 
#if defined (FAILURE_HANDLING)
	if (rf24_millis(rf24) - timer > 95) {			
	  rf24_err_notify(rf24);
  #if defined (FAILURE_HANDLING)
	  return 0;		
  #else
	  _delay_us(100);
  #endif
	}
#endif
  }

  ce(rf24, LOW);
  write_reg(rf24, NRF_STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT));
  return 1;  // TX OK
}

//
////For general use, the interrupt flags are not important to clear
//bool_t rf24_write_blocking(const RF24* rf24 , const void* buf, uint8_t len, uint32_t timeout )
//{
//	//Block until the FIFO is NOT full.
//	//Keep track of the MAX retries and set auto-retry if seeing failures
//	//This way the FIFO will fill up and allow blocking until packets go through
//	//The radio will auto-clear everything in the FIFO as long as CE remains high
//
//	uint32_t timer = rf24_millis(rf24);							  //Get the time that the payload transmission started
//
//	while( ( get_status(rf24)  & ( _BV(TX_FULL) ))) {		  //Blocking only if FIFO is full. This will loop and block until TX is successful or timeout
//
//		if( get_status(rf24) & _BV(MAX_RT)){					  //If MAX Retries have been reached
//			rf24_re_use_tx(rf24);										  //Set re-transmit and clear the MAX_RT interrupt flag
//			if(rf24_millis(rf24) - timer > timeout){ return 0; }		  //If this payload has exceeded the user-defined timeout, exit and return 0
//		}
//		#if defined (FAILURE_HANDLING)
//			if(rf24_millis(rf24) - timer > (timeout+95) ){			
//				rf24_err_notify(rf24);
//				#if defined (FAILURE_HANDLING)
//				return 0;			
//                #endif				
//			}
//		#endif
//
//  	}
//
//  	//Start Writing
//	rf24_start_fast_write(rf24, buf,len,0, TRUE);								  //Write the payload if a buffer is clear
//
//	return 1;												  //Return 1 to indicate successful transmission
//}
//
//
//void rf24_re_use_tx(const RF24* rf24){
//		write_reg(rf24, NRF_STATUS,_BV(MAX_RT) );			  //Clear max retry flag
//		send_spi_cmd(rf24 , REUSE_TX_PL );
//		ce(rf24, LOW);										  //Re-Transfer packet
//		ce(rf24, HIGH);
//}
//
//
//bool_t rf24_write_fast(const RF24* rf24 , const void* buf, uint8_t len, const bool_t multicast )
//{
//	//Block until the FIFO is NOT full.
//	//Keep track of the MAX retries and set auto-retry if seeing failures
//	//Return 0 so the user can control the retrys and set a timer or failure counter if required
//	//The radio will auto-clear everything in the FIFO as long as CE remains high
//
//	#if defined (FAILURE_HANDLING)
//		uint32_t timer = rf24_millis(rf24);
//	#endif
//	
//	while( ( get_status(rf24)  & ( _BV(TX_FULL) ))) {			  //Blocking only if FIFO is full. This will loop and block until TX is successful or fail
//
//		if( get_status(rf24) & _BV(MAX_RT)){
//			//rf24_re_use_tx(rf24);										  //Set re-transmit
//			write_reg(rf24, NRF_STATUS,_BV(MAX_RT) );			  //Clear max retry flag
//			return 0;										  //Return 0. The previous payload has been retransmitted
//															  //From the user perspective, if you get a 0, just keep trying to send the same payload
//		}
//		#if defined (FAILURE_HANDLING)
//			if(rf24_millis(rf24) - timer > 95 ){			
//				rf24_err_notify(rf24);
//				#if defined (FAILURE_HANDLING)
//				return 0;							
//				#endif
//			}
//		#endif
//  	}
//		     //Start Writing
//	rf24_start_fast_write(rf24, buf,len,multicast, TRUE);
//
//	return 1;
//}
//
//bool_t rf24_write_fast(const RF24* rf24 , const void* buf, uint8_t len ){
//	return rf24_write_fast(rf24, buf,len,0);
//}


//Per the documentation, we want to set PTX Mode when not listening. Then all
//we do is write data and set CE high In this mode, if we can keep the FIFO
//buffers loaded, packets will transmit immediately (no 130us delay) Otherwise
//we enter Standby-II mode, which is still faster than standby mode Also, we
//remove the need to keep writing the config register over and over and
//delaying for 150 us each time if sending a stream of data
void rf24_start_fast_write(
	const RF24* rf24,
	const void* buf,
	uint8_t len,
	const bool_t multicast,
	bool_t startTx) { //TMRh20
  write_payload(
	  rf24,
	  buf,
	  len,
	  multicast ? W_TX_PAYLOAD_NO_ACK : W_TX_PAYLOAD);

  if(startTx){
	ce(rf24, HIGH);
  }
}

//
////Added the original startWrite back in so users can still use interrupts, ack payloads, etc
////Allows the library to pass all tests
//void rf24_start_write(const RF24* rf24 , const void* buf, uint8_t len, const bool_t multicast ){
//
//  // Send the payload
//
//  //write_payload(rf24 , buf, len );
//  write_payload(rf24 , buf, len,multicast? W_TX_PAYLOAD_NO_ACK : W_TX_PAYLOAD ) ;
//  ce(rf24, HIGH);
//  #if defined(CORE_TEENSY) || !defined(ARDUINO) || defined (RF24_SPIDEV) || defined (RF24_DUE)
//	_delay_us(10);
//  #endif
//  ce(rf24, LOW);
//
//
//}
//
//
//bool_t rf24_rx_fifo_full(const RF24* rf24){
//	return read_reg(rf24, FIFO_STATUS) & _BV(RX_FULL);
//}
//
//
//bool_t rf24_tx_stand_by(const RF24* rf24, uint32_t timeout, bool_t startTx){
//
//    if(startTx){
//	  rf24_stop_listening(rf24);
//	  ce(rf24, HIGH);
//	}
//	uint32_t start = rf24_millis(rf24);
//
//	while( ! (read_reg(rf24, FIFO_STATUS) & _BV(TX_EMPTY)) ){
//		if( get_status(rf24) & _BV(MAX_RT)){
//			write_reg(rf24, NRF_STATUS,_BV(MAX_RT) );
//				ce(rf24, LOW);										  //Set re-transmit
//				ce(rf24, HIGH);
//				if(rf24_millis(rf24) - start >= timeout){
//					ce(rf24, LOW); rf24_flush_tx(rf24); return 0;
//				}
//		}
//		#if defined (FAILURE_HANDLING)
//			if( rf24_millis(rf24) - start > (timeout+95)){
//				rf24_err_notify(rf24);
//				#if defined (FAILURE_HANDLING)
//				return 0;	
//				#endif
//			}
//		#endif
//	}
//
//	
//	ce(rf24, LOW);				   //Set STANDBY-I mode
//	return 1;
//
//}
//
//
//void rf24_mask_irq(const RF24* rf24, bool_t tx, bool_t fail, bool_t rx){
//
//	uint8_t config = read_reg(rf24, NRF_CONFIG);
//	/* clear the interrupt flags */
//	config &= ~(1 << MASK_MAX_RT | 1 << MASK_TX_DS | 1 << MASK_RX_DR);
//	/* set the specified interrupt flags */
//	config |= fail << MASK_MAX_RT | tx << MASK_TX_DS | rx << MASK_RX_DR;
//	write_reg(rf24, NRF_CONFIG, config);
//}


uint8_t rf24_get_dynamic_payload_size(const RF24* rf24) {
  uint8_t result = 0;
  begin_spi(rf24);
  spi_syncWrite(R_RX_PL_WID);
  result = spi_syncTransact(0xff);
  end_spi(rf24);
  if (result > 32) {
    rf24_flush_rx(rf24);
    _delay_us(2);
    return 0;
  }

  return result;
}


bool_t rf24_available(const RF24* rf24) {
  return rf24_pipe_available(rf24, NULL);
}


bool_t rf24_pipe_available(const RF24* rf24, uint8_t* pipe_num) {
  if (!(read_reg(rf24, FIFO_STATUS) & _BV(RX_EMPTY))) {
    // If the caller wants the pipe number, include that
    if ( pipe_num ){
      uint8_t status = get_status(rf24);
      *pipe_num = ( status >> RX_P_NO ) & 0x07;
    }

    return 1;
  }

  return 0;
}


void rf24_read(const RF24* rf24 , void* buf, uint8_t len) {
  read_payload(rf24, buf, len);

  //Clear the two possible interrupt flags with one command
  write_reg(rf24, NRF_STATUS,_BV(RX_DR) | _BV(MAX_RT) | _BV(TX_DS));
}


//void rf24_what_happened(const RF24* rf24, bool_t& tx_ok,bool_t& tx_fail,bool_t& rx_ready)
//{
//  // Read the status & reset the status in one easy call
//  // Or is that such a good idea?
//  uint8_t status = write_reg(rf24, NRF_STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
//
//  // Report to the user what happened
//  tx_ok = status & _BV(TX_DS);
//  tx_fail = status & _BV(MAX_RT);
//  rx_ready = status & _BV(RX_DR);
//}


void rf24_open_writing_pipe(const RF24* rf24, const uint8_t *address) {
  // Note that AVR 8-bit uC's store this LSB first, and the
  // rf24__n_r_f24_l01(rf24, +) expects it LSB first too, so we're good.
  write_reg_buf(rf24, RX_ADDR_P0,address, rf24->addr_width);
  write_reg_buf(rf24, TX_ADDR, address, rf24->addr_width);
  write_reg(rf24, RX_PW_P0,rf24->payload_size);
}

static const uint8_t child_pipe[] PROGMEM =
{
  RX_ADDR_P0, RX_ADDR_P1, RX_ADDR_P2, RX_ADDR_P3, RX_ADDR_P4, RX_ADDR_P5
};
static const uint8_t child_rf24_payload_size[] PROGMEM =
{
  RX_PW_P0, RX_PW_P1, RX_PW_P2, RX_PW_P3, RX_PW_P4, RX_PW_P5
};


//void rf24_set_address_width(const RF24* rf24, uint8_t a_width){
//
//	if(a_width -= 2){
//		write_reg(rf24, SETUP_AW,a_width%4);
//		rf24->addr_width = (a_width%4) + 2;
//	}else{
//        write_reg(rf24, SETUP_AW,0);
//        rf24->addr_width = 2;
//    }
//
//}


void rf24_open_reading_pipe(
    RF24* rf24, uint8_t child, const uint8_t *address) {
  // If this is pipe 0, cache the address.  This is needed because
  // rf24_open_writing_pipe(rf24) will overwrite the pipe 0 address, so
  // rf24_start_listening(rf24) will have to restore it.
  if (child == 0) {
    memcpy(rf24->pipe0_reading_address,address,rf24->addr_width);
  }

  if (child <= 6) {
    // For pipes 2-5, only write the LSB
    if (child < 2) {
      write_reg_buf(
          rf24, pgm_read_byte(&child_pipe[child]), address, rf24->addr_width);
    } else {
      write_reg_buf(rf24, pgm_read_byte(&child_pipe[child]), address, 1);
	   }

    write_reg(
        rf24,
        pgm_read_byte(&child_rf24_payload_size[child]),
        rf24->payload_size);

    // Note it would be more efficient to set all of the bits for all open
    // pipes at once.  However, I thought it would make the calling code
    // more simple to do it this way.
    write_reg(
        rf24,
        EN_RXADDR,
        read_reg(rf24, EN_RXADDR) | _BV(
          pgm_read_byte(&child_pipe_enable[child])));
  }
}


void rf24_close_reading_pipe(const RF24* rf24 , uint8_t pipe ) {
  write_reg(
      rf24,
      EN_RXADDR,
      read_reg(rf24, EN_RXADDR) & ~_BV(pgm_read_byte(&child_pipe_enable[pipe]))
  );
}


void rf24_enable_dynamic_payloads(RF24* rf24) {
  // Enable dynamic payload throughout the system
  write_reg(rf24, FEATURE,read_reg(rf24, FEATURE) | _BV(EN_DPL));

  // Enable dynamic payload on all pipes
  //
  // Not sure the use case of only having dynamic payload on certain
  // pipes, so the library does not support it.
  write_reg(
	  rf24,
	  DYNPD,
	  read_reg(rf24, DYNPD) | _BV(DPL_P5) | _BV(DPL_P4) | _BV(DPL_P3) |
	                          _BV(DPL_P2) | _BV(DPL_P1) | _BV(DPL_P0));

  rf24->dynamic_payloads_enabled = TRUE;
}


void rf24_disable_dynamic_payloads(RF24* rf24) {
  // Disables dynamic payload throughout the system.  Also disables Ack Payloads
  write_reg(rf24, FEATURE, 0);

  // Disable dynamic payload on all pipes
  //
  // Not sure the use case of only having dynamic payload on certain
  // pipes, so the library does not support it.
  write_reg(rf24, DYNPD, 0);
  rf24->dynamic_payloads_enabled = FALSE;
}


//void rf24_enable_ack_payload(const RF24* rf24)
//{
//  //
//  // enable ack payload and dynamic payload features
//  //
//
//    //toggle_features(rf24);
//    write_reg(rf24, FEATURE,read_reg(rf24, FEATURE) | _BV(EN_ACK_PAY) | _BV(EN_DPL) );
//
//  IF_SERIAL_DEBUG(printf("FEATURE=%i\r\n",read_reg(rf24, FEATURE)));
//
//  //
//  // Enable dynamic payload on pipes 0 & 1
//  //
//
//  write_reg(rf24, DYNPD,read_reg(rf24, DYNPD) | _BV(DPL_P1) | _BV(DPL_P0));
//  rf24->dynamic_payloads_enabled = TRUE;
//}
//
//
//void rf24_enable_dynamic_ack(const RF24* rf24){
//  //
//  // enable dynamic ack features
//  //
//    //toggle_features(rf24);
//    write_reg(rf24, FEATURE,read_reg(rf24, FEATURE) | _BV(EN_DYN_ACK) );
//
//  IF_SERIAL_DEBUG(printf("FEATURE=%i\r\n",read_reg(rf24, FEATURE)));
//
//
//}
//
//
//void rf24_write_ack_payload(const RF24* rf24, uint8_t pipe, const void* buf, uint8_t len)
//{
//  const uint8_t* current = reinterpret_cast<const uint8_t*>(buf);
//
//  uint8_t data_len = rf24_min(len,32);
//
//  begin_spi(rf24);
//  _SPI.transfer(W_ACK_PAYLOAD | ( pipe & 0x07 ) );
//
//  while ( data_len-- )
//    _SPI.transfer(*current++);
//  end_spi(rf24);
//  	
//}
//
//
//bool_t rf24_is_ack_payload_available(const RF24* rf24)
//{
//  return ! (read_reg(rf24, FIFO_STATUS) & _BV(RX_EMPTY));
//}
//
//
//bool_t rf24_is_rf24->p_variant(const RF24* rf24)
//{
//  return rf24->p_variant ;
//}
//
//
//void rf24_set_auto_ack(const RF24* rf24, bool_t enable)
//{
//  if ( enable )
//    write_reg(rf24, EN_AA, 0x3F);
//  else
//    write_reg(rf24, EN_AA, 0);
//}
//
//
//void rf24_set_auto_ack_pipe(const RF24* rf24 , uint8_t pipe, bool_t enable )
//{
//  if ( pipe <= 6 )
//  {
//    uint8_t en_aa = read_reg(rf24 , EN_AA ) ;
//    if( enable )
//    {
//      en_aa |= _BV(pipe) ;
//    }
//    else
//    {
//      en_aa &= ~_BV(pipe) ;
//    }
//    write_reg(rf24 , EN_AA, en_aa ) ;
//  }
//}
//
//
//bool_t rf24_test_carrier(const RF24* rf24)
//{
//  return ( read_reg(rf24, CD) & 1 );
//}
//
//
//bool_t rf24_test_r_p_d(const RF24* rf24)
//{
//  return ( read_reg(rf24, RPD) & 1 ) ;
//}


void rf24_set_pa_level(const RF24* rf24, uint8_t level) {
  uint8_t setup = read_reg(rf24, RF_SETUP) & 0xF8;
  if (level > 3) {  // If invalid level, go to max PA
	  level = (RF24_PA_MAX << 1) + 1;  //+1 to support the SI24R1 chip extra bit
  } else {
	  level = (level << 1) + 1;
  }

  write_reg(rf24 , RF_SETUP, setup |= level);
}

//
//uint8_t rf24_get_pa_level(const RF24* rf24)
//{
//
//  return (read_reg(rf24, RF_SETUP) & (_BV(RF_PWR_LOW) | _BV(RF_PWR_HIGH))) >> 1 ;
//}


bool_t rf24_set_data_rate(RF24* rf24, rf24_datarate_e speed) {
  bool_t result = FALSE;
  uint8_t setup = read_reg(rf24, RF_SETUP);

  // HIGH and LOW '00' is 1Mbs - our default
  setup &= ~(_BV(RF_DR_LOW) | _BV(RF_DR_HIGH)) ;

  rf24->txDelay=85;
  if (speed == RF24_250KBPS) {
    // Must set the RF_DR_LOW to 1; RF_DR_HIGH (used to be RF_DR) is already 0
    // Making it '10'.
    setup |= _BV( RF_DR_LOW ) ;
    rf24->txDelay=155;
  } else {
    // Set 2Mbs, RF_DR (RF_DR_HIGH) is set 1
    // Making it '01'
    if (speed == RF24_2MBPS) {
      setup |= _BV(RF_DR_HIGH);
      rf24->txDelay=65;
    }
  }

  write_reg(rf24, RF_SETUP,setup);
  if (read_reg(rf24, RF_SETUP) == setup) {
    result = TRUE;
  }

  return result;
}

//
//rf24_datarate_e rf24_get_data_rate(const RF24* rf24 , void )
//{
//  rf24_datarate_e result ;
//  uint8_t dr = read_reg(rf24, RF_SETUP) & (_BV(RF_DR_LOW) | _BV(RF_DR_HIGH));
//
//  // switch uses RAM (evil!)
//  // Order matters in our case below
//  if ( dr == _BV(RF_DR_LOW) )
//  {
//    // '10' = 250KBPS
//    result = RF24_250KBPS ;
//  }
//  else if ( dr == _BV(RF_DR_HIGH) )
//  {
//    // '01' = 2MBPS
//    result = RF24_2MBPS ;
//  }
//  else
//  {
//    // '00' = 1MBPS
//    result = RF24_1MBPS ;
//  }
//  return result ;
//}
//
//
//void rf24_set_crc_length(const RF24* rf24, rf24_crclength_e length)
//{
//  uint8_t config = read_reg(rf24, NRF_CONFIG) & ~( _BV(CRCO) | _BV(EN_CRC)) ;
//
//  // switch uses RAM (evil!)
//  if ( length == RF24_CRC_DISABLED )
//  {
//    // Do nothing, we turned it off above.
//  }
//  else if ( length == RF24_CRC_8 )
//  {
//    config |= _BV(EN_CRC);
//  }
//  else
//  {
//    config |= _BV(EN_CRC);
//    config |= _BV( CRCO );
//  }
//  write_reg(rf24 , NRF_CONFIG, config ) ;
//}
//
//
//rf24_crclength_e rf24_get_crc_length(const RF24* rf24)
//{
//  rf24_crclength_e result = RF24_CRC_DISABLED;
//  
//  uint8_t config = read_reg(rf24, NRF_CONFIG) & ( _BV(CRCO) | _BV(EN_CRC)) ;
//  uint8_t AA = read_reg(rf24, EN_AA);
//  
//  if ( config & _BV(EN_CRC ) || AA)
//  {
//    if ( config & _BV(CRCO) )
//      result = RF24_CRC_16;
//    else
//      result = RF24_CRC_8;
//  }
//
//  return result;
//}
//
//
//void rf24_disable_crc(const RF24* rf24 , void )
//{
//  uint8_t disable = read_reg(rf24, NRF_CONFIG) & ~_BV(EN_CRC) ;
//  write_reg(rf24 , NRF_CONFIG, disable ) ;
//}

void rf24_set_retries(const RF24* rf24, uint8_t delay, uint8_t count) {
  write_reg(rf24, SETUP_RETR,(delay&0xf)<<ARD | (count&0xf)<<ARC);
}


////ATTiny support code pulled in from https://github.com/jscrane/RF24
//
//#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
//// see http://gammon.com.au/spi
//#	define DI   0  // D0, pin 5  Data In
//#	define DO   1  // D1, pin 6  Data Out (this is *not* MOSI)
//#	define USCK 2  // D2, pin 7  Universal Serial Interface clock
//#	define SS   3  // D3, pin 2  Slave Select
//#endif
//
//#if defined(RF24_TINY)
//
//void SPIClass::begin() {
//
//  pinMode(USCK, OUTPUT);
//  pinMode(DO, OUTPUT);
//  pinMode(DI, INPUT);
//  USICR = _BV(USIWM0);
//
//}
//
//byte SPIClass::transfer(byte b) {
//
//  USIDR = b;
//  USISR = _BV(USIOIF);
//  do
//    USICR = _BV(USIWM0) | _BV(USICS1) | _BV(USICLK) | _BV(USITC);
//  while ((USISR & _BV(USIOIF)) == 0);
//  return USIDR;
//
//}
//
//void SPIClass::end() {}
//void SPIClass::setDataMode(uint8_t mode){}
//void SPIClass::setBitOrder(uint8_t bitOrder){}
//void SPIClass::setClockDivider(uint8_t rate){}
//
//
//#endif
