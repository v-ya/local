#include "../vkaa.std.h"

int main(void)
{
	vkaa_std_s *restrict std;
	if ((std = vkaa_std_alloc()))
	{
		;
		refer_free(std);
	}
	return 0;
}
