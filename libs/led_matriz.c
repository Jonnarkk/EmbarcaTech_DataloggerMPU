#include "led_matriz.h"


static double ligar_todos_leds[25] = {
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0
};


// Estrutura do semáforo completo (para visualização)
static double apagar[25] = {
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0
};


// Função existente para converter RGB em valor de 32 bits (formato GRB)
// Rotina para definir a intensidade das cores do LED (RGB)
uint32_t matrix_rgb(double b, double r, double g){
  unsigned char R, G, B;
  R = r * 255; // Converte a intensidade de vermelho para 8 bits
  G = g * 255; // Converte a intensidade de verde para 8 bits
  B = b * 255; // Converte a intensidade de azul para 8 bits
  return (G << 24) | (R << 16) | (B << 8); // Retorna o valor RGB no formato 32 bits
}

void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b){
    for (int16_t i = 0; i < NUM_PIXELS; i++) {
        // Define a cor do LED com base no vetor de desenho
        if (desenho[24-i]>0.000) 
        {
            valor_led = matrix_rgb(b, r, g);
        }
        else
        {
            valor_led = matrix_rgb(0.0, 0.0, 0.0);
        }
        pio_sm_put_blocking(pio, sm, valor_led); // Envia o valor para a matriz de LEDs via PIO
    }
}

// Função para exibir o semáforo na matriz de LEDs
void apagar_matriz(PIO pio, uint sm){
    uint32_t valor_led;

    desenho_pio(apagar, valor_led, pio, sm, 0.0, 0.0, 0.0); // Vermelho (r=1.0)
}

void ligar_matriz_toda(PIO pio, uint sm, double r, double g, double b) {
    uint32_t valor_led_dummy = 0; // O valor real é definido dentro de desenho_pio
    
    // Usa o padrão 'padrao_todos_ligados' e os parâmetros de cor RGB
    desenho_pio(ligar_todos_leds, valor_led_dummy, pio, sm, r, g, b);
}

uint pio_init(PIO pio){
    // Ajusta o clock do RP2040 para 128 MHz (128000 kHz)
    set_sys_clock_khz(128000, false);

    // Carrega o programa PIO na memória do PIO e retorna o offset onde ele foi colocado
    uint offset = pio_add_program(pio, &pio_matriz_program);

    // Reserva um state machine livre (bloqueante) e retorna seu índice
    uint sm = pio_claim_unused_sm(pio, true);

    // Inicializa o state machine com o programa carregado, definindo pino de saída
    pio_matriz_program_init(pio, sm, offset, pino_matriz);

    return sm;
}