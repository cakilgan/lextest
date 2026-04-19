#include <lextest.h>

LXTEST(Math, Addition, "add numbers"){
    LXSKIP LXEQ(1,2);
    LXDEPRECATED LXGT(50,10);
    LXLT(10,20);
    LXEQ(10,10);

    LXRETURN;
    LXSTREQ("a", "A");
    LXRETURN;
}