#include "is31io7326.h"
#include "keyscanner.h"

static inline void setup(void)
{
    keyscanner_init();
    issi_init();
}

int main(void)
{
    setup();
    while(1){
        keyscanner_main();
    }
    __builtin_unreachable();
}
