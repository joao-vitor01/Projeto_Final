#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"

#define MATRIX_SIZE 5
#define NUM_PIXELS 25
#define VRX_PIN 26
#define VRY_PIN 27
#define SW_PIN 22
#define WS2812_PIN 7

PIO pio = pio0;
uint sm = 0;

// Estrutura para armazenar posições da cobrinha
typedef struct {
    int x, y;
} Point;

Point snake[NUM_PIXELS];
int snake_length;
int dx, dy;
uint32_t led_buffer[NUM_PIXELS];
Point food;

// Função para reduzir o brilho dos LEDs (20%)
uint32_t dim_color(uint32_t color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    r *= 0.2;
    g *= 0.2;
    b *= 0.2;
    return (r << 16) | (g << 8) | b;
}

// Converte coordenadas para índice no buffer da matriz
int get_index(int x, int y) {
    return (4 - y) * MATRIX_SIZE + ((y % 2 == 0) ? x : (MATRIX_SIZE - 1 - x));
}

// Atualiza os LEDs da matriz
void refresh_display() {
    for (int i = 0; i < NUM_PIXELS; i++) {
        pio_sm_put_blocking(pio, sm, led_buffer[i] << 8u);
    }
}

// Define um LED específico no buffer
void set_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= MATRIX_SIZE || y < 0 || y >= MATRIX_SIZE) return;
    int index = get_index(x, y);
    led_buffer[index] = dim_color(color);
}

// Limpa todos os LEDs do buffer
void clear_display() {
    for (int i = 0; i < NUM_PIXELS; i++) {
        led_buffer[i] = 0x000000;
    }
}

// Verifica se um ponto está na cobra
int is_snake(Point p) {
    for (int i = 0; i < snake_length; i++) {
        if (snake[i].x == p.x && snake[i].y == p.y) {
            return 1;
        }
    }
    return 0;
}

// Gera comida em uma posição válida
void spawn_food() {
    do {
        food.x = rand() % MATRIX_SIZE;
        food.y = rand() % MATRIX_SIZE;
    } while (is_snake(food));
}

// Reinicia o jogo
void reset_game() {
    snake_length = 1;
    snake[0].x = 2;
    snake[0].y = 2;
    dx = 0;
    dy = 0;
    spawn_food();
}

// Atualiza a matriz com a posição da cobrinha e da comida
void update_display() {
    clear_display();
    for (int i = 0; i < snake_length; i++) {
        set_pixel(snake[i].x, snake[i].y, 0x00FF00); // Verde para a cobrinha
    }
    set_pixel(food.x, food.y, 0xFF0000); // Vermelho para a comida
    refresh_display();
}

// Lê o joystick e ajusta a direção
void read_joystick() {
    adc_select_input(0);
    uint16_t vrx = adc_read();
    adc_select_input(1);
    uint16_t vry = adc_read();

    static int last_dx = 0, last_dy = 0;

    if (vry > 3000 && last_dx == 0) { dx = -1; dy = 0; }  // Cima → Esquerda
    else if (vry < 1000 && last_dx == 0) { dx = 1; dy = 0; }  // Baixo → Direita
    else if (vrx < 1000 && last_dy == 0) { dx = 0; dy = 1; }  // Esquerda → Baixo
    else if (vrx > 3000 && last_dy == 0) { dx = 0; dy = -1; }  // Direita → Cima

    last_dx = dx;
    last_dy = dy;
}

// Atualiza a posição da cobrinha
void update_snake() {
    Point next = {snake[0].x + dx, snake[0].y + dy};

    // Verifica colisão com bordas ou consigo mesma
    if (next.x < 0 || next.x >= MATRIX_SIZE || next.y < 0 || next.y >= MATRIX_SIZE || is_snake(next)) {
        dx = 0;
        dy = 0;
        return;
    }

    // Verifica se comeu a comida antes de mover
    int ate_food = (next.x == food.x && next.y == food.y);

    // Move o corpo da cobrinha
    for (int i = snake_length; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    snake[0] = next;

    // Se comeu a comida, cresce e gera nova comida
    if (ate_food) {
        if (snake_length < NUM_PIXELS) {
            snake_length++;
        }
        spawn_food();
    }
}

// Verifica se o botão foi pressionado para reiniciar o jogo
void check_reset_button() {
    if (gpio_get(SW_PIN) == 0) {  // Botão pressionado (ativo em LOW)
        sleep_ms(300); // Tempo de debounce maior
        while (gpio_get(SW_PIN) == 0);  // Espera o botão ser solto
        reset_game();
    }
}


int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(VRX_PIN);
    adc_gpio_init(VRY_PIN);
    
    gpio_init(SW_PIN);
    gpio_set_dir(SW_PIN, GPIO_IN);
    gpio_pull_up(SW_PIN);  

    sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, false);

    reset_game(); // Inicia o jogo

    while (1) {
        read_joystick();
        update_snake();
        update_display();
        check_reset_button(); 
        sleep_ms(200);
    }
}
