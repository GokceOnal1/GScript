obj math = {
    field pi = 3.1415926535897932385;
    field e  = 2.7182818284590452354;

    method abs(param n) {
        if(n >= 0) {
            return n;
        } else {
            return n * -1;
        };
    };

    method sin(param n) {
        return __builtin_sin__(n);
    };
    method cos(param n) {
        return __builtin_cos__(n);
    };
    method tan(param n) {
        return __builtin_tan__(n);
    };
    method sqrt(param n) {
        return __builtin_sqrt__(n);
    };
    method root(param b, param n) {
        return __builtin_root__(b, n);
    };
    method pow(param b, param n) {
        return __builtin_pow__(b, n);
    };
};

