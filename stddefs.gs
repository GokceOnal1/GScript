assign _stdin = "entry.gs";
assign _stdout = false;
assign _stderr = false;

funct __DEBUG (param str) {
  _stderr = false;
  write("DEBUG MESSAGE");
  write(str);
};