#include <stdio.h>
#include <math.h>

int main(void)
{
    int segments;
    float total_surface_area;
    float volume;
    float avg_surface_area;
    float avg_volume;
    const float pi = 3.14159265359;

    printf("How many spherical segments you want to evaluate [2-10]? ");
    scanf("%d", &segments);
    
    for (int i=1; i <= segments; i++) {

        float radius;
        float top_height;
        float bottom_height;

        printf("Obtaining data for spherical segment number %d", i);
        printf("What is the radius of the sphere (R)?");
        scanf("%f", &radius);

        printf("What is the height of the top area of the spherical segment (ha)?");
        scanf("%f", &top_height);

        printf("What is the height of the bottom area of the spherical segment (hb)?");
        scanf("%f", &bottom_height);

        printf("Entered data: R = %0.2f ha = %0.2f hb %0.2f.", radius, top_height, bottom_height);

        printf("Total Surface Area = %0.2f Volume = %0.2f", total_surface_area, volume);
    };

}

// - restart loop iteration if conditionals are not met for user input
// - print error message for invalid input
// - build formula for calculating top SA, bottom SA, lateral SA, volume
// - 