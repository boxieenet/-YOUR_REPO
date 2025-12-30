/*
 * embedded_controller.c
 *
 * Single-file demo suitable for an interview: simulates an embedded
 * controller pipeline — ADC sampling (with noise), moving-average filter,
 * duty-cycle mapping for PWM, a small ring buffer logger, and a CRC-8
 * helper for message integrity. Designed to compile on a PC with `gcc`.
 *
 * Build: gcc -std=c11 -O2 embedded_controller.c -o embedded_controller
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SAMPLE_COUNT 256
#define FILTER_LEN 8
#define LOG_CAP 16

/* Simple ring buffer for log entries (strings) */
typedef struct {
    char buf[LOG_CAP][128];
    int head;
    int count;
} ring_log_t;

static void log_init(ring_log_t *r) { r->head = 0; r->count = 0; }

static void log_push(ring_log_t *r, const char *s) {
    int idx = (r->head + r->count) % LOG_CAP;
    strncpy(r->buf[idx], s, sizeof(r->buf[idx]) - 1);
    r->buf[idx][sizeof(r->buf[idx]) - 1] = '\0';
    if (r->count < LOG_CAP) {
        r->count++;
    } else {
        r->head = (r->head + 1) % LOG_CAP;
    }
}

static void log_dump(const ring_log_t *r) {
    for (int i = 0; i < r->count; ++i) {
        int idx = (r->head + i) % LOG_CAP;
        puts(r->buf[idx]);
    }
}

/* CRC-8 (polynomial 0x07) — common small checksum used in embedded protocols */
static uint8_t crc8(const uint8_t *data, size_t len) {
    uint8_t crc = 0;
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (int b = 0; b < 8; ++b) {
            if (crc & 0x80) crc = (uint8_t)((crc << 1) ^ 0x07);
            else crc <<= 1;
        }
    }
    return crc;
}

/* Moving average filter state */
typedef struct {
    double buf[FILTER_LEN];
    int idx;
    int filled;
    double sum;
} movavg_t;

static void movavg_init(movavg_t *m) { memset(m, 0, sizeof(*m)); }

static double movavg_push(movavg_t *m, double x) {
    if (m->filled < FILTER_LEN) {
        m->buf[m->idx] = x;
        m->sum += x;
        m->idx = (m->idx + 1) % FILTER_LEN;
        m->filled++;
        return m->sum / m->filled;
    } else {
        m->sum -= m->buf[m->idx];
        m->buf[m->idx] = x;
        m->sum += x;
        m->idx = (m->idx + 1) % FILTER_LEN;
        return m->sum / FILTER_LEN;
    }
}

/* Map a 12-bit ADC value (0..4095) to PWM duty (0..100 percent) */
static int adc_to_duty(uint16_t adc) {
    if (adc >= 4095) return 100;
    return (int)((adc * 100u) / 4095u);
}

/* Simulate an ADC reading: a slow sine wave + random noise */
static uint16_t simulate_adc(double t_sec) {
    double freq = 0.5; /* 0.5 Hz sine */
    double v = 0.5 * (1.0 + sin(2.0 * M_PI * freq * t_sec)); /* 0..1 */
    /* scale to 12-bit */
    double adc = v * 4095.0;
    /* add small random noise */
    double noise = ((rand() % 201) - 100) * 0.5; /* -50..50 */
    adc += noise;
    if (adc < 0) adc = 0;
    if (adc > 4095) adc = 4095;
    return (uint16_t)adc;
}

int main(void) {
    printf("Embedded Controller Demo — single-file interview example\n");
    printf("Features: ADC simulation, moving-average filter, duty mapping, ring log, CRC-8\n\n");

    ring_log_t log;
    log_init(&log);
    movavg_t ma;
    movavg_init(&ma);

    srand((unsigned int)time(NULL));

    const double dt = 0.05; /* 50ms per sample (20Hz sample rate) */
    double t = 0.0;

    for (int i = 0; i < SAMPLE_COUNT; ++i) {
        uint16_t raw = simulate_adc(t);
        double filt = movavg_push(&ma, (double)raw);
        uint16_t filt_u = (uint16_t)filt;
        int duty = adc_to_duty(filt_u);

        /* prepare a small status message and compute CRC */
        uint8_t msg[4];
        msg[0] = (uint8_t)(raw >> 8);
        msg[1] = (uint8_t)(raw & 0xFF);
        msg[2] = (uint8_t)duty;
        msg[3] = (uint8_t)(filt_u & 0xFF);
        uint8_t c = crc8(msg, sizeof(msg));

        char line[128];
        snprintf(line, sizeof(line), "t=%.2fs raw=%4u filt=%4u duty=%3d%% crc=0x%02X",
                 t, raw, filt_u, duty, c);
        puts(line);
        log_push(&log, line);

        /* In a microcontroller this is where you'd update PWM registers */
        /* Show an example virtual action for high duty */
        if (duty > 80) {
            log_push(&log, "  -> Actuator: HIGH power (example)\0");
        }

        t += dt;
    }

    printf("\nRecent log (ring buffer):\n");
    log_dump(&log);

    printf("\nWhy this is interview-friendly:\n");
    printf(" - demonstrates signal conditioning (moving average)\n");
    printf(" - shows mapping from sensor domain to actuator command (ADC->PWM)\n");
    printf(" - includes small utility code (ring buffer, CRC) often seen in embedded codebases\n");
    printf(" - single-file, portable, easy to extend into hardware examples\n");

    return 0;
}
