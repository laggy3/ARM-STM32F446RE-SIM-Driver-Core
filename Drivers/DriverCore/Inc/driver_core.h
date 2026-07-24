/**
 ******************************************************************************
 * @file    driver_core.h
 * @brief   ARM-STM32F446RE-SIM-Driver-Core :: common driver framework
 *
 * Shared status codes, timeout primitives, GPIO alt-function helpers and
 * bit-manipulation macros used by every peripheral driver in this suite.
 * No HAL / LL dependency -- register level only (CMSIS device header).
 ******************************************************************************
 * All Rights Reserved.
 ******************************************************************************
 */
#ifndef DRIVER_CORE_H
#define DRIVER_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f446xx.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ------------------------------------------------------------------------ */
/* Common status / error codes returned by every driver in the suite        */
/* ------------------------------------------------------------------------ */
typedef enum
{
    DRV_OK              = 0x00U,
    DRV_ERROR           = 0x01U,   /* generic / unspecified failure          */
    DRV_BUSY            = 0x02U,   /* peripheral currently mid-transaction   */
    DRV_TIMEOUT         = 0x03U,   /* operation exceeded its timeout window  */
    DRV_INVALID_PARAM   = 0x04U,   /* bad argument / out-of-range config     */
    DRV_NOT_READY       = 0x05U,   /* peripheral not initialised             */
    DRV_NACK            = 0x06U,   /* I2C: no acknowledge from slave         */
    DRV_BUS_ERROR       = 0x07U,   /* bus arbitration / framing / CRC error  */
    DRV_OVERRUN         = 0x08U,   /* receive overrun                        */
    DRV_UNDERRUN        = 0x09U,   /* transmit underrun (SAI/SPI)            */
    DRV_NO_MEDIA        = 0x0AU,   /* SDIO: no card present                  */
    DRV_UNSUPPORTED     = 0x0BU    /* feature not implemented on this part   */
} drv_status_t;

/* ------------------------------------------------------------------------ */
/* Default software timeout (SysTick-derived millisecond counter)           */
/* ------------------------------------------------------------------------ */
#define DRV_DEFAULT_TIMEOUT_MS   (100U)

extern volatile uint32_t g_drv_tick_ms;

void     drv_systick_init(uint32_t core_clock_hz);
uint32_t drv_get_tick(void);
void     drv_delay_ms(uint32_t ms);

/* Helper: has `start` + `timeout_ms` elapsed relative to now? */
static inline bool drv_timed_out(uint32_t start, uint32_t timeout_ms)
{
    return ((drv_get_tick() - start) >= timeout_ms);
}

/* ------------------------------------------------------------------------ */
/* GPIO port/pin descriptor + alternate-function helper, reused by every    */
/* peripheral driver that needs to mux pins (USART/I2C/SPI/CAN/SAI/SDIO/USB)*/
/* ------------------------------------------------------------------------ */
typedef enum
{
    GPIO_MODE_INPUT     = 0x0U,
    GPIO_MODE_OUTPUT    = 0x1U,
    GPIO_MODE_AF        = 0x2U,
    GPIO_MODE_ANALOG    = 0x3U
} gpio_mode_t;

typedef enum
{
    GPIO_OTYPE_PP = 0x0U,   /* push-pull  */
    GPIO_OTYPE_OD = 0x1U    /* open-drain */
} gpio_otype_t;

typedef enum
{
    GPIO_SPEED_LOW       = 0x0U,
    GPIO_SPEED_MEDIUM    = 0x1U,
    GPIO_SPEED_HIGH      = 0x2U,
    GPIO_SPEED_VERY_HIGH = 0x3U
} gpio_speed_t;

typedef enum
{
    GPIO_PULL_NONE = 0x0U,
    GPIO_PULL_UP   = 0x1U,
    GPIO_PULL_DOWN = 0x2U
} gpio_pull_t;

typedef struct
{
    GPIO_TypeDef *port;
    uint8_t       pin;      /* 0..15                */
    uint8_t       af;       /* AF0..AF15, ignored unless mode == AF */
} gpio_pin_t;

void drv_gpio_clock_enable(GPIO_TypeDef *port);
void drv_gpio_config(GPIO_TypeDef *port, uint8_t pin, gpio_mode_t mode,
                      gpio_otype_t otype, gpio_speed_t speed, gpio_pull_t pull,
                      uint8_t af);
void drv_gpio_config_pin(const gpio_pin_t *p, gpio_mode_t mode,
                          gpio_otype_t otype, gpio_speed_t speed,
                          gpio_pull_t pull);
void drv_gpio_write(GPIO_TypeDef *port, uint8_t pin, bool set);
void drv_gpio_toggle(GPIO_TypeDef *port, uint8_t pin);
bool drv_gpio_read(GPIO_TypeDef *port, uint8_t pin);

/* ------------------------------------------------------------------------ */
/* Small bit-manipulation helpers used throughout the register-level code  */
/* ------------------------------------------------------------------------ */
#define DRV_SET_BITS(REG, MASK)      ((REG) |=  (MASK))
#define DRV_CLR_BITS(REG, MASK)      ((REG) &= ~(MASK))
#define DRV_MODIFY_BITS(REG, MASK, VAL) \
            ((REG) = (((REG) & ~(MASK)) | ((VAL) & (MASK))))

#ifndef DRV_UNUSED
#define DRV_UNUSED(x)  ((void)(x))
#endif

#ifdef __cplusplus
}
#endif

#endif /* DRIVER_CORE_H */
