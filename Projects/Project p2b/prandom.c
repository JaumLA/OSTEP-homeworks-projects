static int lastresult = 0;
static int a = 53;
static int incre = 1;

int 
randi(int modm){
    lastresult = (a*lastresult + incre) % modm;
    return lastresult;
}
