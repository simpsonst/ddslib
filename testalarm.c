#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "alarm.h"
#include "alarmsrc.h"

alarm_DECL(tvalarm, struct timeval);

typedef struct {
  tvalarm_db *dbp;
  struct timeval when, increment;
  tvalarm_h handle;
  char message[100];
} context_t;

void initcontext(context_t *ct, const char *m,
		 tvalarm_db *dbp, int s, int us)
{
  ct->dbp = dbp;
  ct->increment.tv_sec = s;
  ct->increment.tv_usec = us;
  ct->handle = NULL;
  strncpy(ct->message, m, sizeof ct->message - 1);
  ct->message[sizeof ct->message - 1] = '\0';
}

void contextevent(context_t *ct);

void armcontext(context_t *ct)
{
  ct->when.tv_usec += ct->increment.tv_usec;
  ct->when.tv_sec += ct->increment.tv_sec + ct->when.tv_usec / 1000000;
  ct->when.tv_usec %= 1000000;
  if (ct->handle) tvalarm_cancel(ct->dbp, ct->handle);
  ct->handle = tvalarm_set(ct->dbp, &ct->when,
				 (alarm_f *) &contextevent, ct);
}

void startcontext(context_t *ct)
{
  gettimeofday(&ct->when, NULL);
  armcontext(ct);
}

void contextevent(context_t *ct)
{
  printf("%s\n", ct->message);
  armcontext(ct);
}

int main(void)
{
  tvalarm_db db;
  struct timeval now, end;
  context_t c1, c2, c3;

  tvalarm_init(&db);

  initcontext(&c1, "1Hz", &db, 1, 0);
  initcontext(&c2, "2Hz", &db, 0, 500000);
  initcontext(&c3, "0.5Hz", &db, 2, 0);

  gettimeofday(&now, NULL);
  end.tv_sec = now.tv_sec + 15;
  end.tv_usec = now.tv_usec;

  startcontext(&c1);
  startcontext(&c2);
  startcontext(&c3);

  printf("Beginning...\n");
  while (now.tv_sec < end.tv_sec) {
    struct timeval timeout;

    gettimeofday(&now, NULL);
    switch (tvalarm_polldelay(&db, &now, &timeout)) {
    case alarm_OFF:
      printf("No more events.\n");
      exit(EXIT_SUCCESS);
      break;
    case alarm_OK:
      if (select(0, NULL, NULL, NULL, &timeout) == 0) {
	gettimeofday(&now, NULL);
	tvalarm_process(&db, &now);
      }
      break;
    case alarm_PENDING:
      tvalarm_process(&db, &now);
      break;
    case alarm_ERROR:
      abort();
    }
  }

  tvalarm_term(&db);

  return EXIT_SUCCESS;
}

#define COMPARE(X,Y) ((X)->tv_sec > (Y)->tv_sec ? 1 : \
((X)->tv_sec < (Y)->tv_sec ? -1 : (X)->tv_usec - (Y)->tv_usec))
#define SUBTRACT(A,X,Y) ((X)->tv_usec < (Y)->tv_usec ? \
(((A)->tv_sec = (X)->tv_sec - (Y)->tv_sec - 1), \
((A)->tv_usec = 1000000 + (X)->tv_usec - (Y)->tv_usec)) : \
(((A)->tv_sec = (X)->tv_sec - (Y)->tv_sec), \
((A)->tv_usec = (X)->tv_usec - (Y)->tv_usec)))
#define SETZERO(X) ((X)->tv_sec = (X)->tv_usec = 0)

alarm_DEFN(tvalarm, struct timeval, COMPARE, SUBTRACT, SETZERO);
