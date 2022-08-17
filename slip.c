#include "slip.h"

/**
 * @brief Internal SLIP character values.
 **/
typedef enum {
    END     = 0xC0,
    ESC     = 0xDB,
    ESC_END = 0xDC,
    ESC_ESC = 0xDD
} slip_char_t;

/**
 * @brief Internal SLIP encoder state.
 **/
typedef enum {
    STATE_MESSAGE,
    STATE_ERROR,
    STATE_ESCAPE
} slip_state_t;

/** 
 * @brief Initialise a SLIP encoder or decoder.
 * 
 * @param[in] slip : Pointer to SLIP encoder or decoder.
 * @param[in] buf  : Buffer to hold encoded or decoded message.
 * @param[in] len  : Length of buffer
 **/
void SLIP_init(slip_t *slip, uint8_t *buf, size_t len)
{
    slip->buf = buf;
    slip->len = len;
    slip->state = STATE_MESSAGE;
    slip->wp = 0;
}

/** 
 * @brief Reset SLIP encoder or decoder.
 * 
 * @param[in] slip : Pointer to SLIP encoder or decoder.
 **/
void SLIP_reset(slip_t *slip)
{
    slip->state = STATE_MESSAGE;
    slip->wp = 0;
}

/**
 * @brief Gets the length of the encoded or decoded message so far.
 * 
 * @param[in] slip : Pointer to SLIP encoder or decoder.
 * 
 * @return Length of message.
 */
size_t SLIP_get_length(slip_t *slip)
{
    return slip->wp;
}

/** 
 * @brief Consume byte for SLIP encoding.
 * 
 * @param[in] slip : Pointer to SLIP encoder.
 * @param[in] byte : Byte to be encoded.
 * 
 * @return < 0 : Error
 *         = 0 : Encoded byte
 *         > 0 : Success/Finished (Length of encoded data)
 **/
static int SLIP_encode_byte(slip_t *slip, uint8_t byte)
{
    int remaining = slip->len - slip->wp;
	int required = (byte == ESC || byte == END) ? 2 : 1;

	if (remaining < required) {
		return -1;
	}

    if (byte == ESC)
    {
		slip->buf[slip->wp++] = ESC;
		slip->buf[slip->wp++] = ESC_ESC;
	}
    else if (byte == END) {
		slip->buf[slip->wp++] = ESC;
		slip->buf[slip->wp++] = ESC_END;
	}
    else {
		slip->buf[slip->wp++] = byte;
	}

    return 0;
}

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
size_t SLIP_encode(slip_t *slip, const uint8_t *buf, size_t len)
{
    SLIP_reset(slip);

    while(len--) {
        if(SLIP_encode_byte(slip, *buf++) < 0) {
            /* SLIP encoding failed */
            return 0;
        }
    }

    if((slip->len - slip->wp) < 1) {
        /* No space for END delimiter */
        return 0;
    }

    slip->buf[slip->wp++] = END;
    return slip->wp;
}

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
int SLIP_decode(slip_t *slip, uint8_t byte)
{
    int retVal = 0;

    if(byte == END)
    {
        if(slip->wp > 0 && slip->state == STATE_MESSAGE) {
            retVal = slip->wp;
        } else {
            retVal = -1;
        }
        SLIP_reset(slip);
        return retVal;
    }
    
    switch(slip->state)
    {
        case STATE_MESSAGE:
        {
            if(slip->wp == slip->len) {
                /* ERROR: Ran out of buffer space */
                SLIP_reset(slip);
                return -1;
            } else if(byte == ESC) {
                slip->state = STATE_ESCAPE;
            } else {
                slip->buf[slip->wp++] = byte;
            }
            break;
        }
        case STATE_ESCAPE:
        {
            if(byte == ESC_END) {
                slip->buf[slip->wp++] = END;
                slip->state = STATE_MESSAGE;
            } else if(byte == ESC_ESC) {
                slip->buf[slip->wp++] = ESC;
                slip->state = STATE_MESSAGE;
            } else {
                /* ERROR: Wrong character received */
                SLIP_reset(slip);
                return -1;
            }
            break;
        }
        case STATE_ERROR: {
            SLIP_reset(slip);
            return -1;
        }
    }

    return 0;
}