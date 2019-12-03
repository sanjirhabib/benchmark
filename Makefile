all:
	@bm ./gomap
	@bm ./phpmap
	@bm ./swiftmap
	@bm ./nodemap
	@bm ./pythonmap
	@bm ./luamap
	@bm ./rustmap
	@bm ./rubymap
	@bm java Main
	@bm dotnet dotnetmap/bin/Release/netcoreapp2.2/dotnetmap.dll
go:
	go build gomap.go
swift:
	swiftc swiftmap.swift
