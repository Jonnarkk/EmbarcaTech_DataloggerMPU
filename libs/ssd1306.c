#include "ssd1306.h"
#include "font.h"
#include <string.h>

void ssd1306_init(ssd1306_t *ssd, uint8_t width, uint8_t height, bool external_vcc, uint8_t address, i2c_inst_t *i2c) {
  ssd->width = width;
  ssd->height = height;
  ssd->pages = height / 8U;
  ssd->address = address;
  ssd->i2c_port = i2c;
  ssd->bufsize = ssd->pages * ssd->width + 1;
  ssd->ram_buffer = calloc(ssd->bufsize, sizeof(uint8_t));
  ssd->ram_buffer[0] = 0x40;
  ssd->port_buffer[0] = 0x80;
}

void ssd1306_config(ssd1306_t *ssd) {
  ssd1306_command(ssd, SET_DISP | 0x00);
  ssd1306_command(ssd, SET_MEM_ADDR);
  ssd1306_command(ssd, 0x01);
  ssd1306_command(ssd, SET_DISP_START_LINE | 0x00);
  ssd1306_command(ssd, SET_SEG_REMAP | 0x01);
  ssd1306_command(ssd, SET_MUX_RATIO);
  ssd1306_command(ssd, HEIGHT - 1);
  ssd1306_command(ssd, SET_COM_OUT_DIR | 0x08);
  ssd1306_command(ssd, SET_DISP_OFFSET);
  ssd1306_command(ssd, 0x00);
  ssd1306_command(ssd, SET_COM_PIN_CFG);
  ssd1306_command(ssd, 0x12);
  ssd1306_command(ssd, SET_DISP_CLK_DIV);
  ssd1306_command(ssd, 0x80);
  ssd1306_command(ssd, SET_PRECHARGE);
  ssd1306_command(ssd, 0xF1);
  ssd1306_command(ssd, SET_VCOM_DESEL);
  ssd1306_command(ssd, 0x30);
  ssd1306_command(ssd, SET_CONTRAST);
  ssd1306_command(ssd, 0xFF);
  ssd1306_command(ssd, SET_ENTIRE_ON);
  ssd1306_command(ssd, SET_NORM_INV);
  ssd1306_command(ssd, SET_CHARGE_PUMP);
  ssd1306_command(ssd, 0x14);
  ssd1306_command(ssd, SET_DISP | 0x01);
}

void ssd1306_command(ssd1306_t *ssd, uint8_t command) {
  ssd->port_buffer[1] = command;
  i2c_write_blocking(
    ssd->i2c_port,
    ssd->address,
    ssd->port_buffer,
    2,
    false
  );
}

void ssd1306_send_data(ssd1306_t *ssd) {
  ssd1306_command(ssd, SET_COL_ADDR);
  ssd1306_command(ssd, 0);
  ssd1306_command(ssd, ssd->width - 1);
  ssd1306_command(ssd, SET_PAGE_ADDR);
  ssd1306_command(ssd, 0);
  ssd1306_command(ssd, ssd->pages - 1);
  i2c_write_blocking(
    ssd->i2c_port,
    ssd->address,
    ssd->ram_buffer,
    ssd->bufsize,
    false
  );
}

void ssd1306_pixel(ssd1306_t *ssd, uint8_t x, uint8_t y, bool value) {
  uint16_t index = (y >> 3) + (x << 3) + 1;
  uint8_t pixel = (y & 0b111);
  if (value)
    ssd->ram_buffer[index] |= (1 << pixel);
  else
    ssd->ram_buffer[index] &= ~(1 << pixel);
}

/*
void ssd1306_fill(ssd1306_t *ssd, bool value) {
  uint8_t byte = value ? 0xFF : 0x00;
  for (uint8_t i = 1; i < ssd->bufsize; ++i)
    ssd->ram_buffer[i] = byte;
}*/

void ssd1306_fill(ssd1306_t *ssd, bool value) {
    // Itera por todas as posições do display
    for (uint8_t y = 0; y < ssd->height; ++y) {
        for (uint8_t x = 0; x < ssd->width; ++x) {
            ssd1306_pixel(ssd, x, y, value);
        }
    }
}



void ssd1306_rect(ssd1306_t *ssd, uint8_t top, uint8_t left, uint8_t width, uint8_t height, bool value, bool fill) {
  for (uint8_t x = left; x < left + width; ++x) {
    ssd1306_pixel(ssd, x, top, value);
    ssd1306_pixel(ssd, x, top + height - 1, value);
  }
  for (uint8_t y = top; y < top + height; ++y) {
    ssd1306_pixel(ssd, left, y, value);
    ssd1306_pixel(ssd, left + width - 1, y, value);
  }

  if (fill) {
    for (uint8_t x = left + 1; x < left + width - 1; ++x) {
      for (uint8_t y = top + 1; y < top + height - 1; ++y) {
        ssd1306_pixel(ssd, x, y, value);
      }
    }
  }
}

void ssd1306_line(ssd1306_t *ssd, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool value) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (true) {
        ssd1306_pixel(ssd, x0, y0, value); // Desenha o pixel atual

        if (x0 == x1 && y0 == y1) break; // Termina quando alcança o ponto final

        int e2 = err * 2;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}


void ssd1306_hline(ssd1306_t *ssd, uint8_t x0, uint8_t x1, uint8_t y, bool value) {
  for (uint8_t x = x0; x <= x1; ++x)
    ssd1306_pixel(ssd, x, y, value);
}

void ssd1306_vline(ssd1306_t *ssd, uint8_t x, uint8_t y0, uint8_t y1, bool value) {
  for (uint8_t y = y0; y <= y1; ++y)
    ssd1306_pixel(ssd, x, y, value);
}

// Função para desenhar um caractere no display
void ssd1306_draw_char(ssd1306_t *ssd, char c, uint8_t x, uint8_t y) {
  uint8_t index = (uint8_t)c;  // Obtém o índice do caractere
  if(index == 135) {
    index = 127;
  } else {
    if(index >= 129){
      index = 0;
    }
  }
  
  const uint8_t *char_data = font[index];  // Obtém os dados do caractere a partir da fonte

  // Desenha cada linha do caractere
  for (uint8_t i = 0; i < 8; i++) {
      uint8_t line = char_data[i];  // Linha atual do caractere
      for (uint8_t j = 0; j < 8; j++) {
          if (line & 0x01) {  // Verifica o bit mais à direita
              ssd1306_pixel(ssd, x + j, y + i, 1);  // Desenha o pixel
          } else {
              ssd1306_pixel(ssd, x + j, y + i, 0);  // Apaga o pixel
          }
          line >>= 1;  // Desloca para o próximo bit
      }
  }
}

// Função para desenhar uma string
void ssd1306_draw_string(ssd1306_t *ssd, const char *str, uint8_t x, uint8_t y)
{
  while (*str)
  {
    ssd1306_draw_char(ssd, *str++, x, y);
    x += 8;
    if (x + 8 >= ssd->width)
    {
      x = 0;
      y += 8;
    }
    if (y + 8 >= ssd->height)
    {
      break;
    }
  }
}

// Função para centralizar o texto no display de 128x64 pixels
int centralizar_texto(const char *str) {
  int largura_texto = strlen(str) * 8;  // Cada caractere ocupa 8 pixels de largura
  return (128 - largura_texto) / 2;      // Calcula a posição central
}

void display_init(ssd1306_t *ssd) {
    i2c_init(I2C_PORT_DISP, 400 * 1000);
 
    gpio_set_function(I2C_SDA_DISP, GPIO_FUNC_I2C);                    // Configura o pino para a função I2C
    gpio_set_function(I2C_SCL_DISP, GPIO_FUNC_I2C);                    // Configura o pino para a função I2C
    gpio_pull_up(I2C_SDA_DISP);                                        // Habilita pullup para o pino do SDA
    gpio_pull_up(I2C_SCL_DISP);                                        // Habilita pullup para o pino do SCL

    ssd1306_init(ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT_DISP); // Inicializa o display
    ssd1306_config(ssd);                                         // Configura o display
    ssd1306_send_data(ssd);                                      // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);
}

void tela_inicial(ssd1306_t *ssd){
  ssd1306_fill(ssd, false);   // Limpa a tela

  ssd1306_rect(ssd, 0, 0, 128, 64, true, false);
  ssd1306_draw_string(ssd, "DataLogger", centralizar_texto("DataLogger"), 3);
  ssd1306_draw_string(ssd, "Aperte A", centralizar_texto("Aperte A"), 25);
  ssd1306_draw_string(ssd, "Para mudar modo", centralizar_texto("Para mudar modo"), 35);

  ssd1306_send_data(ssd);
}

void op_concluida(ssd1306_t *ssd){
  ssd1306_fill(ssd, false);   // Limpa a tela

  ssd1306_rect(ssd, 0, 0, 128, 64, true, false);
  ssd1306_draw_string(ssd, "Operacao", centralizar_texto("Operacao"), 25);
  ssd1306_draw_string(ssd, "Concluida!", centralizar_texto("Concluida"), 35);

  ssd1306_send_data(ssd);
}

void modo_atual(ssd1306_t *ssd, bool modo){
  ssd1306_fill(ssd, false);   // Limpa a tela

  ssd1306_rect(ssd, 0, 0, 128, 64, true, false);
  ssd1306_draw_string(ssd, "Modo Atual:", centralizar_texto("Modo Atual:"), 25);

  if(modo)
    ssd1306_draw_string(ssd, "Cartao Micro SD", centralizar_texto("Cartao Micro SD"), 35);
  else  
    ssd1306_draw_string(ssd, "ADC", centralizar_texto("ADC"), 35);

  ssd1306_send_data(ssd);
}

void montagem_sd(ssd1306_t *ssd){
  ssd1306_fill(ssd, false);   // Limpa a tela

  ssd1306_rect(ssd, 0, 0, 128, 64, true, false);  // Retângulo na tela
  ssd1306_draw_string(ssd, "Montando", centralizar_texto("Montando"), 25);
  ssd1306_draw_string(ssd, "Cartao SD", centralizar_texto("Cartao SD"), 35);

  ssd1306_send_data(ssd);
}

void atualizar_display_status(ssd1306_t *ssd, Operacao_t status) {
    ssd1306_fill(ssd, false); // Limpa o display
    char *mensagem = "";

    switch (status) {
        case OPERACAO_MONTANDO:
            mensagem = "Montando SD...";
            break;
        case OPERACAO_DESMONTANDO:
            mensagem = "Desmontando SD";
            break;
        case OPERACAO_LISTAGEM:
            mensagem = "Listando Dir...";
            break;
        case OPERACAO_LEITURA:
            mensagem = "Lendo dados...";
            break;
        case OPERACAO_ESPACO:
            mensagem = "Mostrando Esp...";
            break;
        case OPERACAO_ADC:
            mensagem = "Gravando ADC...";
            break;
        case OPERACAO_FORMATACAO:
            mensagem = "Formatando...";
            break;
        case OPERACAO_MPU:
            mensagem = "Gravando MPU...";
            break;
        case OPERACAO_HELP:
            mensagem = "Menu na serial";
            break;
        case OPERACAO_MODO_ADC:
            mensagem = "Modo: ADC";
            break;
        case OPERACAO_MODO_MPU:
            mensagem = "Modo: MPU";
            break;
    }

    // Centraliza, desenha a mensagem e desenha retângulo
    ssd1306_rect(ssd, 0, 0, 128, 64, true, false);  // Retângulo na tela
    ssd1306_draw_string(ssd, mensagem, centralizar_texto(mensagem), 28); // Supondo que você tenha uma função para centralizar
    ssd1306_send_data(ssd);
}