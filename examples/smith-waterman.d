[H] = sw(a::1024:1, b::1024:1){

  

  [x] = s(i::a::1, b::1){
    x = a == b ? 2 : -1;
  }

  [x] = W(i::1){
    x = -i;
  }
}
