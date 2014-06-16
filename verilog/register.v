module Register(I, Q, clk, ld, rst);
   input [63:0] I;
   input clk, ld, rst;
   output [63:0] Q;
   reg [63:0] Q;

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
   
endmodule; // RegisterTest
