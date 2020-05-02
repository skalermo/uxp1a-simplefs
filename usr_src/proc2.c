#include <stdio.h>
#include "simplefs_api.h"


int main(int argc, char const *argv[])
{
	int result = simplefs_close(0);
	printf("%d\n", result);
	return 0;
}