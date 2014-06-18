`timescale 1 ns / 100 ps

module Register(I, Q, clk, ld, rst);
   input [63:0] I;
   input 	clk, ld, rst;
   output [63:0] Q;
   reg [63:0] 	 Q;

   // Initialize register to NULL
   initial
     begin
	Q <= 0;
     end
   
   // Asynchronous reset
   always @(posedge rst)
     begin
	Q <= 0;
     end

   // Synchronous load or retain value
   always @(posedge clk)
     begin
	if(ld == 1)
	  Q <= I;
	else
	  Q <= Q;
     end

endmodule // Register

module RegisterTest;
   reg [63:0] I;
   reg 	      clk, ld, rst;
   wire [63:0] Q;
   
   // Instantiate one register and a clock
   Register reg(I,Q,clk,ld,rst);
   Clock clock(clk);
   
   initial
     begin
	ld <= 0;
	rst <= 0;
	I <= 0;
	#10;
	I <= 24;
	#10;
	ld <= 1;
	#10;
	ld <= 0;
	I <= 0;
	#5;
	rst <= 1;
     end

   initial
     begin
	$monitor($time, "ld: %b, rst: %b, I: %d, Q: %d",
		 ld, rst, I, Q);
     end

   initial  
     begin
	$dumpfile("register.vcd");
	$dumpvars;
     end 
   
endmodule; // RegisterTest
