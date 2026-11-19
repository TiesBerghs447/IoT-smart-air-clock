#include "microfoon.h"

#include <driver/i2s.h>

#define I2S_WS   12
#define I2S_SD   13
#define I2S_SCK  11

void initMicrophone()
{
    i2s_config_t config =
    {
        .mode =
            (i2s_mode_t)(
                I2S_MODE_MASTER |
                I2S_MODE_RX
            ),

        .sample_rate = 16000,

        .bits_per_sample =
            I2S_BITS_PER_SAMPLE_32BIT,  

        .channel_format =
            I2S_CHANNEL_FMT_ONLY_LEFT,

        .communication_format =
            I2S_COMM_FORMAT_STAND_I2S,

        .intr_alloc_flags = 0,

        .dma_buf_count = 8,

        .dma_buf_len = 64,

        .use_apll = false,

        .tx_desc_auto_clear = false,

        .fixed_mclk = 0
    };

    i2s_pin_config_t pins =
    {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = -1,
        .data_in_num = I2S_SD
    };

    i2s_driver_install(
        I2S_NUM_0,
        &config,
        0,
        NULL
    );

    i2s_set_pin(
        I2S_NUM_0,
        &pins
    );
}

int readSoundLevel()
{
    int32_t sample;

    size_t bytesRead;

    i2s_read(
        I2S_NUM_0,
        &sample,
        sizeof(sample),
        &bytesRead,
        portMAX_DELAY
    );

    sample >>= 14;

    return abs(sample);
}