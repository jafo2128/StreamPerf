#ifndef MTPERFTASKQOS_H_INCLUDED
#define MTPERFTASKQOS_H_INCLUDED

#include "MTPerfTask.h"

class MTPerfTaskRTSP : public MTPerfTask {
public:
    int doTask(void* args);
};

#endif // MTPERFTASKQOS_H_INCLUDED
