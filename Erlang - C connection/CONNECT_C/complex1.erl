-module(complex1).
-export([start/0, stop/0, init/1]).
-export([foo/1, bar/1, lesh/1]).

start() ->
    ExtPrg = "../connect_to_erlang/port",
    spawn(?MODULE, init, [ExtPrg]).
stop() ->
    complex ! stop.

foo(X) ->
    call_port({foo, X}).
bar(Y) ->
    call_port({bar, Y}).
lesh(Z) ->
    call_port({lesh,Z}).

call_port(Msg) ->
    complex ! {call, self(), Msg}.%,
    %receive
	%{complex, Result} ->
	 %   Result
    %end.

init(ExtPrg) ->
    register(complex, self()),
    process_flag(trap_exit, true),
    Port = open_port({spawn, ExtPrg}, [{packet, 2}]),
    loop(Port).

loop(Port) ->
    receive
	{call, Caller, Msg} ->
	    Port ! {self(), {command, encode(Msg)}},
	  	receive1(Caller),
	    loop(Port);
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

receive1(Caller)->
	  receive
		{Port, {data, Data}} ->
		    Caller ! {complex, Data},
		    io:format("~p",[Data]), receive1(Caller)
		 after 1000 -> io:format("time out at receive1~n")
	   end.

encode({foo, X}) -> [1, X];
encode({bar, Y}) -> [2, Y];
encode({lesh,Z}) -> [3, Z].

decode([Int]) -> Int.
