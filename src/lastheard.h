

#ifndef _LAST_HEARD_H_
#define _LAST_HEARD_H_

#define LHCALL_SIZE	12
#define LHNAME_SIZE 12

typedef struct lastHeardData{
	uint8_t call[LHCALL_SIZE];
	uint8_t name[LHNAME_SIZE];
} lastHeardData_t;

void lastheard_init(void);
void lastHeardAdd(lastHeardData_t *data);
#endif
