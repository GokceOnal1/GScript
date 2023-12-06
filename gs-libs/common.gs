
$ Constants and methods related to mathematics
obj math {
    obj const = {
        field pi = 3.1415926;
        field e = 2.7182818;
    };
    obj random = {

    }
    method abs(param n) {
        if(n >= 0) {
            return n;
        } else {
            return n * -1;
        }
    }

}

assign i = 1;
repeat(4) {
    obj a = {field b = 10 + i;};
        repeat(2) {
        obj alpha = { field p = a.b; };
        write("alpha.p is ", alpha.p, "  a.b is  ", a.b, " i is ", i);
        };

    i = i + 1;
};

import #std;

