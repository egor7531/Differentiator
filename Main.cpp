#include "assert.h"
#include <stdio.h>

#include "Differentiator.h"

enum
{
    DERIVATIVE,
    TAYLOR_SERIES
};

const char* nameFile = "Expression.txt";

int main()
{
    printf( "Calculate derivative      - %d\n"
            "arrange the Taylor series - %d\n"
            "Enter mode: ",
            DERIVATIVE, TAYLOR_SERIES);

    int mode = 0;
    scanf("%d", &mode);

    switch(mode)
    {
        case DERIVATIVE:
            get_derivative(nameFile);
            break;
        case TAYLOR_SERIES:
        {
            int order = 0;
            double point = 0;
            
            printf("Enter order: ");
            scanf("%d", &order);
            printf("Enter point: ");
            scanf("%lf", &point);
            
            get_taylor_series(nameFile, order, point);
            break;
        }
        default:
            assert(!"Unknown mode");
            break;
    }

    return 0;
}