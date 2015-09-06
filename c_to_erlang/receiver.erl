-module(receiver).
-export([start/0, stop/0]).

-define(START_BYTE, 0).
-define(END_BYTE, 255).
-define(REG_BYTE, 1).
-define(OK, 1).
-define(ERROR, -1).

start() ->
    ExtPrg = "/home/barlesh/ISG/c_to_erlang/c_receiver",
    PID = self(),
    spawn(fun() -> init_porter(ExtPrg, PID) end).
     
stop() ->
    complex ! stop,
    receiver!stop.

%%initialize the process that actiaaly sends & receive data to/from C program
init_porter(ExtPrg, Receiver) ->
    register(complex, self()),
    process_flag(trap_exit, true),
    Port = open_port({spawn, ExtPrg}, [{packet, 2}]),
    Msg_Buff = [],
    loop(Port, Receiver, Msg_Buff).
    
%%main loop of port procs. sends/receive data to/from C progrm. by receiving data, sends it to receiving pocess for further analyze 
loop(Port, Receiver, Msg_Buff) ->
    receive
		{Port, {data, Data}} ->
				io:format("at porter loop, got data: ~p~n", [Data]),
				Msg_Buff_new = decode(Data, Msg_Buff, Receiver),
				loop(Port, Receiver, Msg_Buff_new);
	stop ->
	    Port ! {self(), close},
	    receive
		{Port, closed} ->
		    io:format("C prog closed normaly~n"),
		    exit(normal)
	    end;
	{'EXIT', Port, Reason} ->
	    io:format("c process killed and notifies to erlang port~n"),
	    exit(port_terminated)
    end.
    

%case last byte of sent buffer, sent the whole buffer to receiver, and return a new empty buffer
decode([Type, Byte], Buff, Receiver) when Type =:= 255 ->  Receiver ! (Buff ++ [Byte]), [];
%else, add new byte to buffer and returne new buffer
decode([Type, Byte], Buff, Receiver) -> Buff ++ [Byte].
