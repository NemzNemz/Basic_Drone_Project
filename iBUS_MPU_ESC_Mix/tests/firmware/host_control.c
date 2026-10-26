#include <setjmp.h>
#include <stdlib.h>
static jmp_buf main_exit;
static int main_active;
int firmware_main(void);
int host_run_main(void)
{
    main_active=1;
    if(setjmp(main_exit)==0){firmware_main();main_active=0;return 0;}
    main_active=0;
    return 1;
}
void host_stop_main(void)
{
    if(main_active) longjmp(main_exit,1);
    abort();
}
