#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "hardware/i2c.h"    
#include "inc/ssd1306_i2c.h"

#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7
#define NUM_DIGITS 10
#define TEMPO 1000
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define LED_PIN_GREEN 11 
#define LED_PIN_RED 13
#define BUTTON_PIN_A 5 // GPIO 5 para o botão A
#define BUTTON_PIN_B 6 // GPIO 6 para o botão B

const uint8_t numbers[NUM_DIGITS][NUM_PIXELS] = {
    {1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,1,1,1,1}, // 0
    {1,1,1,1,1, 0,0,1,0,0, 0,0,1,0,1, 0,1,1,0,0, 0,0,1,0,0}, // 1
    {1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1}, // 2
    {1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1}, // 3
    {1,0,0,0,0, 0,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1}, // 4
    {1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1}, // 5
    {1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1}, // 6
    {0,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 0,0,0,0,1, 1,1,1,1,1}, // 7
    {1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1}, // 8
    {1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1}  // 9
};

uint32_t led_buffer[NUM_PIXELS] = {0};
uint8_t current_digit = 0;
uint8_t selected_r = 0, selected_g = 0, selected_b = 10;
const uint ledBlue_pin = 12;
uint8_t selected_number = 0; // Variável para armazenar o número selecionado
uint8_t random_numbers[10]; // Vetor para armazenar a sequência gerada aleatoriamente
uint8_t selected_sequence[10]; // Vetor para armazenar a sequência selecionada pelo usuário
int selected_index = 0; // Índice de onde estamos na sequência do usuário
int sequence_length = 2; // Começa com 2 números na sequência

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void update_led_buffer() {
    for (int i = 0; i < NUM_PIXELS; i++) {
        if (numbers[current_digit][i] == 1) {
            led_buffer[i] = urgb_u32(selected_r, selected_g, selected_b);
        } else {
            led_buffer[i] = 0;
        }
    }
}

void set_leds_from_buffer() {
    for (int i = 0; i < NUM_PIXELS; i++) {
        put_pixel(led_buffer[i]);
    }
}

// Função para atualizar o display
static void update_display(ssd1306_t *ssd, const char *message, bool cor) {
    ssd1306_fill(ssd, false);
    ssd1306_draw_string(ssd, message, 10, 30);
    ssd1306_send_data(ssd);
}

// Função para comparar as sequências
bool compare_sequences(uint8_t *seq1, uint8_t *seq2, int length) {
    for (int i = 0; i < length; i++) {
        if (seq1[i] != seq2[i]) {
            return false; // Se algum número não coincidir, retorna falso
        }
    }
    return true; // Se todas as posições coincidirem, retorna verdadeiro
}

int main() {
    stdio_init_all();
    // Inicializa os leds
    gpio_init(ledBlue_pin);
    gpio_set_dir(ledBlue_pin, GPIO_OUT);
    gpio_init(LED_PIN_RED);
    gpio_set_dir(LED_PIN_RED, GPIO_OUT);
    gpio_init(LED_PIN_GREEN);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);

    // Inicializa os botões
    gpio_init(BUTTON_PIN_A);
    gpio_set_dir(BUTTON_PIN_A, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_A); // Ativa o pull-up interno para o botão A

    gpio_init(BUTTON_PIN_B);
    gpio_set_dir(BUTTON_PIN_B, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_B); // Ativa o pull-up interno para o botão B

    // Inicializa o barramento I2C para comunicação com o display OLED
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    // Inicializa o display OLED
    ssd1306_t ssd;
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    bool cor = true;

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    srand(time(NULL)); // Inicializa a semente de números aleatórios

    // Gera a sequência inicial de números aleatórios
    for (int i = 0; i < sequence_length; i++) {
        random_numbers[i] = rand() % NUM_DIGITS; // Gera número aleatório entre 0 e 9
    }

    // Exibe os números aleatórios no display e LEDs antes de entrar no loop principal
    for (int i = 0; i < sequence_length; i++) {

        // Atualiza o conteúdo do display com animações
        ssd1306_fill(&ssd, !cor); // Limpa o display
        ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
        ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 10); // Desenha uma string
        ssd1306_draw_string(&ssd, "JOGO DE MEMORIA", 4, 48); // Desenha uma string      
        ssd1306_send_data(&ssd); // Atualiza o display
        
        current_digit = random_numbers[i];

        update_led_buffer();
        set_leds_from_buffer();

        gpio_put(ledBlue_pin, true);
        sleep_ms(TEMPO);
        gpio_put(ledBlue_pin, false);
        sleep_ms(TEMPO);

    }

    // Agora entra no loop principal, onde o display só atualiza o texto
    while (1) {
        // Verifica o estado do botão A (GPIO 5)
        if (!gpio_get(BUTTON_PIN_A)) { // Se o botão A for pressionado
            current_digit = (current_digit + 1) % NUM_DIGITS; // Incrementa o número de 0 a 9
            update_led_buffer();
            set_leds_from_buffer();
            sleep_ms(200); // Debounce para evitar múltiplos incrementos
        }

        // Verifica o estado do botão B (GPIO 6)
        if (!gpio_get(BUTTON_PIN_B)) { // Se o botão B for pressionado
            selected_sequence[selected_index] = current_digit; // Armazena o número selecionado na sequência do usuário
            selected_index++;

            char message[16];
            ssd1306_fill(&ssd, !cor); // Limpa o display
            ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
            ssd1306_draw_string(&ssd, "SELECIONADO: ", 20, 20); // Desenha uma string
            sprintf(message, "%d", selected_sequence[selected_index - 1]);
            ssd1306_draw_string(&ssd, message, 58, 40);
            ssd1306_send_data(&ssd); // Atualiza o display

            sleep_ms(400); // Debounce para evitar múltiplas seleções
        }

        // Verifica se o usuário terminou a sequência (quando a sequência tiver o tamanho esperado)
        if (selected_index == sequence_length) {
            // Compara a sequência gerada com a sequência do usuário
            if (compare_sequences(random_numbers, selected_sequence, sequence_length)) {
                // Atualiza o conteúdo do display com animações
                ssd1306_fill(&ssd, !cor); // Limpa o display
                ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
                ssd1306_draw_string(&ssd, "CORRETO", 36, 30); // Desenha uma string      
                ssd1306_send_data(&ssd); // Atualiza o display
                gpio_put(LED_PIN_GREEN, 1); // Acende o LED vermelho
                sleep_ms(TEMPO);
                gpio_put(LED_PIN_GREEN, 0); // Acende o LED vermelho
        
                sequence_length++; // Aumenta a sequência
            } else {
                // Atualiza o conteúdo do display com animações
                ssd1306_fill(&ssd, !cor); // Limpa o display
                ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
                ssd1306_draw_string(&ssd, "ERRADO", 38, 30); // Desenha uma string      
                ssd1306_send_data(&ssd); // Atualiza o display
                sequence_length = 2; // Reseta para 2 números
                gpio_put(LED_PIN_RED, 1); // Acende o LED vermelho
                sleep_ms(TEMPO);
                gpio_put(LED_PIN_RED, 0); // Acende o LED vermelho
            }

            // Reseta a sequência selecionada
            selected_index = 0;

            // Aguarda 2 segundos para mostrar o resultado e gera uma nova sequência
            sleep_ms(2000);

            // Gera uma nova sequência aleatória com o comprimento atual
            for (int i = 0; i < sequence_length; i++) {
                random_numbers[i] = rand() % NUM_DIGITS; // Gera número aleatório entre 0 e 9
            }

            // Exibe a nova sequência aleatória
            for (int i = 0; i < sequence_length; i++) {
                
                // Atualiza o conteúdo do display com animações
                ssd1306_fill(&ssd, !cor); // Limpa o display
                ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
                ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 10); // Desenha uma string
                ssd1306_draw_string(&ssd, "JOGO DE MEMORIA", 4, 48); // Desenha uma string      
                ssd1306_send_data(&ssd); // Atualiza o display
                
                current_digit = random_numbers[i];

                update_led_buffer();
                set_leds_from_buffer();

                gpio_put(ledBlue_pin, true);
                sleep_ms(TEMPO);
                gpio_put(ledBlue_pin, false);
                sleep_ms(TEMPO);

            }
        }
    }
}
