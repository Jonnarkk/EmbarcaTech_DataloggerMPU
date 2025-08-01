#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define WIDTH 128
#define HEIGHT 64

#define I2C_PORT_DISP i2c1
#define I2C_SDA_DISP 14
#define I2C_SCL_DISP 15
#define endereco 0x3C

typedef enum {
  SET_CONTRAST = 0x81,
  SET_ENTIRE_ON = 0xA4,
  SET_NORM_INV = 0xA6,
  SET_DISP = 0xAE,
  SET_MEM_ADDR = 0x20,
  SET_COL_ADDR = 0x21,
  SET_PAGE_ADDR = 0x22,
  SET_DISP_START_LINE = 0x40,
  SET_SEG_REMAP = 0xA0,
  SET_MUX_RATIO = 0xA8,
  SET_COM_OUT_DIR = 0xC0,
  SET_DISP_OFFSET = 0xD3,
  SET_COM_PIN_CFG = 0xDA,
  SET_DISP_CLK_DIV = 0xD5,
  SET_PRECHARGE = 0xD9,
  SET_VCOM_DESEL = 0xDB,
  SET_CHARGE_PUMP = 0x8D
} ssd1306_command_t;

typedef enum {
  OPERACAO_MONTANDO,
  OPERACAO_DESMONTANDO,
  OPERACAO_LISTAGEM,
  OPERACAO_LEITURA,
  OPERACAO_ESPACO,
  OPERACAO_ADC,
  OPERACAO_FORMATACAO,
  OPERACAO_MPU,
  OPERACAO_HELP,
  OPERACAO_MODO_ADC,
  OPERACAO_MODO_MPU
}Operacao_t;

typedef struct {
  uint8_t width, height, pages, address;
  i2c_inst_t *i2c_port;
  bool external_vcc;
  uint8_t *ram_buffer;
  size_t bufsize;
  uint8_t port_buffer[2];
} ssd1306_t;

void ssd1306_init(ssd1306_t *ssd, uint8_t width, uint8_t height, bool external_vcc, uint8_t address, i2c_inst_t *i2c);
void ssd1306_config(ssd1306_t *ssd);
void ssd1306_command(ssd1306_t *ssd, uint8_t command);
void ssd1306_send_data(ssd1306_t *ssd);

void ssd1306_pixel(ssd1306_t *ssd, uint8_t x, uint8_t y, bool value);
void ssd1306_fill(ssd1306_t *ssd, bool value);
void ssd1306_rect(ssd1306_t *ssd, uint8_t top, uint8_t left, uint8_t width, uint8_t height, bool value, bool fill);
void ssd1306_line(ssd1306_t *ssd, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool value);
void ssd1306_hline(ssd1306_t *ssd, uint8_t x0, uint8_t x1, uint8_t y, bool value);
void ssd1306_vline(ssd1306_t *ssd, uint8_t x, uint8_t y0, uint8_t y1, bool value);
void ssd1306_draw_char(ssd1306_t *ssd, char c, uint8_t x, uint8_t y);
void ssd1306_draw_string(ssd1306_t *ssd, const char *str, uint8_t x, uint8_t y);
int centralizar_texto(const char *str);
void display_init(ssd1306_t *ssd);
void tela_inicial(ssd1306_t *ssd);
void op_concluida(ssd1306_t *ssd);
void modo_atual(ssd1306_t *ssd, bool modo);
void montagem_sd(ssd1306_t *ssd);
void atualizar_display_status(ssd1306_t *disp, Operacao_t status);