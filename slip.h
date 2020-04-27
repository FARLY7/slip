#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Private SLIP structure.
 * @note Structure is not to be accessed directly by user.
 */
typedef struct {
    uint8_t *buf;
    size_t   len;
    size_t   wp;
    uint8_t  state;
} slip_t;

/** 
 * @brief Initialise a SLIP encoder or decoder.
 * 
 * @param[in] slip : Pointer to SLIP encoder or decoder.
 * @param[in] buf  : Buffer to hold encoded or decoded message.
 * @param[in] len  : Length of buffer
 **/
void SLIP_init(slip_t *slip, uint8_t *buf, size_t len);

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
 * @brief Consume byte for SLIP encoding. Used for streams of data.
 * 
 * @note Need to manually add 'END' delimiter when finished encoding.
 * 
 * @param[in] slip : Pointer to SLIP encoder.
 * @param[in] byte : Byte to be encoded.
 * 
 * @return < 0 : Error
 *         = 0 : Encoded byte
 *         > 0 : Success/Finished (Length of encoded data)
 **/
int SLIP_encode(slip_t *slip, uint8_t byte);

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
size_t SLIP_encode_frame(slip_t *slip, uint8_t *buf, size_t len);

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

