/* (program 
 *   (bindfun 
 *     (identifier f)
 *     (params 
 *       (identifier x
 *         (integer 10)
 *         (integer 3)
 *       )
 *       (identifier a
 *         (integer 1)
 *       )
 *     )
 *     (return 
 *       (identifier y)
 *     )
 *     (statement 
 *       (bindvar 
 *         (identifier y)
 *         (add 
 *           (identifier x)
 *           (identifier a)
 *         )
 *       )
 *     )
 *   )
 * )
 */

module f(x,a,y);
   input [63:0] x [0:9] [0:2];
   input [63:0] a;
   output [63:0] y [0:9] [0:2];
   
endmodule // f

module f_datapath(x,a,y,stmt1,clk);
   input [1919:0] x;
   input [63:0]   a;
   output [1919:0] y;
   input 	   stmt1;
   input 	   clk;

   always @(posedge clk)
     begin
	if(stmt1 == 1)
	  
	  end
     end
   
endmodule // f_datapath

module f_controller(start,done,stmt1,clk);
   input start;
   output done;
   output stmt1;
   input  clk;
   
   parameter
     s0 = 2'b00,
     s1 = 2'b01,
     s2 = 2'b10;

   reg [1:0] state;
   reg [1:0] next_state;

   always @(posedge clk)
     begin
	state <= next_state;
     end

   always @(state)
     begin
	case(state)
	  s0: begin
	     done <= 0;
	     if(start == 1)
	       next_state <= s1;
	     else
	       next_state <= s0;
	  end
	  s1: begin
	     stmt1 <= 1;
	     next_state <= s2;
	  end
	  s2: begin
	     stmt1 <= 0;
	     done <= 1;
	     next_state <= s0;
	  end
	endcase
     end
endmodule // f_controller
