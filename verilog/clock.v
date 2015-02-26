`timescale 1 ns / 100 ps

module Clock(clk);
   output clk;
   reg 	  clk;

   always
     begin
	clk <= 0;
	#10;
	clk <= 1;
	#10;
     end
endmodule // clock
