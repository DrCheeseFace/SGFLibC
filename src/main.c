#include <mr_utils.h>
#include <stdio.h>

int main(void)
{
	MrlLogger *logger = mrl_create(stderr, FALSE, FALSE);
        mrl_logln(logger, MRL_SEVERITY_DEFAULT, "supsupsup");
        mrl_destroy(logger);

	printf("hello world!");
}
