

#ifndef _LAST_HEARD_H_
#define _LAST_HEARD_H_

#define LHCALL_SIZE	10
#define LHNAME_SIZE 18

typedef struct lastHeardData{
	uint32_t tg;
	uint8_t call[LHCALL_SIZE];
	uint8_t name[LHNAME_SIZE];
} lastHeardData_t;

void lastheard_init(void);
void lastHeardAdd(lastHeardData_t *data);
void lastHeardAddById(uint32_t id);
void lastHeardAddByIdAndTG(uint32_t id, uint32_t dst);
#endif
