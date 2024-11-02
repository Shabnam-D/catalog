#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cJSON.h"

typedef struct {
    int x;
    unsigned long long y;
} Point;

// Function to convert a string in any base to an integer
unsigned long long convert_to_base(const char *value, int base) {
    return strtoull(value, NULL, base);
}

// Function to read JSON from a file and parse points
int parse_json(const char *filename, Point *points, int *k) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Unable to open file: %s\n", filename);
        return -1;
    }

    // Load JSON from file
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *data = (char *)malloc(length + 1);
    fread(data, 1, length, file);
    fclose(file);
    data[length] = '\0';

    // Parse JSON
    cJSON *json = cJSON_Parse(data);
    if (!json) {
        fprintf(stderr, "Error parsing JSON\n");
        free(data);
        return -1;
    }
    free(data);

    cJSON *keys = cJSON_GetObjectItem(json, "keys");
    *k = cJSON_GetObjectItem(keys, "k")->valueint;

    int count = 0;
    cJSON *item;
    cJSON_ArrayForEach(item, json) {
        if (strcmp(item->string, "keys") != 0 && count < *k) {
            int x = atoi(item->string);
            int base = cJSON_GetObjectItem(item, "base")->valueint;
            const char *value = cJSON_GetObjectItem(item, "value")->valuestring;

            points[count].x = x;
            points[count].y = convert_to_base(value, base);
            count++;
        }
    }
    cJSON_Delete(json);
    return count;
}

// Lagrange interpolation to find the constant term (y-intercept)
unsigned long long lagrange_interpolation(Point *points, int k) {
    unsigned long long result = 0;

    for (int i = 0; i < k; i++) {
        unsigned long long term = points[i].y;
        for (int j = 0; j < k; j++) {
            if (i != j) {
                term = term * (0 - points[j].x) / (points[i].x - points[j].x);
            }
        }
        result += term;
    }
    return result;
}

int main() {
    int k1, k2;
    Point points1[10], points2[10];

    // Parse input JSON files
    int count1 = parse_json("input1.json", points1, &k1);
    int count2 = parse_json("input2.json", points2, &k2);

    if (count1 < k1 || count2 < k2) {
        fprintf(stderr, "Not enough points to solve the polynomial\n");
        return 1;
    }

    // Calculate the constant term using Lagrange interpolation
    unsigned long long constant1 = lagrange_interpolation(points1, k1);
    unsigned long long constant2 = lagrange_interpolation(points2, k2);

    // Print the results
    printf("Constant term for test case 1: %llu\n", constant1);
    printf("Constant term for test case 2: %llu\n", constant2);

    return 0;
}