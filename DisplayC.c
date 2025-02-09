#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C
#define TEMPO 100

const uint ledGreen_pin = 11; // LED verde
const uint ledBlue_pin = 12; // LED azul
const uint button_A = 5; // Botão A 
const uint button_B = 6; // Botão B 

static volatile uint32_t last_time_button_A = 0;
static volatile uint32_t last_time_button_B = 0;

// Prototipo da função de interrupção
static void gpio_irq_handler_A(uint gpio, uint32_t events);
static void gpio_irq_handler_B(uint gpio, uint32_t events);

// Função para configurar I2C e o display
static void init_display(ssd1306_t *ssd) {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    ssd1306_init(ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(ssd);
    ssd1306_send_data(ssd);
    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);
}

// Função para atualizar o display
static void update_display(ssd1306_t *ssd, const char *message, bool cor) {
    ssd1306_fill(ssd, false);
    ssd1306_draw_string(ssd, message, 10, 30);
    ssd1306_send_data(ssd);
}

// Função callback de interrupção
static void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == button_A) {
        gpio_irq_handler_A(gpio, events);
    } else if (gpio == button_B) {
        gpio_irq_handler_B(gpio, events);
    }
}

int main() {
    stdio_init_all(); // Inicializa comunicação USB CDC para monitor serial

    // Inicialização de LEDs e botões
    gpio_init(ledGreen_pin);
    gpio_set_dir(ledGreen_pin, GPIO_OUT);
    gpio_init(ledBlue_pin);
    gpio_set_dir(ledBlue_pin, GPIO_OUT);

    gpio_init(button_A);
    gpio_set_dir(button_A, GPIO_IN);
    gpio_pull_up(button_A);
    gpio_set_irq_enabled_with_callback(button_A, GPIO_IRQ_EDGE_FALL, true, gpio_callback);

    gpio_init(button_B);
    gpio_set_dir(button_B, GPIO_IN);
    gpio_pull_up(button_B);
    gpio_set_irq_enabled_with_callback(button_B, GPIO_IRQ_EDGE_FALL, true, gpio_callback);

    // Inicializa o display
    ssd1306_t ssd;
    init_display(&ssd);
    
    bool cor = true;
    printf("RP2040 inicializado. Envie 1 caractere ou pressione os botões.\n");

    while (true) {
        if (stdio_usb_connected()) { 
            char c;
            if (scanf("%c", &c) == 1) { 
                printf("Recebido: '%c'\n", c);
                char str[2] = {c, '\0'};
                char display_str[50];
                sprintf(display_str, "Recebido %s", str);
                
                cor = !cor;
                update_display(&ssd, display_str, cor);
            }
        }
        sleep_ms(TEMPO);
    }
}

// Função de interrupção para o Botão A
void gpio_irq_handler_A(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    if (current_time - last_time_button_A > 300000) { 
        last_time_button_A = current_time;
        gpio_put(ledGreen_pin, !gpio_get(ledGreen_pin));

        ssd1306_t ssd;
        init_display(&ssd);

        char display_str[50];
        sprintf(display_str, "LED %s", gpio_get(ledGreen_pin) ? "Ligado" : "Desligado");
        update_display(&ssd, display_str, true);
        
        printf("O LED verde foi: %s\n", gpio_get(ledGreen_pin) ? "Ligado" : "Desligado");
    }
}

// Função de interrupção para o Botão B
void gpio_irq_handler_B(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    if (current_time - last_time_button_B > 300000) { 
        last_time_button_B = current_time;
        gpio_put(ledBlue_pin, !gpio_get(ledBlue_pin));

        ssd1306_t ssd;
        init_display(&ssd);

        char display_str[50];
        sprintf(display_str, "LED %s", gpio_get(ledBlue_pin) ? "Ligado" : "Desligado");
        update_display(&ssd, display_str, false);

        printf("O LED azul foi: %s\n", gpio_get(ledBlue_pin) ? "Ligado" : "Desligado");
    }
}
