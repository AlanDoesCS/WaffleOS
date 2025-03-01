#include <ll/i386/pit.h>
#include <ll/sys/ll/time.h>

main()
{
	struct pitspec ps1;
	struct timespec ts1;

	ps1.units = 1432809;
	ps1.gigas = 10;

	PITSPEC2TIMESPEC(&ps1, &ts1);

	message("%lu %lu\n", PITSPEC2USEC(&ps1), TIMESPEC2USEC(&ts1));
	message("%lu %lu\n", ts1.tv_sec, ts1.tv_nsec);
	for(;;);
}
