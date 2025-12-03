// run_context.cpp
#include "run_context.hpp"

RunContext& get_run_context()
{
    // one global instance for the whole program
    static RunContext ctx;
    return ctx;
}

void reset_run_context()
{
    RunContext& ctx = get_run_context();
    ctx = RunContext{};        // reset to default values
    ctx.waveNumber = 1;

    // you can also set starting currency here if you like
}
