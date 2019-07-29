#include "reactor.h"

int main()
{
    Reactor *reactor = new Reactor();
    reactor->init();
    reactor->start();
}