#include "hook/fdhook.h"

FDHook::FDHook(int max_fd): max_fd_(max_fd), fdctxs_(new FDContext[max_fd + 1])
{
    
}