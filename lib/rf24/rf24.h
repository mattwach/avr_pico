#ifndef RF24_H
#define RF24_H

#include <error_codes.h>

/**
 * Power Amplifier level.
 *
 * For use with rf24_set_pa_level()
 */
typedef enum { RF24_PA_MIN = 0,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX, RF24_PA_ERROR } rf24_pa_dbm_e ;

/**
 * Data rate.  How fast data moves through the air.
 *
 * For use with rf24_set_data_rate()
 */
typedef enum { RF24_1MBPS = 0, RF24_2MBPS, RF24_250KBPS } rf24_datarate_e;

/**
 * CRC Length.  How big (if any) of a CRC is included.
 *
 * For use with rf24_set_crc_length()
 */
typedef enum {
    RF24_CRC_DISABLED = 0, RF24_CRC_8, RF24_CRC_16 } rf24_crclength_e;

/**
 * Driver for NRF24L01(+) 2.4GHz Wireless Transceiver
 */

typedef struct
{
  uint8_t ce_pin; /**< "Chip Enable" pin, activates the RX or TX role */
  uint8_t csn_pin; /**< SPI Chip select */
  uint16_t spi_speed; /**< SPI Bus Speed */
  bool_t p_variant; /* False for RF24L01 and true for RF24L01P */
  uint8_t payload_size; /**< Fixed size of payloads */
  bool_t dynamic_payloads_enabled; /**< Whether dynamic payloads are enabled. */
  uint8_t pipe0_reading_address[5]; /**< Last address set on pipe 0 for reading. */
  uint8_t addr_width; /**< The address width to use - 3,4 or 5 bytes. */

/**
* 
* The driver will delay for this duration when rf24_stop_listening() is called
* 
* When responding to payloads, faster devices like rf24__a_r_m(RPi) are much
* faster than Arduino:
* 1. Arduino sends data to RPi, switches to RX mode
* 2. The RPi receives the data, switches to TX mode and sends before the
*   Arduino radio is in RX mode
* 3. If AutoACK is disabled, this can be set as low as 0. If AA/ESB enabled,
*   set to 100uS minimum on RPi
*
* warning: If set to 0, ensure 130uS delay after rf24_stop_listening() and
* before any sends
*/

uint32_t txDelay;

 /**
 * Enable error detection by un-commenting #define FAILURE_HANDLING in
 * RF24_config.h
 * If a failure has been detected, it usually indicates a hardware issue. By
 * default the library will cease operation when a failure is detected.  
 * This should allow advanced users to detect and resolve intermittent
 * hardware issues.  
 *   
 * In most cases, the radio must be re-enabled via radio.begin(); and the
 * appropriate settings applied after a failure occurs, if wanting to re-enable
 * the device immediately.
 * 
 * Usage: (Failure handling must be enabled per above)
 *
 *  if(radio.failureDetected){ 
 *    radio.begin();                       // Attempt to re-configure the radio
 *                                         //with defaults
 *    radio.failureDetected = 0;           // Reset the detection value
 *  radio.rf24_open_writing_pipe(addresses[1]); // Re-configure pipe addresses
 *    radio.rf24_open_reading_pipe(1,addresses[0]);
 *    report_failure();                    // Blink leds, send a message, etc.
 *                                         // to indicate failure
 *  }
 *
*/
//#if defined (FAILURE_HANDLING)
  bool_t failureDetected; 
//#endif
  
} RF24;
  

/**
 * Creates a new instance of this driver.  
 *
 * You need to initialize SPI yourself before calling this
 *
 * cepin The pin attached to Chip Enable on the RF module
 * cspin The pin attached to Chip Select
 */
void rf24_init(RF24* rf24, uint16_t cepin, uint16_t cspin);

/**
 * Begin operation of the chip
 * 
 * Call this before calling any other methods.
 */
bool_t rf24_begin(RF24* rf24);

/**
 * Checks if the chip is connected to the SPI bus
 */
bool_t rf24_is_chip_connected(const RF24* rf24);

/**
 * Start listening on the pipes opened for reading.
 *
 * 1. Be sure to call rf24_open_reading_pipe() first.  
 * 2. Do not call rf24_write() while in this mode, without first calling
 *   rf24_stop_listening().
 * 3. Call rf24_available() to check for incoming traffic, and rf24_read() to
 *   get it. 
 *  
 * Open reading pipe 1 using address CCCECCCECC
 *  
 * byte address[] = { 0xCC,0xCE,0xCC,0xCE,0xCC };
 * rf24_open_reading_pipe(&rf24, 1,address);
 * rf24_start_listening(&rf24);
 */
void rf24_start_listening(const RF24* rf24);

/**
 * Stop listening for incoming messages, and switch to transmit mode.
 *
 * Do this before calling write().
 *
 * rf24_stop_listening(&rf24);
 * rf24_write(&rf24, &data,sizeof(data));
 */
void rf24_stop_listening(const RF24* rf24);

/**
 * Check whether there are bytes available to be read
 *
 * if(rf24_available(&rf24)){
 *   rf24_read(&rf24, &data,sizeof(data));
 * }
 *
 * return True if there is a payload available, false if none is
 */
bool_t rf24_available(const RF24* rf24);

/**
 * Read the available payload
 *
 * The size of data read is the fixed payload size, see rf24_get_payload_size()
 *
 * I specifically chose 'void*' as a data type to make it easier
 * for beginners to use.  No casting needed.
 *
 * No longer boolean. Use available to determine if packets are
 * available. Interrupt flags are now cleared during reads instead of
 * when calling available().
 *
 * buf Pointer to a buffer where the data should be written
 * len Maximum number of bytes to read into the buffer
 *
 * if(radio.available()){
 *   radio.read(&data,sizeof(data));
 * }
 *
 * return No return value. Use available().
 */
void rf24_read(const RF24* rf24, void* buf, uint8_t len );

/**
 * New: Open a pipe for writing via byte array. Old addressing format retained
 * for compatibility.
 *
 * Only one writing pipe can be open at once, but you can change the address
 * you'll write to. Call rf24_stop_listening() first.
 *
 * Addresses are assigned via a byte array, default is 5 byte address length
  *
 *   uint8_t addresses[][6] = {"1Node","2Node"};
 *   radio.rf24_open_writing_pipe(addresses[0]);
 *
 *  uint8_t address[] = { 0xCC,0xCE,0xCC,0xCE,0xCC };
 *  radio.rf24_open_writing_pipe(address);
 *  address[0] = 0x33;
 *  radio.rf24_open_reading_pipe(1,address);
 *
 * see setAddressWidth
 *
 * address: The address of the pipe to open. Coordinate these pipe
 * addresses amongst nodes on the network.
 */

void rf24_open_writing_pipe(const RF24* rf24, const uint8_t *address);

/**
 * Open a pipe for reading
 *
 * Up to 6 pipes can be open for reading at once.  Open all the required
 * reading pipes, and then call rf24_start_listening().
 *
 * see openWritingPipe
 * see setAddressWidth
 *
 * note: Pipes 0 and 1 will store a full 5-byte address. Pipes 2-5 will
 *   technically only store a single byte, borrowing up to 4 additional bytes
 *   from pipe #1 per the assigned address width.
 *
 * warning: Pipes 1-5 should share the same address, except the first byte.
 * Only the first byte in the array should be unique, e.g.
 *
 *   uint8_t addresses[][6] = {"1Node","2Node"};
 *   rf24_open_reading_pipe(1,addresses[0]);
 *   rf24_open_reading_pipe(2,addresses[1]);
 *
 * warning: Pipe 0 is also used by the writing pipe.  So if you open
 * pipe 0 for reading, and then rf24_start_listening(), it will overwrite the
 * writing pipe.  Ergo, do an rf24_open_writing_pipe() again before write().
 *
 * number: Which pipe# to open, 0-5.
 * address: The 24, 32 or 40 bit address of the pipe to open.
 */

void rf24_open_reading_pipe(RF24* rf24, uint8_t number, const uint8_t *address);

/**
 * Print a giant block of debugging information to stdout
 *
 * warning: Does nothing if stdout is not defined.  See fdevopen in stdio.h
 * The printf.h file is included with the library for Arduino.
 *
 * #include <printf.h>
 * setup(){
 *  Serial.begin(115200);
 *  printf_begin();
 *  ...
 * }
 */
void rf24_print_details(const RF24* rf24);

/**
 * Test whether there are bytes available to be read in the
 * FIFO buffers. 
 *
 * pipe_num: Which pipe has the payload available
 *  
 * uint8_t pipeNum;
 * if(radio.available(&pipeNum)){
 *   radio.read(&data,sizeof(data));
 *   Serial.print("Got data on pipe");
 *   Serial.println(pipeNum);
 * }
 *
 * returns True if there is a payload available, false if none is
 */
bool_t rf24_pipe_available(const RF24* rf24, uint8_t* pipe_num);

/**
 * Check if the radio needs to be read. Can be used to prevent data loss
 * returns True if all three 32-byte radio buffers are full
 */
bool_t rf24_rx_fifo_full(const RF24* rf24);

/**
 * Enter low-power mode
 *
 * To return to normal power mode, call rf24_power_up().
 *
 * note: After calling rf24_start_listening(), a basic radio will consume
 *   about 13.5mA at max PA level.
 * During active transmission, the radio will consume about 11.5mA, but this
 *   will be reduced to 26uA (.026mA) between sending.
 * In full powerDown mode, the radio will consume approximately 900nA (.0009mA)   
 *
 * radio.rf24_power_down();
 * avr_enter_sleep_mode(); // Custom function to sleep the device
 * radio.rf24_power_up();
 */
void rf24_power_down(const RF24* rf24);

/**
 * Leave low-power mode - required for normal radio operation after calling
 *   rf24_power_down()
 * 
 * To return to low power mode, call rf24_power_down().
 * note: This will take up to 5ms for maximum compatibility 
 */
void rf24_power_up(const RF24* rf24);

/**
 * Be sure to call rf24_open_writing_pipe() first to set the destination
 * of where to write to.
 *
 * This blocks until the message is successfully acknowledged by
 * the receiver or the timeout/retransmit maxima are reached.  In
 * the current configuration, the max delay here is 60-70ms.
 *
 * The maximum size of data written is the fixed payload size, see
 * rf24_get_payload_size().  However, you can write less, and the remainder
 * will just be filled with zeroes.
 * Can be used with rf24_enable_ack_payload() to request a response
 * see rf24_enable_dynamic_ack(), rf24_set_auto_ack(), rf24_write()
 *
 * TX/RX/RT interrupt flags will be cleared every time write is called
 *
 * buf: Pointer to the data to be sent
 * len: Number of bytes to be sent
 *
 * rf24_stop_listening(&rf24);
 * rf24_write(&data,sizeof(&rf24, data));
 * multicast: Request ACK (0), NOACK (1)
 *
 * return True if the payload was delivered successfully false if not
 */
bool_t rf24_write(
    const RF24* rf24, const void* buf, uint8_t len, const bool_t multicast );

/**
 * This will not block until the 3 FIFO buffers are filled with data.
 * Once the FIFOs are full, writeFast will simply wait for success or
 * timeout, and return 1 or 0 respectively. From a user perspective, just
 * keep trying to send the same data. The library will keep auto retrying
 * the current payload using the built in functionality.
 * warning: It is important to never keep the nRF24L01 in TX mode and FIFO 
 * full for more than 4ms at a time. If the auto retransmit is enabled, the
 * nRF24L01 is never in TX mode long enough to disobey this rule. Allow the
 * FIFO to clear by issuing rf24_tx_stand_by() or ensure appropriate time
 * between transmissions.
 *
 * Example (Partial blocking):
 *
 *      radio.rf24_write_fast(&buf,32);  // Writes 1 payload to the buffers
 *      rf24_tx_stand_by();            // Returns 0 if failed. 1 if success.
 *                                     // Blocks only until MAX_RT timeout or
 *                                     // success. Data flushed on fail.
 *
 *      radio.rf24_write_fast(&buf,32);  // Writes 1 payload to the buffers
 *      rf24_tx_stand_by(1000);       // Using extended timeouts, returns 1 if
 *                                    // success. Retries failed payloads for
 *                                    // 1 seconds before returning 0.
 *
 * see rf24_tx_stand_by(), rf24_write(), rf24_write_blocking()
 *
 * buf: Pointer to the data to be sent
 * len: Number of bytes to be sent
 * multicast: Request ACK (0) or NOACK (1)
 *
 * return True if the payload was delivered successfully false if not
 */
bool_t rf24_write_fast(
    const RF24* rf24, const void* buf, uint8_t len, const bool_t multicast );

/**
 * This function extends the auto-retry mechanism to any specified duration.
 * It will not block until the 3 FIFO buffers are filled with data.
 * If so the library will auto retry until a new payload is written
 * or the user specified timeout period is reached.
 * warning: It is important to never keep the nRF24L01 in TX mode and FIFO full
 * for more than 4ms at a time. If the auto retransmit is enabled, the nRF24L01
 * is never in TX mode long enough to disobey this rule. Allow the FIFO to clear
 * by issuing rf24_tx_stand_by() or ensure appropriate time between
 * transmissions.
 *
 * Example (Full blocking):
 *
 *      radio.rf24_write_blocking(&buf,32,1000); //Wait up to 1 second to write
 *                                               // 1 payload to the buffers
 *      rf24_tx_stand_by(1000);  //Wait up to 1 second for the payload to send.
 *                               //Return 1 if ok, 0 if failed.
 *                               //Blocks only until user timeout or success.
 *                               // Data flushed on fail.
 *
 * note: If used from within an interrupt, the interrupt should be disabled
 * until completion, and sei(); called to enable millis().
 * see rf24_tx_stand_by(), rf24_write(), rf24_write_fast()
 *
 * buf: Pointer to the data to be sent
 * len: Number of bytes to be sent
 * timeout: User defined timeout in milliseconds.
 *
 * return True if the payload was loaded into the buffer successfully false if
 * not
 */
bool_t rf24_write_blocking(const RF24* rf24, const void* buf, uint8_t len, uint32_t timeout );

/**
 * This function allows extended blocking and auto-retries per a user defined
 * timeout
 *
 *  Fully Blocking Example:
 *
 *      radio.rf24_write_fast(&buf,32);
 *      radio.rf24_write_fast(&buf,32);
 *      radio.rf24_write_fast(&buf,32);   //Fills the FIFO buffers up
 *      bool ok = rf24_tx_stand_by(1000);  //Returns 0 if failed after 1 second
 *                                         //of retries. 1 if success.
 *                        //Blocks only until user defined timeout or success.
 *                        // Data flushed on fail.
 *
 * note: If used from within an interrupt, the interrupt should be disabled
 *   until completion, and sei(); called to enable millis().
 * timeout: Number of milliseconds to retry failed payloads
 * return True if transmission is successful
 *
 */
 bool_t rf24_tx_stand_by(const RF24* rf24, uint32_t timeout, bool_t startTx);  // startTX=0 was the default

/**
 * Write an ack payload for the specified pipe
 *
 * The next time a message is received on pipe, the data in buf will
 * be sent back in the acknowledgement.
 * see rf24_enable_ack_payload(), rf24_enable_dynamic_payloads()
 *
 * warning: Only three of these can be pending at any time as there are only 3
 * FIFO buffers.<br> Dynamic payloads must be enabled.
 * note: Ack payloads are handled automatically by the radio chip when a payload
 *   is received. Users should generally write an ack payload as soon as
 *   rf24_start_listening() is called, so one is available when a regular
 *   payload is received.
 * note: Ack payloads are dynamic payloads. This only works on pipes 0&1 by
 *   default. Call  rf24_enable_dynamic_payloads() to enable on all pipes.
 *
 * pipe: Which pipe# (typically 1-5) will get this response.
 * buf: Pointer to data that is sent
 * len: Length of the data to send, up to 32 bytes max.  Not affected
 * by the static payload set by rf24_set_payload_size().
 */
void rf24_write_ack_payload(
    const RF24* rf24, uint8_t pipe, const void* buf, uint8_t len);

/**
 * Determine if an ack payload was received in the most recent call to
 * rf24_write(). The regular available() can also be used.
 *
 * Call rf24_read() to retrieve the ack payload.
 *
 * returns True if an ack payload is available.
 */
bool_t rf24_is_ack_payload_available(const RF24* rf24);

/**
 * Call this when you get an interrupt to find out why
 *
 * Tells you what caused the interrupt, and clears the state of
 * interrupts.
 *
 * tx_ok: The send was successful (TX_DS)
 * tx_fail: The send failed, too many retries (MAX_RT)
 * rx_ready: There is a message waiting to be read (RX_DS)
 */
void rf24_what_happened(
    const RF24* rf24, bool_t* tx_ok, bool_t* tx_fail, bool_t* rx_ready);

/**
 * Non-blocking write to the open writing pipe used for buffered writes
 *
 * Optimization: This function now leaves the CE pin high, so the radio
 * will remain in TX or STANDBY-II Mode until a rf24_tx_stand_by() command is
 * issued. Can be used as an alternative to rf24_start_write()
 * if writing multiple payloads at once.
 * warning: It is important to never keep the nRF24L01 in TX mode with FIFO
 *   full for more than 4ms at a time. If the auto retransmit/autoAck is
 *   enabled, the nRF24L01 is never in TX mode long enough to disobey this
 *   rule. Allow the FIFO to clear by issuing rf24_tx_stand_by() or ensure
 *   appropriate time between transmissions.
 *
 * see write(), rf24_write_fast(), rf24_start_write(), rf24_write_blocking()
 *
 * For single noAck writes see:
 * see rf24_enable_dynamic_ack(), rf24_set_auto_ack()
 *
 * buf: Pointer to the data to be sent
 * len: Number of bytes to be sent
 * multicast: Request ACK (0) or NOACK (1)
 * returns True if the payload was delivered successfully false if not
 */
void rf24_start_fast_write(
    const RF24* rf24,
    const void* buf,
    uint8_t len,
    const bool_t multicast,
    bool_t startTx );  // default startTx was 1

/**
 * Non-blocking write to the open writing pipe
 *
 * Just like write(), but it returns immediately. To find out what happened
 * to the send, catch the IRQ and then call rf24_what_happened().
 *
 * see write(), rf24_write_fast(), rf24_start_fast_write(), rf24_what_happened()
 *
 * For single noAck writes see:
 * see rf24_enable_dynamic_ack(), rf24_set_auto_ack()
 *
 * buf: Pointer to the data to be sent
 * len: Number of bytes to be sent
 * multicast: Request ACK (0) or NOACK (1)
 *
 */
void rf24_start_write(
    const RF24* rf24, const void* buf, uint8_t len, const bool_t multicast );

/**
 * This function is mainly used internally to take advantage of the auto payload
 * re-use functionality of the chip, but can be beneficial to users as well.
 *
 * The function will instruct the radio to re-use the data in the FIFO buffers,
 * and instructs the radio to re-send once the timeout limit has been reached.
 * Used by writeFast and writeBlocking to initiate retries when a TX failure
 * occurs. Retries are automatically initiated except with the standard write().
 * This way, data is not flushed from the buffer until switching between modes.
 *
 * note: This is to be used AFTER auto-retry fails if wanting to resend
 * using the built-in payload reuse features.
 * After issuing rf24_re_use_t_x(), it will keep reending the same payload
 * forever or until a payload is written to the FIFO, or a flush_tx command is
 * given.
 */
 void rf24_re_use_tx(const RF24* rf24);

/**
 * Empty the transmit buffer. This is generally not required in standard
 * operation.
 * May be required in specific cases after rf24_stop_listening() , if
 * operating at 250KBPS data rate.
 *
 * returns Current value of status register
 */
uint8_t rf24_flush_tx(const RF24* rf24);

/**
 * Test whether there was a carrier on the line for the
 * previous listening period.
 *
 * Useful to check for interference on the current channel.
 *
 * returns: true if was carrier, false if not
 */
bool_t rf24_test_carrier(const RF24* rf24);

/**
 * Test whether a signal (carrier or otherwise) greater than
 * or equal to -64dBm is present on the channel. Valid only
 * on nRF24L01P (+) hardware. On nRF24L01, use rf24_test_carrier().
 *
 * Useful to check for interference on the current channel and
 * channel hopping strategies.
 *
 *
 * bool goodSignal = radio.rf24_test_r_p_d();
 * if(radio.available()){
 *    Serial.println(
 *      goodSignal ? "Strong signal > 64dBm" : "Weak signal < 64dBm" );
 *    radio.read(0,0);
 * }
 *
 * returns: true if signal => -64dBm, false if not
 */
bool_t rf24_test_r_p_d(const RF24* rf24) ;

 /**
 * Close a pipe after it has been previously opened.
 * Can be safely called without having previously opened a pipe.
 * pipe: Which pipe # to close, 0-5.
 */
void rf24_close_reading_pipe(const RF24* rf24, uint8_t pipe ) ;


/**
* Set the address width from 3 to 5 bytes (24, 32 or 40 bit)
*
* a:_width The address width to use: 3,4 or 5
*/

void rf24_set_address_width(const RF24* rf24, uint8_t a_width);

/**
 * Set the number and delay of retries upon failed submit
 *
 * delay: How long to wait between each retry, in multiples of 250us,
 * max is 15.  0 means 250us, 15 means 4000us.
 * count: How many retries before giving up, max 15
 */
void rf24_set_retries(const RF24* rf24, uint8_t delay, uint8_t count);

/**
 * Set RF communication channel
 *
 * channel: Which RF channel to communicate on, 0-125
 */
void rf24_set_channel(const RF24* rf24, uint8_t channel);

  /**
 * Get RF communication channel
 *
 * returns: The currently configured RF Channel
 */
uint8_t rf24_get_channel(const RF24* rf24);

/**
 * Set Static Payload Size
 *
 * This implementation uses a pre-stablished fixed payload size for all
 * transmissions.  If this method is never called, the driver will always
 * transmit the maximum payload size (32 bytes), no matter how much
 * was sent to write().
 *
 * size: The number of bytes in the payload
 */
void rf24_set_payload_size(RF24* rf24, uint8_t size);

/**
 * Get Static Payload Size
 *
 * see rf24_set_payload_size()
 *
 * returns: The number of bytes in the payload
 */
uint8_t rf24_get_payload_size(const RF24* rf24);

/**
 * Get Dynamic Payload Size
 *
 * For dynamic payloads, this pulls the size of the payload off
 * the chip
 *
 * note: Corrupt packets are now detected and flushed per the
 * manufacturer.
 *
 * if(radio.available()){
 *   if(radio.rf24_get_dynamic_payload_size() < 1){
 *     // Corrupt payload has been flushed
 *     return; 
 *   }
 *   radio.read(&data,sizeof(data));
 * }
 *
 *
 * returns: Payload length of last-received dynamic payload
 */
uint8_t rf24_get_dynamic_payload_size(const RF24* rf24);

/**
 * Enable custom payloads on the acknowledge packets
 *
 * Ack payloads are a handy way to return data back to senders without
 * manually changing the radio modes on both units.
 *
 * note: Ack payloads are dynamic payloads. This only works on pipes 0&1 by
 *   default. Call rf24_enable_dynamic_payloads() to enable on all pipes.
 */
void rf24_enable_ack_payload(const RF24* rf24);

/**
 * Enable dynamically-sized payloads
 *
 * This way you don't always have to send large packets just to send them
 * once in a while.  This enables dynamic payloads on ALL pipes.
 *
 */
void rf24_enable_dynamic_payloads(RF24* rf24);

/**
 * Disable dynamically-sized payloads
 *
 * This disables dynamic payloads on ALL pipes. Since Ack Payloads
 * requires Dynamic Payloads, Ack Payloads are also disabled.
 * If dynamic payloads are later re-enabled and ack payloads are desired
 * then rf24_enable_ack_payload() must be called again as well.
 *
 */
void rf24_disable_dynamic_payloads(RF24* rf24);

/**
 * Enable dynamic ACKs (single write multicast or unicast) for chosen messages
 *
 * note: To enable full multicast or per-pipe multicast, use rf24_set_auto_ack()
 *
 * warning: This MUST be called prior to attempting single write NOACK calls
 *
 * radio.rf24_enable_dynamic_ack();
 * radio.write(&data,32,1);  // Sends a payload with no acknowledgement
 *                           // requested
 * radio.write(&data,32,0);  // Sends a payload using auto-retry/autoACK
 *
 */
void rf24_enable_dynamic_ack(const RF24* rf24);

/**
 * Determine whether the hardware is an nRF24L01+ or not.
 *
 * returns: true if the hardware is nRF24L01+ (or compatible) and false
 * if its not.
 */
bool_t rf24_is_p_variant(const RF24* rf24) ;

/**
 * Enable or disable auto-acknowlede packets
 *
 * This is enabled by default, so it's only needed if you want to turn
 * it off for some reason.
 *
 * enable: Whether to enable (true) or disable (false) auto-acks
 */
void rf24_set_auto_ack(const RF24* rf24, bool_t enable);

/**
 * Enable or disable auto-acknowlede packets on a per pipeline basis.
 *
 * AA is enabled by default, so it's only needed if you want to turn
 * it off/on for some reason on a per pipeline basis.
 *
 * pipe: Which pipeline to modify
 * enable: Whether to enable (true) or disable (false) auto-acks
 */
void rf24_set_auto_ack_pipe(const RF24* rf24, uint8_t pipe, bool_t enable ) ;

/**
 * Set Power Amplifier (PA) level to one of four levels:
 * RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX
 *
 * The power levels correspond to the following output levels respectively:
 * NRF24L01: -18dBm, -12dBm,-6dBM, and 0dBm
 *
 * SI24R1: -6dBm, 0dBm, 3dBM, and 7dBm.
 *
 * level: Desired PA level.
 */
void rf24_set_pa_level(const RF24* rf24, uint8_t level );

/**
 * Fetches the current PA level.
 *
 * NRF24L01: -18dBm, -12dBm, -6dBm and 0dBm
 * SI24R1:   -6dBm, 0dBm, 3dBm, 7dBm
 *
 * returns: Returns values 0 to 3 representing the PA Level.
 */
uint8_t rf24_get_pa_level(const RF24* rf24);

/**
 * Set the transmission data rate
 *
 * warning: setting RF24_250KBPS will fail for non-plus units
 *
 * speed: RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
 * returns: true if the change was successful
 */
bool_t rf24_set_data_rate(RF24* rf24, rf24_datarate_e speed);

/**
 * Fetches the transmission data rate
 *
 * returns: Returns the hardware's currently configured datarate. The value
 * is one of 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS, as defined in the
 * rf24_datarate_e enum.
 */
rf24_datarate_e rf24_get_data_rate(const RF24* rf24) ;

/**
 * Set the CRC length
 * <br>CRC checking cannot be disabled if auto-ack is enabled
 * length: RF24_CRC_8 for 8-bit or RF24_CRC_16 for 16-bit
 */
void rf24_set_crc_length(const RF24* rf24, rf24_crclength_e length);

/**
 * Get the CRC length
 * <br>CRC checking cannot be disabled if auto-ack is enabled
 * returns: RF24_CRC_DISABLED if disabled or RF24_CRC_8 for 8-bit or
 *   RF24_CRC_16 for 16-bit
 */
rf24_crclength_e rf24_get_crc_length(const RF24* rf24);

/**
 * Disable CRC validation
 * 
 * warning: CRC cannot be disabled if auto-ack/ESB is enabled.
 */
void rf24_disable_crc(const RF24* rf24) ;

/**
* The radio will generate interrupt signals when a transmission is complete,
* a transmission fails, or a payload is received. This allows users to mask
* those interrupts to prevent them from generating a signal on the interrupt
* pin. Interrupts are enabled on the radio chip by default.
*
*
*   Mask all interrupts except the receive interrupt:
*
*    radio.rf24_mask_irq(1,1,0);
*
*
* tx:_ok  Mask transmission complete interrupts
* tx:_fail  Mask transmit failure interrupts
* rx:_ready Mask payload received interrupts
*/
void rf24_mask_irq(const RF24* rf24, bool_t tx_ok, bool_t tx_fail, bool_t rx_ready);

/**
 * Empty the receive buffer
 *
 * returns: Current value of status register
 */
uint8_t rf24_flush_rx(const RF24* rf24);

#endif // RF24_H
