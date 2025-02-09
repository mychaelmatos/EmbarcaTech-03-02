#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

// Definições para o I2C e Display OLED
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define led_pin_g 11
#define led_pin_b 12
#define led_pin_r 13

// Adicionar definições para WS2812
#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7

// Adicionar definições dos botões
const uint button_A = 5;
const uint button_B = 6;

// Adicionar variáveis para controle do estado dos LEDs
static volatile bool led_green_state = false;
static volatile bool led_blue_state = false;
static volatile uint32_t last_time = 0;

// Adicionar arrays de padrões dos números
const bool number_patterns[10][NUM_PIXELS] = {
    {0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0}, // Padrão para o número 0

    {0, 1, 1, 1, 0,
     0, 0, 1, 0, 0,
     0, 0, 1, 0, 0,
     0, 1, 1, 0, 0,
     0, 0, 1, 0, 0}, // Padrão para o número 1

    {0, 1, 1, 1, 0,
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0}, // Padrão para o número 2

    {0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0}, // Padrão para o número 3

    {0, 1, 0, 0, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 0, 1, 0}, // Padrão para o número 4

    {0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0}, // Padrão para o número 5

    {0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0}, // Padrão para o número 6

    {0, 1, 0, 0, 0,
     0, 0, 0, 1, 0,
     1, 1, 1, 0, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0}, // Padrão para o número 7

    {0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0}, // Padrão para o número 8

    {0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0} // 0
    // ... copiar os outros padrões do matriz5x5.c ...
};

// Adicionar variável global para o display OLED
static ssd1306_t ssd;  // Declare a variável ssd como global

// Adicionar funções auxiliares para WS2812
static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void display_number_matrix(int number) {
    uint32_t color = urgb_u32(100, 0, 0);
    for (int i = 0; i < NUM_PIXELS; i++) {
        if (number_patterns[number][i]) {
            put_pixel(color);
        } else {
            put_pixel(0);
        }
    }
}

// Função de interrupção para os botões
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (current_time - last_time > 400000) { // 400ms debouncing
        last_time = current_time;

        if (gpio == button_A) {
            led_green_state = !led_green_state;
            gpio_put(led_pin_g, led_green_state);
            
            // Mensagem no display
            ssd1306_fill(&ssd, false);
            ssd1306_draw_string(&ssd, led_green_state ? "LED Verde: ON" : "LED Verde: OFF", 10, 20);
            ssd1306_send_data(&ssd);
            
            // Mensagem no Serial Monitor
            printf("LED Verde %s\n", led_green_state ? "ligado" : "desligado");
        }
        else if (gpio == button_B) {
            led_blue_state = !led_blue_state;
            gpio_put(led_pin_b, led_blue_state);
            
            // Mensagem no display
            ssd1306_fill(&ssd, false);
            ssd1306_draw_string(&ssd, led_blue_state ? "LED Azul: ON" : "LED Azul: OFF", 10, 20);
            ssd1306_send_data(&ssd);
            
            // Mensagem no Serial Monitor
            printf("LED Azul %s\n", led_blue_state ? "ligado" : "desligado");
        }
    }
}

int main()
{
    stdio_init_all(); // Inicializa comunicação USB CDC para monitor serial

    // Inicialização do I2C
    i2c_init(I2C_PORT, 400 * 1000);  // Inicializa I2C com 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Configura os pinos dos LEDs como saída
    gpio_init(led_pin_r);
    gpio_set_dir(led_pin_r, GPIO_OUT);
    gpio_put(led_pin_r, 0); // Inicialmente desligado

    gpio_init(led_pin_g);
    gpio_set_dir(led_pin_g, GPIO_OUT);
    gpio_put(led_pin_g, 0); // Inicialmente desligado

    gpio_init(led_pin_b);
    gpio_set_dir(led_pin_b, GPIO_OUT);
    gpio_put(led_pin_b, 0); // Inicialmente desligado

    // Inicialização do display
    ssd1306_init(&ssd, 128, 64, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    
    // Limpa o display
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Escreve as mensagens fixas
    ssd1306_draw_string(&ssd, " TAREFA  U4C6", 8, 10);
    ssd1306_draw_string(&ssd, " FEITA  POR", 20, 30);
    ssd1306_draw_string(&ssd, "MYCHAEL MATOS", 15, 48);
    ssd1306_send_data(&ssd);

    printf("RP2040 inicializado. Envie 'r', 'g' ou 'b' para alternar os LEDs.\n");

    // Área para caracteres digitados
    int x = 0;
    char texto[2] = {0, 0};

    // Adicionar inicialização do WS2812
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    // Adicionar inicialização dos botões
    gpio_init(button_A);
    gpio_set_dir(button_A, GPIO_IN);
    gpio_pull_up(button_A);
    
    gpio_init(button_B);
    gpio_set_dir(button_B, GPIO_IN);
    gpio_pull_up(button_B);

    // Configurar interrupções dos botões
    gpio_set_irq_enabled_with_callback(button_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(button_B, GPIO_IRQ_EDGE_FALL, true);

    while (true)
    {
        if (stdio_usb_connected())
        {
            char c;
            if (scanf("%c", &c) == 1)
            {
                printf("Recebido: '%c'\n", c);

                // Limpa a área superior do display
                ssd1306_rect(&ssd, 0, 0, 128, 8, false, false);
                
                // Escreve o caractere recebido
                texto[0] = c;
                ssd1306_draw_string(&ssd, texto, x, 0);
                ssd1306_send_data(&ssd);
                
                // Atualiza a posição X
                x += 8;
                if (x >= 120) {
                    x = 0;
                }

                // Verifica se é um número e mostra o padrão correspondente
                if (c >= '0' && c <= '9') {
                    int number = c - '0';
                    display_number_matrix(number);
                } else {
                    // Se não for um número, apaga todos os LEDs da matriz
                    for (int i = 0; i < NUM_PIXELS; i++) {
                        put_pixel(0);
                    }
                }

                // Limpa o display e mostra o caractere no centro
                ssd1306_fill(&ssd, false);
                ssd1306_draw_char(&ssd, c, 30, 20);
                ssd1306_send_data(&ssd);
            }
        }
        sleep_ms(10);  // Pequeno delay para não sobrecarregar o processador
    }

    return 0;
}
 