#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include "SSU_Sem.h"

void SSU_Sem_init(SSU_Sem *s, int value) {
		s->shared=value;
}

void SSU_Sem_down(SSU_Sem *s) {
		//0이면 블록
		//양의 정수면 -1
		while(1){
				if(s->shared==0){
						//busy waiting
				}else if(s->shared>0){
						s->shared--;
						break;
				}
		}

}

void SSU_Sem_up(SSU_Sem *s) {
		s->shared++;

}
