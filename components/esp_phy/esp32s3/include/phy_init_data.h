/*
 * SPDX-FileCopyrightText: 2016-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PHY_INIT_DATA_H
#define PHY_INIT_DATA_H /* don't use #pragma once here, we compile this file sometimes */
#include "esp_phy_init.h"
#include "esp_attr.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif
// constrain a value between 'low' and 'high', inclusive
#define LIMIT(val, low, high) ((val < low) ? low : (val > high) ? high : val)
#define PHY_INIT_MAGIC "PHYINIT"
#define PHY_INIT_MAGIC_LEN 8 // should be strlen(PHY_INIT_MAGIC) + 1

// define the lowest tx power as LOWEST_PHY_TX_POWER
#define PHY_TX_POWER_LOWEST LIMIT(CONFIG_ESP_PHY_MAX_TX_POWER * 4, 0, 52)
#define PHY_TX_POWER_OFFSET 2
#define PHY_TX_POWER_NUM    14

#if CONFIG_ESP_PHY_MULTIPLE_INIT_DATA_BIN
#define PHY_CRC_ALGORITHM 1
#define PHY_COUNTRY_CODE_LEN 2
#define PHY_INIT_DATA_TYPE_OFFSET 126
#define PHY_SUPPORT_MULTIPLE_BIN_OFFSET 125
#endif

extern const char phy_init_magic_pre[];
extern const esp_phy_init_data_t phy_init_data;
extern const char phy_init_magic_post[];

#if CONFIG_ESP_PHY_MULTIPLE_INIT_DATA_BIN
/**
 * @brief PHY init data control information structure
 */
typedef struct {
    uint8_t control_info_checksum[4];     /*!< 4-byte control information checksum */
    uint8_t multiple_bin_checksum[4];     /*!< 4-byte multiple bin checksum */
    uint8_t check_algorithm;              /*!< check algorithm */
    uint8_t version;                      /*!< PHY init data bin version */
    uint8_t number;                       /*!< PHY init data bin number */
    uint8_t length[2];                    /*!< Length of each PHY init data bin */
    uint8_t reserved[19];                 /*!< 19-byte reserved  */
} __attribute__ ((packed)) phy_control_info_data_t;

/**
 * @brief Country corresponds to PHY init data type structure
 */
typedef struct {
    NONSTRING_ATTR char cc[PHY_COUNTRY_CODE_LEN];
    uint8_t type;
} phy_country_to_bin_type_t;
#endif

#ifdef __cplusplus
}
#endif

#endif /* PHY_INIT_DATA_H */
