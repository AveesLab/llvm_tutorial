int a,b,c,d;

void function4()
{
    b++;
    d++;
}

void function3(long y)
{
    c = y;
    d--;
    function4();
}

int function2(int x)
{
    int e;

    e = x + 2;

    return e;
}

void function1()
{
    a = function2(1);
}
