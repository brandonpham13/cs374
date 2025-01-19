#include <stdio.h>
#include <math.h>

const float pi = 3.14159265359;

float calc_SA(float radius, float ha, float hb) {
    float a = sqrt((radius*radius) - (ha*ha));
    float b = sqrt((radius*radius) - (hb*hb));
    float h = ha - hb;

    float top_SA = pi * (a*a);
    float bottom_SA = pi * (b*b);
    float lateral_SA = 2 * pi * radius * h;

    return top_SA + bottom_SA + lateral_SA;
}

float calc_volume(float radius, float ha, float hb) {
    float a = sqrt((radius*radius) - (ha*ha));
    float b = sqrt((radius*radius) - (hb*hb));
    float h = ha - hb;

    float volume = ((1.00/6.00) * pi * h) * ((3*a*a) + (3*b*b) + (h*h));
    return volume;
}

float calc_avg(float total, int segments) {
    return total / segments;
}

int valid_inputs(float radius, float ha, float hb) {
    if (radius <= 0 || ha <= 0 || hb <= 0) {
        printf("Invalid Input: R = %f ha = %f hb = %f. R, ha, and hb must all be positive real values.\n", radius, ha, hb);
        return 0;
    }
    if (ha > radius) {
        printf("Invalid Input: R = %f ha = %f. R must be greater than or equal to ha.\n", radius, ha);
        return 0;
    }
    if (hb > radius) {
        printf("Invalid Input: R = %f hb = %f. R must be greater than or equal to hb.\n", radius, hb);
        return 0;
    }
    if (hb > ha) {
        printf("Invalid Input: ha = %f hb = %f. ha must be greater than or equal to hb.\n", ha, hb);
        return 0;
    }
    return 1;
}

int main(void)
{
    int segments;
    float total_surface_area = 0;
    float total_volume = 0;
    float avg_surface_area;
    float avg_volume;

    while(1) {
        printf("How many spherical segments you want to evaluate [2-10]?\n");
        scanf("%d", &segments);
        if (segments > 1 && segments < 11) {
            break;
        }
    };

    for (int i=1; i <= segments; i++) {

        float radius, top_height, bottom_height, surface_area, volume;

        while(1) {
            printf("Obtaining data for spherical segment number %d\n", i);
            printf("What is the radius of the sphere (R)?\n");
            scanf("%f", &radius);

            printf("What is the height of the top area of the spherical segment (ha)?\n");
            scanf("%f", &top_height);

            printf("What is the height of the bottom area of the spherical segment (hb)?\n");
            scanf("%f", &bottom_height);

            printf("Entered data: R = %0.2f ha = %0.2f hb = %0.2f.\n", radius, top_height, bottom_height);

            if (valid_inputs(radius, top_height, bottom_height)) {
                break;
            }
        }

            surface_area = calc_SA(radius, top_height, bottom_height);
            volume = calc_volume(radius, top_height, bottom_height);

            total_surface_area += surface_area;
            total_volume += volume;

            printf("Total Surface Area = %0.2f Volume = %0.2f\n", surface_area, volume);  
    };

    avg_surface_area = calc_avg(total_surface_area, segments);
    avg_volume = calc_avg(total_volume, segments);

    printf("Average Surface Area = %0.2f Average Volume = %0.2f.\n", avg_surface_area, avg_volume);
}
