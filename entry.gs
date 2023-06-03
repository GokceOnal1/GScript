funct exp(param base, param pow) {
  if(pow == 0) {
    return 1;
  } else {
    return base * exp(base, pow-1);
  };
};
funct cat(param str1, param str2) {
  return str1+str2;
};

write(exp(7, 3));

assign example = "hello ";
assign another = "world!";

write(cat(example, another));