#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of SLIP encoding to use.
 */
typedef enum {
    SLIP_ENCODING_SINGLE_ENDED, /* END delimiter is only added to the end of the msg */
    SLIP_ENCODING_DOUBLE_ENDED  /* END delimiter is added to the beginning and the end of the msg */
} slip_encoding_t;

/**
 * @brief Private SLIP structure.
 * @warning: Structure is not to be accessed directly by user.
 */
typedef struct {
    uint8_t *buf;
    size_t   len;
    size_t   wp;
    uint8_t  state;
    slip_encoding_t encoding;
} slip_t;

/** 
 * @brief Initialise a SLIP encoder or decoder.
 * 
 * @param[in] slip : Pointer to SLIP encoder or decoder.
 * @param[in] buf  : Buffer to hold encoded or decoded message.
 * @param[in] len  : Length of buffer
 **/
void SLIP_init(slip_t *slip, uint8_t *buf, size_t len, slip_encoding_t encoding);

/** 
 * @brief Reset SLIP encoder or decoder.
 * 
 * @param[in] slip : Pointer to SLIP encoder or decoder.
 **/
void SLIP_reset(slip_t *slip);

/**
 * @brief Gets the length of the encoded or decoded message so far.
 * 
 * @param[in] slip : Pointer to SLIP encoder or decoder.
 * 
 * @return Length of message.
 */
size_t SLIP_get_length(slip_t *slip);

/** 
 * @brief SLIP encodes a given frame
 * 
 * @param[in] slip : Pointer to SLIP encoder.
 * @param[in] buf  : Buffer containing frame.
 * @param[in] len  : Length of buffer
 * 
 * @return > 0 : Success (Length of encoded data)
 *         = 0 : Failed
 **/
size_t SLIP_encode(slip_t *slip, const uint8_t *buf, size_t len);

/** 
 * @brief Consume byte for SLIP decoding.
 * 
 * @param[in] slip : Pointer to SLIP decoder.
 * @param[in] byte : Byte to be decoded.
 * 
 * @return < 0 : Error
 *         = 0 : Decoded byte
 *         > 0 : Success/Finished (Length of decoded data)
 **/
int SLIP_decode(slip_t *slip, uint8_t byte);



#ifdef __cplusplus
}
#endif

