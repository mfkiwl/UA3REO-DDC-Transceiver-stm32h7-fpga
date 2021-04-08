module DAC_corrector(
clk_in,
DATA_IN,
shift,

DATA_OUT
);

parameter in_width = 27;
parameter out_width = 14;

input clk_in;
input signed [(in_width-1):0] DATA_IN;
input unsigned [7:0] shift;

output reg unsigned [(out_width-1):0] DATA_OUT;

reg signed [(out_width-1):0] tmp=0;

always @ (posedge clk_in)
begin
	//получаем 14 бит
	if (shift<out_width)
	begin
		tmp[(out_width-1):0] = DATA_IN[(out_width-1):0];
	end
	if (shift>in_width)
	begin
		tmp[(out_width-1):0] = DATA_IN[(in_width-1) -: out_width];
	end
	else
	begin
		tmp[(out_width-1):0] = DATA_IN[(shift-1) -: out_width];
	end
	
	//инвертируем первый бит, получая unsigned из two's complement
	DATA_OUT[(out_width-1):0]={~tmp[(out_width-1)],tmp[(out_width-2):0]}; 
	
end


endmodule
